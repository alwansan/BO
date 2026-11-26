#!/usr/bin/env python3
"""Count collisions in the two authored words copied to a live unit object.

Read the original extracted levels locally. Print only aggregate counts:
no asset names, raw values, positions, or paths. The pair is a selector
candidate; even a unique pair would not establish runtime object lifetime.
"""

import argparse
from collections import Counter
import json
from pathlib import Path
import re
import struct
import sys

HEADER_BYTES = 0x92
RECORD_BYTES = 0x30
SELECTOR_OFFSET = 0x20
LEVEL_NAME = re.compile(r"level_[0-9]{2}", re.IGNORECASE)
UNIT_NAME = re.compile(r"unit_[0-9]{2}\.bin", re.IGNORECASE)


def read_selectors(path):
    if path.is_symlink() or not path.is_file():
        raise ValueError("Invalid unit file")
    with path.open("rb") as source:
        header = source.read(HEADER_BYTES)
        if len(header) != HEADER_BYTES or struct.unpack_from("<I", header)[0] != 24:
            raise ValueError("Invalid unit-v24 header")
        table_offset = struct.unpack_from("<I", header, 0x1C)[0]
        count = struct.unpack_from("<H", header, 0x90)[0]
        source.seek(0, 2)
        if (count and table_offset < HEADER_BYTES
                or table_offset + count * RECORD_BYTES > source.tell()):
            raise ValueError("Invalid unit-v24 instance table")
        source.seek(table_offset)
        table = source.read(count * RECORD_BYTES)
        if len(table) != count * RECORD_BYTES:
            raise ValueError("Truncated unit-v24 instance table")
        return Counter(
            struct.unpack_from("<Q", table, offset + SELECTOR_OFFSET)[0]
            for offset in range(0, len(table), RECORD_BYTES)
        )


def audit(levels_root):
    if levels_root.is_symlink() or not levels_root.is_dir():
        raise ValueError("Expected extracted levels directory")
    report = {
        "schema": "BO-COOP-unit-v24-selector-collisions-v1",
        "levels_with_unit_files": 0,
        "unit_files": 0,
        "instance_records": 0,
        "zero_selector_records": 0,
        "units_with_duplicate_selectors": 0,
        "excess_records_sharing_selector_in_same_unit": 0,
        "levels_with_duplicate_selectors": 0,
    }
    for level in levels_root.iterdir():
        if not LEVEL_NAME.fullmatch(level.name):
            continue
        if level.is_symlink() or not level.is_dir():
            raise ValueError("Invalid level directory")
        level_selectors = Counter()
        units_seen = set()
        for path in level.iterdir():
            if not UNIT_NAME.fullmatch(path.name):
                continue
            name = path.name.lower()
            if name in units_seen:
                raise ValueError("Ambiguous unit file")
            units_seen.add(name)
            selectors = read_selectors(path)
            records = sum(selectors.values())
            duplicates = records - len(selectors)
            report["unit_files"] += 1
            report["instance_records"] += records
            report["zero_selector_records"] += selectors[0]
            report["units_with_duplicate_selectors"] += bool(duplicates)
            report["excess_records_sharing_selector_in_same_unit"] += duplicates
            level_selectors.update(selectors)
        if units_seen:
            report["levels_with_unit_files"] += 1
            report["levels_with_duplicate_selectors"] += any(
                count > 1 for count in level_selectors.values()
            )
    if not report["instance_records"]:
        raise ValueError("No unit-v24 instance records")
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--levels-root", required=True, type=Path)
    args = parser.parse_args()
    try:
        print(json.dumps(audit(args.levels_root), indent=2, sort_keys=True))
    except (OSError, ValueError):
        print("BO_UNIT_IDENTITY_AUDIT_REJECTED", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
