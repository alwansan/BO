"""Synthetic strict RCB fixups, hierarchy, model hashes and privacy boundaries."""
import hashlib
import json
from pathlib import Path
import struct
import tempfile
import unittest

from ally_rcb_hierarchy_gate import (
    RCBS, hash_ascii, read_skeleton, inspect_stage, audit, LEVEL_FILES
)
from test_ally_model_geometry_gate import fake_model

def put32(b,at,x):struct.pack_into("<I",b,at,x)
def synthetic_stage():
    out=bytearray(0x2800)
    put32(out,0,0x0a)
    put32(out,0x0c,0x400)
    db=0x400
    put32(out,db+0x18,9)
    put32(out,db+0x20,0xc00)    # MODELS at 0x1000
    put32(out,db+0x34,0x1400)   # RCBS at 0x1800
    out[0x1000:0x1800]=fake_model() # MODEL root 0x1040
    dic=0x1800
    struct.pack_into("<QII",out,dic,RCBS,1,0x10)
    struct.pack_into("<QII",out,dic+0x10,0x1234567800000000,0x80,0)
    base=dic+0x80
    put32(out,base,0x600)
    put32(out,base+4,21)
    put32(out,base+8,0xAABBCCDD)
    put32(out,base+0x18,1)
    put32(out,base+0x1c,0x80)
    root=base+0x80
    put32(out,root,3)
    put32(out,root+4,0x100)
    put32(out,root+8,0x140)
    put32(out,root+12,0x240)
    for i,parent in enumerate([-1,0,1]):
        struct.pack_into("<iI",out,base+0x100+i*8,parent,0)
        for diagonal in (0,5,10,15):
            struct.pack_into("<f",out,base+0x140+i*64+4*diagonal,1.0)
    names=[b"head",b"spine",b"foot"]
    pairs=sorted((hash_ascii(n),i) for i,n in enumerate(names))
    for i,(key,index) in enumerate(pairs):
        struct.pack_into("<IIIII",out,base+0x240+i*20,key,0,0,0,index)
    for i,offset in enumerate([0x1c,0x84,0x88,0x8c,0]):
        put32(out,base+0x600+i*4,offset)
    return out

class TestRcbStrict(unittest.TestCase):
    def test_hash_ascii_case_insensitive_and_invalid(self):
        self.assertEqual(hash_ascii(b"HeAd"),hash_ascii(b"head"))
        self.assertNotEqual(hash_ascii(b"head"),hash_ascii(b"spine"))
        with self.assertRaises(ValueError):hash_ascii(b"\xff")
        with self.assertRaises(ValueError):hash_ascii(b"x\0")
    def test_fixture_hierarchy_matching_and_privacy(self):
        data=synthetic_stage()
        skeleton=read_skeleton(data,0x1880,len(data))
        self.assertEqual(skeleton["joints"],3)
        self.assertEqual(skeleton["roots"],1)
        self.assertEqual(skeleton["max_parent_depth"],3)
        self.assertEqual(skeleton["fixups"],4)
        report=inspect_stage(data,"levels/level_00/stg_0001/StLevel.bin")
        self.assertEqual(report["rcb_dictionary_entries"],1)
        self.assertEqual(report["rcb_hierarchy_validated"],1)
        self.assertEqual(report["rcb_invalid"],0)
        self.assertEqual(report["full_body_model_records_scanned"],1)
        self.assertEqual(report["pairs"][0]["best_matching_joint_name_hashes"],3)
        self.assertFalse(report["pairs"][0]["strong_same_file_name_overlap_candidate"])
        self.assertFalse(report["pairs"][0]["skin_weights_and_animation_binding_proven"])
        safe=json.dumps(report)
        for private in ("head","spine","foot","aabbccdd","12345678","0x1880"):
            self.assertNotIn(private,safe.lower())
    def test_corruption_and_cycles_rejected(self):
        base=0x1880
        def fails(change):
            b=synthetic_stage()
            change(b)
            with self.assertRaises(ValueError):read_skeleton(b,base,len(b))
        fails(lambda b:put32(b,base+4,22)) # wrong revision
        fails(lambda b:put32(b,base+0x600+4,0x1c)) # duplicate fixup
        fails(lambda b:put32(b,base+0x600+4,0x700)) # outside table
        fails(lambda b:put32(b,base+0x1c,0x601)) # pointer outside table
        fails(lambda b:put32(b,base+0x80+0x4,0x700)) # parent location outside
        fails(lambda b:struct.pack_into("<i",b,base+0x100+1*8,2)) # cycle 1->2->1
        fails(lambda b:struct.pack_into("<i",b,base+0x100+1*8,1)) # self-cycle
        fails(lambda b:put32(b,base+0x240+20+16,struct.unpack_from("<I",b,base+0x240+16)[0])) # force duplicate index from record 0
        fails(lambda b:struct.pack_into("<f",b,base+0x140,float("inf")))
    def test_malformed_rcb_remains_reported_as_invalid_not_valid(self):
        d=synthetic_stage()
        put32(d,0x1880+4,19)
        report=inspect_stage(d,"synthetic")
        self.assertEqual(report["rcb_hierarchy_validated"],0)
        self.assertEqual(report["rcb_invalid"],1)
        self.assertEqual(report["pairs"][0]["best_matching_joint_name_hashes"],0)
    def test_exact_owner_stage_files_bounded_read_only(self):
        with tempfile.TemporaryDirectory() as t:
            root=Path(t)
            for name in LEVEL_FILES:
                path=root/name;path.parent.mkdir(parents=True,exist_ok=True)
                path.write_bytes(synthetic_stage())
            result=audit(root)
            self.assertEqual(result["files_checked"],3)
            self.assertEqual(result["rcb_hierarchies_validated"],3)
            self.assertFalse(result["skin_vertex_influences_decoded"])
            self.assertFalse(result["renderable_remote_human_body_verified"])
            self.assertFalse(result["raw_assets_bones_hashes_matrices_or_vertices_exported"])
            (root/LEVEL_FILES[0]).unlink()
            with self.assertRaises(ValueError):audit(root)

if __name__=="__main__":
    unittest.main()
