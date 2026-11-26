"""Audit authored destruction objective identities; publish only aggregate counts."""

import argparse
from collections import Counter
import json
from pathlib import Path
import struct
import sys

from .original_unit_instance_identity import LEVEL_NAME, UNIT_NAME, read_selectors


def read_descriptors(path):
    if path.is_symlink() or not path.is_file():
        raise ValueError("Invalid LevelDat file")
    with path.open("rb") as source:
        header = source.read(0x3A0)
        if len(header) != 0x3A0:
            raise ValueError("Truncated LevelDat header")
        version, count, relative = struct.unpack_from("<III", header)
        if version != 17 or count > 4096:
            raise ValueError("Unrecognized LevelDat layout")
        if not count and not relative:
            return Counter()
        # c5: 000B70E0 rebases file+8 relative to file+4. 001C2420
        # reads that pointer and the count at file+4, with a 0x18 stride.
        start = 4 + relative
        source.seek(0, 2)
        if start < len(header) or start % 4 or start + count * 0x18 > source.tell():
            raise ValueError("Invalid objective descriptor table")
        source.seek(start)
        table = source.read(count * 0x18)
        if len(table) != count * 0x18:
            raise ValueError("Truncated objective descriptor table")
        return Counter(
            struct.unpack_from("<Q", table, offset)[0]
            for offset in range(0, len(table), 0x18)
        )


def audit(levels_root):
    if levels_root.is_symlink() or not levels_root.is_dir():
        raise ValueError("Expected extracted levels directory")
    report = {
        "schema": "BO-COOP-leveldat-v17-objective-identity-v1",
        "levels_with_descriptors": 0,
        "objective_descriptors": 0,
        "zero_descriptor_ids": 0,
        "levels_with_duplicate_objective_ids": 0,
        "objective_ids_reused_across_levels": 0,
        "levels_compared_with_unit_selectors": 0,
        "unit_files_compared": 0,
        "unit_records_compared": 0,
        "objective_ids_matching_same_level_unit_selectors": 0,
    }
    levels_per_id = Counter()
    levels_seen = set()
    for level in levels_root.iterdir():
        level_name = level.name.lower()
        if not LEVEL_NAME.fullmatch(level_name):
            continue
        if level.is_symlink() or not level.is_dir() or level_name in levels_seen:
            raise ValueError("Invalid or ambiguous level directory")
        levels_seen.add(level_name)
        paths = [path for path in level.iterdir() if path.name.lower() == "leveldat.bin"]
        if not paths:
            continue
        if len(paths) != 1:
            raise ValueError("Ambiguous LevelDat file")
        descriptors = read_descriptors(paths[0])
        if not descriptors:
            continue
        report["levels_with_descriptors"] += 1
        report["objective_descriptors"] += sum(descriptors.values())
        report["zero_descriptor_ids"] += descriptors[0]
        report["levels_with_duplicate_objective_ids"] += any(n > 1 for n in descriptors.values())
        levels_per_id.update(descriptors.keys())
        selectors = Counter()
        units_seen = set()
        for path in level.iterdir():
            unit_name = path.name.lower()
            if not UNIT_NAME.fullmatch(unit_name):
                continue
            if unit_name in units_seen:
                raise ValueError("Ambiguous unit file")
            units_seen.add(unit_name)
            selectors.update(read_selectors(path))
        if units_seen:
            report["levels_compared_with_unit_selectors"] += 1
            report["unit_files_compared"] += len(units_seen)
            report["unit_records_compared"] += sum(selectors.values())
            report["objective_ids_matching_same_level_unit_selectors"] += len(
                descriptors.keys() & selectors.keys()
            )
    if not report["objective_descriptors"]:
        raise ValueError("No LevelDat objective descriptors")
    report["objective_ids_reused_across_levels"] = sum(n > 1 for n in levels_per_id.values())
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--levels-root", required=True, type=Path)
    args = parser.parse_args()
    try:
        print(json.dumps(audit(args.levels_root), indent=2, sort_keys=True))
    except (OSError, ValueError):
        print("BO_OBJECTIVE_DESCRIPTOR_AUDIT_REJECTED", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
