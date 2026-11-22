import hashlib,struct,shutil,json,unittest
from story_entry_static import UnsafeImage
from original_mission_choice_xrefs import audit,TARGETS,WINDOW
def fake():
    low=min(TARGETS.values())-0x100
    high=max(TARGETS.values())+WINDOW+0x100
    n=high-low
    b=bytearray(0x400+n);b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,0x10000)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,0x10200)
    struct.pack_into("<6I",b,0x200,4,low,n,0x400,n,0)
    b[0x400:]=b"\x90"*n
    for at in TARGETS.values():
        o=0x400+at-low
        b[o:o+12]=bytes.fromhex("a1 9e 0d 2d 00 a3 34 12 2e 00 c3 90")
    return bytes(b)
@unittest.skipUnless(shutil.which("objdump"),"objdump needed")
class MissionChoices(unittest.TestCase):
    def test_synthetic_stats_and_source_privacy(self):
        b=fake();report=audit(b,hashlib.sha1(b).hexdigest())
        self.assertTrue(report["owner_image_pinned_sha1_verified"])
        self.assertEqual(set(report["entries"]),set(TARGETS))
        for entry in report["entries"].values():
            self.assertGreater(entry["absolute_global_xref_count"],0)
            self.assertTrue(entry["known_original_mission_unlock_table_referenced"])
            self.assertFalse(entry["selected_mission_field_proven"])
        for word in ["XBEH","private.bin","a1 9e 0d 2d","mov eax,ds:"]:
            self.assertNotIn(word,json.dumps(report))
    def test_wrong_image_or_ambiguous_sections(self):
        b=fake()
        with self.assertRaises(UnsafeImage):audit(b,"0"*40)
        m=bytearray(b);m[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(m),hashlib.sha1(m).hexdigest())
        m=bytearray(b);struct.pack_into("<I",m,0x200,0)
        with self.assertRaises(UnsafeImage):audit(bytes(m),hashlib.sha1(m).hexdigest())
if __name__=="__main__":unittest.main()
