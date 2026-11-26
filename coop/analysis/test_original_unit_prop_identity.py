"""Synthetic layouts only; exercise the two independent unit tables."""
from pathlib import Path
import struct
import tempfile
import unittest

from coop.analysis.original_unit_prop_identity import audit, read_prop_selectors
from coop.analysis.test_original_objective_descriptor_audit import level_data
from coop.analysis.test_original_unit_instance_identity import write_unit


def write_props(path, instances, props):
    write_unit(path, *instances)
    blob = bytearray(path.read_bytes())
    start = len(blob)
    struct.pack_into("<I", blob, 0x2C, start)
    struct.pack_into("<H", blob, 0x92, len(props))
    for key in props:
        row = bytearray(0x50)
        struct.pack_into("<Q", row, 0x48, key)
        blob.extend(row)
    path.write_bytes(blob)


class OriginalUnitPropAuditTest(unittest.TestCase):
    def setUp(self):
        workspace = tempfile.TemporaryDirectory()
        self.addCleanup(workspace.cleanup)
        self.root = Path(workspace.name)
        self.level = self.root / "level_00"
        self.level.mkdir()
        self.unit = self.level / "Unit_01.bin"

    def test_same_level_compares_against_each_authored_table_separately(self):
        common = 0xF00000002
        write_props(self.unit, (common, 3), (common, 3, 0))
        write_props(self.level / "Unit_02.bin", (), (common,))
        (self.level / "LevelDat.bin").write_bytes(level_data(0x100000003, 0, 6))
        other = self.root / "level_01"
        other.mkdir()
        write_props(other / self.unit.name, (6,), (6,))
        self.assertEqual(audit(self.root), {
            "schema": "BO-COOP-unit-v24-prop-identity-v1",
            "levels_with_prop_tables": 2,
            "unit_files": 3,
            "prop_descriptors": 5,
            "first_level_prop_descriptors": 4,
            "zero_prop_selectors": 1,
            "levels_with_duplicate_prop_selectors": 1,
            "shared_unit_instance_selectors": 3,
            "levels_compared_with_objectives": 1,
            "shared_objective_selectors": 1,
        })

    def test_rejects_wrong_version_offset_or_truncated_record(self):
        write_props(self.unit, (1,), (2,))
        original = self.unit.read_bytes()
        for offset, value in ((0, 25), (0x2C, 0), (0x2C, 0xFFFFFFFC)):
            blob = bytearray(original)
            struct.pack_into("<I", blob, offset, value)
            self.unit.write_bytes(blob)
            with self.assertRaises(ValueError):
                read_prop_selectors(self.unit)
        self.unit.write_bytes(original[:-1])
        with self.assertRaises(ValueError):
            read_prop_selectors(self.unit)
