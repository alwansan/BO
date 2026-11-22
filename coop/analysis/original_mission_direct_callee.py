#!/usr/bin/env python3
"""Inspect ONE pinned original BLACK mission-launch direct CALL edge.

Previous private pinned XBE xref survey established an immediate direct call
from original sub_00124CF0 (launch attempt) to 0x0011C3C0 within a conservatively
first-linear-RET-clipped window. This does NOT prove call reachability, loader
semantics, ABI, mission ID, or scene lifecycle. No proprietary instructions,
bytes or string literals leave the private runner.
"""
import argparse,hashlib,json,re,sys
from pathlib import Path
from original_mission_choice_xrefs import inspect
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections

LAUNCH=0x00124CF0
DIRECT_TARGET_CANDIDATE=0x0011C3C0
def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Wrong owner image")
    if not re.fullmatch("[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Owner XBE hash mismatch")
    sections=executable_sections(data)
    launch=inspect(data,sections,LAUNCH)
    if not any(int(c["address"],16)==DIRECT_TARGET_CANDIDATE
               for c in launch["direct_call_target_candidates"]):
        raise UnsafeImage("Pinned original launch direct call source edge absent")
    callee=inspect(data,sections,DIRECT_TARGET_CANDIDATE)
    # Strictly bounded source-derived topology; NOT binary or disassembly.
    result={
        "schema":"bo-private-original-launch-direct-callee-candidate-v1",
        "owner_xbe_pinned_sha1_checked":True,
        "launch_source_address":"%08X"%LAUNCH,
        "direct_callee_candidate_address":"%08X"%DIRECT_TARGET_CANDIDATE,
        "source_launch_direct_edge_appears_before_first_linear_ret":True,
        "callee_derived_families":callee["instruction_families"],
        "callee_first_linear_ret_offset":callee["first_linear_ret_offset"],
        "callee_direct_target_hints":callee["direct_call_target_candidates"][:6],
        "callee_global_ref_hints":callee["bounded_global_address_hints"][:8],
        "original_loader_abi_proven":False,
        "actual_selected_mission_index_proven":False,
        "original_guest_stage_load_supported":False,
        "stage_loaded_callback_proven":False,
        "raw_proprietary_instructions_or_bytes_exported":False,
        "caveat":"Linear first-RET-limited x86 window; not CFG/branch reachability, loader ABI or semantics. Indirect calls and adjacent functions unresolved. Never execute target or claim a real stage loader from this static report."
    }
    payload=json.dumps(result,sort_keys=True,separators=(",",":"))
    if len(payload)>5500:raise UnsafeImage("Derived metadata privacy budget")
    return result
def main():
    a=argparse.ArgumentParser()
    a.add_argument("--owner-xbe",required=True,type=Path)
    a.add_argument("--report",required=True,type=Path)
    opt=a.parse_args()
    if opt.owner_xbe.is_symlink() or not opt.owner_xbe.is_file() or opt.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Unsafe original owner image input")
    result=audit(opt.owner_xbe.read_bytes())
    opt.report.write_text(json.dumps(result,sort_keys=True,separators=(",",":"))+"\n")
    print("BO_PRIVATE_ORIGINAL_MISSION_DIRECT_CALLEE_PASS: derived pointer hints only, NO verified game stage loader")
if __name__=="__main__":
    try:main()
    except (OSError,UnsafeImage) as exc:
        print("BO_ORIGINAL_LAUNCH_CALLEE_BLOCKED:"+type(exc).__name__,file=sys.stderr)
        sys.exit(1)
