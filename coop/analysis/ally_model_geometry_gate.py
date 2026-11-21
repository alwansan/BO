#!/usr/bin/env python3
"""PRIVATE read-only owner MODEL 0x23 candidate geometry sanity gate.

Checks descriptor-relative indices and vertex geometry without exporting any
models, matrices, bones, textures, raw positions or source assets. A candidate
with head/torso/leg bone-name tags is NOT an ally, nor a verified animated rig.
"""
import argparse
import json
import math
from pathlib import Path
import struct
from model_inventory import MAGICS, label, model, span, u32, u64

MODEL_MAX_BUFFERS=16
MODEL_MAX_DRAWS=128
DRAW_MAX_INDICES=131072
MODEL_MAX_TOTAL_INDICES=2*1024*1024
MAX_MODELS_PER_FILE=256
MAX_REPORT_CANDIDATES=48
MAX_FILE=8*1024*1024
STRIDES=(24,28,36)
# Explicitly limited to existing owner-provided extracted assets. No game ISO.
LEVEL_FILES=(
    "levels/level_00/stg_0001/StLevel.bin",
    "levels/level_07/stg_0001/StLevel.bin",
    "levels/level_08/stg_0001/StLevel.bin",
)

def candidate_models(data):
    seen=set()
    for magic in MAGICS:
        needle=struct.pack("<Q",magic)
        cursor=0
        while (at:=data.find(needle,cursor))>=0:
            cursor=at+8
            try:
                count=u32(data,at+8)
                if count>16384: continue
                table=at+u32(data,at+12)
                span(data,table,count*16)
            except (ValueError,struct.error):continue
            for i in range(count):
                try:
                    entry=table+i*16
                    key=u64(data,entry)
                    start=at+u32(data,entry+8)
                    if (start,key) in seen:continue
                    details=model(data,start)
                    seen.add((start,key))
                    if details["full_body_candidate"]:
                        yield key,start,details
                except (ValueError,UnicodeError,struct.error):continue

def bounded_geometry(data,base):
    """The model's V and D offsets have DIFFERENT relocation bases."""
    buffers=u32(data,base+0x24)
    if not 1<=buffers<=MODEL_MAX_BUFFERS:
        raise ValueError("Unusable buffer count")
    array=base+u32(data,base+0x1C)
    span(data,array,buffers*0x1C)
    total_draws,total_indices,nonrepeat,referenced=0,0,0,set()
    for b in range(buffers):
        v=array+0x1C*b
        stride=u32(data,v+0x14)
        draws=u32(data,v+0x10)
        if stride not in STRIDES or draws>MODEL_MAX_DRAWS:
            raise ValueError("Unknown vertex descriptor layout")
        vertex=v+u32(data,v+0x04)
        descriptors=v+u32(data,v+0x0C)
        span(data,descriptors,draws*0x30)
        for di in range(draws):
            d=descriptors+0x30*di
            if u32(data,d+0x20)!=6:
                raise ValueError("Unsupported triangle primitive")
            count=u32(data,d+0x24)
            if count>DRAW_MAX_INDICES or total_indices+count>MODEL_MAX_TOTAL_INDICES:
                raise ValueError("Excessive model draw budget")
            indices=d+u32(data,d+0x28)
            raw=span(data,indices,2*count)
            # Use descriptor-relative indices, then V-relative vertices.
            ids=struct.unpack("<"+"H"*count,raw) if count else ()
            for index in set(ids):
                pos=vertex+index*stride
                xyz=struct.unpack("<3f",span(data,pos,12))
                if not all(math.isfinite(v) for v in xyz):
                    raise ValueError("Nonfinite geometry")
                referenced.add((b,index))
            nonrepeat+=sum(len(set(ids[i-2:i+1]))==3 for i in range(2,count))
            total_indices+=count
            total_draws+=1
    if not total_draws or not total_indices or not nonrepeat or not referenced:
        raise ValueError("Candidate lacks indexed surface geometry")
    return {
        "buffers_checked":buffers,
        "draw_descriptors_checked":total_draws,
        "index_references_checked":total_indices,
        "nonrepeat_strip_triangle_candidates":nonrepeat,
        "distinct_buffer_vertex_references":len(referenced),
        "finite_referenced_positions":True,
    }

def analyze_file(data,filename):
    if not 0<len(data)<=MAX_FILE: raise ValueError("Asset budget exceeded")
    successes={};failures=0;seen_count=0
    for key,base,details in candidate_models(data):
        seen_count+=1
        if seen_count>MAX_MODELS_PER_FILE:raise ValueError("Model candidate budget exceeded")
        try:
            g=bounded_geometry(data,base)
        except (ValueError,struct.error,OverflowError):
            failures+=1;continue
        readable=label(key)
        # Only owner filename & already known base40 key, no bone strings.
        if not readable or len(readable)>12:continue
        # Same key may occur multiple times. Keep only bounded metadata.
        existing=successes.get(readable)
        if existing is None or g["nonrepeat_strip_triangle_candidates"]>existing["nonrepeat_strip_triangle_candidates"]:
            successes[readable]={
                "key":readable,"file":filename,"bone_name_count":details["bone_names"],
                "bone_tag_classes":details["bone_tags"],**g,
                "full_body_identity_proven":False,
                "rig_and_skinning_proven":False,
            }
    return list(successes.values()),failures,seen_count

def audit(root):
    root=Path(root)
    if not root.is_dir() or root.is_symlink():raise ValueError("Invalid owner dir")
    candidates=[];rejected=0;examined=0
    for relative in LEVEL_FILES:
        p=root/relative
        if not p.is_file() or p.is_symlink() or p.stat().st_size>MAX_FILE:
            raise ValueError("Missing or invalid bounded owner asset")
        raw=p.read_bytes()
        found,bad,seen=analyze_file(raw,relative)
        rejected+=bad;examined+=seen;candidates.extend(found)
    # Retain separate scene-resource contexts; no original geometry.
    if len(candidates)>MAX_REPORT_CANDIDATES:
        raise ValueError("Excessive asset report size")
    return {
        "schema":"BO-COOP-owner-ally-geometry-candidate-v1",
        "source":"pinned owner biso extracted assets, read-only in CI",
        "files_read":len(LEVEL_FILES),
        "metadata_full_body_candidates_examined":examined,
        "metadata_candidates_with_geometry_checks_failed":rejected,
        "geometry_checked_candidate_records":len(candidates),
        "candidates":sorted(candidates,key=lambda x:(x["file"],x["key"])),
        "raw_assets_or_vertices_exported":False,
        "rig_skinning_materials_animation_rendering_verified":False,
        "friendly_ally_or_remote_player_identity_verified":False,
        "actual_black_actor_spawned":False,
        "caveat":"Bone-name tags and finite indexed surface geometry only. MODEL buffers, animation RCB hierarchy, skin weights, materials, faction and safe actor ownership remain unproven."
    }

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--owner-root",required=True,type=Path)
    p.add_argument("--report",required=True,type=Path)
    options=p.parse_args()
    result=audit(options.owner_root)
    options.report.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n",encoding="utf8")
    print("BO_ALLY_OWNER_GEOMETRY_CANDIDATE_PASS: bounded derived counts only; not a renderable ally proof")

if __name__=="__main__":
    try: main()
    except (ValueError,OSError) as error:
        print("BO_ALLY_OWNER_GEOMETRY_CANDIDATE_BLOCKED: "+type(error).__name__)
        raise SystemExit(1)
