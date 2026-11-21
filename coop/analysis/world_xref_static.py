#!/usr/bin/env python3
"""Read-only owner XBE absolute world-pointer cross-reference *candidates*.

This does NOT prove a scene loader, world lifetime, safe hook or story epoch.
Never print/write original code, disassembly, addresses of user devices,
game assets, or raw ISO. Only tiny derived metadata may leave the CI runner.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

from story_entry_static import MAX_FILE, OWNER_SHA1, UnsafeImage, executable_sections

WORLD_ADDRESS = 0x2D45D4  # pinned source src/game/layout.h kWorld
MAX_EXECUTABLE_BYTES = 8 * 1024 * 1024
MAX_WRITER_ADDRESSES = 32
MAX_XREFS = 10000
OP_LINE = re.compile(r"^\s*([0-9a-fA-F]+):\s+([A-Za-z][\w.]*)\s*(.*)$")
WORLD_OPERAND = re.compile(r"(?:\[\s*0x2d45d4\s*\]|\bds:0x2d45d4\b(?!\[))", re.I)


def classify_xref(mnemonic, operands):
    """Only an explicitly referenced absolute memory operand can qualify."""
    match = WORLD_OPERAND.search(operands)
    if not match:
        return None
    first = operands.split(",", 1)[0]
    if WORLD_OPERAND.search(first):
        if mnemonic in ("mov", "movnti", "movd", "movq", "movss", "movsd", "stos"):
            return "write_candidate"
        if mnemonic in ("cmp", "test", "push", "lea", "prefetchnta"):
            return "read_or_address"
        return "read_modify_write_candidate"
    return "read_or_address"


def analyze_bytes(data: bytes, expected_sha1: str = OWNER_SHA1) -> dict:
    if not isinstance(data, (bytes, bytearray)) or not 0 < len(data) <= MAX_FILE:
        raise UnsafeImage("Unexpected XBE image size")
    if not re.fullmatch(r"[0-9a-f]{40}", expected_sha1 or ""):
        raise UnsafeImage("Invalid pinned image SHA1")
    if hashlib.sha1(data).hexdigest() != expected_sha1:
        raise UnsafeImage("Owner image mismatch")
    if not shutil.which("objdump"):
        raise UnsafeImage("objdump unavailable")
    sections = executable_sections(data)
    total = sum(n for _, n, _ in sections)
    if total > MAX_EXECUTABLE_BYTES:
        raise UnsafeImage("Executable section budget exceeded")
    counters = {
        "read_or_address": 0, "write_candidate": 0,
        "read_modify_write_candidate": 0,
    }
    writers = []
    with tempfile.TemporaryDirectory(prefix="bo-owner-xref-private-") as tmp:
        for index, (vaddr, length, offset) in enumerate(sections):
            p = Path(tmp) / ("section-" + str(index) + ".bin")
            p.write_bytes(data[offset:offset + length])
            cmd = [
                "objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel",
                "--no-show-raw-insn", "--adjust-vma=" + hex(vaddr), str(p)
            ]
            try:
                process = subprocess.run(cmd, capture_output=True, text=True,
                                         timeout=55, check=False)
            except subprocess.TimeoutExpired as error:
                raise UnsafeImage("Private disassembler exceeded budget") from error
            if process.returncode:
                raise UnsafeImage("Private disassembler failed")
            for line in process.stdout.splitlines():
                match = OP_LINE.match(line)
                if not match:
                    continue
                insn_addr = int(match.group(1), 16)
                if not vaddr <= insn_addr < vaddr + length:
                    continue
                op = match.group(2).lower()
                kind = classify_xref(op, match.group(3))
                if kind is None:
                    continue
                counters[kind] += 1
                if sum(counters.values()) > MAX_XREFS:
                    raise UnsafeImage("Excessive world-pointer matches")
                if kind != "read_or_address" and len(writers) < MAX_WRITER_ADDRESSES:
                    writers.append("%08X" % insn_addr)
    return {
        "schema": "BO-COOP-world-xrefs-heuristic-v1",
        "source": "owner-pinned-XBE-executable-sections",
        "owner_sha1_matched": True,
        "owner_binary_or_disassembly_exported": False,
        "world_global_address": "%08X" % WORLD_ADDRESS,
        "executable_section_count": len(sections),
        "bytes_privately_disassembled": total,
        "instruction_xref_candidates": sum(counters.values()),
        "read_or_address_candidates": counters["read_or_address"],
        "write_candidates": counters["write_candidate"],
        "read_modify_write_candidates": counters["read_modify_write_candidate"],
        "bounded_possible_writer_addresses": writers,
        "true_scene_loader_proven": False,
        "mission_identity_proven": False,
        "world_epoch_proven": False,
        "safe_hook_proven": False,
        "no_gameplay_performed": True,
        "limitation": (
            "Linear disassembly of executable sections may cross embedded data, "
            "thunks, misaligned instructions or unreachable code. Xrefs and "
            "writer addresses are candidates, not runtime traces or proof of "
            "a BLACK stage-loaded engine callback."
        )
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--owner-xbe", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    if args.owner_xbe.stat().st_size > MAX_FILE:
        raise UnsafeImage("Oversized XBE")
    result = analyze_bytes(args.owner_xbe.read_bytes())
    args.report.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n",
                           encoding="utf8")
    print("BO_WORLD_XREF_STATIC_PASS: derived candidates only; NO engine scene proof")


if __name__ == "__main__":
    try:
        main()
    except (OSError, UnsafeImage) as error:
        print("BO_WORLD_XREF_STATIC_BLOCKED: " + type(error).__name__, file=sys.stderr)
        sys.exit(1)
