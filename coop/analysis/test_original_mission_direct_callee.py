import hashlib,struct,unittest,shutil
from original_mission_direct_callee import audit,LAUNCH,DIRECT_TARGET_CANDIDATE
from original_mission_choice_xrefs import WINDOW
from story_entry_static import UnsafeImage

def synthetic():
    lo=min(LAUNCH,DIRECT_TARGET_CANDIDATE)-0x100
    hi=max(LAUNCH,DIRECT_TARGET_CANDIDATE)+WINDOW+0x100
    size=hi-lo;head=0x200;raw=0x400
    b=bytearray(raw+size)
    b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,head,4,lo,size,raw,size,0)
    b[raw:]=b"\x90"*size
    p=raw+LAUNCH-lo
    b[p]=0x90
    b[p+1]=0xE8
    struct.pack_into("<i",b,p+2,DIRECT_TARGET_CANDIDATE-(LAUNCH+6))
    b[p+6]=0x90;b[p+7]=0xC3
    q=raw+DIRECT_TARGET_CANDIDATE-lo
    b[q:q+5]=bytes.fromhex("a1 9e 0d 2d 00")
    b[q+5]=0xE8
    target=DIRECT_TARGET_CANDIDATE+0x120
    struct.pack_into("<i",b,q+6,target-(DIRECT_TARGET_CANDIDATE+10))
    b[q+10]=0x90;b[q+11]=0xC3
    return bytes(b),target

@unittest.skipUnless(shutil.which("objdump"),"requires objdump")
class DirectCallee(unittest.TestCase):
    def test_exact_source_call_and_callee_candidate_private(self):
        b,expected=synthetic()
        report=audit(b,hashlib.sha1(b).hexdigest())
        self.assertEqual(report["source_launch_direct_edge_appears_before_first_linear_ret"],True)
        self.assertEqual(report["callee_direct_target_hints"],[
            {"address":"%08X"%expected,"references":1}])
        self.assertTrue(report["callee_derived_families"])
        for claim in ("original_loader_abi_proven","actual_selected_mission_index_proven",
                      "original_guest_stage_load_supported","stage_loaded_callback_proven",
                      "raw_proprietary_instructions_or_bytes_exported"):
            self.assertFalse(report[claim])
    def test_reject_changed_source_edge_and_identity(self):
        b,_=synthetic()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        v=bytearray(b);v[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(v),hashlib.sha1(v).hexdigest())
        lo=min(LAUNCH,DIRECT_TARGET_CANDIDATE)-0x100
        v=bytearray(b);v[0x400+LAUNCH-lo+1]=0x90
        with self.assertRaises(UnsafeImage):audit(bytes(v),hashlib.sha1(v).hexdigest())
if __name__=="__main__":unittest.main()
