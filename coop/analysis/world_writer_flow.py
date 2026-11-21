#!/usr/bin/env python3
"""Bounded, PRIVATE x86 control-flow vicinity audit for owner BLACK world stores.

Use only inside a private CI runner on an exact SHA1-pinned XBE. Emits small
derived metadata only: no instructions, bytes, symbols, names or disassembly.
Linear disassembly is NOT proof of function boundary or runtime reachability.
"""
from collections import Counter, deque
import hashlib
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

from story_entry_static import MAX_FILE, OWNER_SHA1, UnsafeImage, executable_sections
from world_xref_static import OP_LINE, WORLD_ADDRESS, classify_xref, classify_write_source

EXPECTED = (0x0020BF48, 0x0020BF51)
MAX_BYTES = 8 * 1024 * 1024
MAX_TARGETS = 4
MAX_CONTEXT_OPS = 12
MAX_REFERENCES = 64
DIRECT_TARGET = re.compile(r"^\s*(?:0x)?([0-9a-fA-F]{5,8})(?:\s|$)")
REGISTER = re.compile(r"^(?:eax|ebx|ecx|edx|esi|edi|ebp|esp)$")


def instruction_family(op):
    if op.startswith("call"): return "call"
    if op.startswith("ret"): return "return"
    if op.startswith("jmp"): return "unconditional_branch"
    if op.startswith("j") or op.startswith("loop"): return "conditional_branch"
    if op in ("mov", "movzx", "movsx", "lea", "movd", "movss"): return "data_transfer"
    if op in ("xor", "sub", "add", "and", "or", "test", "cmp"): return "arithmetic_or_test"
    if op.startswith("push") or op.startswith("pop"): return "stack"
    return "other"


def direct_target(operands):
    """Do NOT infer an indirect jump through a pointer is a direct call."""
    if "[" in operands or "ptr " in operands.lower(): return None
    m = DIRECT_TARGET.match(operands)
    return int(m.group(1), 16) if m else None


def write_source_register(operands):
    items = operands.lower().split(",", 1)
    if len(items) != 2: return None
    src = items[1].strip()
    return src if REGISTER.fullmatch(src) else None


def _summarize_context(commands):
    c = Counter(instruction_family(op) for op, _ in commands)
    return dict(sorted(c.items()))


