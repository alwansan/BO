#!/usr/bin/env python3
"""Read-only identity checks for the owner's extracted XISO manifest.

No copyrighted game bytes, paths to local files, or private captures are
emitted by this tool; output consists only of counts and pass/fail gates.
The archive restoration script lives in alwansan/biso, not here.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct

ROOT = "Black_Xbox_Extracted/"
MAGIC = b"MICROSOFT*XBOX*MEDIA"
SECTOR = 2048
CHUNK = 1024 * 1024
MAX_IMAGE = 8 * 1024**3
MAX_FILE = 2 * 1024**3

def safe_path(value):
    if (not isinstance(value, str) or not value or "\\" in value or
        "\x00" in value or value.startswith("/") or
        any(part in ("", ".", "..") for part in value.split("/"))):
        raise ValueError("unsafe or invalid manifest path")
    return value

def load_manifest(path):
    data = json.loads(Path(path).read_text(encoding="utf-8"))
    if data.get("format") != "github-auto-upload-v1":
        raise ValueError("unsupported owner manifest format")
    records = data.get("files")
    if not isinstance(records, list) or not records or len(records) > 10000:
        raise ValueError("invalid manifest record count")
    files, seen_parts, splits = {}, set(), 0
    for record in records:
        name = safe_path(record.get("path"))
        if not name.startswith(ROOT) or not name[len(ROOT):] or name[len(ROOT):] in files:
            raise ValueError("invalid or duplicate target")
        size, sha, parts = record.get("bytes"), record.get("sha256"), record.get("parts")
        if type(size) is not int or size < 0 or size > MAX_FILE:
            raise ValueError("invalid target size")
        if not isinstance(sha, str) or not re.fullmatch(r"[0-9a-f]{64}", sha):
            raise ValueError("invalid target hash")
        if not isinstance(parts, list) or not 1 <= len(parts) <= 256:
            raise ValueError("invalid parts")
        if len(parts) == 1:
            if safe_path(parts[0]) != name: raise ValueError("single-file path mismatch")
        else:
            splits += 1
            for i, part in enumerate(parts):
                if safe_path(part) != f"{name}/part-{i:05d}":
                    raise ValueError("split parts out of order")
        for part in parts:
            if part in seen_parts: raise ValueError("duplicate part assignment")
            seen_parts.add(part)
        files[name[len(ROOT):]] = (size, sha, tuple(parts))
    empty = data.get("empty_dirs", [])
    if not isinstance(empty, list): raise ValueError("invalid empty directories")
    for entry in empty: safe_path(entry)
    return files, {"original_file_count":len(files), "stored_part_count":len(seen_parts),
                   "multi_part_file_count":splits,
                   "original_payload_bytes":sum(x[0] for x in files.values()),
                   "manifest_structure_verified":True}

def validate_stored_parts(checkout, files):
    """Optional verification from a *local* biso checkout, no restoration."""
    root = Path(checkout).resolve()
    total = 0
    for _, (length, expected_sha, parts) in files.items():
        digest = hashlib.sha256()
        size = 0
        for part in parts:
            source = (root / part).resolve()
            if not source.is_relative_to(root) or not source.is_file():
                raise ValueError("missing or unsafe original part")
            with source.open("rb") as handle:
                while block := handle.read(CHUNK):
                    digest.update(block)
                    size += len(block)
                    if size > length: raise ValueError("stored source exceeds size limit")
        if size != length or digest.hexdigest() != expected_sha:
            raise ValueError("stored source hash or size mismatch")
        total += size
    return total

def read_xdvdfs(file):
    """Bounded XDVDFS metadata walker. Returns relative names and disc extents."""
    file.seek(0, 2); size=file.tell()
    if size <= SECTOR * 40 or size > MAX_IMAGE:
        raise ValueError("invalid image size")
    for base in (0, 0x18300000, 0xFD90000, 0x2080000):
        if base + 32 * SECTOR + 28 > size: continue
        file.seek(base + 32 * SECTOR)
        if file.read(20) == MAGIC:
            sector, bytes_ = struct.unpack("<II",file.read(8))
            break
    else:
        raise ValueError("XDVDFS volume not found")
    extents = {}
    visited_dirs = set()
    def walk(dir_sector, dir_size, parent="", depth=0):
        if depth > 20 or not 14 <= dir_size <= 32 * 1024 * 1024:
            raise ValueError("invalid directory size/depth")
        extent = (dir_sector, dir_size)
        if extent in visited_dirs: raise ValueError("cyclic directory")
        visited_dirs.add(extent)
        at = base + dir_sector * SECTOR
        if at < 0 or at + dir_size > size: raise ValueError("directory outside image")
        file.seek(at)
        table = file.read(dir_size)
        nodes = [0]; seen = set()
        while nodes:
            offset = nodes.pop()
            if offset in seen: continue
            seen.add(offset)
            pos = offset * 4
            if pos + 14 > len(table): raise ValueError("invalid tree node")
            left, right, child_sector, child_bytes, flags, nlen = struct.unpack_from(
                "<HHIIBB", table, pos)
            if left == 0xffff: continue
            if not nlen or pos + 14 + nlen > len(table):
                raise ValueError("invalid XDVDFS filename length")
            name = table[pos + 14:pos + 14 + nlen].decode("latin-1")
            if name in (".", "..") or "/" in name or "\\" in name:
                raise ValueError("unsafe XDVDFS entry name")
            full = parent + name
            child_offset = base + child_sector * SECTOR
            if child_offset < 0 or child_offset + child_bytes > size:
                raise ValueError("XDVDFS entry outside image")
            if flags & 0x10:
                walk(child_sector, child_bytes, full + "/", depth + 1)
            else:
                if full in extents or len(extents) >= 10000:
                    raise ValueError("duplicate or too many image entries")
                extents[full] = (child_offset, child_bytes)
            nodes.extend(x for x in (left, right) if x)
    walk(sector, bytes_)
    return extents

def verify_iso(path, files):
    """Streaming SHA-256 of each XDVDFS original file. Writes nothing."""
    image = Path(path)
    with image.open("rb") as fd:
        entries = read_xdvdfs(fd)
        if set(entries) != set(files):
            raise ValueError("image and extracted manifest file sets differ")
        checked = 0
        for name, (length, expected_sha, _) in files.items():
            pos, available = entries[name]
            if available != length: raise ValueError("image and manifest sizes differ")
            fd.seek(pos)
            h=hashlib.sha256()
            remaining=length
            while remaining:
                block=fd.read(min(CHUNK, remaining))
                if not block: raise ValueError("image truncated")
                h.update(block);remaining-=len(block)
            if h.hexdigest() != expected_sha:
                raise ValueError("image file differs from extracted manifest hash")
            checked += 1
        return checked

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest",type=Path,required=True)
    parser.add_argument("--checkout",type=Path,help="optional source parts checkout, read-only")
    parser.add_argument("--iso",type=Path,help="optional owner XISO, read-only full file check")
    parser.add_argument("--expect-files",type=int)
    parser.add_argument("--expect-parts",type=int)
    args=parser.parse_args()
    try:
        files, summary=load_manifest(args.manifest)
        if args.expect_files is not None and len(files)!=args.expect_files:
            raise ValueError("original file count differs from pinned inventory")
        if args.expect_parts is not None and summary["stored_part_count"]!=args.expect_parts:
            raise ValueError("stored part count differs from pinned inventory")
        summary["part_bytes_sha256_verified"]=False
        summary["image_files_sha256_verified"]=False
        if args.checkout:
            summary["source_payload_bytes_verified"]=validate_stored_parts(args.checkout,files)
            summary["part_bytes_sha256_verified"]=True
        if args.iso:
            summary["image_files_checked"]=verify_iso(args.iso,files)
            summary["image_files_sha256_verified"]=True
        print(json.dumps(summary,sort_keys=True,indent=2))
    except (OSError,ValueError,KeyError,TypeError,json.JSONDecodeError):
        raise SystemExit("Owner extraction audit failed (details withheld; check input/manifest).")
if __name__=="__main__": main()
