"""Purely synthetic stride36 candidate lanes, bounds and no raw export."""
import json
from pathlib import Path
import struct
import tempfile
import unittest

from ally_vertex36_layout_probe import inspect_model, inspect_stage, audit, LEVEL_FILES
from test_ally_model_geometry_gate import fake_model


def human36_fixture():
    b=fake_model()
    # Original tests locate one V descriptor and 3 fake geometry vertices.
    v=0x40+0x70
    struct.pack_into("<I",b,v+0x14,36)
    for i in range(3):
        start=0x480+i*36
        struct.pack_into("<3f",b,start,float(i),float(i+1),0.)
        # Exactly the six arbitrary trailing lanes are under investigation.
        struct.pack_into("<f",b,start+12,0.5)
        b[start+16:start+20]=bytes([0,0,1,0])
        b[start+20:start+24]=bytes([255,0,0,0])
        struct.pack_into("<f",b,start+24,float("nan") if i==2 else 0.)
        struct.pack_into("<f",b,start+28,-0.5)
        b[start+32:start+36]=bytes([i,i+1,0,0])
    return b


class PrivateStride36StatsTest(unittest.TestCase):
    def test_lane_offsets_and_counts(self):
        b=human36_fixture()
        buffers=inspect_model(b,0x40,25)
        self.assertEqual(len(buffers),1)
        stats=buffers[0]
        self.assertEqual(stats["sampled_vertices"],3)
        lanes=stats["trailing_24byte_word_lanes"]
        self.assertEqual([x["offset"] for x in lanes],[12,16,20,24,28,32])
        self.assertEqual(lanes[0]["finite_f32"],3)
        self.assertEqual(lanes[0]["f32_positive_normalized_nonzero"],3)
        self.assertEqual(lanes[1]["all_u8_below_joint_count"],3)
        self.assertEqual(lanes[2]["byte4_sum_255_plusminus_1"],3)
        self.assertEqual(lanes[3]["finite_f32"],2)
        self.assertEqual(lanes[4]["f32_minus1_to_1"],3)
        self.assertEqual(lanes[5]["distinct_word_patterns"],3)
        self.assertEqual(lanes[0]["observations"],3)
    def test_unknown_stride_and_index_offsets_fail_closed(self):
        b=human36_fixture()
        struct.pack_into("<I",b,0xB0+0x14,28)
        with self.assertRaises(ValueError):inspect_model(b,0x40,25)
        b=human36_fixture()
        struct.pack_into("<I",b,0x100+0x24,131073)
        with self.assertRaises(ValueError):inspect_model(b,0x40,25)
        b=human36_fixture()
        struct.pack_into("<I",b,0x100+0x28,0xFFFFFFFF)
        with self.assertRaises(ValueError):inspect_model(b,0x40,25)
        b=human36_fixture()
        struct.pack_into("<H",b,0x140,0xFFFF)
        with self.assertRaises(ValueError):inspect_model(b,0x40,25)
    def test_three_pinned_stages_and_privacy_guards(self):
        with tempfile.TemporaryDirectory() as root:
            root=Path(root)
            for file in LEVEL_FILES:
                path=root/file
                path.parent.mkdir(parents=True,exist_ok=True)
                path.write_bytes(human36_fixture())
            report=audit(root)
            self.assertEqual(report["files_checked"],3)
            self.assertEqual(report["models_checked"],3)
            self.assertEqual(report["model_layout_rejections"],0)
            self.assertTrue(report["all_original_vertex_words_and_vertex_flags_redacted"])
            self.assertFalse(report["skin_weight_or_bone_index_offset_proven"])
            self.assertFalse(report["model_vertex_pose_renderer_built"])
            serialized=json.dumps(report)
            for forbidden in ('"raw_vertex_flags"','"vertex_bytes"','"raw_positions"','"matrix_bytes"'):
                self.assertNotIn(forbidden,serialized)
            (root/LEVEL_FILES[1]).unlink()
            with self.assertRaises(ValueError):audit(root)

if __name__=="__main__":
    unittest.main()
