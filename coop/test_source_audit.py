import hashlib
import pathlib
import struct
import tempfile
import unittest
from source_audit import restore, xbe_from_iso, MAGIC

class AuditTest(unittest.TestCase):
    def test_restore_rejects_bad_hash(self):
        with tempfile.TemporaryDirectory() as d:
            r = pathlib.Path(d)
            (r/"a").write_bytes(b"a")
            (r/"b").write_bytes(b"b")
            rec = {"parts":["a","b"],"bytes":2,"sha256":hashlib.sha256(b"ab").hexdigest()}
            restore(r,rec,r/"out")
            self.assertEqual((r/"out").read_bytes(),b"ab")
            rec["sha256"]="0"*64
            with self.assertRaises(ValueError): restore(r,rec,r/"out")
    def test_xdvdfs(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/"disc"
            data=bytearray(35*2048)
            data[32*2048:32*2048+20]=MAGIC
            struct.pack_into("<II",data,32*2048+20,33,64)
            name=b"default.xbe"
            struct.pack_into("<HHIIBB",data,33*2048,0,0,34,8,0,len(name))
            data[33*2048+14:33*2048+14+len(name)]=name
            data[34*2048:34*2048+8]=b"XBEHtest"
            p.write_bytes(data)
            self.assertEqual(xbe_from_iso(p)["sha1"],hashlib.sha1(b"XBEHtest").hexdigest())
            struct.pack_into("<I",data,33*2048+4,9999)
            p.write_bytes(data)
            with self.assertRaises(ValueError): xbe_from_iso(p)
if __name__ == "__main__":
    unittest.main()
