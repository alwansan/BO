"""Bounded x86 heuristic writer context test; synthetic XBE only."""
import hashlib
import json
import shutil
import struct
import unittest
from story_entry_static import UnsafeImage
from world_writer_flow import analyze_context, direct_target

def synthetic():
    data=bytearray(0x400+0x300)
    data[:4]=b"XBEH"
    struct.pack_into("<I",data,0x104,0x10000)
    struct.pack_into("<I",data,0x11C,1)
    struct.pack_into("<I",data,0x120,0x10200)
    struct.pack_into("<6I",data,0x200,4,0x110000,0x300,0x400,0x300,0)
    data[0x400:]=b"\x90"*0x300
    # Direct CALL into first store and JMP to second, never executed here.
    data[0x420:0x425]=bytes.fromhex("e8 23 00 00 00")
    data[0x42a:0x42f]=bytes.fromhex("e9 22 00 00 00")
    # xor eax,eax; mov ds:0x2d45d4,eax at VA 0x110048
    data[0x446:0x448]=bytes.fromhex("31 c0")
    data[0x448:0x44d]=bytes.fromhex("a3 d4 45 2d 00")
    # 4 NOPs, second store at VA 0x110051, source ecx
    data[0x451:0x457]=bytes.fromhex("89 0d d4 45 2d 00")
    return bytes(data)

class TestWorldWriterFlow(unittest.TestCase):
    def test_rejects_indirect_call_and_non_direct_values(self):
        self.assertIsNone(direct_target("DWORD PTR [eax]"))
        self.assertIsNone(direct_target("eax"))
        self.assertIsNone(direct_target("DWORD PTR ds:0x110048"))
        self.assertEqual(direct_target("110048 <foo>"),0x110048)
        self.assertEqual(direct_target("0x110048"),0x110048)
    @unittest.skipUnless(shutil.which("objdump"),"objdump unavailable")
    def test_actual_disassembler_synthetic_two_register_stores_and_privacy(self):
        data=synthetic()
        out=analyze_context(data,hashlib.sha1(data).hexdigest(),(0x110048,0x110051))
        self.assertEqual(out["expected_register_world_writes"],2)
        self.assertEqual(out["store_address_separation_bytes"],[9])
        self.assertEqual([x["source_register"] for x in out["writers"]],["eax","ecx"])
        self.assertTrue(out["writers"][0]["possible_zeroed_register_in_previous_six"])
        self.assertGreaterEqual(out["nearby_direct_call_targets_count"],1)
        self.assertGreaterEqual(out["nearby_direct_branch_targets_count"],1)
        self.assertFalse(out["verified_scene_loader"])
        self.assertFalse(out["verified_game_thread_hook"])
        safe=json.dumps(out)
        for forbidden in ["xor eax,eax","mov ds:0x","a3 d4 45","XBEH"]:
            self.assertNotIn(forbidden,safe)
    @unittest.skipUnless(shutil.which("objdump"),"objdump unavailable")
    def test_mutated_target_fails_closed(self):
        data=bytearray(synthetic())
        data[0x451]=0x90
        payload=bytes(data)
        with self.assertRaises(UnsafeImage):
            analyze_context(payload,hashlib.sha1(payload).hexdigest(),(0x110048,0x110051))
    def test_pin_and_missing_writer_fail_closed(self):
        data=synthetic()
        with self.assertRaises(UnsafeImage):
            analyze_context(data,"0"*40,(0x110048,0x110051))
        with self.assertRaises(UnsafeImage):
            analyze_context(data,hashlib.sha1(data).hexdigest(),(0x110048,0x110050))
        with self.assertRaises(UnsafeImage):
            analyze_context(data,hashlib.sha1(data).hexdigest(),(0x110051,0x110048))

if __name__=="__main__":
    unittest.main()
