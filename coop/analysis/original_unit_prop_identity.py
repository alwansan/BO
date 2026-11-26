"""Count identities in the authored unit-v24 prop table; output aggregates only."""

import argparse
from collections import Counter
import json
from pathlib import Path
import struct
import sys

from .original_objective_descriptor_audit import read_descriptors
from .original_unit_instance_identity import LEVEL_NAME, UNIT_NAME, read_selectors


PROP_HEADER_BYTES = 0x94
PROP_DESCRIPTOR_BYTES = 0x50


def read_prop_selectors(path):
    if path.is_symlink() or not path.is_file():
        raise ValueError("Invalid unit file")
    with path.open("rb") as source:
        header = source.read(PROP_HEADER_BYTES)
        if len(header) != PROP_HEADER_BYTES or struct.unpack_from("<I", header)[0] != 24:
            raise ValueError("Invalid unit-v24 prop header")
        # 001D3290 relocates unit file+0x2C against the file base;
        # 001D3610 iterates +0x92 records and passes each to one of two constructors.
        start = struct.unpack_from("<I", header, 0x2C)[0]
        count = struct.unpack_from("<H", header, 0x92)[0]
        source.seek(0, 2)
        if (count and (start < PROP_HEADER_BYTES or start % 4) or
                start + count * PROP_DESCRIPTOR_BYTES > source.tell()):
            raise ValueError("Invalid unit-v24 prop table")
        source.seek(start)
        table = source.read(count * PROP_DESCRIPTOR_BYTES)
        if len(table) != count * PROP_DESCRIPTOR_BYTES:
            raise ValueError("Truncated unit-v24 prop table")
        return Counter(
            struct.unpack_from("<Q", table, row + 0x48)[0]
            for row in range(0, len(table), PROP_DESCRIPTOR_BYTES)
        )


def audit(levels_root):
    if levels_root.is_symlink() or not levels_root.is_dir():
        raise ValueError("Expected extracted levels directory")
    counts = Counter(dict.fromkeys((
        "levels_with_prop_tables", "unit_files", "prop_descriptors",
        "first_level_prop_descriptors", "zero_prop_selectors",
        "levels_with_duplicate_prop_selectors", "shared_unit_instance_selectors",
        "levels_compared_with_objectives", "shared_objective_selectors",
    ), 0))
    levels_seen = set()
    for level in levels_root.iterdir():
        level_name = level.name.lower()
        if not LEVEL_NAME.fullmatch(level_name):
            continue
        if level.is_symlink() or not level.is_dir() or level_name in levels_seen:
            raise ValueError("Invalid or ambiguous level directory")
        levels_seen.add(level_name)
        props = Counter()
        instances = Counter()
        unit_names = set()
        for path in level.iterdir():
            name = path.name.lower()
            if not UNIT_NAME.fullmatch(name):
                continue
            if name in unit_names:
                raise ValueError("Ambiguous unit file")
            unit_names.add(name)
            props.update(read_prop_selectors(path))
            instances.update(read_selectors(path))
        counts["unit_files"] += len(unit_names)
        if not props:
            continue
        counts["levels_with_prop_tables"] += 1
        rows = sum(props.values())
        counts["prop_descriptors"] += rows
        if level_name == "level_00":
            counts["first_level_prop_descriptors"] = rows
        counts["zero_prop_selectors"] += props[0]
        counts["levels_with_duplicate_prop_selectors"] += rows != len(props)
        counts["shared_unit_instance_selectors"] += len(props.keys() & instances.keys())
        objective_files = [p for p in level.iterdir() if p.name.lower() == "leveldat.bin"]
        if len(objective_files) > 1:
            raise ValueError("Ambiguous LevelDat file")
        if objective_files:
            objectives = read_descriptors(objective_files[0])
            counts["levels_compared_with_objectives"] += 1
            counts["shared_objective_selectors"] += len(props.keys() & objectives.keys())
    if not counts["prop_descriptors"]:
        raise ValueError("No unit-v24 prop descriptors")
    return {"schema": "BO-COOP-unit-v24-prop-identity-v1", **counts}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--levels-root", required=True, type=Path)
    args = parser.parse_args()
    try:
        print(json.dumps(audit(args.levels_root), indent=2, sort_keys=True))
    except (OSError, ValueError):
        print("BO_UNIT_PROP_IDENTITY_AUDIT_REJECTED", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
