#!/usr/bin/env python3
"""Private pinned original XBE static mission-menu xrefs. NO scene proof."""
from collections import Counter
import argparse,hashlib,json,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections
from world_xref_static import OP_LINE
from world_writer_flow import instruction_family
TARGETS={"mission_launch_candidate":0x124CF0,"level_choice_tick":0x11B890,"difficulty_bindings":0x124F30}
WINDOW=768
MEM=re.compile(r"(?:\[(?:ds:)?0x([0-9a-f]+)\]|\bds:0x([0-9a-f]+)\b)",re.I)
KNOWN_OPEN_TABLE=0x2D0D9E
def inspect(data,sections,address):
    offsets=[raw+address-va for va,n,raw in sections if va<=address and address+WINDOW<=va+n]
    if len(offsets)!=1:raise UnsafeImage("Mission candidate not uniquely executable")
    with tempfile.TemporaryDirectory(prefix="bo-original-mission-private-") as folder:
        p=Path(folder)/"private.bin";p.write_bytes(data[offsets[0]:offsets[0]+WINDOW])
        try:
            r=subprocess.run(["objdump","-D","-b","binary","-m","i386","-M","intel",
                              "--no-show-raw-insn","--adjust-vma="+hex(address),str(p)],
                             capture_output=True,text=True,timeout=20)
        except subprocess.TimeoutExpired as e:raise UnsafeImage("Decoder budget") from e
        if r.returncode:raise UnsafeImage("Mission decoder error")
    regs=Counter();families=Counter();first_ret=None;count=0
    access={}  # per target only derived read/write/other aggregate
    for line in r.stdout.splitlines():
        m=OP_LINE.match(line)
        if not m:continue
        pc=int(m.group(1),16)
        if not address<=pc<address+WINDOW:continue
        # Conservative linear first-RET fence, no adjacent unrelated code.
        # This is NOT a real control-flow graph or proof of reachability.
        if first_ret is not None and pc>address+first_ret:continue
        op,args=m.group(2).lower(),m.group(3).lower();count+=1
        if count>WINDOW:raise UnsafeImage("Instruction budget")
        families[instruction_family(op)]+=1
        if first_ret is None and op.startswith("ret"):first_ret=pc-address
        for ref in MEM.finditer(args):
            addr=int(ref.group(1) or ref.group(2),16)
            if 0x200000<=addr<=0x3fffff:
                regs[addr]+=1
                counts=access.setdefault(addr,{"read_candidate":0,"write_candidate":0,"other":0})
                first_operand=args.split(",",1)[0]
                # Only explicit original absolute memory destination could
                # be write; do not call it a mission selection field.
                matched=(ref.group(0) in first_operand)
                if matched and (op.startswith("mov") or op in ("inc","dec","add","sub","xor","and","or")):
                    counts["write_candidate"]+=1
                elif not matched and op!="lea":
                    counts["read_candidate"]+=1
                else:
                    counts["other"]+=1
    if count<3:raise UnsafeImage("Mission window unmapped")
    return {
       "window_bytes":WINDOW,"decoded_instruction_candidates":count,
       "instruction_families":dict(sorted(families.items())),
       "first_linear_ret_offset":first_ret,"absolute_global_xref_count":sum(regs.values()),
       "bounded_global_address_hints":[
           {"address":"%08X"%addr,"references":n,"access":access[addr]}
           for addr,n in sorted(regs.items(),key=lambda x:(-x[1],x[0]))[:16]],
       "known_original_mission_unlock_table_referenced":KNOWN_OPEN_TABLE in regs,
       "known_difficulty_selection_count_source_address_seen":0x2EC560 in regs,
       "known_game_object_global_seen":0x2D45F0 in regs,
       "known_ui_object_global_seen":0x2D199C in regs,
       "conservatively_clipped_at_first_linear_return":first_ret is not None,
       "selected_mission_field_proven":False,"scene_loaded_callback_proven":False
    }
def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Invalid owner image")
    if not re.fullmatch("[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Identity mismatch")
    if not shutil.which("objdump"):raise UnsafeImage("Decoder unavailable")
    sections=executable_sections(data)
    if sum(n for _,n,_ in sections)>8*1024*1024:raise UnsafeImage("Budget")
    entries={name:inspect(data,sections,at) for name,at in TARGETS.items()}
    result={
        "schema":"bo-private-original-mission-choice-global-hints-v2",
        "owner_image_pinned_sha1_verified":True,
        "entries":entries,
        "original_mission_id_proven":False,"guest_story_launch_supported":False,
        "original_scene_loaded_stage_epoch_verified":False,
        "raw_proprietary_bytes_or_disassembly_exported":False,
        "caveat":"Conservative first-RET clip is NOT a CFG; other jump paths/function boundaries may differ. Data references and read/write candidates are NOT selected mission field, stage loader or game-thread proof."
    }
    if len(json.dumps(result))>9500:raise UnsafeImage("Metadata privacy budget")
    return result
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--owner-xbe",required=True,type=Path)
    p.add_argument("--report",required=True,type=Path)
    a=p.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Owner source absent")
    a.report.write_text(json.dumps(audit(a.owner_xbe.read_bytes()),indent=2,sort_keys=True)+"\n")
    print("BO_ORIGINAL_MISSION_CHOICES_STATIC_PASS: owner-pinned xref hints only; no scene loader claimed")
if __name__=="__main__":
    try:main()
    except (OSError,UnsafeImage) as e:
        print("BO_ORIGINAL_MISSION_CHOICES_BLOCKED: "+type(e).__name__,file=sys.stderr)
        sys.exit(1)
