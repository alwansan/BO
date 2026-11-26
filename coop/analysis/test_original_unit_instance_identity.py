"""Synthetic Unit-v24 fixtures; owner game files are never test inputs."""
from pathlib import Path
import struct
import tempfile
import unittest

from coop.analysis.original_unit_instance_identity import audit


def write_unit(path, *selectors):
    table_offset = 0xA0
    data = bytearray(table_offset + len(selectors) * 0x30)
    struct.pack_into("<I", data, 0, 24)
    struct.pack_into("<I", data, 0x1C, table_offset)
    struct.pack_into("<H", data, 0x90, len(selectors))
    for index, selector in enumerate(selectors):
        struct.pack_into("<Q", data, table_offset + index * 0x30 + 0x20, selector)
    path.write_bytes(data)


class OriginalUnitIdentityTest(unittest.TestCase):
    def setUp(self):
        workspace = tempfile.TemporaryDirectory()
        self.addCleanup(workspace.cleanup)
        self.root = Path(workspace.name)
        self.level = self.root / "level_01"
        self.level.mkdir()

    def test_reports_within_unit_and_cross_unit_collisions_without_selectors(self):
        write_unit(self.level / "Unit_01.bin", 0xF000000010, 0xF000000010, 0)
        other_level = self.root / "level_02"
        other_level.mkdir()
        write_unit(other_level / "Unit_01.bin", 0xF000000020)
        write_unit(other_level / "Unit_02.bin", 0xF000000020)
        self.assertEqual(audit(self.root), {
            "schema": "BO-COOP-unit-v24-selector-collisions-v1",
            "levels_with_unit_files": 2,
            "unit_files": 3,
            "instance_records": 5,
            "zero_selector_records": 1,
            "units_with_duplicate_selectors": 1,
            "excess_records_sharing_selector_in_same_unit": 1,
            "levels_with_duplicate_selectors": 2,
        })

    def test_uses_both_words_in_the_selector(self):
        write_unit(self.level / "Unit_01.bin", 4, (1 << 32) | 4)
        self.assertEqual(audit(self.root)["units_with_duplicate_selectors"], 0)

    def test_rejects_truncated_record(self):
        path = self.level / "Unit_01.bin"
        write_unit(path, 1)
        path.write_bytes(path.read_bytes()[:-1])
        with self.assertRaises(ValueError):
            audit(self.root)

    def test_rejects_wrong_version(self):
        path = self.level / "Unit_01.bin"
        write_unit(path, 1)
        data = bytearray(path.read_bytes())
        data[0] = 23
        path.write_bytes(data)
        with self.assertRaises(ValueError):
            audit(self.root)
