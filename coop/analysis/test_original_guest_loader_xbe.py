import hashlib
import shutil
import struct
import unittest
from original_guest_loader_xbe import audit,TARGETS,WINDOW
from story_entry_static import UnsafeImage

def image(mixed=False):
    bottom=min(TARGETS.values())-0x200
    top=max(TARGETS.values())+WINDOW+0x200
    count=top-bottom
    b=bytearray(0x400+count);b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,bottom,count,0x400,count,0)
    b[0x400:]=b"\x90"*count
    for label,addr in TARGETS.items():
        code=(bytes.fromhex("0f b6 90 0c 02 02 00 0f b6 80 0e 02 02 00 e8 00 00 00 00 c3")
              if mixed or label=="original_stream_loader_entry"
              else bytes.fromhex("55 89 e5 e8 00 00 00 00 c3"))
        at=0x400+addr-bottom
        b[at:at+len(code)]=code
    return bytes(b)
@unittest.skipUnless(shutil.which("objdump"),"objdump required")
class GuestLaunchOriginalXbe(unittest.TestCase):
    def test_three_bounded_candidates_and_two_source_bytes(self):
        b=image()
        result=audit(b,hashlib.sha1(b).hexdigest())
        self.assertEqual(set(result["candidates"]),set(TARGETS))
        self.assertEqual(result["schema"],"bo-owner-guest-loader-bridge-private-v1")
        self.assertGreaterEqual(result["candidates"]["original_stream_loader_entry"]["source_level_byte_read_candidates"],1)
        self.assertGreaterEqual(result["candidates"]["original_stream_loader_entry"]["source_stage_byte_read_candidates"],1)
        self.assertEqual(result["candidates"]["original_menu_launch_attempt"]["source_level_byte_read_candidates"],0)
        self.assertFalse(result["original_game_guest_mission_start_verified"])
        self.assertFalse(result["true_game_scene_loaded_callback_verified"])
        self.assertFalse(result["private_executable_data_exported"])
        self.assertNotIn("movzx",str(result))
    def test_independent_windows_do_not_mean_real_call_abi(self):
        b=image(True);r=audit(b,hashlib.sha1(b).hexdigest())
        for v in r["candidates"].values():
            self.assertGreaterEqual(v["source_level_byte_read_candidates"],1)
            self.assertGreaterEqual(v["source_stage_byte_read_candidates"],1)
            self.assertFalse(v["proven_unique_function_boundary"])
            self.assertFalse(v["safe_guest_loader_abi_verified"])
    def test_bad_image_and_pinned_sha1_mismatch_fail_closed(self):
        b=image()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        with self.assertRaises(UnsafeImage):audit(b,"bogus")
        bad=bytearray(b);bad[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(bad),hashlib.sha1(bad).hexdigest())
        altered=bytearray(b);struct.pack_into("<I",altered,0x200,0)
        with self.assertRaises(UnsafeImage):
            audit(bytes(altered),hashlib.sha1(altered).hexdigest())
if __name__=="__main__":unittest.main()
