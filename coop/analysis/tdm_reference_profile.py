#!/usr/bin/env python3
"""Safe manifest-only comparison to a third-party BLACK TDM resource profile.

Original implementation by BO-COOP; no third-party program code, binary,
animation assets, game files, ISO or guest data are bundled or emitted.
Reference: Peterson047/BLACK-XBEMultiplayer (read-only), pinned in docs.
This identifies assets only, NOT gameplay/engine/actor compatibility.
"""
import argparse
import collections
import json
from pathlib import Path
import sys

ROOT="Black_Xbox_Extracted/"
STAGE="levels/level_00/"
# File paths and public byte counts documented by the other project's
# map preflight. These are reference DATA, not code or copyrighted assets.
STAGE_PROFILE=(
    ("Unit_01.bin",10526464),
    ("Collide.awd",2973696),
    ("LevelDat.bin",711088),
    ("Level.awd",968704),
    ("stg_0001/StLevel.bin",2378464),
    ("stg_0001/StUnit01.bin",265504),
    ("stg_0001/guns.bin",275968),
    ("stg_0001/guns_s.bin",276736),
    ("stg_0001/AIWpns.awd",606208),
    ("stg_0001/Emitter.awd",4096),
    ("speech.slb",6944),
    ("Destruct.bks",3899392),
    ("Destruct.ssh",448),
)
WEAPON_EXTENSIONS=(".db",".wdd")

def profile(manifest):
    if not isinstance(manifest,dict) or manifest.get("format")!="github-auto-upload-v1":
        raise ValueError("invalid owner manifest")
    records=manifest.get("files")
    if not isinstance(records,list) or not records or len(records)>10000:
        raise ValueError("invalid file list")
    entries={}
    for item in records:
        if not isinstance(item,dict):
            raise ValueError("invalid record")
        name,bytes_=item.get("path"),item.get("bytes")
        if (not isinstance(name,str) or not name.startswith(ROOT) or
            not name[len(ROOT):] or "\\" in name or "\x00" in name or
            any(part in ("",".","..") for part in name.split("/")) or
            type(bytes_) is not int or not 0<=bytes_<=2*1024**3):
            raise ValueError("invalid item")
        relative=name[len(ROOT):].lower()
        if relative in entries:
            raise ValueError("case-insensitive name collision")
        entries[relative]=bytes_
    missing=0
    wrong_size=0
    for name,wanted in STAGE_PROFILE:
        size=entries.get((STAGE+name).lower())
        if size is None: missing+=1
        elif size!=wanted: wrong_size+=1

    weapon_prefix=(STAGE+"fpguns/").lower()
    groups=collections.defaultdict(set)
    for name in entries:
        if not name.startswith(weapon_prefix):continue
        suffix=name[len(weapon_prefix):]
        if "/" in suffix: continue
        for ext in WEAPON_EXTENSIONS:
            if suffix.endswith(ext) and len(suffix)>len(ext):
                groups[suffix[:-len(ext)]].add(ext)
    matched=sum(len(exts)==2 for exts in groups.values())
    orphans=sum(len(exts)!=2 for exts in groups.values())
    complete=(missing==0 and wrong_size==0 and matched>=16 and orphans==0)
    return {
       "schema":"BO-COOP-reference-tdm-asset-profile-v1",
       "source":"owner_pinned_extracted_xiso_manifest_only",
       "required_stage_files":len(STAGE_PROFILE),
       "required_stage_files_present_and_correct":len(STAGE_PROFILE)-missing-wrong_size,
       "missing_stage_files":missing,
       "stage_file_size_mismatches":wrong_size,
       "weapon_db_wdd_pairs":matched,
       "unpaired_weapon_resources":orphans,
       "matches_third_party_reference_asset_profile":complete,
       "tdm_runtime_proven":False,
       "story_coop_actor_compatibility_proven":False,
       "windows_binary_compatible_with_android":False,
       "third_party_source_copied":False,
    }

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest",required=True,type=Path)
    parser.add_argument("--require-reference-match",action="store_true")
    options=parser.parse_args()
    # Explicit small metadata input, no ISO/image or binary access.
    if options.manifest.stat().st_size>2*1024*1024:
        raise ValueError("oversized manifest")
    answer=profile(json.loads(options.manifest.read_text(encoding="utf-8")))
    print(json.dumps(answer,sort_keys=True,indent=2))
    if options.require_reference_match and not answer["matches_third_party_reference_asset_profile"]:
        raise SystemExit(1)

if __name__=="__main__":
    try:main()
    except (OSError,UnicodeError,json.JSONDecodeError,ValueError):
        print("Cannot audit owner metadata (no game bytes or details printed).",file=sys.stderr)
        sys.exit(1)
