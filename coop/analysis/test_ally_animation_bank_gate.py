"""Synthetic-only v8 bank linked to v21 RCB, encoding4 clip boundary tests."""
import json
from pathlib import Path
import struct
import tempfile
import unittest

from test_ally_rcb_hierarchy_gate import synthetic_stage
from ally_animation_bank_gate import (
    Blob, read_bank, track_end, audit, inspect_stage, LEVEL_FILES
)

def p32(b,pos,x):struct.pack_into("<I",b,pos,x)
def synthetic_bank():
    data=synthetic_stage()
    db=0x400
    p32(data,db+0x2c,0x1c00) # BNKS root at 0x2000
    dic=0x2000
    struct.pack_into("<QII",data,dic,0x555C57E557000000,1,0x10)
    struct.pack_into("<QII",data,dic+0x10,1,0x80,0)
    base=dic+0x80
    p32(data,base,0x400) # table 0x2480
    p32(data,base+4,8)
    p32(data,base+8,0x11223344)
    p32(data,base+12,0xAABBCCDD) # original RCB v21 ID
    p32(data,base+0x2c,1)
    p32(data,base+0x30,0x80)
    p32(data,base+0x80,0xA0) # relative pointer from B to clip C
    c=base+0xA0
    p32(data,c+0x14,3)
    p32(data,c+0x1c,4)
    # c+0x20 stays zero => sample inline at c+0x30.
    for i,tag in enumerate([17,9,17,9,17,9]):
        data[c+0x30+i*4]=tag
    for i,value in enumerate([0x30,0x80,0]):
        p32(data,base+0x400+i*4,value)
    return data,base,c

class BankBoundaryTests(unittest.TestCase):
    def test_synthetic_bank_tracks_and_linked_rcb(self):
        data,base,c=synthetic_bank()
        v=read_bank(data,base,len(data),{0xAABBCCDD:3})
        self.assertEqual(v["joint_count"],3)
        self.assertEqual(v["clip_count"],1)
        self.assertEqual(v["top_level_joint_tracks"],6)
        self.assertEqual(v["records_including_segments"],6)
        self.assertEqual(v["track_tag_counts"],{17:3,9:3})
        report=inspect_stage(data,"synthetic/StLevel.bin")
        self.assertEqual(report["bank_records"],1)
        self.assertEqual(report["banks_with_supported_encodings"],1)
        self.assertEqual(report["validated_clip_headers_and_tracks"],1)
        self.assertEqual(report["validated_track_records"],6)
        safe=json.dumps(report).lower()
        for bad in ("aabbccdd","11223344",'"head"','"spine"','"foot"',"identity rotation"):
            self.assertNotIn(bad,safe)
    def test_rejects_fake_clip_and_wrong_rcb_identity(self):
        def fails(change):
            b,base,c=synthetic_bank()
            change(b,base,c)
            with self.assertRaises(ValueError):
                read_bank(b,base,len(b),{0xAABBCCDD:3})
        fails(lambda b,B,C:p32(b,B+4,9))
        fails(lambda b,B,C:p32(b,B+12,0))
        fails(lambda b,B,C:p32(b,C+0x14,0))
        fails(lambda b,B,C:p32(b,C+0x1c,3))
        fails(lambda b,B,C:p32(b,B+0x80,0x800))
        fails(lambda b,B,C:p32(b,B+0x400,0x80))
        fails(lambda b,B,C:b.__setitem__(slice(C+0x30,C+0x31),b"\xff"))
        fails(lambda b,B,C:p32(b,B+0x2c,1025))
    def test_tags_segments_and_alignment(self):
        b,base,c=synthetic_bank()
        f=Blob(b,base,len(b),8,0x3c)
        segment=c+0xF0
        # One segmented track with two child records: identity and zero.
        b[segment]=1;struct.pack_into("<H",b,segment+1,2)
        b[segment+4]=17;b[segment+8]=9
        hist={}
        from collections import Counter
        hist=Counter();budget=[0]
        self.assertEqual(track_end(f,segment,hist,budget),segment+12)
        self.assertEqual(budget[0],3)
        self.assertEqual(hist[1],1)
        self.assertEqual(hist[17],1)
        self.assertEqual(hist[9],1)
        b[segment]=7;struct.pack_into("<H",b,segment+1,3)
        self.assertEqual(track_end(f,segment,Counter(),[0]),segment+40) # 28+align4(9)
        b[segment]=8
        self.assertEqual(track_end(f,segment,Counter(),[0]),segment+48) # 28+align4(18)
        b[segment]=17
        self.assertEqual(track_end(f,segment,Counter(),[0]),segment+4)
    def test_pinned_three_files_aggregation_without_raw_data(self):
        with tempfile.TemporaryDirectory() as root:
            root=Path(root)
            for name in LEVEL_FILES:
                p=root/name;p.parent.mkdir(parents=True,exist_ok=True)
                b,_,_=synthetic_bank();p.write_bytes(b)
            r=audit(root)
            self.assertEqual(r["files_read"],3)
            self.assertEqual(r["validated_clips"],3)
            self.assertEqual(r["validated_track_records"],18)
            self.assertFalse(r["bone_to_skin_weight_attributes_known"])
            self.assertFalse(r["remote_human_ally_rendering_implemented"])
            self.assertFalse(r["original_game_bytes_or_hashes_exported"])
            (root/LEVEL_FILES[0]).unlink()
            with self.assertRaises(ValueError):audit(root)

if __name__=="__main__":
    unittest.main()
