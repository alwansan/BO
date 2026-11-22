#!/usr/bin/env python3
"""Owner MODEL stride36 U8 skin influence *hypothesis testing*, not a decoder.
Only anonymized bounded aggregate counters leave CI; NEVER raw game bytes.
"""
import argparse
from collections import Counter
import json
from pathlib import Path
import struct
from ally_model_geometry_gate import LEVEL_FILES,MAX_FILE,MAX_MODELS_PER_FILE,candidate_models
from ally_vertex36_layout_probe import MAX_BUFFERS,MAX_DRAW_PER_BUFFER,MAX_INDICES_PER_DRAW,MAX_SAMPLES_PER_BUFFER
from model_inventory import span,u32

HYPOTHESES=((12,20),(12,16),(12,24),(16,20),(20,16),(24,32))
MAX_REPORT=196608

def pair_counts(data,base,indices,joints,weights_at,indices_at):
    if not 1<=joints<=64 or not 0<=weights_at<=32 or not 0<=indices_at<=32 or weights_at%4 or indices_at%4 or weights_at==indices_at:
        raise ValueError("Invalid hypothesis")
    if len(indices)>MAX_SAMPLES_PER_BUFFER:raise ValueError("Sample budget")
    c=Counter()
    for vertex in indices:
        if not isinstance(vertex,int) or vertex<0:raise ValueError("Invalid index")
        w=span(data,base+vertex*36+weights_at,4)
        ids=span(data,base+vertex*36+indices_at,4)
        c["samples"]+=1
        s=sum(w)
        c["sum_exact_255"]+=s==255
        c["sum_near_255"]+=254<=s<=256
        active=[i for i in range(4) if w[i]!=0]
        c["nonzero_weight_slots_"+str(len(active))]+=1
        all_ok=all(i<joints for i in ids)
        active_ok=bool(active) and all(ids[i]<joints for i in active)
        c["all_four_indices_in_range"]+=all_ok
        c["active_indices_in_range"]+=active_ok
        c["near_255_and_active_indices_in_range"]+=254<=s<=256 and active_ok
        c["invalid_only_in_zero_weight_slots"]+=active_ok and not all_ok
        c["at_least_one_active_index_out_of_range"]+=bool(active) and not active_ok
        c["active_duplicate_indices"]+=len({ids[i] for i in active})<len(active)
        # Alternative *index encoding* checks, still unverified even if they fit.
        # A bone palette may differ from MODEL bone-name count, and shader
        # constants can use multiplied row-register indices. Report only totals.
        for cap in (32,64,96):
            c["active_indices_below_"+str(cap)]+=bool(active) and all(ids[i]<cap for i in active)
        c["active_indices_tripled_rows_25"]+=bool(active) and all(
            ids[i]%3==0 and ids[i]//3<joints for i in active)
        c["active_indices_quad_rows_25"]+=bool(active) and all(
            ids[i]%4==0 and ids[i]//4<joints for i in active)
        c["active_index_high_byte_128"]+=any(ids[i]>=128 for i in active)
        c["active_index_25_to_63"]+=any(25<=ids[i]<64 for i in active)
        # Additional tests are NOT proof of the layout or a functioning GPU.
    return dict(sorted(c.items()))

def stage_scan(data,stage_ordinal):
    if not data or len(data)>MAX_FILE:raise ValueError("Stage budget")
    models=[]
    for ordinal,(_key,base,details) in enumerate(candidate_models(data)):
        if ordinal>=MAX_MODELS_PER_FILE:raise ValueError("Model budget")
        joints=details["bone_names"]
        n=u32(data,base+0x24)
        if not 1<=n<=MAX_BUFFERS:raise ValueError("Buffer budget")
        table=base+u32(data,base+0x1c)
        span(data,table,n*0x1c)
        buffers=[]
        for bi in range(n):
            v=table+bi*0x1c
            if u32(data,v+0x14)!=36:raise ValueError("Unexpected stride")
            draws=u32(data,v+0x10)
            if not 1<=draws<=MAX_DRAW_PER_BUFFER:raise ValueError("Draw budget")
            descriptors=v+u32(data,v+0x0c)
            vb=v+u32(data,v+0x04)
            span(data,descriptors,draws*0x30)
            refs=set()
            for di in range(draws):
                d=descriptors+di*0x30
                if u32(data,d+0x20)!=6:raise ValueError("Unexpected primitive")
                count=u32(data,d+0x24)
                if count>MAX_INDICES_PER_DRAW:raise ValueError("Index budget")
                raw=span(data,d+u32(data,d+0x28),count*2)
                refs.update(t[0] for t in struct.iter_unpack("<H",raw))
            sampled=sorted(refs)[:MAX_SAMPLES_PER_BUFFER]
            for i in sampled:span(data,vb+i*36,36)
            hypotheses=[{"weight_offset":w,"index_offset":idx,**pair_counts(data,vb,sampled,joints,w,idx)} for w,idx in HYPOTHESES]
            buffers.append({"ordinal":bi,"referenced_indices":len(refs),"sampled_indices":len(sampled),"hypotheses":hypotheses})
        models.append({"ordinal":ordinal,"bone_count":joints,"buffers":buffers})
    return {"stage_ordinal":stage_ordinal,"models":models}

def audit(root):
    root=Path(root)
    if not root.is_dir() or root.is_symlink():raise ValueError("Invalid root")
    stages=[]
    for i,name in enumerate(LEVEL_FILES):
        f=root/name
        if f.is_symlink() or not f.is_file() or f.stat().st_size>MAX_FILE:raise ValueError("Missing stage")
        stages.append(stage_scan(f.read_bytes(),i))
    result={"schema":"bo-owner-stride36-skin-influence-hypothesis-v1",
        "stage_count":len(stages),
        "model_candidates":sum(len(s["models"]) for s in stages),
        "buffer_candidates":sum(len(m["buffers"]) for s in stages for m in s["models"]),
        "hypotheses":[list(h) for h in HYPOTHESES],
        "stages":stages,
        "original_vertex_words_exported":False,
        "original_bone_indices_exported":False,
        "actual_shader_binding_proven":False,
        "real_bone_weights_decoded":False,
        "original_skinning_renderer_ready":False,
        "interpretation":"Conditional validity only; no proof of type, bone mapping, model-to-draw, or skinned animation."}
    if len(json.dumps(result,separators=(",",":")))>MAX_REPORT:raise ValueError("Report budget")
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--owner-root",type=Path,required=True)
    p.add_argument("--report",type=Path,required=True)
    a=p.parse_args()
    a.report.write_text(json.dumps(audit(a.owner_root),separators=(",",":"),sort_keys=True)+"\n",encoding="utf-8")
    print("BO_OWNER_SKIN_HYPOTHESES_AGGREGATE_PASS: no original words or proven skinning")

if __name__=="__main__":
    try:main()
    except (OSError,ValueError,struct.error):
        print("BO_OWNER_SKIN_HYPOTHESES_BLOCKED: PRIVATE_BOUNDED_LAYOUT")
        raise SystemExit(1)
