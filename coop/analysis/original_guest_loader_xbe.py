#!/usr/bin/env python3
"""Pinned owner XBE private read-only audit of ORIGINAL frontend -> launch -> stream.
No recompilation, guest calls, XBE bytes, disassembly, names, private pointers
or code exported. This does NOT prove the correct guest auto-launch ABI.
"""
import argparse,hashlib,json,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections
from world_xref_static import OP_LINE

WINDOW=1536
TARGETS={
  "original_frontend_tick":0x00115030,
  "original_menu_launch_attempt":0x00124CF0,
  "original_stream_loader_entry":0x001EDC90,
}
DIRECT_LEVEL=re.compile(r"\bbyte ptr \[[a-z0-9_]+\+0x2020c\]",re.I)
DIRECT_STAGE=re.compile(r"\bbyte ptr \[[a-z0-9_]+\+0x2020e\]",re.I)

def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Owner image input invalid")
    if not re.fullmatch(r"[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Owner image not pinned")
    if not shutil.which("objdump"):raise UnsafeImage("No bounded private decoder")
    sections=executable_sections(data)
    summary={}
    for label,entry in TARGETS.items():
        offsets=[raw+entry-va for va,n,raw in sections
                 if va<=entry and entry+WINDOW<=va+n]
        if len(offsets)!=1:raise UnsafeImage("Candidate not uniquely mapped")
        with tempfile.TemporaryDirectory(prefix="bo-private-guest-loader-") as d:
            part=Path(d)/"private.bin"
            part.write_bytes(data[offsets[0]:offsets[0]+WINDOW])
            x=subprocess.run(["objdump","-D","-b","binary","-m","i386","-M","intel",
                "--no-show-raw-insn","--adjust-vma="+hex(entry),str(part)],
                text=True,capture_output=True,timeout=20)
            if x.returncode:raise UnsafeImage("Private disassembler failed")
        ops=[]
        for line in x.stdout.splitlines():
            m=OP_LINE.match(line)
            if m and entry<=int(m.group(1),16)<entry+WINDOW:
                ops.append((m.group(2).lower(),m.group(3).lower()))
        if not 3<=len(ops)<=WINDOW:raise UnsafeImage("Unexpected bounded decode")
        row={
            "mapped_original_executable_candidate":True,
            "private_window_bytes":WINDOW,
            "decoded_op_count":len(ops),
            "near_call_candidates":sum(op.startswith("call") for op,_ in ops),
            "return_candidates":sum(op.startswith("ret") for op,_ in ops),
            "source_level_byte_read_candidates":sum(bool(DIRECT_LEVEL.search(args)) for _,args in ops),
            "source_stage_byte_read_candidates":sum(bool(DIRECT_STAGE.search(args)) for _,args in ops),
            "proven_unique_function_boundary":False,
            "safe_original_menu_start_abi_verified":False,
            "safe_guest_loader_abi_verified":False
        }
        summary[label]=row
    report={
        "schema":"bo-owner-guest-loader-bridge-private-v1",
        "owner_original_xbe_sha1_pinned":True,
        "source_android_reference":"RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806",
        "pc_lab_reference":"portingpete/Black-2006-Recomp@b2512fafd scripts/generate.py:391-398",
        "candidates":summary,
        "original_game_guest_mission_start_verified":False,
        "true_game_scene_loaded_callback_verified":False,
        "multiplayer_or_npc_rendering_verified":False,
        "private_executable_data_exported":False,
        "note":"Bounded linear windows may include later functions and data; 2020c/2020e reads confirm field mention only, NOT control-flow reachability or a callable launcher. PC level override is not a scene loader."
    }
    if len(json.dumps(report,separators=(",",":")))>3800:
        raise UnsafeImage("Private output budget exceeded")
    return report

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--owner-xbe",required=True,type=Path)
    p.add_argument("--report",required=True,type=Path)
    a=p.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Invalid private input path")
    r=audit(a.owner_xbe.read_bytes())
    a.report.write_text(json.dumps(r,sort_keys=True,separators=(",",":"))+"\n",encoding="utf-8")
    for label,value in r["candidates"].items():
        print("BO_ORIGINAL_GUEST_LOADER_HINT %s executable=%d calls=%d level_reads=%d stage_reads=%d NO_ENGINE_ABI"%(
            label,int(value["mapped_original_executable_candidate"]),
            value["near_call_candidates"],value["source_level_byte_read_candidates"],
            value["source_stage_byte_read_candidates"]))
    print("BO_PRIVATE_GUEST_LOADER_SOURCE_SURVEY_PASS: 3 bounded numeric candidate windows, NO AUTO-LAUNCH")
if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError,subprocess.TimeoutExpired) as e:
        print("BO_ORIGINAL_GUEST_LOADER_SOURCE_SURVEY_BLOCKED:"+type(e).__name__,file=sys.stderr)
        sys.exit(1)
