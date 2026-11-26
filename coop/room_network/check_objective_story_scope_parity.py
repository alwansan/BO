#!/usr/bin/env python3
# Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
"""Compare production C++ objective scope and Java Ticket value semantics.

This is an offline scalar check, not a network, TLS, Android, or engine test.
The generated harnesses share explicit cases and independent expected answers;
agreement between two incorrect implementations is not a passing result.

Example:
  python3 check_objective_story_scope_parity.py \
      --source-root /path/to/BO --output /path/to/parity-results
"""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys
import time


JAVA_REFERENCE_COMMIT = "f7551ada9e7e352f937bd24e7b5ba405b947602d"
SCOPE_HEADER = "coop/room_network/room_original_objective_story_scope.hpp"
JAVA_DIRECTORY = "coop/lobby/java/com/racas/black/coop"
JAVA_SOURCES = (
    JAVA_DIRECTORY + "/RoomWire.java",
    JAVA_DIRECTORY + "/RoomStoryWire.java",
)
INT64_MIN = -(1 << 63)
INT64_MAX = (1 << 63) - 1
INT32_MIN = -(1 << 31)
INT32_MAX = (1 << 31) - 1
UINT32_MAX = (1 << 32) - 1
CLASS_NAME = "ObjectiveStoryScopeParity"


def value(**changes: object) -> dict:
    result = {
        "session": [0] * 16,
        "token": 1,
        "rosterRevision": 1,
        "mission": 0,
        "stage": 0,
        "sceneEpoch": 1,
        "participants": 3,
    }
    if set(changes) - set(result):
        raise ValueError("Unknown scope field in case definition")
    result.update(changes)
    result["session"] = list(result["session"])
    return result


def copy_value(original: dict) -> dict:
    return {**original, "session": list(original["session"])}


