import hashlib,json,shutil,struct,unittest
from original_guest_launch_cfg import audit,ENTRIES,WINDOW
from story_entry_static import UnsafeImage

def fake(decoy=False):
    lo=min(ENTRIES.values())-256
    hi=max(ENTRIES.values())+WINDOW+256
    n=hi-lo
    b=bytearray(0x400+n);b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11c,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,lo,n,0x400,n,0)
    b[0x400:]=b"\x90"*n
    def pos(at):return 0x400+at-lo
    launch=ENTRIES["launch"];callee=ENTRIES["callee"]
    b[pos(launch):pos(launch)+6]=(
        b"\xc3\x90\x90\x90\x90\x90" if decoy
        else b"\x83\xf8\x00\x74\x01\xc3")
    src=launch+6
    b[pos(src)]=0xe8
    struct.pack_into("<i",b,pos(src)+1,callee-(src+5))
    b[pos(src)+5]=0xc3
    at=callee
    b[pos(at):pos(at)+3]=b"\x55\x89\xe5"
    at+=3
    for child in (ENTRIES["child_a"],ENTRIES["child_b"]):
        b[pos(at)]=0xe8
        struct.pack_into("<i",b,pos(at)+1,child-(at+5))
        at+=5
    b[pos(at):pos(at)+2]=b"\xc9\xc3"
    for key in ("child_a","child_b"):
        at=ENTRIES[key]
        b[pos(at):pos(at)+5]=b"\x55\x89\xe5\xc9\xc3"
    return bytes(b)

@unittest.skipUnless(shutil.which("objdump"),"objdump needed")
class EntryRooted(unittest.TestCase):
    def test_branch_reaches_call_after_linear_ret(self):
        b=fake();result=audit(b,hashlib.sha1(b).hexdigest())
        m=result["control_flow_candidate_graphs"]
        self.assertTrue(m["launch"]["expected_direct_calls_reachable_candidates"]["0011C3C0"])
        self.assertTrue(all(m["callee"]["expected_direct_calls_reachable_candidates"].values()))
        self.assertEqual(m["launch"]["direct_return_candidates"],2)
        self.assertEqual(m["launch"]["direct_conditional_branch_candidates"],1)
        self.assertFalse(result["real_engine_callable_abi_proven"])
        self.assertFalse(result["guest_mission_auto_start_implemented"])
        self.assertFalse(result["private_code_or_disassembly_exported"])
        for private in ("XBEH","call 0x","cmp eax","owner-biso"):
            self.assertNotIn(private,json.dumps(result))
    def test_unreachable_decoy_call_after_return(self):
        b=fake(True);result=audit(b,hashlib.sha1(b).hexdigest())
        m=result["control_flow_candidate_graphs"]["launch"]
        self.assertFalse(m["expected_direct_calls_reachable_candidates"]["0011C3C0"])
        self.assertEqual(m["direct_return_candidates"],1)
    def test_wrong_image_identity(self):
        b=fake()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        v=bytearray(b);v[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(v),hashlib.sha1(v).hexdigest())
        v=bytearray(b);struct.pack_into("<I",v,0x200,0)
        with self.assertRaises(UnsafeImage):audit(bytes(v),hashlib.sha1(v).hexdigest())
if __name__=="__main__":unittest.main()
