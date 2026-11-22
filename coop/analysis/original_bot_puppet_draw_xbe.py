#!/usr/bin/env python3
"""Private, pin-verified OWNER original BLACK XBE bot draw entry survey.
PC MIT recomp contains actual cloned soldier puppet at source:
    black_puppets_make -> black_puppet_follow -> black_guest_method draw.
This survey ONLY verifies bounded original executable window candidates.
No XBE bytes, disassembly, pointers, registers or assets leave the CI runner.
"""
import argparse
import hashlib
import json
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from story_entry_static import OWNER_SHA1, MAX_FILE, UnsafeImage, executable_sections
from world_xref_static import OP_LINE

WINDOW=640
TARGETS={
    "original_world_grid_draw":0x000D1500,
    "original_character_draw_dispatch":0x001B7440,
}

def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Invalid private owner input")
    if not isinstance(pinned,str) or not re.fullmatch(r"[0-9a-f]{40}",pinned):
        raise UnsafeImage("Pinned SHA1 missing")
    if hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Original owner XBE SHA1 mismatch")
    if not shutil.which("objdump"):
        raise UnsafeImage("No private bounded decoder")
    sections=executable_sections(data)
    windows={}
    for title,entry in TARGETS.items():
        offsets=[raw+entry-va for va,n,raw in sections
                 if va<=entry and entry+WINDOW<=va+n]
        if len(offsets)!=1:
            raise UnsafeImage("Original candidate not uniquely executable")
        with tempfile.TemporaryDirectory(prefix="bo-original-puppet-draw-") as d:
            loc=Path(d)/"private.bin"
            loc.write_bytes(data[offsets[0]:offsets[0]+WINDOW])
            command=["objdump","-D","-b","binary","-m","i386","-M","intel",
                     "--no-show-raw-insn","--adjust-vma="+hex(entry),str(loc)]
            result=subprocess.run(command,capture_output=True,text=True,timeout=15)
            if result.returncode:
                raise UnsafeImage("Original decoder failed")
        ops=[]
        for line in result.stdout.splitlines():
            m=OP_LINE.match(line)
            if m and entry<=int(m.group(1),16)<entry+WINDOW:
                ops.append((m.group(2).lower(),m.group(3).lower()))
        if not 3<=len(ops)<=WINDOW:
            raise UnsafeImage("Original draw candidate decode invalid")
        windows[title]={
            "original_executable_candidate":True,
            "bounded_window_bytes":WINDOW,
            "decoded_instruction_candidates":len(ops),
            "possible_calls":sum(op.startswith("call") for op,_ in ops),
            "possible_returns":sum(op.startswith("ret") for op,_ in ops),
            "known_character_vtable_draw_slot_5_from_pc_only":True,
            "exact_android_draw_abi_verified":False,
            "original_renderer_can_safely_clone_character":False,
        }
    report={
        "schema":"bo-private-original-puppet-draw-source-v1",
        "owner_xbe_sha1_pinned":True,
        "source_reference":"portingpete/Black-2006-Recomp@b2512fafd src/recomp_manual.c:2009-2130,2481-2490",
        "source_prototype_visual_slots":32,
        "source_prototype_appearance_model_offset":0x320,
        "source_prototype_controller_offset":0x324,
        "source_prototype_instance_offset":0x328,
        "source_prototype_parts_offsets":[0x34c,0x350],
        "source_prototype_draw_virtual_slot":5,
        "owner_original_candidate_windows":windows,
        "android_guest_call_abi_verified":False,
        "android_original_scene_actor_creation_verified":False,
        "android_avatar_drawn_in_game_verified":False,
        "original_binary_or_disassembly_exported":False,
        "note":"Bounded linear windows may cross function boundaries or data. Candidate mapping is not proof of callable ABI, same renderer state, safely owned actor memory, or actual human body rendering."
    }
    if len(json.dumps(report,separators=(",",":")))>2900:
        raise UnsafeImage("Private output exceeds numeric-only cap")
    return report

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--owner-xbe",type=Path,required=True)
    p.add_argument("--report",type=Path,required=True)
    a=p.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Invalid private XBE path")
    result=audit(a.owner_xbe.read_bytes())
    a.report.write_text(json.dumps(result,separators=(",",":"),sort_keys=True)+"\n",encoding="utf-8")
    for label,row in result["owner_original_candidate_windows"].items():
        print("BO_ORIGINAL_PUPPET_DRAW %s executable=%d calls=%d returns=%d NO_ABI_NO_ACTOR"%(
            label,int(row["original_executable_candidate"]),
            row["possible_calls"],row["possible_returns"]))
    print("BO_ORIGINAL_PUPPET_DRAW_PRIVATE_PASS: two original draw candidates, numeric-only, NO RENDER")
if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError,subprocess.TimeoutExpired) as e:
        print("BO_ORIGINAL_PUPPET_DRAW_PRIVATE_BLOCKED:"+type(e).__name__,file=sys.stderr)
        sys.exit(1)
