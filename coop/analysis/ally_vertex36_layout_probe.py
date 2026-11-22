#!/usr/bin/env python3
"""Read-only candidate stride36 trailing-24B statistics, NOT a skin decoder.

For each owner MODEL with a head/torso/leg name tag, bounded referenced
vertices are sampled and six 4-byte lanes from byte 12..35 are classified.
No original raw words, flags, shader instructions, names, positions, matrices
or game files leave CI. A plausible byte-sum is NOT proof of skinning weights.
"""
import argparse
from collections import Counter
import json
import math
from pathlib import Path
import struct

from ally_model_geometry_gate import (
    LEVEL_FILES, MAX_FILE, MAX_MODELS_PER_FILE, STRIDES, candidate_models)
from model_inventory import span, u32, label

MAX_SAMPLES_PER_BUFFER=2048
MAX_BUFFERS=12
MAX_DRAW_PER_BUFFER=128
MAX_INDICES_PER_DRAW=131072
MAX_SAMPLES_REPORT=48
MAX_REPORT_BYTES=256*1024

def lane_counts(data,vertex_base,stride,indices,joints):
    """Aggregate summaries only: original float/u16/u8 values never returned."""
    result=[]
    for offset in range(12,36,4):
        counters=Counter()
        distinct=set()
        for index in indices:
            word=bytes(span(data,vertex_base+index*stride+offset,4))
            distinct.add(word)
            bytes4=tuple(word)
            f=struct.unpack("<f",word)[0]
            counters["observations"]+=1
            counters["zero_word"]+=word==b"\0"*4
            counters["finite_f32"]+=math.isfinite(f)
            counters["f32_0_to_1"]+=math.isfinite(f) and 0<=f<=1
            counters["f32_minus1_to_1"]+=math.isfinite(f) and -1<=f<=1
            counters["f32_positive_normalized_nonzero"]+=math.isfinite(f) and 0<f<=1
            counters["byte4_sum_255_plusminus_1"]+=254<=sum(bytes4)<=256
            counters["all_u8_below_joint_count"]+=all(b<joints for b in bytes4)
            counters["all_u8_below_4joint_limit"]+=all(b<4*joints for b in bytes4)
            a,b=struct.unpack("<HH",word)
            counters["both_u16_below_joint_count"]+=a<joints and b<joints
        # Do not output word or per-index values, only aggregate counters.
        result.append({"offset":offset,"distinct_word_patterns":len(distinct),**counters})
    return result

def inspect_model(data,base,joints):
    buffers=u32(data,base+0x24)
    if not 1<=buffers<=MAX_BUFFERS:raise ValueError("Model buffer count")
    table=base+u32(data,base+0x1c)
    span(data,table,buffers*0x1c)
    output=[]
    for bi in range(buffers):
        v=table+bi*0x1c
        stride=u32(data,v+0x14)
        if stride!=36:raise ValueError("Only original verified human stride36 accepted")
        draws=u32(data,v+0x10)
        if not 1<=draws<=MAX_DRAW_PER_BUFFER:raise ValueError("Draw count exceeded")
        descriptors=v+u32(data,v+0x0c)
        vertex=v+u32(data,v+0x04)
        span(data,descriptors,draws*0x30)
        indices=set()
        for di in range(draws):
            d=descriptors+di*0x30
            if u32(data,d+0x20)!=6:raise ValueError("Expected triangle strip")
            count=u32(data,d+0x24)
            if count>MAX_INDICES_PER_DRAW:raise ValueError("Index count budget")
            ind=d+u32(data,d+0x28)
            packed=span(data,ind,count*2)
            indices.update(i[0] for i in struct.iter_unpack("<H",packed))
        sampled=sorted(indices)[:MAX_SAMPLES_PER_BUFFER]
        # Validate 36 bytes for each referenced vertex *before* classifying.
        for i in sampled:
            span(data,vertex+i*36,36)
        lanes=lane_counts(data,vertex,stride,sampled,joints)
        output.append({
            "buffer_ordinal":bi,
            "referenced_indices":len(indices),
            "sampled_vertices":len(sampled),
            "sampled_fraction_denominator":len(indices),
            "trailing_24byte_word_lanes":lanes
        })
    return output

