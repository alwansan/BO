import hashlib,struct,unittest,shutil
from original_core_running_xbe import audit,TARGETS,WINDOW
from story_entry_static import UnsafeImage
def synthetic(yes=True,direct_literal=False,core_stage_bytes=False):
    low=min(TARGETS.values())-0x100
    top=max(TARGETS.values())+WINDOW+0x100
    count=top-low
    b=bytearray(0x400+count)
    b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,low,count,0x400,count,0)
    b[0x400:]=b"\x90"*count
    op=bytes.fromhex("8b 80 e0 5a 00 00 83 f8 1c c3") if yes \
       else bytes.fromhex("89 d8 83 f8 1c c3")
    for at in TARGETS.values():
        raw=0x400+at-low
        body=(bytes.fromhex("c7 80 e0 5a 00 00 1c 00 00 00 83 b8 e0 5a 00 00 1c c3")
              if direct_literal else
              bytes.fromhex("0f b6 80 ec 5a 00 00 0f b6 88 ed 5a 00 00 c3")
              if core_stage_bytes else op)
        b[raw:raw+len(body)]=body
    return bytes(b)
@unittest.skipUnless(shutil.which("objdump"),"objdump required")
class CorePhase(unittest.TestCase):
    def test_positive_derived_only(self):
        b=synthetic()
        r=audit(b,hashlib.sha1(b).hexdigest())
        self.assertEqual(set(r["windows"]),set(TARGETS))
        self.assertTrue(all(x["relative_5ae0_mentions"]==1 for x in r["windows"].values()))
        self.assertTrue(all(x["near_1c_compare_candidates"]==1 for x in r["windows"].values()))
        for k in ("core_phase_semantics_proven","scene_loaded_proven","safe_guest_loader_proven",
                  "remote_actor_proven","original_bytes_or_disassembly_exported"):
            self.assertFalse(r[k])
        self.assertNotIn("mov eax",str(r))
    def test_no_candidate_does_not_assert_scene(self):
        b=synthetic(False)
        r=audit(b,hashlib.sha1(b).hexdigest())
        self.assertTrue(all(x["relative_5ae0_mentions"]==0 for x in r["windows"].values()))
        self.assertFalse(r["scene_loaded_proven"])
    def test_direct_running_literal_assignment_and_compare_are_candidates(self):
        b=synthetic(direct_literal=True)
        r=audit(b,hashlib.sha1(b).hexdigest())
        for row in r["windows"].values():
            self.assertEqual(row["direct_write_candidates"],1)
            self.assertEqual(row["literal_1c_memory_write_candidates"],1)
            self.assertEqual(row["direct_1c_memory_compare_candidates"],1)
        self.assertFalse(r["scene_loaded_proven"])
        self.assertFalse(r["core_phase_semantics_proven"])
        self.assertNotIn("c7 80",str(r))
    def test_pc_checkpoint_core_byte_refs_are_candidate_only(self):
        b=synthetic(core_stage_bytes=True)
        r=audit(b,hashlib.sha1(b).hexdigest())
        for row in r["windows"].values():
            self.assertEqual(row["core_level_byte_5aec_candidates"],1)
            self.assertEqual(row["core_stage_byte_5aed_candidates"],1)
            self.assertEqual(row["first_core_level_relative_offsets"],[0])
            self.assertEqual(row["first_core_stage_relative_offsets"],[7])
        self.assertFalse(r["owner_core_level_stage_fields_verified"])
        self.assertFalse(r["scene_loaded_proven"])
        b=synthetic()
        r=audit(b,hashlib.sha1(b).hexdigest())
        self.assertTrue(all(row["core_level_byte_5aec_candidates"]==0 for row in r["windows"].values()))
        self.assertTrue(all(row["core_stage_byte_5aed_candidates"]==0 for row in r["windows"].values()))
    def test_mismatched_owner_is_rejected(self):
        b=synthetic()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        v=bytearray(b);v[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(v),hashlib.sha1(v).hexdigest())
if __name__=="__main__":unittest.main()