def cases() -> list[dict]:
    """Expected validity is stated per row, not calculated by a copied rule."""
    result: list[dict] = []

    def validity(name: str, expected: bool, **changes: object) -> None:
        result.append({"id": "validity_" + name, "operation": "validity",
                       "left": value(**changes), "expected": expected})

    def equality(name: str, left: dict, right: dict, expected: bool) -> None:
        result.append({"id": "equality_" + name, "operation": "equality",
                       "left": copy_value(left), "right": copy_value(right),
                       "expected": expected})

    # A fixed 16-byte all-zero session is accepted by the production Ticket.
    validity("minimum_fields_zero_session", True)
    validity("session_all_ff", True, session=[255] * 16)
    validity("session_mixed_high_bits", True,
             session=[0, 128, 255, 1, 127, 254, 2, 129] * 2)

    token_rows = (
        ("minimum_signed", INT64_MIN, False), ("negative_one", -1, False),
        ("zero", 0, False), ("one", 1, True), ("two", 2, True),
        ("uint32_max", UINT32_MAX, True),
        ("above_uint32", UINT32_MAX + 2, True),
        ("maximum_minus_one", INT64_MAX - 1, True),
        ("maximum_signed", INT64_MAX, True),
    )
    revision_rows = (
        ("minimum_signed", INT64_MIN, False), ("negative_one", -1, False),
        ("zero", 0, False), ("one", 1, True),
        ("maximum_minus_one", UINT32_MAX - 1, True),
        ("maximum_wire", UINT32_MAX, True),
        ("above_wire_maximum", UINT32_MAX + 1, False),
        ("maximum_signed", INT64_MAX, False),
    )
    mission_rows = (
        ("minimum_signed", INT32_MIN, False), ("negative_one", -1, False),
        ("zero", 0, True), ("one", 1, True), ("seven", 7, True),
        ("above_maximum", 8, False), ("maximum_signed", INT32_MAX, False),
    )
    stage_rows = (
        ("minimum_signed", INT32_MIN, False), ("negative_one", -1, False),
        ("zero", 0, True), ("one", 1, True),
        ("maximum_minus_one", 65534, True), ("maximum_wire", 65535, True),
        ("above_wire_maximum", 65536, False),
        ("maximum_signed", INT32_MAX, False),
    )
    epoch_rows = (
        ("minimum_signed", INT32_MIN, False), ("negative_one", -1, False),
        ("zero", 0, False), ("one", 1, True), ("two", 2, True),
        ("maximum_minus_one", INT32_MAX - 1, True),
        ("maximum_signed", INT32_MAX, True),
    )
    participant_rows = (
        ("minimum_signed", INT32_MIN, False), ("negative_one", -1, False),
        ("zero", 0, False), ("host_only", 1, False), ("two", 2, False),
        ("mask_3", 3, True), ("mask_4", 4, False), ("mask_5", 5, True),
        ("mask_6", 6, False), ("mask_7", 7, True), ("mask_8", 8, False),
        ("mask_9", 9, True), ("mask_10", 10, False), ("mask_11", 11, True),
        ("mask_12", 12, False), ("mask_13", 13, True),
        ("mask_14", 14, False), ("mask_15", 15, True),
        ("above_maximum", 16, False), ("above_maximum_odd", 17, False),
        ("maximum_signed", INT32_MAX, False),
    )
    for field, rows in (
        ("token", token_rows), ("rosterRevision", revision_rows),
        ("mission", mission_rows), ("stage", stage_rows),
        ("sceneEpoch", epoch_rows), ("participants", participant_rows),
    ):
        for suffix, scalar, expected in rows:
            validity(field + "_" + suffix, expected, **{field: scalar})

    validity("all_upper_bounds", True, session=[255] * 16,
             token=INT64_MAX, rosterRevision=UINT32_MAX, mission=7,
             stage=65535, sceneEpoch=INT32_MAX, participants=15)

    # Equality can only be compared on values the Java constructor admits.
    # Every accepted boundary must compare equal to an independently built copy.
    for case in tuple(result):
        if case["expected"]:
            equality("copy_of_" + case["id"], case["left"], case["left"], True)

    baseline = value()
    alternatives = (
        ("token", 2), ("token", UINT32_MAX + 2), ("token", INT64_MAX),
        ("rosterRevision", 2), ("rosterRevision", UINT32_MAX),
        ("mission", 1), ("mission", 7), ("stage", 1), ("stage", 65535),
        ("sceneEpoch", 2), ("sceneEpoch", INT32_MAX),
        ("participants", 5), ("participants", 9), ("participants", 13),
        ("participants", 15),
    )
    for field, scalar in alternatives:
        changed = value(**{field: scalar})
        name = "only_" + field + "_" + str(scalar)
        equality(name, baseline, changed, False)
        equality(name + "_reverse", changed, baseline, False)
    for index in range(16):
        session = [0] * 16
        session[index] = 128 if index % 2 else 255
        changed = value(session=session)
        equality("only_session_byte_" + str(index), baseline, changed, False)
        equality("only_session_byte_" + str(index) + "_reverse", changed,
                 baseline, False)

    identifiers = [case["id"] for case in result]
    if len(set(identifiers)) != len(identifiers):
        raise ValueError("Duplicate case identifier")
    for case in result:
        for side in ("left", "right"):
            if side not in case:
                continue
            scope = case[side]
            if len(scope["session"]) != 16 or any(
                    not 0 <= byte <= 255 for byte in scope["session"]):
                raise ValueError("Session fixture is not exactly 16 bytes")
            for field in ("token", "rosterRevision"):
                if not INT64_MIN <= scope[field] <= INT64_MAX:
                    raise ValueError("Fixture exceeds signed 64-bit API storage")
            for field in ("mission", "stage", "sceneEpoch", "participants"):
                if not INT32_MIN <= scope[field] <= INT32_MAX:
                    raise ValueError("Fixture exceeds signed 32-bit API storage")
    return result


def cpp_integer(number: int, bits: int) -> str:
    if number == -(1 << (bits - 1)):
        return "std::numeric_limits<std::int" + str(bits) + "_t>::min()"
    return str(number) + ("LL" if bits == 64 else "")


def java_integer(number: int, bits: int) -> str:
    if number == -(1 << (bits - 1)):
        return ("Long" if bits == 64 else "Integer") + ".MIN_VALUE"
    return str(number) + ("L" if bits == 64 else "")


def arguments(scope: dict, language: str) -> str:
    if language == "cpp":
        session = "std::array<std::uint8_t, 16>{" + ", ".join(
            str(byte) for byte in scope["session"]) + "}"
        integer = cpp_integer
    else:
        session = "new byte[]{" + ", ".join(
            "(byte)" + str(byte) for byte in scope["session"]) + "}"
        integer = java_integer
    scalars = [integer(scope[field], bits) for field, bits in (
        ("token", 64), ("rosterRevision", 64), ("mission", 32),
        ("stage", 32), ("sceneEpoch", 32), ("participants", 32))]
    return ", ".join([session, *scalars])


