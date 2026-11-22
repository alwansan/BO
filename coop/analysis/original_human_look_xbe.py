#!/usr/bin/env python3
"""Private, bounded owner-XBE survey of 3 PC-prototype human LOOK helpers.
The functions' names/roles come from MIT PC source, not verified Android ABI.
NO guest call, binaries, disassembly or executable-address transfer.
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
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections
from world_xref_static import OP_LINE

WINDOW=768
TARGETS={
    "soldier_visual_init":0x0017E420,
    "soldier_animation_request":0x0017E130,
    "soldier_pose_update":0x001801B0,
}
FIELDS={
    "look_model_320":0x320,
    "npc_controller_324":0x324,
    "look_instance_328":0x328,
    "part_buffer_a_34c":0x34c,
    "part_buffer_b_350":0x350,
    "player_feet_a0":0xa0,
}
def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Invalid private owner XBE")
    if not re.fullmatch(r"[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Wrong pinned owner XBE")
    if not shutil.which("objdump"):raise UnsafeImage("No bounded x86 decoder")
    sections=executable_sections(data)
    summary={}
    for name,entry in TARGETS.items():
        matches=[raw+entry-va for va,n,raw in sections
                 if va<=entry and entry+WINDOW<=va+n]
        if len(matches)!=1:raise UnsafeImage("Unmapped or ambiguous candidate")
        with tempfile.TemporaryDirectory(prefix="bo-private-look-xbe-") as tmp:
            f=Path(tmp)/"private-window.bin"
            f.write_bytes(data[matches[0]:matches[0]+WINDOW])
            decoded=subprocess.run([
                "objdump","-D","-b","binary","-m","i386","-M","intel",
                "--no-show-raw-insn","--adjust-vma="+hex(entry),str(f)],
                text=True,capture_output=True,timeout=20)
            if decoded.returncode:raise UnsafeImage("Decoder failed")
        instructions=[]
        for line in decoded.stdout.splitlines():
            m=OP_LINE.match(line)
            if m and entry<=int(m.group(1),16)<entry+WINDOW:
                instructions.append((m.group(2).lower(),m.group(3).lower()))
        if not 3<=len(instructions)<=WINDOW:raise UnsafeImage("Unexpected decoder result")
        fields={
            key:sum(bool(re.search(r"\+0x%x\]"%offset,args)) for _,args in instructions)
            for key,offset in FIELDS.items()
        }
        summary[name]={
            "executable_window_mapped":True,
            "window_size":WINDOW,
            "decoded_instructions":len(instructions),
            "relative_call_candidates":sum(op.startswith("call") for op,_ in instructions),
            "return_candidates":sum(op.startswith("ret") for op,_ in instructions),
            "field_reference_counts":fields,
            "safe_guest_call_abi_verified":False,
            "safe_engine_actor_creation_verified":False,
        }
    report={
        "schema":"bo-original-human-look-private-v1",
        "owner_xbe_sha1_pinned":True,
        "reference_pc_source":"portingpete/Black-2006-Recomp@b2512fafd",
        "windows":summary,
        "android_engine_guest_abi_verified":False,
        "actor_instantiation_or_rendering_verified":False,
        "source_function_boundaries_verified":False,
        "original_bytes_exported":False,
        "note":"Linear disassembly can cross adjacent functions/data. Executable mapping and field references do NOT confirm call ABI, controlflow, renderer lifetime or scene-loaded state."
    }
    if len(json.dumps(report,separators=(",",":")))>3200:
        raise UnsafeImage("Privacy output budget exceeded")
    return report

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--owner-xbe",required=True,type=Path)
    p.add_argument("--report",required=True,type=Path)
    a=p.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Private source missing or invalid")
    r=audit(a.owner_xbe.read_bytes())
    a.report.write_text(json.dumps(r,sort_keys=True,separators=(",",":"))+"\n",encoding="utf-8")
    for name,v in r["windows"].items():
        print("BO_OWNER_LOOK_XBE %s executable=%d calls=%d returns=%d NO_ABI_NO_ACTOR"%(
            name,int(v["executable_window_mapped"]),
            v["relative_call_candidates"],v["return_candidates"]))
    print("BO_OWNER_LOOK_XBE_PRIVATE_PASS: bounded numeric evidence only, NO ACTOR ABI")
if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError,subprocess.TimeoutExpired) as e:
        print("BO_OWNER_LOOK_XBE_BLOCKED:"+type(e).__name__,file=sys.stderr)
        sys.exit(1)
