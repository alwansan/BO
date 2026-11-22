#!/usr/bin/env python3
"""Bounded/private pinned XBE game-core phase reference audit.
The MIT PC port describes core+0x5AE0 == 0x1C as playing; no scene proof.
"""
import argparse,hashlib,json,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,executable_sections
from world_xref_static import OP_LINE
WINDOW=2048
TARGETS={"update":0x001F1080,"stream":0x001EDC90,"spawn":0x0020BC40}
CORE=re.compile(r"\b(?:byte|word|dword) ptr \[e(?:ax|bx|cx|dx|si|di|bp|sp)\+0x5ae0\]",re.I)
CMP=re.compile(r"(?:^|,)\s*0x1c(?:\s|$)",re.I)
def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Bad owner XBE")
    if not re.fullmatch("[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Wrong pinned owner XBE")
    if not shutil.which("objdump"):raise UnsafeImage("No x86 decoder")
    sec=executable_sections(data)
    windows={}
    for label,entry in TARGETS.items():
        raw=[ofs+entry-va for va,n,ofs in sec if va<=entry and entry+WINDOW<=va+n]
        if len(raw)!=1:raise UnsafeImage("Unmapped or ambiguous executable candidate")
        with tempfile.TemporaryDirectory(prefix="bo-private-corephase-") as tmp:
            f=Path(tmp)/"private.bin";f.write_bytes(data[raw[0]:raw[0]+WINDOW])
            decoded=subprocess.run(["objdump","-D","-b","binary","-m","i386","-M","intel",
                  "--no-show-raw-insn","--adjust-vma="+hex(entry),str(f)],
                  text=True,capture_output=True,timeout=20)
            if decoded.returncode:raise UnsafeImage("Private decoder failed")
        ops=[]
        for line in decoded.stdout.splitlines():
            m=OP_LINE.match(line)
            if m and entry<=int(m.group(1),16)<entry+WINDOW:
                ops.append((int(m.group(1),16),m.group(2).lower(),m.group(3).lower()))
        if not 3<=len(ops)<=WINDOW:raise UnsafeImage("Unexpected decoded window")
        found=[];neighbor=[]
        for i,(at,op,args) in enumerate(ops):
            if CORE.search(args):
                found.append(at-entry)
                if any(op2 in ("cmp","sub","test") and CMP.search(args2)
                       for _,op2,args2 in ops[i+1:i+5]):
                    neighbor.append(at-entry)
        windows[label]={"entry":"%08X"%entry,"decoded":len(ops),
            "relative_5ae0_mentions":len(found),"near_1c_compare_candidates":len(neighbor),
            "first_offsets":found[:4],"first_near_compare_offsets":neighbor[:4]}
    report={"schema":"bo-core-phase-candidate-private-v1",
      "owner_source_pinned":True,
      "pc_reference":"portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628",
      "source_value":"core+0x5AE0==0x1C","window_bytes_each":WINDOW,
      "windows":windows,"core_phase_semantics_proven":False,
      "scene_loaded_proven":False,"safe_guest_loader_proven":False,
      "remote_actor_proven":False,"original_bytes_or_disassembly_exported":False,
      "warning":"Static linear bounded windows can include adjacent code; nearby compare may use another value, no proof of CFG/runtime/READY. Absence not global absence."}
    if len(json.dumps(report,separators=(",",":")))>3000:raise UnsafeImage("Privacy budget exceeded")
    return report
def main():
    p=argparse.ArgumentParser()
    p.add_argument("--owner-xbe",required=True,type=Path)
    p.add_argument("--report",required=True,type=Path)
    a=p.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Unsafe private input")
    d=audit(a.owner_xbe.read_bytes())
    a.report.write_text(json.dumps(d,sort_keys=True,separators=(",",":"))+"\n")
    for key,r in d["windows"].items():
        print("BO_CORE_PHASE_HINT %s memory=%d near1c=%d NOT_READY"%(
           key,r["relative_5ae0_mentions"],r["near_1c_compare_candidates"]))
    print("BO_PRIVATE_CORE_PHASE_PASS: only source-derived numeric counters")
if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError,subprocess.TimeoutExpired) as e:
        print("BO_PRIVATE_CORE_PHASE_BLOCKED:"+type(e).__name__,file=sys.stderr)
        sys.exit(1)
