import hashlib
import json
import pathlib
import struct
import tempfile
import unittest
from biso_manifest_audit import load_manifest, validate_stored_parts, verify_iso, MAGIC, SECTOR

class AuditTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root=pathlib.Path(self.tmp.name)
    def manifest(self, records):
        m=self.root/"manifest.json"
        m.write_text(json.dumps({"format":"github-auto-upload-v1",
                                "files":records,"empty_dirs":[]}),encoding="utf-8")
        return m
    @staticmethod
    def record(name, data, parts=None):
        return {"path":"Black_Xbox_Extracted/"+name,"bytes":len(data),
                "sha256":hashlib.sha256(data).hexdigest(),
                "parts":parts or ["Black_Xbox_Extracted/"+name]}
    def test_manifest_structure_and_split_reassembly(self):
        source=self.root/"Black_Xbox_Extracted"
        source.mkdir()
        item=self.record("example.bin",b"abc")
        (source/"example.bin").write_bytes(b"abc")
        large=source/"big.bks"
        large.mkdir()
        a,b="Black_Xbox_Extracted/big.bks/part-00000","Black_Xbox_Extracted/big.bks/part-00001"
        (large/"part-00000").write_bytes(b"first")
        (large/"part-00001").write_bytes(b"second")
        parts=self.record("big.bks",b"firstsecond",[a,b])
        entries,summary=load_manifest(self.manifest([item,parts]))
        self.assertEqual(summary["original_file_count"],2)
        self.assertEqual(summary["stored_part_count"],3)
        self.assertEqual(summary["multi_part_file_count"],1)
        self.assertEqual(validate_stored_parts(self.root,entries),14)
        (large/"part-00001").write_bytes(b"SECOND")
        with self.assertRaisesRegex(ValueError,"hash or size"):
            validate_stored_parts(self.root,entries)
    def test_malicious_paths_rejected(self):
        for name in ("../bad","ok/../../bad","/absolute","bad\\name","bad//file"):
            with self.subTest(name=name):
                with self.assertRaises(ValueError):
                    load_manifest(self.manifest([self.record(name,b"x")]))
        a=self.record("big.bks",b"ab",[
            "Black_Xbox_Extracted/big.bks/part-00001",
            "Black_Xbox_Extracted/big.bks/part-00000"])
        with self.assertRaisesRegex(ValueError,"out of order"):
            load_manifest(self.manifest([a]))
    def test_malicious_duplicates_rejected(self):
        rec=self.record("foo",b"data")
        with self.assertRaisesRegex(ValueError,"duplicate target"):
            load_manifest(self.manifest([rec,rec]))
    def test_synthetic_xdvdfs_iso_match_and_tamper(self):
        # Synthetic image contains exactly one original file, not copyrighted.
        file_data=b"TEST"
        sector_root=50
        sector_file=80
        entry=struct.pack("<HHIIBB",0,0,sector_file,len(file_data),0,4)+b"x.db"
        image=bytearray(sector_file*SECTOR+len(file_data))
        image[32*SECTOR:32*SECTOR+20]=MAGIC
        struct.pack_into("<II",image,32*SECTOR+20,sector_root,len(entry))
        image[sector_root*SECTOR:sector_root*SECTOR+len(entry)]=entry
        image[sector_file*SECTOR:sector_file*SECTOR+len(file_data)]=file_data
        path=self.root/"synthetic.xiso"
        path.write_bytes(image)
        records,_=load_manifest(self.manifest([self.record("x.db",file_data)]))
        self.assertEqual(verify_iso(path,records),1)
        image[-1]=ord("X")
        path.write_bytes(image)
        with self.assertRaisesRegex(ValueError,"hash"):
            verify_iso(path,records)
if __name__=="__main__":unittest.main()