def cpp_harness(test_cases: list[dict]) -> str:
    lines = [
        '// Generated offline scalar harness; production header is included below.',
        '#include "room_original_objective_story_scope.hpp"',
        '#include <array>', '#include <cstdint>', '#include <iostream>',
        '#include <limits>',
        'using Scope = blackcoop::OriginalObjectiveStoryScope;',
        'static Scope make_scope(std::array<std::uint8_t, 16> session,',
        '    std::int64_t token, std::int64_t revision, std::int32_t mission,',
        '    std::int32_t stage, std::int32_t epoch, std::int32_t participants) {',
        '    Scope s{};',
        '    s.session = session; s.token = token; s.rosterRevision = revision;',
        '    s.mission = mission; s.stage = stage; s.sceneEpoch = epoch;',
        '    s.participants = participants;',
        '    return s;',
        '}',
        'static void emit(const char* id, bool result) {',
        '    std::cout << id << "\\t" << (result ? "1" : "0") << "\\n";',
        '}',
        'int main() {',
    ]
    for case in test_cases:
        left = "make_scope(" + arguments(case["left"], "cpp") + ")"
        if case["operation"] == "validity":
            expression = "blackcoop::ValidOriginalObjectiveStoryScope(" + left + ")"
        else:
            right = "make_scope(" + arguments(case["right"], "cpp") + ")"
            expression = "(" + left + " == " + right + ")"
        lines.append("    emit(" + json.dumps(case["id"]) + ", " + expression + ");")
    lines.extend(['    return 0;', '}', ''])
    return "\n".join(lines)


def java_harness(test_cases: list[dict]) -> str:
    lines = [
        '// Generated offline scalar harness; uses the production Ticket class.',
        'package com.racas.black.coop;',
        'import java.io.IOException;',
        'public final class ' + CLASS_NAME + ' {',
        '    private ' + CLASS_NAME + '() {}',
        '    private static RoomStoryWire.Ticket ticket(byte[] session, long token,',
        '            long revision, int mission, int stage, int epoch, int participants)',
        '            throws IOException {',
        '        return new RoomStoryWire.Ticket(session, token, revision, mission,',
        '                stage, epoch, participants);',
        '    }',
        '    private static boolean valid(byte[] session, long token, long revision,',
        '            int mission, int stage, int epoch, int participants) {',
        '        try {',
        '            ticket(session, token, revision, mission, stage, epoch, participants);',
        '            return true;',
        '        } catch (IOException rejected) {',
        '            return false;',
        '        }',
        '    }',
        '    private static void emit(String id, boolean result) {',
        '        System.out.println(id + "\\t" + (result ? "1" : "0"));',
        '    }',
        '    public static void main(String[] args) throws IOException {',
    ]
    for case in test_cases:
        left = arguments(case["left"], "java")
        if case["operation"] == "validity":
            expression = "valid(" + left + ")"
        else:
            right = arguments(case["right"], "java")
            expression = "ticket(" + left + ").same(ticket(" + right + "))"
        lines.append("        emit(" + json.dumps(case["id"]) + ", " + expression + ");")
    lines.extend(['    }', '}', ''])
    return "\n".join(lines)


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def local_cpp_sources(source_root: Path, header: Path) -> dict[str, Path]:
    """Record the production header and every quoted local include it uses."""
    result: dict[str, Path] = {}

    def visit(path: Path) -> None:
        path = path.resolve()
        relative = str(path.relative_to(source_root))
        if relative in result:
            return
        if not path.is_file():
            raise FileNotFoundError("Missing local C++ include: " + str(path))
        result[relative] = path
        for included in re.findall(r'^\s*#\s*include\s*"([^\"]+)"',
                                   path.read_text(encoding="utf-8"), re.MULTILINE):
            visit(path.parent / included)

    visit(header)
    return result


