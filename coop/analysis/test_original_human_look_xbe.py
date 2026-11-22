import hashlib
import shutil
import struct
import unittest
from original_human_look_xbe import audit,TARGETS,WINDOW,FIELDS
from story_entry_static import UnsafeImage

def fake_image(valid_refs=True):
    low=min(TARGETS.values())-0x100
    top=max(TARGETS.values())+WINDOW+0x100
    n=top-low
    b=bytearray(0x400+n)
    b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,low,n,0x400,n,0)
    b[0x400:]=b"\\x90"*n
    body=(bytes.fromhex("55 89 e5 8b 81 28 03 00 00 e8 00 00 00 00 c3")
          if valid_refs else bytes.fromhex("55 89 e5 89 d8 c3"))
    for entry in TARGETS.values():
        off=0x400+entry-low
        b[off:off+len(body)]=body
    return bytes(b)
@unittest.skipUnless(shutil.which("objdump"),"objdump required")
class SourceOnlyLookXbe(unittest.TestCase):
    def test_bounded_private_look_candidate_mapping(self):
        b=fake_image()
        report=audit(b,hashlib.sha1(b).hexdigest())
        self.assertEqual(report["schema"],"bo-original-human-look-private-v1")
        self.assertEqual(set(report["windows"]),set(TARGETS))
        self.assertTrue(all(row["executable_window_mapped"] for row in report["windows"].values()))
        self.assertTrue(all(row["relative_call_candidates"]>=1 for row in report["windows"].values()))
        self.assertTrue(all(row["field_reference_counts"]["look_instance_328"]>=1 for row in report["windows"].values()))
        for name in ("android_engine_guest_abi_verified","actor_instantiation_or_rendering_verified",
                     "source_function_boundaries_verified","original_bytes_exported"):
            self.assertFalse(report[name])
        self.assertNotIn("mov eax",str(report))
        self.assertNotIn("55 89 e5",str(report))
    def test_no_reference_is_not_abi_proof(self):
        b=fake_image(False)
        report=audit(b,hashlib.sha1(b).hexdigest())
        for v in report["windows"].values():
            self.assertEqual(v["field_reference_counts"]["look_instance_328"],0)
            self.assertFalse(v["safe_guest_call_abi_verified"])
            self.assertFalse(v["safe_engine_actor_creation_verified"])
    def test_mismatched_or_malformed_original_is_rejected(self):
        b=fake_image()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        v=bytearray(b);v[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(v),hashlib.sha1(v).hexdigest())
        with self.assertRaises(UnsafeImage):audit(b,"not sha")
    def test_executable_candidate_mapping_is_required(self):
        b=bytearray(fake_image())
        struct.pack_into("<I",b,0x200,0)
        with self.assertRaises(UnsafeImage):audit(bytes(b),hashlib.sha1(b).hexdigest())
if __name__=="__main__":unittest.main()
