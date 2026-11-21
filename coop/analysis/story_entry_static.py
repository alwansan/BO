#!/usr/bin/env python3
"""Bounded, read-only XBE mission-entry candidate inspection.

Only derived counts are returned. Never print, upload or persist original
game bytes, disassembly, owner ISO, save contents or guest memory.
A recognized executable address is NOT a verified mission or scene callback.
"""
import argparse
import hashlib
import json
import pathlib
import re
import shutil
import struct
import subprocess
import sys
import tempfile

OWNER_SHA1 = "c5bfed25449c8212d7a976f15246e87ead57713f"
TARGETS = {
    "level_selection_ticks": 0x0011B890,
    "difficulty_selection_ticks": 0x00124F30,
    "mission_launch_candidate": 0x00124CF0,
}
MAX_FILE = 64 * 1024 * 1024
WINDOW = 160


class UnsafeImage(ValueError):
    """Reject missing, ambiguous or unexpected executable mapping."""


def word(data, offset):
    if offset < 0 or offset + 4 > len(data):
        raise UnsafeImage("XBE header out of bounds")
    return struct.unpack_from("<I", data, offset)[0]


def executable_sections(data):
    if len(data) < 0x160 or data[:4] != b"XBEH":
        raise UnsafeImage("Invalid XBE magic or header")
    base = word(data, 0x104)
    count = word(data, 0x11C)
    table_address = word(data, 0x120)
    if base < 0x10000 or base > 0xFFFF0000 or not 1 <= count <= 96:
        raise UnsafeImage("Unexpected image base or section count")
    table = table_address - base
    if table < 0x160 or table + count * 56 > len(data):
        raise UnsafeImage("Invalid section table")
    sections = []
    for i in range(count):
        flags, va, virtual_bytes, raw, raw_bytes, _ = struct.unpack_from(
            "<6I", data, table + i * 56
        )
        if not flags & 4:
            continue
        if not virtual_bytes or not raw_bytes or raw_bytes > MAX_FILE:
            raise UnsafeImage("Invalid executable section length")
        if raw > len(data) or raw_bytes > len(data) - raw:
            raise UnsafeImage("Executable section exceeds file")
        if va < base or va + min(raw_bytes, virtual_bytes) > 0x100000000:
            raise UnsafeImage("Invalid executable virtual address")
        sections.append((va, min(raw_bytes, virtual_bytes), raw))
    if not sections:
        raise UnsafeImage("No executable section found")
    return sections


def window_at(data, sections, address):
    candidates = [
        raw + address - va
        for va, n, raw in sections
        if va <= address and address + WINDOW <= va + n
    ]
    if len(candidates) != 1:
        raise UnsafeImage("Candidate address missing or ambiguously mapped")
    start = candidates[0]
    return bytes(data[start:start + WINDOW])


def instruction_counts(code, virtual_address):
    """Run system binutils on a temporary bounded window only."""
    if len(code) != WINDOW or not shutil.which("objdump"):
        raise UnsafeImage("Missing bounded code or objdump decoder")
    with tempfile.TemporaryDirectory(prefix="bo-static-private-") as folder:
        location = pathlib.Path(folder) / "private-window.bin"
        location.write_bytes(code)
        command = [
            "objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel",
            "--adjust-vma=" + hex(virtual_address), str(location)
        ]
        result = subprocess.run(command, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, text=True, timeout=15)
        if result.returncode:
            raise UnsafeImage("Could not decode private function window")
    mnemonics = []
    for line in result.stdout.splitlines():
        match = re.match(
            r"^\s*[0-9a-fA-F]+:\s+(?:[0-9a-fA-F]{2}\s+)+([a-zA-Z][a-zA-Z0-9.]*)",
            line
        )
        if match:
            mnemonics.append(match.group(1).lower())
    if not mnemonics:
        raise UnsafeImage("No decodable x86 instruction")
    return {
        "instructions_in_fixed_window": len(mnemonics),
        "call_instructions": sum(op.startswith("call") for op in mnemonics),
        "conditional_branches": sum(op.startswith("j") and not op.startswith("jmp") for op in mnemonics),
        "unconditional_jumps": sum(op.startswith("jmp") for op in mnemonics),
        "return_instructions": sum(op.startswith("ret") for op in mnemonics),
        "interrupt_instructions": sum(op.startswith("int") for op in mnemonics),
    }


def audit_bytes(data, expected_sha1=OWNER_SHA1):
    if not isinstance(data, (bytes, bytearray)) or not 0 < len(data) <= MAX_FILE:
        raise UnsafeImage("Unexpected XBE image size")
    if not isinstance(expected_sha1, str) or not re.fullmatch(r"[0-9a-f]{40}", expected_sha1):
        raise UnsafeImage("Invalid expected image hash")
    if hashlib.sha1(data).hexdigest() != expected_sha1:
        raise UnsafeImage("Owner image SHA-1 mismatch")
    sections = executable_sections(data)
    observations = {}
    for name, addr in TARGETS.items():
        observations[name] = {
            "candidate_address": "%08X" % addr,
            "executable_window_mapped": True,
            **instruction_counts(window_at(data, sections, addr), addr)
        }
    return {
        "schema": "BO-COOP-story-entry-static-v1",
        "owner_pinned_sha1_matched": True,
        "fixed_window_bytes_per_candidate": WINDOW,
        "executable_section_count": len(sections),
        "candidate_windows": observations,
        "source_comment_semantics_proven": False,
        "mission_choice_reliably_extracted": False,
        "actual_scene_loaded_callback_found": False,
        "game_thread_identity_proven": False,
        "multiplayer_gameplay_proven": False,
        "caveat": (
            "Bounded disassembly counts may include adjacent functions or data. "
            "They do not establish entry ABI, mission ID, branch reachability, "
            "scene loader completion or safe hook addresses."
        ),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--owner-xbe", required=True, type=pathlib.Path)
    parser.add_argument("--report", required=True, type=pathlib.Path)
    options = parser.parse_args()
    if options.owner_xbe.stat().st_size > MAX_FILE:
        raise UnsafeImage("Oversized owner input")
    report = audit_bytes(options.owner_xbe.read_bytes())
    options.report.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n",
                              encoding="utf-8")
    print("BO_STORY_XBE_STATIC_PASS: three pinned executable candidates mapped; "
          "disassembly counts only, NO engine hook or gameplay claimed")


if __name__ == "__main__":
    try:
        main()
    except (OSError, UnsafeImage, subprocess.TimeoutExpired) as exc:
        print("BO_STORY_XBE_STATIC_BLOCKED: " + type(exc).__name__, file=sys.stderr)
        sys.exit(1)
