"""Synthetic-only tests for the privacy-safe bounded mission entry audit."""
import hashlib
import json
import shutil
import struct
import unittest

from story_entry_static import TARGETS, UnsafeImage, audit_bytes


def image():
    data = bytearray(0x400 + 0x18000)
    data[:4] = b"XBEH"
    struct.pack_into("<I", data, 0x104, 0x10000)      # image base
    struct.pack_into("<I", data, 0x11C, 1)            # section count
    struct.pack_into("<I", data, 0x120, 0x10200)      # section table VA
    struct.pack_into("<6I", data, 0x200,
                     4, 0x110000, 0x18000, 0x400, 0x18000, 0)
    for address in TARGETS.values():
        at = 0x400 + address - 0x110000
        data[at:at + 12] = b"\x55\x89\xe5\xe8\x00\x00\x00\x00\x5d\xc3\x90\x90"
    return data


@unittest.skipUnless(shutil.which("objdump"), "binutils objdump unavailable")
class StoryEntryStaticTests(unittest.TestCase):
    def test_three_mapped_windows_and_no_raw_game_content(self):
        owner = image()
        report = audit_bytes(owner, hashlib.sha1(owner).hexdigest())
        self.assertTrue(report["owner_pinned_sha1_matched"])
        self.assertEqual(len(report["candidate_windows"]), 3)
        self.assertEqual(set(report["candidate_windows"]), set(TARGETS))
        for value in report["candidate_windows"].values():
            self.assertGreater(value["instructions_in_fixed_window"], 0)
            self.assertGreater(value["call_instructions"], 0)
            self.assertTrue(value["executable_window_mapped"])
        for indicator in ("source_comment_semantics_proven", "actual_scene_loaded_callback_found",
                          "mission_choice_reliably_extracted", "game_thread_identity_proven",
                          "multiplayer_gameplay_proven"):
            self.assertIs(report[indicator], False)
        safe = json.dumps(report)
        self.assertNotIn("private-window.bin", safe)
        self.assertNotIn("XBEH", safe)
        self.assertNotIn("0x400", safe)

    def test_wrong_hash_rejected_before_decode(self):
        with self.assertRaises(UnsafeImage):
            audit_bytes(image(), "0" * 40)

    def test_bad_xbe_magic(self):
        data = image()
        data[0] = 0
        with self.assertRaises(UnsafeImage):
            audit_bytes(data, hashlib.sha1(data).hexdigest())

    def test_non_executable_section(self):
        data = image()
        struct.pack_into("<I", data, 0x200, 0)
        with self.assertRaises(UnsafeImage):
            audit_bytes(data, hashlib.sha1(data).hexdigest())

    def test_missing_candidate_window_and_overlapping_sections(self):
        data = image()
        struct.pack_into("<I", data, 0x208, 0x100)
        with self.assertRaises(UnsafeImage):
            audit_bytes(data, hashlib.sha1(data).hexdigest())
        data = image()
        struct.pack_into("<I", data, 0x11C, 2)
        struct.pack_into("<6I", data, 0x238,
                         4, 0x110000, 0x18000, 0x400, 0x18000, 0)
        with self.assertRaises(UnsafeImage):
            audit_bytes(data, hashlib.sha1(data).hexdigest())

    def test_truncated_raw_mapping(self):
        data = image()
        struct.pack_into("<I", data, 0x210, 0xFFFFFFFF)
        with self.assertRaises(UnsafeImage):
            audit_bytes(data, hashlib.sha1(data).hexdigest())


if __name__ == "__main__":
    unittest.main()
