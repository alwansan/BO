import hashlib,struct,unittest
from original_level_name_order import audit,KNOWN
from story_entry_static import UnsafeImage
def make(order=KNOWN):
    base=0x10000;head=0x300;va=0x40000;raw=0x400;size=0x2000
    b=bytearray(raw+size)
    b[:4]=b"XBEH"
    struct.pack_into("<I",b,0x104,base)
    struct.pack_into("<I",b,0x11C,1)
    struct.pack_into("<I",b,0x120,base+head)
    struct.pack_into("<6I",b,head,0,va,size,raw,size,0)
    ptr=[]
    for i,n in enumerate(KNOWN):
        ofs=0x400+i*0x20
        name=f"level_{n:02d}".encode()
        b[raw+ofs:raw+ofs+len(name)]=name
        ptr.append(va+ofs)
    for i,n in enumerate(order):
        struct.pack_into("<I",b,raw+0x100+i*4,ptr[KNOWN.index(n)])
    return bytes(b)
class MissionOrder(unittest.TestCase):
    def test_possible_absolute_pointer_permutation_only(self):
        owner=make()
        report=audit(owner,hashlib.sha1(owner).hexdigest())
        self.assertEqual(report["bounded_possible_orderings"],[list(KNOWN)])
        self.assertFalse(report["level_index_and_original_menu_mission_mapping_proven"])
        self.assertFalse(report["real_host_scene_loader_proven"])
        self.assertFalse(report["raw_XBE_strings_and_bytes_exported"])
        self.assertNotIn("level_00",str(report["bounded_possible_orderings"]))
    def test_image_and_mapping_fail_closed(self):
        owner=make()
        with self.assertRaises(UnsafeImage):audit(owner,"0"*40)
        dirty=bytearray(owner);dirty[0]=0
        with self.assertRaises(UnsafeImage):audit(bytes(dirty),hashlib.sha1(dirty).hexdigest())
        dirty=bytearray(owner);struct.pack_into("<I",dirty,0x11C,99)
        with self.assertRaises(UnsafeImage):audit(bytes(dirty),hashlib.sha1(dirty).hexdigest())
        dirty=bytearray(owner);struct.pack_into("<I",dirty,0x300+12,len(dirty))
        with self.assertRaises(UnsafeImage):audit(bytes(dirty),hashlib.sha1(dirty).hexdigest())
if __name__=="__main__":unittest.main()