def run_command(name: str, command: list[str], directory: Path,
                report: dict, timeout: int = 60) -> dict:
    started = time.monotonic()
    record = {"name": name, "command": command, "cwd": str(directory),
              "timeout_seconds": timeout, "exit_code": None,
              "stdout": "", "stderr": ""}
    try:
        completed = subprocess.run(command, cwd=directory, capture_output=True,
                                   text=True, encoding="utf-8", errors="replace",
                                   timeout=timeout, check=False)
        record.update(exit_code=completed.returncode, stdout=completed.stdout,
                      stderr=completed.stderr)
    except subprocess.TimeoutExpired as failure:
        def decoded(stream: object) -> str:
            if isinstance(stream, bytes):
                return stream.decode("utf-8", errors="replace")
            return str(stream) if stream is not None else ""
        record.update(error="command timed out", stdout=decoded(failure.stdout),
                      stderr=decoded(failure.stderr))
    except OSError as failure:
        record["error"] = str(failure)
    record["seconds"] = round(time.monotonic() - started, 3)
    report["commands"].append(record)
    return record


def parse_results(stdout: str, expected_ids: set[str]) -> dict[str, bool]:
    actual: dict[str, bool] = {}
    for line in stdout.splitlines():
        fields = line.split("\t")
        if len(fields) != 2 or fields[1] not in ("0", "1"):
            raise ValueError("Malformed harness output line: " + repr(line))
        identifier, bit = fields
        if identifier not in expected_ids or identifier in actual:
            raise ValueError("Unknown or duplicate harness case: " + identifier)
        actual[identifier] = bit == "1"
    if set(actual) != expected_ids:
        raise ValueError("Missing harness cases: " + ", ".join(sorted(expected_ids - set(actual))))
    return actual


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--java-current-runtime", action="store_true",
                        help="Explicit host-runtime scalar check when --release 8 is unavailable; does not verify Java 8 compatibility")
    args = parser.parse_args()
    source_root = args.source_root.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    test_cases = cases()
    report = {
        "schema": 1,
        "scope": "Production C++/Java scalar validity and equality only; no TLS, Android, engine, or assets",
        "started_at_utc": datetime.now(timezone.utc).isoformat(),
        "source_root": str(source_root), "output": str(output),
        "platform": platform.platform(), "python": sys.version,
        "java_reference_commit": JAVA_REFERENCE_COMMIT,
        "java_compile_target": "current runtime" if args.java_current_runtime else "--release 8",
        "java8_target_requested": not args.java_current_runtime,
        "provenance_note": "Sources are compiled from --source-root. Their recorded hashes are authoritative; this script makes no GitHub or other network request.",
        "sanitizers": {"enabled": False,
                       "reason": "Scalar parity only; the separate C++ unit test supplies ASan/UBSan coverage."},
        "coverage_notes": [
            "All seven fields participate in equality, including each of the 16 session bytes.",
            "The C++ std::array fixes session width to 16. Null and wrong-length Java sessions have no corresponding C++ value and are outside this comparison.",
            "INT64_MAX+1 for token and INT32_MAX+1 for sceneEpoch cannot be represented by either API field; no narrowing or overflowing cast is used to simulate them.",
            "Equality is compared only for constructor-valid Tickets; invalid C++ values have no constructed Java counterpart. Java same(null) likewise has no C++ reference counterpart.",
            "Zero-filled sessions and noncontiguous odd participant masks 5, 9, and 13 are expected to be accepted.",
            "Expected answers are explicit case data. Passing requires each language to match them as well as one another.",
        ],
        "cases": test_cases,
        "case_counts": {operation: sum(c["operation"] == operation for c in test_cases)
                        for operation in ("validity", "equality")},
        "commands": [], "results": [], "errors": [],
        "passed": 0, "total": len(test_cases), "success": False,
    }
    relative_sources = (SCOPE_HEADER, *JAVA_SOURCES)
    source_paths = {name: source_root / name for name in relative_sources}
    before: dict[str, str] = {}
    try:
        source_paths.update(local_cpp_sources(source_root, source_root / SCOPE_HEADER))
        for name, path in source_paths.items():
            if not path.is_file():
                raise FileNotFoundError("Missing production source: " + str(path))
            before[name] = sha256(path)
        report["source_sha256"] = before
        report["checker_sha256"] = sha256(Path(__file__).resolve())

        cpp_directory = output / "cpp"
        java_directory = output / "java"
        classes_directory = output / "classes"
        for directory in (cpp_directory, java_directory, classes_directory):
            directory.mkdir(parents=True, exist_ok=True)
        cpp_source = cpp_directory / "objective_story_scope_parity.cpp"
        java_source = java_directory / (CLASS_NAME + ".java")
        cpp_source.write_text(cpp_harness(test_cases), encoding="utf-8")
        java_source.write_text(java_harness(test_cases), encoding="utf-8")
        (output / "cases.json").write_text(json.dumps(test_cases, indent=2) + "\n",
                                          encoding="utf-8")
        report["generated_sha256"] = {
            str(path.relative_to(output)): sha256(path)
            for path in (cpp_source, java_source, output / "cases.json")
        }

        compiler = shutil.which("g++") or shutil.which("c++")
        java = shutil.which("java")
        javac = shutil.which("javac")
        if not compiler or not java:
            raise RuntimeError("A C++20 compiler and Java runtime/compiler are required")
        java_compiler = [javac] if javac else [java, "-m", "jdk.compiler/com.sun.tools.javac.Main"]
        report["java_compiler_selection"] = "javac" if javac else "java jdk.compiler module fallback"
        run_command("cpp_version", [compiler, "--version"], output, report)
        run_command("java_version", [java, "-version"], output, report)
        run_command("java_compiler_version", [*java_compiler, "-version"], output, report)

        executable = cpp_directory / "objective_story_scope_parity"
        cpp_compile = run_command("cpp_compile", [
            compiler, "-std=c++20", "-O1", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
            "-I", str((source_root / SCOPE_HEADER).parent), str(cpp_source),
            "-o", str(executable)], output, report)
        java_target = [] if args.java_current_runtime else ["--release", "8"]
        java_compile = run_command("java_compile", [
            *java_compiler, *java_target, "-Xlint:all,-options", "-Werror",
            "-encoding", "UTF-8", "-classpath", str(classes_directory),
            "-d", str(classes_directory),
            *(str(source_root / name) for name in JAVA_SOURCES), str(java_source)],
            output, report)

        observations: dict[str, dict[str, bool]] = {}
        expected_ids = {case["id"] for case in test_cases}
        for language, compiled, command in (
            ("cpp", cpp_compile, [str(executable)]),
            ("java", java_compile,
             [java, "-cp", str(classes_directory), "com.racas.black.coop." + CLASS_NAME]),
        ):
            if compiled["exit_code"] != 0:
                report["errors"].append(language + " compilation failed; its harness was not run")
                continue
            execution = run_command(language + "_run", command, output, report, timeout=20)
            if execution["exit_code"] != 0:
                report["errors"].append(language + " harness did not exit successfully")
                continue
            try:
                observations[language] = parse_results(execution["stdout"], expected_ids)
            except ValueError as failure:
                report["errors"].append(language + ": " + str(failure))

        for case in test_cases:
            identifier = case["id"]
            cpp = observations.get("cpp", {}).get(identifier)
            java_result = observations.get("java", {}).get(identifier)
            cpp_expected = cpp is not None and cpp == case["expected"]
            java_expected = java_result is not None and java_result == case["expected"]
            parity = cpp is not None and java_result is not None and cpp == java_result
            passed = cpp_expected and java_expected and parity
            report["results"].append({
                "id": identifier, "operation": case["operation"],
                "expected": case["expected"], "cpp": cpp, "java": java_result,
                "cpp_matches_expectation": cpp_expected,
                "java_matches_expectation": java_expected,
                "parity_matches": parity, "passed": passed,
            })
        report["passed"] = sum(row["passed"] for row in report["results"])
    except (OSError, ValueError, RuntimeError) as failure:
        report["errors"].append(type(failure).__name__ + ": " + str(failure))
    finally:
        after = {name: sha256(path) for name, path in source_paths.items() if path.is_file()}
        report["source_sha256_after"] = after
        report["sources_unchanged"] = bool(before) and before == after
        if before and before != after:
            report["errors"].append("Production source bytes changed during the check")
        report["success"] = (report["passed"] == report["total"] and
                             report["sources_unchanged"] and not report["errors"])
        report["finished_at_utc"] = datetime.now(timezone.utc).isoformat()
        (output / "results.json").write_text(json.dumps(report, indent=2) + "\n",
                                            encoding="utf-8")
    status = "PASS" if report["success"] else "FAIL"
    print("OBJECTIVE_STORY_SCOPE_PARITY_" + status + " cases=" +
          str(report["passed"]) + "/" + str(report["total"]) +
          " SCALAR_ONLY_NO_ANDROID_OR_TLS")
    print(str(output / "results.json"))
    return 0 if report["success"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
