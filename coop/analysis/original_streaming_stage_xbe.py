#!/usr/bin/env python3
"""Private, bounded original BLACK streaming-entry source survey.

The MIT PC reference generate.py (pinned b2512faf) instruments
sub_001EDC90 after byte reads at +0x2020C and +0x2020E.
These are *candidate level/stage inputs*, not mission identity or READY.
Only small derived counters/booleans ever leave the ephemeral runner.
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

ENTRY = 0x001EDC90
WINDOW = 0x900
FIELDS = {"level_candidate": 0x2020C, "stage_candidate": 0x2020E}
MEM = re.compile(r"\bbyte\s+ptr\s*\[\s*(e(?:ax|bx|cx|dx|si|di|bp|sp))\s*\+\s*0x(2020c|2020e)\s*\]", re.I)
REG8 = frozenset(("al","ah","bl","bh","cl","ch","dl","dh"))
DECODER_DEADLINE_S = 20

def audit(data, pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Invalid owner image")
    if not re.fullmatch(r"[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Owner XBE is not the pinned original")
    if not shutil.which("objdump"):
        raise UnsafeImage("No private x86 decoder")
    sections=executable_sections(data)
    matches=[raw+ENTRY-va for va,n,raw in sections if va<=ENTRY and ENTRY+WINDOW<=va+n]
    if len(matches)!=1:
        raise UnsafeImage("Streaming candidate address not uniquely executable")
    with tempfile.TemporaryDirectory(prefix="bo-original-streaming-private-") as tmp:
        p=Path(tmp)/"private-code.bin"
        p.write_bytes(data[matches[0]:matches[0]+WINDOW])
        out=subprocess.run(["objdump","-D","-b","binary","-m","i386","-M","intel",
            "--no-show-raw-insn","--adjust-vma="+hex(ENTRY),str(p)],
            capture_output=True,text=True,timeout=DECODER_DEADLINE_S)
        if out.returncode:
            raise UnsafeImage("Private decode failed")
    counts={name:{"byte_reg_loads":0,"byte_other_accesses":0} for name in FIELDS}
    observed={"level_candidate":[],"stage_candidate":[]}
    lines=0
    for line in out.stdout.splitlines():
        m=OP_LINE.match(line)
        if not m:continue
        address=int(m.group(1),16)
        if not ENTRY<=address<ENTRY+WINDOW:continue
        lines+=1
        if lines>WINDOW:raise UnsafeImage("Unbounded decoder instructions")
        op,args=m.group(2).lower(),m.group(3).lower()
        field=MEM.search(args)
        if not field:continue
        off=int(field.group(2),16)
        label=next((k for k,v in FIELDS.items() if v==off),None)
        if label is None:continue
        first=args.split(",",1)[0].strip()
        if op in ("mov","movzx","movsx") and first in REG8 | frozenset(("eax","ebx","ecx","edx","esi","edi")):
            counts[label]["byte_reg_loads"]+=1
            if len(observed[label])<4: observed[label].append(address-ENTRY)
        else:
            counts[label]["byte_other_accesses"]+=1
    if lines<3: raise UnsafeImage("No executable streaming candidate")
    # Numeric offsets do not establish control-flow, provenance or stage-loading.
    report={
       "schema":"bo-private-original-streaming-byte-candidates-v1",
       "owner_xbe_sha1_pinned":True,
       "pc_reference":"portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628",
       "candidate_function":"001EDC90","window_bytes":WINDOW,
       "decoded_instruction_candidates":lines,
       "field_2020c":counts["level_candidate"],
       "field_2020e":counts["stage_candidate"],
       "first_up_to_four_relative_offsets":{
           "level":observed["level_candidate"],"stage":observed["stage_candidate"]
       },
       "both_candidate_byte_loads_present":all(counts[k]["byte_reg_loads"]>0 for k in FIELDS),
       "dynamic_call_reachability_proven":False,
       "actual_host_mission_identity_proven":False,
       "safe_guest_stage_loader_abi_proven":False,
       "actual_stage_loaded_callback_proven":False,
       "story_ready_or_coop_gameplay_proven":False,
       "raw_original_instructions_or_assets_exported":False,
       "caveat":"Linear 0x900 private decoder window can cross branches or embedded data. Byte loads are not a verified live scene loader or stage-completion event. Never launch guests or READY using this scan."
    }
    if len(json.dumps(report,sort_keys=True,separators=(",",":")))>3000:
        raise UnsafeImage("Privacy report exceeded")
    return report

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--owner-xbe",required=True,type=Path)
    parser.add_argument("--report",required=True,type=Path)
    a=parser.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Unsafe owner XBE path")
    result=audit(a.owner_xbe.read_bytes())
    a.report.write_text(json.dumps(result,sort_keys=True,separators=(",",":"))+"\n")
    print("BO_ORIGINAL_STREAMING_SOURCE_PASS: level_byte_reads=%d stage_byte_reads=%d both=%d not scene READY" % (
        result["field_2020c"]["byte_reg_loads"],
        result["field_2020e"]["byte_reg_loads"],
        result["both_candidate_byte_loads_present"]))

if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError,subprocess.TimeoutExpired) as exc:
        print("BO_ORIGINAL_STREAMING_SOURCE_BLOCKED:"+type(exc).__name__,file=sys.stderr)
        sys.exit(1)
