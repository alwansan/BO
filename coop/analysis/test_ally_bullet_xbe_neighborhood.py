#!/usr/bin/env python3
"""Synthetic x86 XBE only; never original owner's game or private memory."""
import hashlib
import json
import shutil
import struct
import unittest

from story_entry_static import UnsafeImage
from ally_bullet_xbe_neighborhood import audit,TARGETS,WINDOW

def image():
    lo=min(TARGETS.values())-0x1000
    hi=max(TARGETS.values())+0x1000
    count=hi-lo+0x1000
    raw=bytearray(0x400+count)
    raw[:4]=b"XBEH"
    struct.pack_into("<I",raw,0x104,0x10000)
    struct.pack_into("<I",raw,0x11C,1)
    struct.pack_into("<I",raw,0x120,0x10200)
    struct.pack_into("<6I",raw,0x200,4,lo,count,0x400,count,0)
    raw[0x400:]=b"\x90"*count
    # First: cmp byte [ecx+0x8F2],0; jne; cmp dword [ecx+0x2F0],0; ret
    code=bytes.fromhex("80 b9 f2 08 00 00 00 75 05 83 b9 f0 02 00 00 00 c3")
    for at in TARGETS.values():
        off=0x400+at-lo
        raw[off:off+len(code)]=code
    return bytes(raw)

@unittest.skipUnless(shutil.which("objdump"),"system binutils required")
class TestOriginalSoldierBulletMap(unittest.TestCase):
    def test_three_source_confirmed_windows_and_anonymization(self):
        data=image()
        result=audit(data,hashlib.sha1(data).hexdigest())
        self.assertTrue(result["owner_original_xbe_sha1_verified"])
        self.assertEqual(set(result["targets"]),set(TARGETS))
        for counts in result["targets"].values():
            self.assertEqual(counts["window_bytes"],WINDOW)
            self.assertGreater(counts["conditional_branch_candidates"],0)
            self.assertGreater(counts["decoded_instruction_count"],3)
            self.assertGreater(counts["known_actor_offset_references"]["local_player_cheat_invulnerable_source"],0)
            self.assertGreater(counts["known_actor_offset_references"]["character_health_source"],0)
            self.assertFalse(counts["friendly_fire_damage_rejection_found"])
        self.assertFalse(result["original_friend_foe_field_verified"])
        self.assertFalse(result["original_no_friendly_fire_hook_verified"])
        output=json.dumps(result)
        for secret in ("80 b9 f2","XBEH","cmp BYTE PTR","window.bin","owner.iso"):
            self.assertNotIn(secret,output)
    def test_pinned_identity_and_ambiguous_window_fail(self):
        source=image()
        with self.assertRaises(UnsafeImage):audit(source,"0"*40)
        with self.assertRaises(UnsafeImage):audit(source,hashlib.sha1(source).hexdigest(),{"bad":0xFFFF})
        with self.assertRaises(UnsafeImage):audit(source,hashlib.sha1(source).hexdigest(),{"a":1,"b":1})
        changed=bytearray(source);changed[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(changed),hashlib.sha1(changed).hexdigest())

if __name__=="__main__":unittest.main()
