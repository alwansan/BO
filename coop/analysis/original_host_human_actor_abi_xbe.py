#!/usr/bin/env python3
"""Private, pin-verified OWNER original BLACK human actor/physics ABI survey.
Only bounded executable-window counts are exported; no source XBE bytes,
instruction text, machine register contents, private paths or code listings.
An executable window is NOT a verified Android calling convention or actor.
"""
import argparse,hashlib,json,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections
from world_xref_static import OP_LINE

WINDOW=320
CANDIDATES={
 "owner_camera_initializer":0x00014380,
 "host_physics_proxy_acquire":0x000DF400,
 "host_physics_proxy_release":0x000DBBE0,
 "soldier_anim_select":0x0017E130,
 "soldier_model_init":0x0017E420,
 "soldier_pose_update":0x001801B0,
 "original_player_setup":0x001E0E40,
 "original_player_tick":0x001F1080,
}

def audit(image,pinned=OWNER_SHA1):
 if not isinstance(image,(bytes,bytearray)) or not 0<len(image)<=MAX_FILE:
  raise UnsafeImage("Invalid private XBE input")
 if not isinstance(pinned,str) or not re.fullmatch("[0-9a-f]{40}",pinned):
  raise UnsafeImage("Missing pinned owner identity")
 if hashlib.sha1(image).hexdigest()!=pinned:
  raise UnsafeImage("Wrong owner original BLACK image")
 if not shutil.which("objdump"):raise UnsafeImage("No private decoder")
 sections=executable_sections(image)
 result={}
 for label,addr in CANDIDATES.items():
  match=[(v,n,raw) for v,n,raw in sections if v<=addr and addr+WINDOW<=v+n]
  if len(match)!=1:raise UnsafeImage("Ambiguous original executable candidate")
  va,n,raw=match[0];start=raw+addr-va
  if start<0 or start+WINDOW>len(image):raise UnsafeImage("Image window out of bounds")
  with tempfile.TemporaryDirectory(prefix="bo-owner-actor-candidate-") as temp:
   source=Path(temp)/"private.bin";source.write_bytes(image[start:start+WINDOW])
   proc=subprocess.run(["objdump","-D","-b","binary","-m","i386","-M","intel",
      "--no-show-raw-insn","--adjust-vma="+hex(addr),str(source)],
      capture_output=True,text=True,timeout=12)
   if proc.returncode:raise UnsafeImage("Private original decoder failure")
  op=[]
  for line in proc.stdout.splitlines():
   m=OP_LINE.match(line)
   if m and addr<=int(m.group(1),16)<addr+WINDOW:
    op.append(m.group(2).lower())
  if not 3<=len(op)<=WINDOW:raise UnsafeImage("Unexpected decoded candidate length")
  result[label]={
   "unique_original_executable_candidate":True,
   "bytes_examined":WINDOW,
   "decoded_instruction_candidates":len(op),
   "possible_calls":sum(t.startswith("call") for t in op),
   "possible_returns":sum(t.startswith("ret") for t in op),
   "android_guest_abi_verified":False,
   "original_actor_runtime_created":False
  }
 report={
  "schema":"bo-original-black-host-human-actor-ABI-candidate-v1",
  "owner_xbe_sha1_pinned":True,
  "reference":"portingpete/Black-2006-Recomp@b2512fafd3/src/recomp_manual.c",
  "source_actor_bytes":0x900,"source_owner_bytes":0x16c0,
  "source_proxy_pool_capacity":20,
  "candidate_windows":result,
  "full_xbe_or_disassembly_exported":False,
  "android_runtime_guest_call_abi_proven":False,
  "host_extra_player_actor_allocated":False,
  "host_extra_physics_proxy_allocated":False,
  "shared_gameplay_or_ai_proven":False,
  "caveat":"Bounded linear decoding may cross data and function boundaries; candidate executable addresses do not prove any runtime ABI, actor lifetime, physics, original AI or multi-phone gameplay."
 }
 if len(json.dumps(report,separators=(",",":")))>3600:
  raise UnsafeImage("Unexpected private report length")
 return report

def main():
 p=argparse.ArgumentParser()
 p.add_argument("--owner-xbe",required=True,type=Path)
 p.add_argument("--report",required=True,type=Path)
 a=p.parse_args()
 if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
  raise UnsafeImage("Unsafe private owner XBE")
 report=audit(a.owner_xbe.read_bytes())
 a.report.write_text(json.dumps(report,sort_keys=True,separators=(",",":"))+"\n")
 print("OWNER_ORIGINAL_HUMAN_ACTOR_ABI_CANDIDATES_PASS:",
       len(report["candidate_windows"]),"private bounded executable windows; NO Android actor/physics/AI")
if __name__=="__main__":
 try:main()
 except (UnsafeImage,OSError,subprocess.TimeoutExpired) as e:
  print("OWNER_ORIGINAL_HUMAN_ACTOR_ABI_CANDIDATES_BLOCKED:"+type(e).__name__,file=sys.stderr)
  sys.exit(1)
