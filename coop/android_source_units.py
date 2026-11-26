"""Compile complete staged Android translation units without any game image.

Frontend check only: no generated guest bodies, linking, APK, ISO, execution,
original asset reads or gameplay claim. image_symbols.h follows the pinned
SDK codegen_emit.cpp metadata emitter using the PUBLIC config TOML only.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re
import shutil
import subprocess
import tomllib

from prepare_room_network import INPUTS, prepare
from prepare_original_system_lifetime import prepare as prepare_lifetime
from prepare_stage_resource_watch import prepare as prepare_files

ANDROID_PIN = "f3754d6985a8c35204e7df071546e586b10f1806"
SDK_PIN = "99078b119b5d5e0d1d6afb1390e29437c0c984c0"
SDL_PIN = "fa2c02bb6e21974a89ea9824bc53c9932abe5f9c"  # release-3.4.16, baseline Gradle
UNITS = (
    "src/game/hooks.cpp", "src/d3d/d3d8.cpp", "src/game/cheats.cpp",
    "src/game/options_menu.cpp", "src/game/exit_menu.cpp",
    "src/game/difficulty.cpp", "native/input.cpp",
)
SOURCE_SUFFIXES = {".h", ".hpp", ".cpp", ".inc", ".inl"}


def symbols(config):
    """Same ordered-map metadata algorithm as SDK codegen_emit.cpp:203–227.

    Missing FUNCTION symbols receive distinct FFFF0000+ placeholders. Missing
    DATA stays zero. ABI/layout declarations also come from the config. This
    emits no original instructions and does not discover/analyze an XBE.
    """
    lines = ['#pragma once', '#define IMAGE_XBE "default.xbe"']
    absent = 0
    for name, value in sorted(config["symbols"].items()):
        address = value if value else 0xFFFF0000 + absent
        if not value:
            absent += 1
        lines += [f"#define SYM_{name} {address:08X}",
                  f"#define HAS_{name} {int(bool(value))}"]
    for name, value in sorted(config.get("data", {}).items()):
        lines += [f"#define SYM_{name} {value:08X}",
                  f"#define HAS_{name} {int(bool(value))}"]
    for name, value in sorted(config.get("layout", {}).items()):
        lines.append(f"#define XDK_{name} 0x{value:X}")
    abi = config.get("abi", {})
    if abi:
        rows = ["X(" + name + ", " + ", ".join(values) + ")"
                for name, values in sorted(abi.items())]
        lines.append("#define IMAGE_ABI(X) " + " \\\n".join(rows))
    return "\n".join(lines) + "\n"


def pin(root, expected):
    actual = subprocess.check_output(
        ["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
    if actual != expected:
        raise ValueError(f"Unexpected source revision: {root.name}")
    return actual


def copy_sources(source, destination):
    for original in source.rglob("*"):
        if original.is_file() and original.suffix in SOURCE_SUFFIXES:
            target = destination / original.relative_to(source)
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(original, target)


def stage(upstream, sdk, output):
    # No broad repository copy: only known public source inputs and headers.
    # Never inspect, extract, assemble or execute an owner game image.
    output.mkdir(exist_ok=False, parents=True)
    title, runtime = output / "title", output / "sdk"
    for name in INPUTS:
        target = title / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(upstream / name, target)
    for directory in ("src", "native"):
        copy_sources(upstream / directory, title / directory)
    prepare(title)
    copy_sources(sdk / "runtime", runtime / "runtime")
    prepare_lifetime(runtime)
    prepare_files(runtime)
    config = tomllib.loads((title / "config/game.toml").read_text())
    (output / "image_symbols.h").write_text(symbols(config))
    return title, runtime


def compile_units(upstream, sdk, sdl, output, cxx, expected_api=None):
    pins = {"android": pin(upstream, ANDROID_PIN), "sdk": pin(sdk, SDK_PIN),
            "sdl": pin(sdl, SDL_PIN)}
    macros = subprocess.check_output(
        [cxx, "-dM", "-E", "-x", "c++", "-"], input="", text=True)
    if not re.search(r"^#define __ANDROID__ 1$", macros, re.M) or not re.search(
            r"^#define __aarch64__ 1$", macros, re.M):
        raise ValueError("This check requires an actual Android ARM64 compiler")
    api_match = re.search(r"^#define __ANDROID_MIN_SDK_VERSION__ (\d+)$", macros, re.M)
    api = int(api_match.group(1)) if api_match else None
    if expected_api is not None and api != expected_api:
        raise ValueError(f"Expected Android API {expected_api}, got {api}")
    title, runtime = stage(upstream, sdk, output)
    logs = output / "logs"
    logs.mkdir()
    includes = [title / "native/compat", title / "native", title / "src",
                runtime / "runtime", output, sdl / "include"]
    flags = ["-std=c++20", "-fsyntax-only", "-fno-strict-aliasing", "-fsigned-char",
             "-include", str(title / "native/compat/crt.h")]
    for directory in includes:
        flags += ["-I", str(directory)]
    records = []
    for unit in UNITS:
        log = logs / (unit.replace("/", "_") + ".log")
        with log.open("w") as stream:
            result = subprocess.run([cxx, *flags, str(title / unit)],
                                    stdout=stream, stderr=subprocess.STDOUT)
        records.append({"unit": unit, "exit_code": result.returncode,
                        "staged_sha256": hashlib.sha256((title / unit).read_bytes()).hexdigest()})
        print(f"ANDROID_SOURCE_UNIT {'PASS' if result.returncode == 0 else 'FAIL'} {unit}", flush=True)
    report = {"passed": all(r["exit_code"] == 0 for r in records), "pins": pins,
              "compiler": subprocess.check_output([cxx, "--version"], text=True).splitlines()[0],
              "actual_android_arm64": True, "android_min_api": api, "units": records,
              "public_config_symbols_sha256": hashlib.sha256((output / "image_symbols.h").read_bytes()).hexdigest(),
              "original_assets_used": False, "guest_code_generated": False,
              "linked": False, "apk_built": False, "gameplay_verified": False}
    (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    return report


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    for argument in ("upstream", "sdk", "sdl", "output"):
        parser.add_argument("--" + argument, required=True, type=Path)
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--expected-api", type=int)
    args = parser.parse_args()
    report = compile_units(args.upstream.resolve(), args.sdk.resolve(), args.sdl.resolve(),
                           args.output.resolve(), args.cxx, args.expected_api)
    raise SystemExit(0 if report["passed"] else 1)
