import hashlib,json,struct,unittest,shutil
from original_mission_choice_xrefs import TARGETS,WINDOW
from original_mission_field_widths import audit,classify_memory_instruction,FIELDS
from story_entry_static import UnsafeImage

def original_style_xbe():
    lo=min(TARGETS.values())-0x100
    hi=max(TARGETS.values())+WINDOW+0x100
    n=hi-lo
    b=bytearray(0x400+n)
    b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,lo,n,0x400,n,0)
    b[0x400:]=b"\x90"*n
    for target in TARGETS.values():
        at=0x400+target-lo
        # movzx eax,BYTE PTR ds:0x2d0d60
        # mov eax,ds:0x2d0d68
        # mov ds:0x2ec598,eax
        # ret
        b[at:at+18]=bytes.fromhex(
            "0f b6 05 60 0d 2d 00 a1 68 0d 2d 00 a3 98 c5 2e 00 c3"
        )
    return bytes(b)

class Widths(unittest.TestCase):
    def test_pure_classifier(self):
        self.assertEqual(classify_memory_instruction(
           "movzx","eax,BYTE PTR ds:0x2d0d60",FIELDS["choice_candidate_A"]),
           ("read_candidate","BYTE"))
        self.assertEqual(classify_memory_instruction(
           "mov","ds:0x2ec598,eax",FIELDS["choice_dynamic_candidate"]),
           ("write_candidate","UNSPECIFIED"))
        self.assertEqual(classify_memory_instruction(
           "lea","eax,[0x2d0d60]",FIELDS["choice_candidate_A"]),
           ("address_only","UNSPECIFIED"))
        self.assertIsNone(classify_memory_instruction("mov","eax,ebx",FIELDS["choice_candidate_A"]))
    @unittest.skipUnless(shutil.which("objdump"),"Binutils required")
    def test_private_pinned_source_window_and_redaction(self):
        b=original_style_xbe()
        r=audit(b,hashlib.sha1(b).hexdigest())
        self.assertEqual(len(r["source_windows"]),3)
        for item in r["source_windows"].values():
            self.assertEqual(item["direct_memory_widths"]["choice_candidate_A"]["read_candidate_BYTE"],1)
            self.assertEqual(item["direct_memory_widths"]["launch_candidate_B"]["read_candidate_UNSPECIFIED"],1)
            self.assertEqual(item["direct_memory_widths"]["choice_dynamic_candidate"]["write_candidate_UNSPECIFIED"],1)
            self.assertFalse(item["original_menu_choice_field_proven"])
        self.assertFalse(r["selected_campaign_mission_proven"])
        self.assertFalse(r["original_guest_scene_loader_proven"])
        self.assertFalse(r["original_instructions_or_bytes_exported"])
        rendered=json.dumps(r)
        for forbidden in ("movzx","source.bin","0f b6 05","a3 98 c5"):
            self.assertNotIn(forbidden,rendered)
        with self.assertRaises(UnsafeImage):
            audit(b,"0"*40)
        altered=bytearray(b);altered[0]=0
        with self.assertRaises(UnsafeImage):
            audit(bytes(altered),hashlib.sha1(altered).hexdigest())
if __name__=="__main__":unittest.main()
