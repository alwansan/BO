import hashlib
import shutil
import struct
import unittest
from original_bot_puppet_draw_xbe import audit,WINDOW,TARGETS
from story_entry_static import UnsafeImage

def fake_xbe():
    start=min(TARGETS.values())-0x80
    end=max(TARGETS.values())+WINDOW+0x80
    n=end-start
    b=bytearray(0x400+n)
    b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,start,n,0x400,n,0)
    b[0x400:]=b"\x90"*n
    for entry in TARGETS.values():
        off=0x400+entry-start
        b[off:off+9]=bytes.fromhex("55 89 e5 e8 00 00 00 00 c3")
    return bytes(b)

@unittest.skipUnless(shutil.which("objdump"),"objdump required")
class OriginalPuppetDrawXbe(unittest.TestCase):
    def test_two_pinned_function_windows_numeric_only(self):
        b=fake_xbe(); report=audit(b,hashlib.sha1(b).hexdigest())
        self.assertEqual(report["schema"],"bo-private-original-puppet-draw-source-v1")
        self.assertEqual(set(report["owner_original_candidate_windows"]),set(TARGETS))
        for item in report["owner_original_candidate_windows"].values():
            self.assertTrue(item["original_executable_candidate"])
            self.assertEqual(item["bounded_window_bytes"],640)
            self.assertGreater(item["decoded_instruction_candidates"],3)
            self.assertFalse(item["exact_android_draw_abi_verified"])
            self.assertFalse(item["original_renderer_can_safely_clone_character"])
        self.assertEqual(report["source_prototype_draw_virtual_slot"],5)
        self.assertFalse(report["android_avatar_drawn_in_game_verified"])
        self.assertFalse(report["original_binary_or_disassembly_exported"])
        self.assertNotIn("mov",str(report))
    def test_missing_wrong_owner_or_overlap_fails_closed(self):
        b=fake_xbe()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        with self.assertRaises(UnsafeImage):audit(b,"wrong")
        invalid=bytearray(b);invalid[0]=0
        with self.assertRaises(UnsafeImage):
            audit(bytes(invalid),hashlib.sha1(invalid).hexdigest())
        invalid=bytearray(b);struct.pack_into("<I",invalid,0x200,0)
        with self.assertRaises(UnsafeImage):
            audit(bytes(invalid),hashlib.sha1(invalid).hexdigest())
if __name__=="__main__":unittest.main()
