"""Read-only identity audit; prints metadata only. No asset export."""
import argparse
import hashlib
import json
import pathlib
import re
import struct
import subprocess
import tempfile

ISO = "BLACK-Arabic_src/assets/bundle/game.iso"
LIB = "BLACK-Arabic_src/lib/arm64-v8a/libmain.so"
MAGIC = b"MICROSOFT*XBOX*MEDIA"
EUROPE = "796ad8369354e48aa15644d4707dd70bd3bc3b4c"

def restore(root, rec, dest):
    h = hashlib.sha256()
    n = 0
    if not rec["parts"] or len(set(rec["parts"])) != len(rec["parts"]):
        raise ValueError("invalid parts")
    with dest.open("wb") as out:
        for name in rec["parts"]:
            src = (root / name).resolve()
            if not src.is_relative_to(root.resolve()) or not src.is_file():
                raise ValueError("invalid part path")
            with src.open("rb") as f:
                for block in iter(lambda: f.read(1024 * 1024), b""):
                    h.update(block)
                    n += len(block)
                    if n > rec["bytes"]:
                        raise ValueError("size exceeded")
                    out.write(block)
    if n != rec["bytes"] or h.hexdigest() != rec["sha256"]:
        raise ValueError("manifest hash/size mismatch")

def xbe_from_iso(path):
    total = path.stat().st_size
    with path.open("rb") as f:
        for base in (0, 0x18300000, 0xFD90000, 0x2080000):
            f.seek(base + 32 * 2048)
            if f.read(20) == MAGIC:
                sector, size = struct.unpack("<II", f.read(8))
                break
        else:
            raise ValueError("XDVDFS magic missing")
        at = base + sector * 2048
        if not (14 <= size <= 16 * 1024 * 1024 and at + size <= total):
            raise ValueError("invalid root table bounds")
        f.seek(at)
        table = f.read(size)
        todo, seen, hits = [0], set(), []
        while todo:
            off = todo.pop()
            if off in seen or off * 4 + 14 > len(table):
                raise ValueError("invalid/cyclic directory")
            seen.add(off)
            pos = off * 4
            left, right, sec, count, attr, nlen = struct.unpack_from("<HHIIBB", table, pos)
            if left == 0xffff:
                continue
            if not nlen or pos + 14 + nlen > len(table):
                raise ValueError("invalid entry name")
            if table[pos+14:pos+14+nlen].lower() == b"default.xbe":
                if attr & 0x10:
                    raise ValueError("XBE is a directory")
                hits.append((sec, count))
            todo.extend(x for x in (left, right) if x)
        if len(hits) != 1:
            raise ValueError("missing or duplicate default.xbe")
        sec, count = hits[0]
        at = base + sec * 2048
        if not (4 <= count <= 128 * 1024 * 1024 and at + count <= total):
            raise ValueError("invalid XBE bounds")
        f.seek(at)
        data = f.read(count)
        if len(data) != count or data[:4] != b"XBEH":
            raise ValueError("invalid XBE header")
        return {"bytes":count, "sha1":hashlib.sha1(data).hexdigest(),
                "sha256":hashlib.sha256(data).hexdigest(), "partition_offset":base}

def elf_identity(path, xbe_sha):
    with path.open("rb") as f:
        header = f.read(64)
        if header[:6] != b"\x7fELF\x02\x01" or struct.unpack_from("<H", header, 18)[0] != 183:
            raise ValueError("not little-endian ELF64 AArch64")
        phoff, = struct.unpack_from("<Q", header, 32)
        entsize, count = struct.unpack_from("<HH", header, 54)
        if entsize < 56 or count > 1024 or phoff + entsize * count > path.stat().st_size:
            raise ValueError("invalid program header")
        loads = []
        for i in range(count):
            f.seek(phoff + i * entsize)
            typ, flags, off, va, pa, filesz, memsz, align = struct.unpack("<IIQQQQQQ", f.read(56))
            if typ == 1:
                loads.append((off, va, filesz))
        syms = subprocess.check_output(["readelf", "-Ws", str(path)], text=True)
        candidates = set()
        for line in syms.splitlines():
            fields = line.split()
            if len(fields) >= 8 and fields[-1] == "kImageSha1" and fields[6] not in ("UND","ABS"):
                addr, size = int(fields[1], 16), int(fields[2])
                if size != 41:
                    continue
                for off, va, filesz in loads:
                    if va <= addr and addr + size <= va + filesz:
                        f.seek(off + addr - va)
                        raw = f.read(size)
                        if re.fullmatch(rb"[0-9a-f]{40}\x00", raw):
                            candidates.add(raw[:-1].decode())
        if len(candidates) > 1:
            raise ValueError("ambiguous kImageSha1")
        embedded = next(iter(candidates), None)
        f.seek(0)
        # Raw occurrence is supporting evidence, not a symbol match.
        contains = (xbe_sha.encode() + b"\x00") in f.read()
    return {"architecture":"AArch64", "compile_image_sha1":embedded,
            "xbe_hash_literal_present":contains,
            "symbol_matches_disc":embedded == xbe_sha if embedded else None}

def audit(root):
    manifest = json.loads((root/"BLACK_FILE_MANIFEST.json").read_text())
    records = {r["path"]:r for r in manifest["files"]}
    with tempfile.TemporaryDirectory(prefix="bo-audit-") as tmp:
        iso, lib = pathlib.Path(tmp)/"disc.iso", pathlib.Path(tmp)/"libmain.so"
        restore(root, records[ISO], iso)
        restore(root, records[LIB], lib)
        xbe = xbe_from_iso(iso)
        elf = elf_identity(lib, xbe["sha1"])
        return {"manifest_verified":True,"iso_bytes":records[ISO]["bytes"],
                "lib_bytes":records[LIB]["bytes"], "xbe":xbe,"elf":elf,
                "disc_matches_known_europe":xbe["sha1"] == EUROPE,
                "source_commit_proven":False,"gameplay_tested":False}

def main():
    p = argparse.ArgumentParser()
    p.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    args = p.parse_args()
    print(json.dumps(audit(args.root), indent=2))

if __name__ == "__main__":
    main()
