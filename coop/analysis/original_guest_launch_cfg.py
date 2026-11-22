#!/usr/bin/env python3
"""Bounded private entry-rooted BLACK menu CALL graph; no game execution/ABI."""
import argparse,hashlib,json,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections
from world_xref_static import OP_LINE

WINDOW=1024
ENTRIES={"launch":0x00124CF0,"callee":0x0011C3C0,
         "child_a":0x0011BC00,"child_b":0x001B6EB0}
WANTED={"launch":[ENTRIES["callee"]],
        "callee":[ENTRIES["child_a"],ENTRIES["child_b"]]}
DIRECT=re.compile(r"^(?:0x)?([0-9a-f]{5,8})(?:\s*<[^>]+>)?$",re.I)
def target(s):
    m=DIRECT.fullmatch(s.strip().lower())
    return int(m.group(1),16) if m else None

def decode(image,sections,at):
    offsets=[raw+at-va for va,n,raw in sections if va<=at and at+WINDOW<=va+n]
    if len(offsets)!=1:raise UnsafeImage("Ambiguous original executable mapping")
    with tempfile.TemporaryDirectory(prefix="bo-private-guest-cfg-") as d:
        p=Path(d)/"in.bin"
        p.write_bytes(image[offsets[0]:offsets[0]+WINDOW])
        r=subprocess.run(["objdump","-D","-b","binary","-m","i386",
            "-M","intel","--no-show-raw-insn","--adjust-vma="+hex(at),str(p)],
            capture_output=True,text=True,timeout=25)
        if r.returncode:raise UnsafeImage("Private decoder failed")
    decoded={}
    for line in r.stdout.splitlines():
        m=OP_LINE.match(line)
        if m and at<=int(m.group(1),16)<at+WINDOW:
            decoded[int(m.group(1),16)]=(m.group(2).lower(),m.group(3).lower())
    if not 3<=len(decoded)<=WINDOW:raise UnsafeImage("Bad decoded candidate window")
    order=sorted(decoded)
    fall={pc:(order[i+1] if i+1<len(order) else None)
          for i,pc in enumerate(order)}
    return decoded,fall

def inspect(decoded,fall,start,wanted):
    pending=[start];visited=set();calls=set()
    branches=returns=outside=0
    while pending:
        pc=pending.pop()
        if pc in visited:continue
        if len(visited)>=WINDOW:raise UnsafeImage("CFG candidate budget")
        if pc not in decoded:
            outside+=1;continue
        visited.add(pc)
        op,args=decoded[pc]
        if op.startswith(("ret","iret","ud2","hlt","int3")):
            returns+=int(op.startswith("ret"))
            continue
        if op in ("jmp","jmpq"):
            to=target(args)
            if to in decoded:pending.append(to)
            else:outside+=1
            continue
        if op.startswith("j") or op.startswith("loop"):
            branches+=1
            to=target(args)
            if to in decoded:pending.append(to)
            else:outside+=1
        if op in ("call","callq"):
            to=target(args)
            if to is not None:calls.add(to)
            else:outside+=1
        if fall.get(pc) is None:outside+=1
        else:pending.append(fall[pc])
    return {"bounded_entry_reachable_instructions":len(visited),
            "direct_conditional_branch_candidates":branches,
            "direct_return_candidates":returns,
            "unresolved_indirect_or_external_edges":outside,
            "expected_direct_calls_reachable_candidates":{
                "%08X"%v:(v in calls) for v in wanted},
            "original_abi_verified":False,"complete_cfg_verified":False}

def audit(image,pinned=OWNER_SHA1):
    if not isinstance(image,(bytes,bytearray)) or not 0<len(image)<=MAX_FILE:
        raise UnsafeImage("Invalid input length")
    if not re.fullmatch(r"[0-9a-f]{40}",pinned or "") or hashlib.sha1(image).hexdigest()!=pinned:
        raise UnsafeImage("Original XBE hash mismatch")
    if not shutil.which("objdump"):raise UnsafeImage("Missing private decoder")
    sections=executable_sections(image)
    if sum(n for _,n,_ in sections)>8*1024*1024:
        raise UnsafeImage("Oversized executable")
    rows={}
    for name,entry in ENTRIES.items():
        ops,nxt=decode(image,sections,entry)
        rows[name]=inspect(ops,nxt,entry,WANTED.get(name,[]))
    result={"schema":"bo-original-guest-start-cfg-candidates-v1",
            "pinned_owner_original_xbe":True,"bounded_entry_bytes":WINDOW,
            "control_flow_candidate_graphs":rows,
            "guest_mission_auto_start_implemented":False,
            "actual_original_scene_loaded_verified":False,
            "real_engine_callable_abi_proven":False,
            "private_code_or_disassembly_exported":False,
            "caveat":"Static entry-rooted branch candidate graph, not runtime path or full CFG. Indirect flows, wrong instruction boundaries, tail calls and engine lifecycle remain unknown."}
    if len(json.dumps(result,separators=(",",":")).encode())>3500:
        raise UnsafeImage("Privacy output cap")
    return result

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--owner-xbe",type=Path,required=True)
    p.add_argument("--report",type=Path,required=True)
    a=p.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Invalid source file")
    result=audit(a.owner_xbe.read_bytes())
    a.report.write_text(json.dumps(result,sort_keys=True,separators=(",",":"))+"\n")
    for name in ("launch","callee"):
        row=result["control_flow_candidate_graphs"][name]
        print("BO_ORIGINAL_LAUNCH_ENTRY_CFG %s candidates=%d reachable_known_direct_calls=%d unresolved=%d NO_ABI" %
              (name,row["bounded_entry_reachable_instructions"],
               sum(row["expected_direct_calls_reachable_candidates"].values()),
               row["unresolved_indirect_or_external_edges"]))
    print("BO_ORIGINAL_GUEST_LAUNCH_CFG_STATIC_PASS: NO AUTO START")
if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError,subprocess.TimeoutExpired) as e:
        print("BO_ORIGINAL_GUEST_LAUNCH_CFG_BLOCKED:"+type(e).__name__,file=sys.stderr)
        sys.exit(1)
