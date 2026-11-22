import hashlib
import struct
import shutil
import unittest
from original_streaming_stage_xbe import audit, ENTRY, WINDOW
from story_entry_static import UnsafeImage

def fake_xbe(with_stage=True):
    va=ENTRY-0x100
    size=WINDOW+0x400
    b=bytearray(0x400+size)
    b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,va,size,0x400,size,0)
    b[0x400:]=b"\x90"*size
    point=0x400+0x100+0x680
    # mov dl,BYTE PTR [eax+0x2020c] ; mov al,[eax+0x2020e]
    b[point:point+6]=bytes.fromhex("8a 90 0c 02 02 00")
    if with_stage:
        b[point+6:point+12]=bytes.fromhex("8a 80 0e 02 02 00")
    b[point+12]=0xc3
    return bytes(b)

@unittest.skipUnless(shutil.which("objdump"),"objdump required")
class StreamingSource(unittest.TestCase):
    def test_both_byte_reads_in_candidate_and_redaction(self):
        data=fake_xbe()
        info=audit(data,hashlib.sha1(data).hexdigest())
        self.assertTrue(info["both_candidate_byte_loads_present"])
        self.assertEqual(info["field_2020c"]["byte_reg_loads"],1)
        self.assertEqual(info["field_2020e"]["byte_reg_loads"],1)
        self.assertTrue(info["first_up_to_four_relative_offsets"]["level"])
        self.assertTrue(info["first_up_to_four_relative_offsets"]["stage"])
        self.assertFalse(info["safe_guest_stage_loader_abi_proven"])
        self.assertFalse(info["actual_stage_loaded_callback_proven"])
        self.assertFalse(info["actual_host_mission_identity_proven"])
        self.assertFalse(info["story_ready_or_coop_gameplay_proven"])
        self.assertFalse(info["raw_original_instructions_or_assets_exported"])
        self.assertNotIn("mov dl",str(info))
        self.assertNotIn("8a 90",str(info))
    def test_negative_source_but_successful_scan(self):
        data=fake_xbe(False)
        info=audit(data,hashlib.sha1(data).hexdigest())
        self.assertFalse(info["both_candidate_byte_loads_present"])
        self.assertEqual(info["field_2020e"]["byte_reg_loads"],0)
    def test_wrong_owner_hash_or_bad_image_rejected(self):
        b=fake_xbe()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        v=bytearray(b);v[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(v),hashlib.sha1(v).hexdigest())

if __name__=="__main__":unittest.main()