def inspect_stage(data,file):
    if not data or len(data)>MAX_FILE:raise ValueError("Asset bound")
    output=[]
    scanned=0
    for key,base,details in candidate_models(data):
        scanned+=1
        if scanned>MAX_MODELS_PER_FILE:raise ValueError("Model budget")
        readable=label(key)
        if not readable:continue
        try:
            buffers=inspect_model(data,base,details["bone_names"])
        except (ValueError,struct.error):
            output.append({"model_key":readable,"error":"OUT_OF_BOUNDS_OR_UNVERIFIED_LAYOUT"})
            continue
        output.append({"model_key":readable,"bone_name_count":details["bone_names"],"buffers":buffers})
    if len(output)>MAX_SAMPLES_REPORT:raise ValueError("Report model count")
    return {"file":file,"models_scanned":scanned,"models":output}

def audit(root):
    root=Path(root)
    if not root.is_dir() or root.is_symlink():raise ValueError("Invalid original owner data root")
    stages=[]
    for name in LEVEL_FILES:
        f=root/name
        if f.is_symlink() or not f.is_file() or f.stat().st_size>MAX_FILE:
            raise ValueError("Missing pinned original stage")
        stages.append(inspect_stage(f.read_bytes(),name))
    data={
        "schema":"BO-COOP-owned-stride36-lane-statistics-v1",
        "stages":stages,
        "files_checked":len(stages),
        "models_checked":sum(len(s["models"]) for s in stages),
        "model_layout_rejections":sum("error" in m for s in stages for m in s["models"]),
        "all_original_vertex_words_and_vertex_flags_redacted":True,
        "skin_weight_or_bone_index_offset_proven":False,
        "normals_uv_or_tangent_layout_proven":False,
        "model_vertex_pose_renderer_built":False,
        "interpretation":"Bounded aggregate lane statistics only. Joint-count checks and 255 byte sums are weak heuristics, NOT proof of skinning."
    }
    if len(json.dumps(data))>MAX_REPORT_BYTES:
        raise ValueError("Report limit")
    return data

def main():
    arg=argparse.ArgumentParser(description=__doc__)
    arg.add_argument("--owner-root",type=Path,required=True)
    arg.add_argument("--report",type=Path,required=True)
    o=arg.parse_args()
    result=audit(o.owner_root)
    o.report.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n",encoding="utf8")
    print("BO_OWNER_VERTEX36_LANE_STATS_PASS: bounded metadata; no guessed weights/skin renderer")

if __name__=="__main__":
    try:main()
    except (ValueError,OSError) as ex:
        # Only fixed, non-sensitive error reason names leave private CI.
        reasons={
            "Report limit":"SAFE_REPORT_BUDGET",
            "Model budget":"MODEL_ENUMERATION_BUDGET",
            "Asset bound":"PRIVATE_ASSET_BUDGET",
            "Model buffer count":"MODEL_BUFFER_BUDGET",
            "Only original verified human stride36 accepted":"UNEXPECTED_VERTEX_STRIDE",
            "Draw count exceeded":"DRAW_COUNT_BUDGET",
            "Index count budget":"INDEX_COUNT_BUDGET",
            "Report model count":"REPORT_MODEL_LIMIT",
            "Missing pinned original stage":"PRIVATE_STAGE_MISSING",
            "Invalid original owner data root":"OWNER_ROOT_INVALID",
        }
        reason=reasons.get(str(ex),"UNVERIFIED_DATA_RANGE_OR_FORMAT")
        print("BO_OWNER_VERTEX36_LANE_STATS_BLOCKED: "+reason)
        raise SystemExit(1)
