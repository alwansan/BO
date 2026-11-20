"""Compare the published v1.5.0 release to BO's native library. Metadata only."""
import argparse
import hashlib
import json
import pathlib
import re
import struct
import subprocess
import tempfile
import zipfile
from source_audit import restore, elf_identity, LIB

APK_SHA = "102340f238a97ef18a28c9a2e0e2ec47c3e7a982ab1b3845cd65305cad07d28a"
DISC_SHA = "c5bfed25449c8212d7a976f15246e87ead57713f"
REV = "f3754d6985a8c35204e7df071546e586b10f1806"
HOOKS = ("sub_001E5FA0", "__imp__sub_001E5FA0", "sub_001E74B0",
         "__imp__sub_001E74B0", "sub_001E8830", "__imp__sub_001E8830")

def digest(path):
    h = hashlib.sha256()
    with path.open("rb") as f:
        for b in iter(lambda: f.read(1024*1024), b""):
            h.update(b)
    return h.hexdigest()

def inspect(path):
    result = elf_identity(path, DISC_SHA)
    result["sha256"] = digest(path)
    result["bytes"] = path.stat().st_size
    sections = subprocess.check_output(["readelf","-SW",str(path)],text=True)
    # Compare section contents without dumping binary bytes.
    result["section_hashes"] = {}
    with path.open("rb") as f:
        for line in sections.splitlines():
            match = re.match(r"\s*\[\s*\d+\]\s+(\.text|\.rodata|\.dynsym|\.dynstr)\s+\S+\s+[0-9a-fA-F]+\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)",line)
            if match:
                name, off, size = match.groups()
                off, size = int(off,16),int(size,16)
                if off+size > result["bytes"]:
                    raise ValueError("section outside ELF")
                f.seek(off)
                result["section_hashes"][name] = hashlib.sha256(f.read(size)).hexdigest()
    syms = subprocess.check_output(["readelf","-Ws",str(path)],text=True)
    names = set()
    for line in syms.splitlines():
        parts = line.split()
        if len(parts)>=8 and parts[0].endswith(":") and parts[6] not in ("UND","ABS"):
            names.add(parts[-1])
    result["hook_symbol_candidates"] = {h:any(h in n for n in names) for h in HOOKS}
    return result

def compare(root, apk):
    if digest(apk)!=APK_SHA:
        raise ValueError("published APK hash mismatch")
    metadata = (root/"BLACK-Arabic/META-INF/version-control-info.textproto").read_text()
    if f'revision: "{REV}"' not in metadata:
        raise ValueError("Arabic revision mismatch")
    manifest = json.loads((root/"BLACK_FILE_MANIFEST.json").read_text())
    records={r["path"]:r for r in manifest["files"]}
    with tempfile.TemporaryDirectory(prefix="bo-release-") as tmp:
        d=pathlib.Path(tmp)
        orig, mod=d/"official.so",d/"arabic.so"
        with zipfile.ZipFile(apk) as z:
            info=z.getinfo("lib/arm64-v8a/libmain.so")
            if not 0 < info.file_size < 256*1024*1024:
                raise ValueError("unexpected library size")
            with z.open(info) as src,orig.open("wb") as dest:
                import shutil
                shutil.copyfileobj(src,dest)
        restore(root,records[LIB],mod)
        a,b=inspect(orig),inspect(mod)
        sections={k:a["section_hashes"][k]==b["section_hashes"][k] for k in a["section_hashes"].keys() & b["section_hashes"].keys()}
        return {"official_apk_sha256_verified":True,"apk_revision_matches_tag":True,
                "official":a,"arabic":b,"same_native_binary":a["sha256"]==b["sha256"],
                "same_compile_image":bool(a["compile_image_sha1"] and a["compile_image_sha1"]==b["compile_image_sha1"]),
                "section_identity":sections,"arabic_source_exact_proven":False,"device_tested":False}

if __name__=="__main__":
    p=argparse.ArgumentParser()
    p.add_argument("--root",type=pathlib.Path,default=pathlib.Path("."))
    p.add_argument("--apk",type=pathlib.Path,required=True)
    a=p.parse_args()
    print(json.dumps(compare(a.root,a.apk),indent=2,sort_keys=True))
