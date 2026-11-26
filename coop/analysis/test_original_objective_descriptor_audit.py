"""Synthetic LevelDat fixtures; no owner data is copied into tests."""
from pathlib import Path
import struct
import tempfile
import unittest

from coop.analysis.original_objective_descriptor_audit import audit, read_descriptors
from coop.analysis.test_original_unit_instance_identity import write_unit


def level_data(*descriptors):
    start = 0x3B0
    data = bytearray(start + len(descriptors) * 0x18)
    struct.pack_into("<III", data, 0, 17, len(descriptors), start - 4)
    for i, descriptor in enumerate(descriptors):
        struct.pack_into("<Q", data, start + i * 0x18, descriptor)
    return data


class OriginalObjectiveDescriptorAuditTest(unittest.TestCase):
    def setUp(self):
        workspace = tempfile.TemporaryDirectory()
        self.addCleanup(workspace.cleanup)
        self.root = Path(workspace.name)
        self.level = self.root / "level_00"
        self.level.mkdir()
        self.path = self.level / "LevelDat.bin"

    def test_cross_reference_requires_both_words_and_the_same_level(self):
        repeated = 0xFEDCBA9876543210
        self.path.write_bytes(level_data(repeated, repeated, 0))
        write_unit(self.level / "Unit_01.bin", repeated, repeated)
        other = self.root / "level_01"
        other.mkdir()
        (other / self.path.name).write_bytes(level_data(repeated, 7))
        write_unit(other / "Unit_02.bin", 0x100000007)
        self.assertEqual(audit(self.root), {
            "schema": "BO-COOP-leveldat-v17-objective-identity-v1",
            "levels_with_descriptors": 2,
            "objective_descriptors": 5,
            "zero_descriptor_ids": 1,
            "levels_with_duplicate_objective_ids": 1,
            "objective_ids_reused_across_levels": 1,
            "levels_compared_with_unit_selectors": 2,
            "unit_files_compared": 2,
            "unit_records_compared": 3,
            "objective_ids_matching_same_level_unit_selectors": 1,
        })

    def test_rejects_wrong_version_count_offset_and_truncated_table(self):
        for offset, value in ((0, 18), (4, 4097), (8, 0), (8, 0xFFFFFFFF), (8, 0x3AD)):
            data = level_data(1)
            struct.pack_into("<I", data, offset, value)
            self.path.write_bytes(data)
            with self.assertRaises(ValueError):
                read_descriptors(self.path)
        self.path.write_bytes(level_data(1)[:-1])
        with self.assertRaises(ValueError):
            read_descriptors(self.path)

    def test_uncompared_level_is_counted_separately(self):
        self.path.write_bytes(level_data(1))
        report = audit(self.root)
        self.assertEqual(report["levels_with_descriptors"], 1)
        self.assertEqual(report["levels_compared_with_unit_selectors"], 0)
