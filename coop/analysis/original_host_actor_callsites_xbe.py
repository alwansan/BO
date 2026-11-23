#!/usr/bin/env python3
"""Pin-verified private original BLACK x86 actor/proxy direct-caller survey.
Only bounded control-flow *counts*, not guest bytes, instructions, file paths,
stack values or addresses, may leave the private CI working directory.
Linear disassembly can be wrong across embedded data; zero direct calls is
not proof a source function is unused (indirect calls exist).
"""
import argparse
from collections import Counter,deque
import hashlib,json,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from story_entry_static import MAX_FILE,OWNER_SHA1,UnsafeImage,executable_sections
from world_xref_static import OP_LINE

TARGETS={
 "camera_owner_setup":0x00014380,
 "proxy_acquire":0x000DF400,
 "proxy_release":0x000DBBE0,
 "original_actor_setup":0x001E0E40,
 "original_actor_tick":0x001F1080,
}
MAX_EXEC_BYTES=9*1024*1024
MAX_TOTAL_REFS=512
WINDOW_PRECEDING=8
DIRECT_CALL=re.compile(r"^\s*(?:0x)?([a-fA-F0-9]{5,8})(?:\s|$)")
REGS=("eax","ebx","ecx","edx","esi","edi","ebp")
REG_WRITE=re.compile(r"^\s*(eax|ebx|ecx|edx|esi|edi|ebp)\s*,",re.I)

def direct_target(mnemonic,operands):
 if mnemonic not in ("call","calll") or "[" in operands or "*" in operands:
  return None
 m=DIRECT_CALL.match(operands)
 return int(m.group(1),16) if m else None

def summarize_predecessors(rows):
 # Source register pattern only; no constants, memory offsets or actual
 # stack arguments are exported.
 written=set()
 pushes=0
 for op,arg in rows:
  if op in ("push","pushl"):pushes+=1
  if op.startswith(("mov","lea","xor","add","sub","and","or")):
   m=REG_WRITE.match(arg)
   if m:written.add(m.group(1).lower())
 return {"nearby_push_count":min(pushes,8),
         "registers_written":sorted(written),
         "window_size":len(rows)}

def audit(data,pinned=OWNER_SHA1):
 if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
  raise UnsafeImage("Invalid original image input")
 if not isinstance(pinned,str) or not re.fullmatch("[a-f0-9]{40}",pinned):
  raise UnsafeImage("Invalid pinned owner XBE hash")
 if hashlib.sha1(data).hexdigest()!=pinned:
  raise UnsafeImage("Owner XBE does not match pinned ORIGINAL game")
 if not shutil.which("objdump"):raise UnsafeImage("Private decoder unavailable")
 sections=executable_sections(data)
 if sum(length for _,length,_ in sections)>MAX_EXEC_BYTES:
  raise UnsafeImage("Unexpected executable budget")
 total=0
 counts={key:Counter() for key in TARGETS}
 before={key:Counter() for key in TARGETS}
 examples={key:0 for key in TARGETS}
 mapping={v:k for k,v in TARGETS.items()}
 for va,length,raw in sections:
  if length<=0 or raw<0 or raw+length>len(data):
   raise UnsafeImage("Bad pinned source section bounds")
  with tempfile.TemporaryDirectory(prefix="bo-actor-private-") as tmp:
   file=Path(tmp)/"code.bin";file.write_bytes(data[raw:raw+length])
   try:
    process=subprocess.run(["objdump","-D","-b","binary","-m","i386",
       "-M","intel","--no-show-raw-insn","--adjust-vma="+hex(va),str(file)],
       capture_output=True,text=True,timeout=55)
   except subprocess.TimeoutExpired as exc:
    raise UnsafeImage("Private control-flow survey timeout") from exc
   if process.returncode:raise UnsafeImage("Private decoder failure")
  recent=deque(maxlen=WINDOW_PRECEDING)
  for line in process.stdout.splitlines():
   m=OP_LINE.match(line)
   if not m:continue
   ip=int(m.group(1),16)
   if not va<=ip<va+length:continue
   op=m.group(2).lower();args=m.group(3).lower()
   dest=direct_target(op,args)
   if dest in mapping:
    total+=1
    if total>MAX_TOTAL_REFS:raise UnsafeImage("Unbounded original calls")
    key=mapping[dest];examples[key]+=1
    x=summarize_predecessors(recent)
    counts[key][x["nearby_push_count"]]+=1
    for reg in x["registers_written"]:before[key][reg]+=1
   recent.append((op,args))
 result={}
 for name in TARGETS:
  result[name]={
   "direct_call_candidates":examples[name],
   "nearby_push_histogram":{str(k):int(v) for k,v in sorted(counts[name].items())},
   "nearby_register_write_counts":{r:int(before[name][r]) for r in REGS},
   "x86_original_abi_proven":False,"android_guest_abi_proven":False,
  }
 output={
  "schema":"bo-original-human-actor-private-direct-calls-v1",
  "owner_original_xbe_sha1_pinned":True,
  "reference":"portingpete/Black-2006-Recomp@b2512fafd3 (PC only)",
  "source_candidate_entries":len(TARGETS),
  "direct_call_candidates_total":total,
  "direct_calls_not_equal_complete_callgraph":True,
  "target_summaries":result,
  "instructions_or_source_bytes_exported":False,
  "call_arguments_or_register_values_exported":False,
  "actor2_native_heap_allocated":False,
  "actor2_original_collision_proven":False,
  "note":"x86 linear call vicinity counts are NOT function ABI proof, original actor constructor safety, physical proxy ownership or 2-4 actual shared-world gameplay"
 }
 if len(json.dumps(output,separators=(",",":")))>4500:
  raise UnsafeImage("Private report exceeds safe size")
 return output

def main():
 p=argparse.ArgumentParser()
 p.add_argument("--owner-xbe",type=Path,required=True)
 p.add_argument("--report",type=Path,required=True)
 a=p.parse_args()
 if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
  raise UnsafeImage("Invalid XBE path")
 data=audit(a.owner_xbe.read_bytes())
 a.report.write_text(json.dumps(data,separators=(",",":"),sort_keys=True)+"\n")
 print("BO_ORIGINAL_EXTRA_ACTOR_DIRECT_CALL_CANDIDATES_PASS",
       data["direct_call_candidates_total"],"bounded private references, no ABI or game actor")
if __name__=="__main__":
 try:main()
 except (UnsafeImage,OSError,subprocess.TimeoutExpired) as exc:
  print("BO_ORIGINAL_EXTRA_ACTOR_DIRECT_CALL_CANDIDATES_BLOCKED:"+type(exc).__name__,
        file=sys.stderr)
  sys.exit(1)
