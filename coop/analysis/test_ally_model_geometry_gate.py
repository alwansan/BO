"""Synthetic-only META+MODEL geometry bounds checks, no game asset bytes."""
import json
from pathlib import Path
import struct
import tempfile
import unittest
from model_inventory import MAGICS, MODEL
from ally_model_geometry_gate import bounded_geometry,analyze_file,audit,LEVEL_FILES


def fake_model():
    b=bytearray(0x800)
    key=0x53B1113A7CC40000  # Known safe base40 metadata key BC1_MAT.
    struct.pack_into("<QII",b,0,MAGICS[0],1,0x20)
    struct.pack_into("<QII",b,0x20,key,0x40,0)
    base=0x40
    struct.pack_into("<QI",b,base+0x10,MODEL,35)
    for off,value in [
        (0x1C,0x70),(0x24,1),(0x54,0x180),(0x58,0x280),
        (0x5C,3),(0x60,0x290)
    ]:struct.pack_into("<I",b,base+off,value)
    for i,name in enumerate([b"head\0",b"spine\0",b"foot\0"]):
        struct.pack_into("<I",b,base+0x290+i*4,0x300+i*16)
        b[base+0x300+i*16:base+0x300+i*16+len(name)]=name
    v=base+0x70
    struct.pack_into("<I",b,v+0x04,0x3D0) # V relative vertex data at 0x480
    struct.pack_into("<I",b,v+0x0C,0x50)  # V relative descriptor at 0x100
    struct.pack_into("<I",b,v+0x10,1)
    struct.pack_into("<I",b,v+0x14,24)
    d=v+0x50
    struct.pack_into("<I",b,d+0x20,6)
    struct.pack_into("<I",b,d+0x24,3)
    struct.pack_into("<I",b,d+0x28,0x40)
    struct.pack_into("<3H",b,d+0x40,0,1,2)
    for i,xyz in enumerate([(0.,0.,0.),(1.,0.,0.),(0.,1.,0.)]):
        struct.pack_into("<3f",b,0x480+24*i,*xyz)
    return b


class GeometryTests(unittest.TestCase):
    def test_triangle_and_candidate_context(self):
        buf=fake_model()
        geometry=bounded_geometry(buf,0x40)
        self.assertEqual(geometry["buffers_checked"],1)
        self.assertEqual(geometry["draw_descriptors_checked"],1)
        self.assertEqual(geometry["index_references_checked"],3)
        self.assertEqual(geometry["nonrepeat_strip_triangle_candidates"],1)
        self.assertEqual(geometry["distinct_buffer_vertex_references"],3)
        self.assertEqual(geometry["vertex_buffer_stride_distribution"],{"24":1})
        self.assertEqual(geometry["distinct_raw_vertex_format_pattern_count"],1)
        self.assertEqual(geometry["model_texture_name_slots"],0)
        self.assertFalse(geometry["skin_attribute_layout_decoded"])
        self.assertFalse(geometry["material_texture_bindings_proven"])
        values,failures,seen=analyze_file(buf,"levels/level_00/stg_0001/StLevel.bin")
        self.assertEqual(failures,0)
        self.assertEqual(seen,1)
        self.assertEqual(len(values),1)
        self.assertTrue(values[0]["finite_referenced_positions"])
        self.assertFalse(values[0]["rig_and_skinning_proven"])
        self.assertFalse(values[0]["full_body_identity_proven"])
        self.assertNotIn("1.0, 0.0",json.dumps(values))
    def test_malformed_geometry_and_unknown_layout_fail_closed(self):
        def invalid(patch):
            b=fake_model()
            patch(b)
            with self.assertRaises(ValueError):
                bounded_geometry(b,0x40)
        invalid(lambda b:struct.pack_into("<I",b,0xB0+0x14,32)) # not known
        invalid(lambda b:struct.pack_into("<I",b,0x100+0x20,5)) # not triangles
        invalid(lambda b:struct.pack_into("<I",b,0x100+0x24,131073))
        invalid(lambda b:struct.pack_into("<I",b,0x100+0x28,0xFFFFFFFF))
        invalid(lambda b:struct.pack_into("<3H",b,0x140,0,0,0))
        invalid(lambda b:struct.pack_into("<f",b,0x480,float("nan")))
        invalid(lambda b:struct.pack_into("<I",b,0x40+0x50,65))
        invalid(lambda b:(struct.pack_into("<I",b,0x40+0x50,1),
                          struct.pack_into("<I",b,0x40+0x4C,0xFFFFFFFE)))
    def test_three_explicit_files_only_and_no_raw_assets_exported(self):
        with tempfile.TemporaryDirectory() as root:
            root=Path(root)
            for filename in LEVEL_FILES:
                p=root/filename;p.parent.mkdir(parents=True,exist_ok=True)
                p.write_bytes(fake_model())
            report=audit(root)
            self.assertEqual(report["files_read"],3)
            self.assertEqual(report["geometry_checked_candidate_records"],3)
            self.assertFalse(report["raw_assets_or_vertices_exported"])
            self.assertFalse(report["actual_black_actor_spawned"])
            self.assertNotIn("head\\0",json.dumps(report))
            (root/LEVEL_FILES[0]).unlink()
            with self.assertRaises(ValueError):audit(root)
    def test_nonmodel_or_metadata_only_reports_no_false_geometry(self):
        b=fake_model();struct.pack_into("<3H",b,0x140,0,0,0)
        records,failures,seen=analyze_file(b,"synthetic")
        self.assertEqual((len(records),failures,seen),(0,1,1))
        b=fake_model();struct.pack_into("<I",b,0x40+0x5C,513)
        records,failures,seen=analyze_file(b,"synthetic")
        self.assertEqual((len(records),failures,seen),(0,0,0))

if __name__=="__main__":
    unittest.main()
