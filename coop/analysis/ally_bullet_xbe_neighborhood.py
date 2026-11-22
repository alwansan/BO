#!/usr/bin/env python3
"""PRIVATE owner XBE soldier bullet and local-player damage window survey.

This is a *control-flow candidate map*, never a proven team/faction check,
safe hook address, remote-human hit path, or gameplay modification.
Emits aggregate counts and only four public-source-known actor offset labels.
NO raw owner instruction/byte/disassembly/texture/actor values leave CI.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

from story_entry_static import OWNER_SHA1, MAX_FILE, UnsafeImage, executable_sections
from world_xref_static import OP_LINE
from world_writer_flow import direct_target, instruction_family

TARGETS = {
    "soldier_bullet_receiver": 0x001E74B0,
    "local_player_damage_candidate_a": 0x001ED990,
    "local_player_damage_candidate_b": 0x001ED700,
}
WINDOW = 256
MAX_REPORT_BYTES = 8192
OFFSETS = {
    0xC4: "character_kind_source",
    0x2F0: "character_health_source",
    0x384: "character_dying_source",
    0x8F2: "local_player_cheat_invulnerable_source",
}
# The regex matches an explicit pointer+offset; local variable offsets or
# integers within arithmetic must not be mistaken for a faction member.
MEMBER = re.compile(r"\[\s*(?:e(?:ax|bx|cx|dx|si|di))\s*\+\s*0x([0-9a-f]+)\s*\]",re.I)
CONDITIONAL = re.compile(r"^j(?!mp)[a-z]+$")
MAX_OPS = 256

def classify_window(data, address, sections):
    if not isinstance(address,int) or address<=0:
        raise UnsafeImage("Bad target")
    mapping=[off+address-va for va,n,off in sections
             if va<=address and address+WINDOW<=va+n]
    if len(mapping)!=1:
        raise UnsafeImage("Missing or ambiguous executable window")
    raw=bytes(data[mapping[0]:mapping[0]+WINDOW])
    with tempfile.TemporaryDirectory(prefix="bo-owner-bullet-private-") as folder:
        loc=Path(folder)/"window.bin"
        loc.write_bytes(raw)
        command=["objdump","-D","-b","binary","-m","i386","-M","intel",
                 "--no-show-raw-insn","--adjust-vma="+hex(address),str(loc)]
        try:
            p=subprocess.run(command,capture_output=True,text=True,
                             timeout=15,check=False)
        except subprocess.TimeoutExpired as e:
            raise UnsafeImage("Bounded disassembler timeout") from e
        if p.returncode:raise UnsafeImage("Private x86 decoder failed")
    family=Counter()
    known=Counter()
    call_near=0;conditional=0;branch_inside=0;return_count=0
    member_comparison_count=0
    instructions=0
    for line in p.stdout.splitlines():
        m=OP_LINE.match(line)
        if m is None:continue
        ip=int(m.group(1),16)
        if ip<address or ip>=address+WINDOW:continue
        instructions+=1
        if instructions>MAX_OPS:raise UnsafeImage("Instruction budget")
        op,operands=m.group(2).lower(),m.group(3).lower()
        family[instruction_family(op)]+=1
        if CONDITIONAL.fullmatch(op):
            conditional+=1
        if op.startswith("ret"):return_count+=1
        if op.startswith(("call","j")):
            target=direct_target(operands)
            if target is not None and address<=target<address+WINDOW:
                branch_inside+=1
                if op.startswith("call"):call_near+=1
        for part in MEMBER.finditer(operands):
            off=int(part.group(1),16)
            if off in OFFSETS:
                known[OFFSETS[off]]+=1
                if op.startswith(("cmp","test")):member_comparison_count+=1
    if instructions<3:raise UnsafeImage("Too few decoded instructions")
    return {
        "mapped_executable_window":True,
        "window_bytes":WINDOW,
        "decoded_instruction_count":instructions,
        "instruction_families":dict(sorted(family.items())),
        "conditional_branch_candidates":conditional,
        "nearby_direct_branches_and_calls_inside_window":branch_inside,
        "nearby_direct_calls_inside_window":call_near,
        "return_candidates":return_count,
        "known_actor_offset_references":dict(sorted(known.items())),
        "known_actor_offset_comparisons":member_comparison_count,
        "original_friendly_relation_found":False,
        "original_reticle_green_path_found":False,
        "friendly_fire_damage_rejection_found":False,
        "actual_function_boundary_or_reachability_proven":False
    }

def audit(data, expected_sha1=OWNER_SHA1, targets=TARGETS):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Invalid original size")
    if not isinstance(expected_sha1,str) or not re.fullmatch("[0-9a-f]{40}",expected_sha1):
        raise UnsafeImage("Invalid identity pin")
    if hashlib.sha1(data).hexdigest()!=expected_sha1:
        raise UnsafeImage("Wrong original XBE")
    if not shutil.which("objdump"):raise UnsafeImage("Missing binutils")
    if not isinstance(targets,dict) or len(targets)<1 or len(targets)>6 or len(set(targets.values()))!=len(targets):
        raise UnsafeImage("Bad bounded targets")
    sections=executable_sections(data)
    if sum(n for _,n,_ in sections)>8*1024*1024:
        raise UnsafeImage("Executable budget")
    windows={}
    for key,address in targets.items():
        if not isinstance(key,str) or not re.fullmatch("[a-z_]{3,60}",key):
            raise UnsafeImage("Bad target key")
        windows[key]=classify_window(data,address,sections)
    output={
       "schema":"bo-private-original-bullet-damage-neighborhood-v1",
       "owner_original_xbe_sha1_verified":True,
       "targets":windows,
       "source_attribution":"Pinned upstream cheats.cpp identifies the soldier bullet receiver and LOCAL-PLAYER-only damage candidates; these are static disassembly neighborhoods, not friend/foe code.",
       "hud_tint_original_proven":False,
       "original_friend_foe_field_verified":False,
       "original_no_friendly_fire_hook_verified":False,
       "original_soldier_body_ally_identified":False,
       "live_game_thread_hook_installed":False,
       "original_game_assets_or_disassembly_exported":False,
       "caveat":"Fixed windows can include other functions, data and unreachable blocks; direct edges omit register-indirect calls; source-known invulnerable offset refers to local player cheat only."
    }
    if len(json.dumps(output,separators=(",",":")))>MAX_REPORT_BYTES:
        raise UnsafeImage("Report budget")
    return output

def main():
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument("--owner-xbe",required=True,type=Path)
    a.add_argument("--report",required=True,type=Path)
    opts=a.parse_args()
    p=opts.owner_xbe
    if p.is_symlink() or not p.is_file() or p.stat().st_size>MAX_FILE:
        raise UnsafeImage("Invalid original file")
    result=audit(p.read_bytes())
    opts.report.write_text(json.dumps(result,sort_keys=True,indent=2)+"\n",encoding="utf8")
    print("BO_OWNER_BULLET_NEIGHBORHOOD_PASS: read-only bounded source-confirmed damage neighborhoods, no ally/reticle proof")

if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError) as err:
        print("BO_OWNER_BULLET_NEIGHBORHOOD_BLOCKED: BOUNDED_OR_UNVERIFIED_INPUT")
        raise SystemExit(1)