def analyze_context(data, expected_sha1=OWNER_SHA1, expected_stores=EXPECTED):
    if not isinstance(data, (bytes, bytearray)) or not 0 < len(data) <= MAX_FILE:
        raise UnsafeImage("Unexpected XBE byte count")
    if not re.fullmatch("[0-9a-f]{40}", expected_sha1 or "") or hashlib.sha1(data).hexdigest()!=expected_sha1:
        raise UnsafeImage("Owner XBE identity mismatch")
    if (len(expected_stores)<1 or len(expected_stores)>MAX_TARGETS or
        tuple(sorted(set(expected_stores)))!=tuple(expected_stores)):
        raise UnsafeImage("Invalid bounded world write target set")
    sections=executable_sections(data)
    if sum(length for _,length,_ in sections)>MAX_BYTES or not shutil.which("objdump"):
        raise UnsafeImage("Unexpected decoder or executable budget")
    before={}; after={k:[] for k in expected_stores}; source={}
    incoming_calls=[]; incoming_branches=[]; direct_call_count=0; branch_count=0
    section_number={}
    stores_seen={}
    lo=expected_stores[0]-192
    hi=expected_stores[-1]+192
    with tempfile.TemporaryDirectory(prefix="bo-private-writer-flow-") as temp:
        for section_index,(vaddr,length,raw) in enumerate(sections):
            if not any(vaddr<=k<vaddr+length for k in expected_stores):
                # Still need to check callers in ALL executable sections.
                pass
            filename=Path(temp)/("section"+str(section_index)+".bin")
            filename.write_bytes(data[raw:raw+length])
            try:
                p=subprocess.run([
                    "objdump","-D","-b","binary","-m","i386","-M","intel",
                    "--no-show-raw-insn","--adjust-vma="+hex(vaddr),str(filename)
                ],capture_output=True,text=True,check=False,timeout=55)
            except subprocess.TimeoutExpired as error:
                raise UnsafeImage("Private decoder timeout") from error
            if p.returncode: raise UnsafeImage("Private decoder failed")
            prev=deque(maxlen=MAX_CONTEXT_OPS)
            pending={k:0 for k in expected_stores}
            for line in p.stdout.splitlines():
                match=OP_LINE.match(line)
                if not match: continue
                ip=int(match.group(1),16)
                if not vaddr<=ip<vaddr+length: continue
                op=match.group(2).lower()
                args=match.group(3)
                # Collect only mnemonic categories; never persist code bytes.
                for key in expected_stores:
                    if pending[key]>0 and len(after[key])<MAX_CONTEXT_OPS:
                        after[key].append((op,args))
                        pending[key]-=1
                if ip in expected_stores:
                    if ip in stores_seen: raise UnsafeImage("Ambiguous writer address")
                    if classify_xref(op,args)!="write_candidate":
                        raise UnsafeImage("Expected world write instruction not found")
                    if classify_write_source(args)!="register":
                        raise UnsafeImage("Writer no longer register-sourced")
                    reg=write_source_register(args)
                    if reg is None: raise UnsafeImage("Unexpected writer register format")
                    stores_seen[ip]=True
                    section_number[ip]=section_index
                    source[ip]=reg
                    before[ip]=list(prev)
                    pending[ip]=MAX_CONTEXT_OPS
                # A direct CALL/JUMP near the site is only a candidate.
                if op.startswith(("call","j","loop")):
                    target=direct_target(args)
                    if target is not None and lo<=target<=hi:
                        if op.startswith("call"):
                            direct_call_count+=1
                            if len(incoming_calls)<MAX_REFERENCES:
                                incoming_calls.append({"from":"%08X"%ip,"target":"%08X"%target})
                        else:
                            branch_count+=1
                            if len(incoming_branches)<MAX_REFERENCES:
                                incoming_branches.append({"from":"%08X"%ip,"target":"%08X"%target})
                prev.append((op,args))
    if set(stores_seen)!=set(expected_stores):
        raise UnsafeImage("Pinned owner stores missing in executable section")
    descriptors=[]
    for addr in expected_stores:
        prev=before[addr]
        last_few=prev[-6:]
        # This is only a *hint*; the source register may have been reassigned.
        recent_zero_setter=any(
            (op=="xor" and args.replace(" ","").lower()==source[addr]+","+source[addr]) or
            (op=="mov" and args.replace(" ","").lower() in (
                source[addr]+",0x0",source[addr]+",0"))
            for op,args in last_few
        )
        descriptors.append({
            "writer_address":"%08X"%addr,
            "source_register":source[addr],
            "same_linear_section_index":section_number[addr],
            "preceding_instruction_family_counts":_summarize_context(prev),
            "following_instruction_family_counts":_summarize_context(after[addr]),
            "possible_zeroed_register_in_previous_six":recent_zero_setter,
            "instructions_sampled_before":len(prev),
            "instructions_sampled_after":len(after[addr])
        })
    return {
        "schema":"BO-COOP-world-writer-neighborhood-v1",
        "sha1_pinned_owner_xbe_matched":True,
        "expected_register_world_writes":len(expected_stores),
        "world_global":"%08X"%WORLD_ADDRESS,
        "store_address_separation_bytes":[expected_stores[i]-expected_stores[i-1] for i in range(1,len(expected_stores))],
        "writers":descriptors,
        "nearby_direct_call_targets_count":direct_call_count,
        "nearby_direct_branch_targets_count":branch_count,
        "bounded_direct_call_refs":incoming_calls,
        "bounded_direct_branch_refs":incoming_branches,
        "full_binary_or_instructions_exported":False,
        "verified_scene_loader":False,
        "verified_game_thread_hook":False,
        "verified_mission_or_scene_epoch":False,
        "caveat":"Linear x86 disassembly can cross data and indirect calls are not resolved. Nearness, source register and direct control references do not prove executable reachability, actual world creation or BLACK mission scene load."
    }
