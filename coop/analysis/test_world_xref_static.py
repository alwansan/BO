"""Synthetic-only x86 world-pointer xref analyzer safety/accuracy checks."""
import hashlib
import json
import shutil
import struct
import unittest

from story_entry_static import UnsafeImage
from world_xref_static import analyze_bytes, classify_xref


def synthetic():
    data=bytearray(0x400+0x800)
    data[:4]=b"XBEH"
    struct.pack_into("<I", data, 0x104, 0x10000)
    struct.pack_into("<I", data, 0x11C, 1)
    struct.pack_into("<I", data, 0x120, 0x10200)
    struct.pack_into("<6I", data, 0x200, 4, 0x110000, 0x800, 0x400, 0x800, 0)
    data[0x400:]=b"\x90"*0x800
    # Intel x86: mov eax, [0x2d45d4]; mov [0x2d45d4], eax;
    # mov dword ptr [0x2d45d4], 0; cmp [0x2d45d4], eax.
    instructions=bytes.fromhex(
        "a1 d4 45 2d 00 "
        "a3 d4 45 2d 00 "
        "c7 05 d4 45 2d 00 00 00 00 00 "
        "39 05 d4 45 2d 00"
    )
    data[0x440:0x440+len(instructions)]=instructions
    return bytes(data)


class WorldXrefTests(unittest.TestCase):
    def test_operand_classifier_disallows_immediates_and_nonworld_refs(self):
        self.assertEqual(classify_xref("mov","DWORD PTR ds:0x2d45d4,eax"),None)
        self.assertIsNone(classify_xref("mov","eax,DWORD PTR ds:0x2d45dc"))
        self.assertEqual(classify_xref("mov","DWORD PTR ds:0x2d45d4[eax],ecx"),None)
        self.assertEqual(classify_xref("mov","DWORD PTR ds:0x2d45d4, 0x0"),None)
        self.assertEqual(classify_xref("mov","DWORD PTR [0x2d45d4],eax"),"write_candidate")
        self.assertEqual(classify_xref("mov","eax,DWORD PTR [0x2d45d4]"),"read_or_address")
        self.assertEqual(classify_xref("cmp","DWORD PTR [0x2d45d4],eax"),"read_or_address")
        self.assertEqual(classify_xref("inc","DWORD PTR [0x2d45d4]"),"read_modify_write_candidate")

    @unittest.skipUnless(shutil.which("objdump"),"objdump unavailable")
    def test_synthetic_real_binutils_and_output_privacy(self):
        src=synthetic()
        result=analyze_bytes(src,hashlib.sha1(src).hexdigest())
        self.assertEqual(result["executable_section_count"],1)
        self.assertEqual(result["write_candidates"],2)
        self.assertEqual(result["read_or_address_candidates"],2)
        self.assertEqual(result["world_global_address"],"002D45D4")
        self.assertGreaterEqual(len(result["bounded_possible_writer_addresses"]),2)
        self.assertFalse(result["true_scene_loader_proven"])
        self.assertFalse(result["safe_hook_proven"])
        self.assertFalse(result["world_epoch_proven"])
        report=json.dumps(result)
        self.assertNotIn("c7 05",report)
        self.assertNotIn("mov DWORD PTR",report)
        self.assertNotIn("XBEH",report)

    def test_bad_owner_image_rejected(self):
        src=synthetic()
        with self.assertRaises(UnsafeImage):
            analyze_bytes(src,"0"*40)
        with self.assertRaises(UnsafeImage):
            analyze_bytes(src,"notsure")

    def test_invalid_section_rejected(self):
        src=bytearray(synthetic())
        struct.pack_into("<I",src,0x200,0)
        with self.assertRaises(UnsafeImage):
            analyze_bytes(src,hashlib.sha1(src).hexdigest())


if __name__=="__main__":
    unittest.main()
