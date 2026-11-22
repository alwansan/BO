#!/usr/bin/env python3
"""Private original BLACK XBE direct-memory-field WIDTH evidence.

A static read of candidate fields is NOT a proven selected mission. The goal
is to stop accidentally treating a source BYTE as a 32-bit field when
probing game state. Export only bounded address/width/access categories,
NEVER original disassembly, instructions, strings, or game assets.
"""
from collections import Counter
import argparse,hashlib,json,re,subprocess,sys,tempfile
from pathlib import Path
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections
from original_mission_choice_xrefs import TARGETS,WINDOW,MEM
from world_xref_static import OP_LINE

FIELDS={
    "choice_candidate_A":0x002D0D60,
    "launch_candidate_B":0x002D0D68,
    "choice_dynamic_candidate":0x002EC598,
    "launch_state_candidate":0x002D12DC,
}
WIDTHS=("BYTE PTR","WORD PTR","DWORD PTR","QWORD PTR")
WRITES={"mov","movzx","movsx","inc","dec","add","sub","xor","and","or","xchg"}
def classify_memory_instruction(op,args,field):
    matches=[m for m in MEM.finditer(args) if int(m.group(1) or m.group(2),16)==field]
    if not matches:return None
    left=args.split(",",1)[0]
    # Conservative: count only destination writes of arithmetic or mov
    # (MOVZX/MOVSX destinations are registers, not memory in x86).
    target_on_left=any(m.group(0) in left for m in matches)
    if target_on_left and op in WRITES-{"movzx","movsx"}:role="write_candidate"
    elif op=="lea":role="address_only"
    else:role="read_candidate"
    width=next((name.split()[0] for name in WIDTHS if name in args.upper()),"UNSPECIFIED")
    # No offset-specific provenance can be inferred from width=UNSPECIFIED.
    return role,width

def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Unexpected pinned XBE bytes")
    if not re.fullmatch("[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Original owner XBE not pinned")
    sections=executable_sections(data)
    report={}
    for function,entry in TARGETS.items():
        address=int(entry)
        offsets=[raw+address-va for va,n,raw in sections
                 if va<=address and address+WINDOW<=va+n]
        if len(offsets)!=1:raise UnsafeImage("Original XBE source function not uniquely mapped")
        with tempfile.TemporaryDirectory(prefix="bo-mission-field-widths-private-") as temp:
            name=Path(temp)/"source.bin"
            name.write_bytes(bytes(data[offsets[0]:offsets[0]+WINDOW]))
            r=subprocess.run(["objdump","-D","-b","binary","-m","i386","-M","intel",
                              "--no-show-raw-insn",
                              "--adjust-vma="+hex(address),str(name)],
                             capture_output=True,text=True,timeout=20)
            if r.returncode:raise UnsafeImage("Original XBE private decode failed")
        observed={key:Counter() for key in FIELDS}
        total=0;first_ret=None
        for line in r.stdout.splitlines():
            m=OP_LINE.match(line)
            if not m:continue
            pc=int(m.group(1),16)
            if not address<=pc<address+WINDOW:continue
            if first_ret is not None and pc>first_ret:continue
            total+=1
            op,args=m.group(2).lower(),m.group(3).lower()
            for key,at in FIELDS.items():
                sample=classify_memory_instruction(op,args,at)
                if sample:
                    observed[key]["%s_%s"%sample]+=1
            if op.startswith("ret") and first_ret is None:first_ret=pc
        if total<3:raise UnsafeImage("Source window too short")
        report[function]={
            "first_linear_ret_bounded":first_ret is not None,
            "direct_memory_widths":{k:dict(sorted(v.items())) for k,v in observed.items() if v},
            "original_menu_choice_field_proven":False,
            "can_read_or_write_game_memory":False
        }
    result={
        "schema":"bo-original-xbe-mission-field-width-only-v1",
        "pinned_owner_xbe_sha1_checked":True,
        "window_bytes_each":WINDOW,
        "source_windows":report,
        "selected_campaign_mission_proven":False,
        "original_guest_scene_loader_proven":False,
        "read_only_source_evidence":True,
        "original_instructions_or_bytes_exported":False,
        "note":"Heuristic x86 direct global access width, bounded at first linear RET, NOT CFG or field identity. UNSPECIFIED does not imply 32-bit. Never modify source game memory based solely on this."
    }
    encoded=json.dumps(result,sort_keys=True,separators=(",",":"))
    if len(encoded.encode())>4500:raise UnsafeImage("Output budget exceeded")
    return result
def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--owner-xbe",type=Path,required=True)
    parser.add_argument("--report",type=Path,required=True)
    opt=parser.parse_args()
    if opt.owner_xbe.is_symlink() or not opt.owner_xbe.is_file() or opt.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Untrusted owner XBE input")
    result=audit(opt.owner_xbe.read_bytes())
    opt.report.write_text(json.dumps(result,sort_keys=True,separators=(",",":"))+"\n")
    print("ORIGINAL_MISSION_FIELD_WIDTHS_STATIC_PASS: bounded read-only source width hints, no selected mission/loader claim")
if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError,subprocess.TimeoutExpired) as exc:
        print("ORIGINAL_MISSION_WIDTHS_BLOCKED:"+type(exc).__name__,file=sys.stderr)
        sys.exit(1)
