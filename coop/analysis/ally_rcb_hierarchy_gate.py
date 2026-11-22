#!/usr/bin/env python3
"""Read-only RCB v21 skeleton hierarchy + MODEL bone-name overlap on owner StLevel.

This does NOT decode skinning weights, identify the friendly ally, construct
poses, prove bind matrices or call BLACK. No original matrices, names, hashes,
bones, coordinates, animation clips or raw asset bytes leave CI.
Reference: Black-2006-Recomp@b2512f... animation.md (RCB fixups, 65599 hash).
"""
import argparse
import hashlib
import json
import math
import pathlib
import struct

from ally_model_geometry_gate import LEVEL_FILES, candidate_models, MAX_FILE
from model_inventory import span, u32, u64, label

RCBS = 0xB0DC09942F000000
MODELS = 0x95820AF2485C0000
MAX_RCB = 128
MAX_JOINTS = 256
MAX_FIXUPS = 8192
MAX_REPORT_PAIRS = 48

def hash_ascii(name):
    if not isinstance(name,bytes) or not name or len(name)>127 or any(c<32 or c>126 for c in name):
        raise ValueError("Noncanonical ASCII bone name")
    result=0
    for byte in name:
        if 65<=byte<=90:byte+=32
        result=(result*65599+byte)&0xFFFFFFFF
    return result

def checked_range(data, base, table, offset, size):
    if offset<base or size<0 or offset>table or size>table-offset:
        raise ValueError("RCB payload/table boundary")
    return span(data,offset,size)

def read_skeleton(data, base, limit):
    """Strict validated RCB, no relocation or raw parent export."""
    span(data,base,0x40)
    if limit>len(data) or base>=limit or u32(data,base+4)!=21:
        raise ValueError("Wrong RCB version/limit")
    relative=u32(data,base)
    table=base+relative
    if relative<0x40 or table+4>limit:
        raise ValueError("Invalid RCB fixup table offset")
    fixups=set()
    cursor=table
    for _ in range(MAX_FIXUPS):
        if cursor+4>limit:raise ValueError("Unterminated fixups")
        field=u32(data,cursor);cursor+=4
        if field==0:break
        if field<4 or field+4>relative or field in fixups:
            raise ValueError("Invalid/duplicate fixup field")
        if u32(data,base+field)>relative:
            raise ValueError("Invalid fixup target")
        fixups.add(field)
    else:raise ValueError("Too many fixups")
    def pointer(field):
        checked_range(data,base,table,field,4)
        relative_field=field-base
        if relative_field not in fixups:raise ValueError("Missing relocation fixup")
        target=base+u32(data,field)
        checked_range(data,base,table,target,1)
        return target
    if u32(data,base+0x18)!=1:
        raise ValueError("Only one checked RCB skeleton")
    root=pointer(base+0x1C)
    checked_range(data,base,table,root,16)
    joints=u32(data,root)
    if not 1<=joints<=MAX_JOINTS:
        raise ValueError("RCB joint count budget")
    parents=pointer(root+4)
    matrices=pointer(root+8)
    lookups=pointer(root+12)
    checked_range(data,base,table,parents,joints*8)
    checked_range(data,base,table,matrices,joints*64)
    checked_range(data,base,table,lookups,joints*20)
    p=[]
    roots=0
    for i in range(joints):
        parent=struct.unpack("<i",span(data,parents+i*8,4))[0]
        if parent< -1 or parent>=joints or parent==i:
            raise ValueError("Invalid RCB parent")
        p.append(parent)
        roots+=parent==-1
    if roots<1:raise ValueError("No RCB root")
    # Validate hierarchy has no cycle, not merely parent index range.
    # Bounded O(J^2) on <=256 joints; no hierarchy is output.
    longest=0
    for i in range(joints):
        visited=set()
        node=i
        while node!=-1:
            if node in visited:raise ValueError("RCB cyclic hierarchy")
            visited.add(node)
            node=p[node]
        longest=max(longest,len(visited))
    matrices_data=checked_range(data,base,table,matrices,joints*64)
    if any(not math.isfinite(v[0]) for v in struct.iter_unpack("<f",matrices_data)):
        raise ValueError("Nonfinite RCB matrix")
    previous=-1
    hashes=set()
    indices=set()
    lookup_pairs=[]
    for i in range(joints):
        at=lookups+i*20
        value=u32(data,at)
        index=u32(data,at+16)
        if value<previous or index>=joints or index in indices:
            raise ValueError("RCB lookup not sorted/permutation")
        previous=value
        hashes.add(value)
        indices.add(index)
        lookup_pairs.append((value,index))
    if len(indices)!=joints:
        raise ValueError("Incomplete RCB lookup")
    # Private signatures are ONLY compared within the CI process; NEVER
    # serialize their values (IDs, hash list, hierarchy, bind matrices).
    return {
        "joints":joints, "roots":roots, "max_parent_depth":longest,
        "fixups":len(fixups), "_hashes":hashes,
        "_id":u32(data,base+8),
        "_hierarchy_signature":(tuple(p),tuple(lookup_pairs)),
        "_matrix_digest":hashlib.sha256(matrices_data).digest(),
    }

def rcb_dictionary(data):
    """Discover verified embedded DB v9 roots, NOT accidental raw byte scans."""
    if u32(data,0)!=0x0A:
        raise ValueError("Not StLevel version10")
    db=u32(data,12)
    span(data,db,0x44)
    if u32(data,db+0x18)!=9:
        raise ValueError("Not embedded DB version9")
    dic=db+u32(data,db+0x34)
    span(data,dic,16)
    if u64(data,dic)!=RCBS:raise ValueError("RCBS dictionary root ID mismatch")
    count=u32(data,dic+8)
    if count>MAX_RCB:raise ValueError("RCBS dictionary count exceeded")
    entries=dic+u32(data,dic+12)
    span(data,entries,count*16)
    values=[]
    seen=set()
    for i in range(count):
        entry=entries+i*16
        resource=dic+u32(data,entry+8)
        if resource in seen:continue
        seen.add(resource)
        # No resource span assumptions: relative fixups bound payload;
        # upper read limit conservatively is end of current file.
        values.append((u64(data,entry),resource))
    return values

def model_name_hashes(data,base):
    count=u32(data,base+0x5C)
    if count<1 or count>256:raise ValueError("Unbounded model bone name count")
    names=base+u32(data,base+0x60)
    span(data,names,count*4)
    hashes=set()
    for i in range(count):
        offset=base+u32(data,names+4*i)
        raw=bytes(span(data,offset,min(128,len(data)-offset)))
        stop=raw.find(b"\0")
        if stop<1:raise ValueError("Unterminated MODEL bone name")
        hashes.add(hash_ascii(raw[:stop]))
    return hashes

def inspect_stage(data,relative):
    if len(data)>MAX_FILE or not data:raise ValueError("Input size budget")
    records=rcb_dictionary(data)
    valid=[]
    invalid=0
    identities={}
    conflicts=set()
    matrix_variants={}
    for _,base in records:
        try:
            sk=read_skeleton(data,base,len(data))
            valid.append(sk)
            ident=sk["_id"]
            current=identities.get(ident)
            if current is None:
                identities[ident]=sk
            elif current["_hierarchy_signature"]!=sk["_hierarchy_signature"] or current["joints"]!=sk["joints"]:
                conflicts.add(ident)
            matrix_variants.setdefault(ident,set()).add(sk["_matrix_digest"])
        except (ValueError,struct.error):
            invalid+=1
    # Deduplicate repeated serialized RCB occurrences by their actual
    # skeleton identifier, but NEVER conflate conflicting lookup/parents.
    distinct=[sk for ident,sk in identities.items() if ident not in conflicts]
    pairs=[]
    model_count=0
    # Only full-body bone-tag candidates from validated MODEL dictionary scans.
    for key,base,details in candidate_models(data):
        model_count+=1
        if model_count>128:raise ValueError("Model enumeration budget")
        hashes=model_name_hashes(data,base)
        rankings=[]
        for idx,sk in enumerate(valid):
            hits=len(hashes&sk["_hashes"])
            rankings.append((hits,idx))
        rankings.sort(reverse=True)
        best_hits,best_idx=rankings[0] if rankings else (0,-1)
        second_hits=rankings[1][0] if len(rankings)>1 else 0
        best=valid[best_idx] if best_idx>=0 else None
        identity_ranks=sorted(
            (len(hashes&sk["_hashes"]),idx) for idx,sk in enumerate(distinct)
        )
        identity_top=identity_ranks[-1][0] if identity_ranks else 0
        identity_second=identity_ranks[-2][0] if len(identity_ranks)>1 else 0
        # Enough matching hashed names to warrant a *candidate* association,
        # but no proven bone influences/animation connection.
        sufficient=bool(best and best_hits>=8 and
                        best_hits*2>=len(hashes) and
                        best_hits*2>=best["joints"])
        safe_key=label(key)
        if safe_key and len(pairs)<MAX_REPORT_PAIRS:
            pairs.append({
                "model_key":safe_key,
                "model_name_count":details["bone_names"],
                "same_file_rcb_candidates":len(valid),
                "same_file_distinct_rcb_ids":len(identities),
                "best_rcb_joint_count":best["joints"] if best else 0,
                "best_matching_joint_name_hashes":best_hits,
                "runner_up_match_count":second_hits,
                "best_rcb_parent_hierarchy_validated":best is not None,
                "strong_same_file_name_overlap_candidate":sufficient,
                "unique_best_match":bool(sufficient and best_hits>second_hits),
                # Distinct verified RCB identities, not repeated dict entries.
                "unique_best_distinct_rcb_id":bool(
                    sufficient and identity_top==best_hits and
                    identity_top>identity_second and not conflicts
                ),
                "skin_weights_and_animation_binding_proven":False,
            })
    return {
        "file":relative, "rcb_dictionary_entries":len(records),
        "rcb_hierarchy_validated":len(valid),"rcb_invalid":invalid,
        "rcb_distinct_ids":len(identities),
        "rcb_repeated_id_records":len(valid)-len(identities),
        "rcb_ids_with_conflicting_parents_or_lookup":len(conflicts),
        "rcb_ids_with_multiple_matrix_payloads":sum(len(v)>1 for v in matrix_variants.values()),
        "max_validated_rcb_joint_count":max((x["joints"] for x in valid),default=0),
        "full_body_model_records_scanned":model_count,
        "model_rcb_pair_candidate_records":len(pairs),
        "pairs":pairs
    }

def audit(root):
    root=pathlib.Path(root)
    if root.is_symlink() or not root.is_dir():raise ValueError("Invalid input directory")
    stages=[]
    for relative in LEVEL_FILES:
        file=root/relative
        if file.is_symlink() or not file.is_file() or file.stat().st_size>MAX_FILE:
            raise ValueError("Invalid/missing pinned stage")
        stages.append(inspect_stage(file.read_bytes(),relative))
    return {
        "schema":"BO-COOP-owned-stlevel-rcb-hierarchy-v1",
        "source":"pinned owner biso StLevel, read-only",
        "files_checked":len(stages),
        "rcb_entries_total":sum(s["rcb_dictionary_entries"] for s in stages),
        "rcb_hierarchies_validated":sum(s["rcb_hierarchy_validated"] for s in stages),
        "rcb_invalid_entries":sum(s["rcb_invalid"] for s in stages),
        "rcb_distinct_ids_total_per_file":sum(s["rcb_distinct_ids"] for s in stages),
        "rcb_ids_with_conflicting_parents_or_lookup":sum(s["rcb_ids_with_conflicting_parents_or_lookup"] for s in stages),
        "rcb_ids_with_multiple_matrix_payloads":sum(s["rcb_ids_with_multiple_matrix_payloads"] for s in stages),
        "unique_best_distinct_rcb_id_candidate_records":sum(
            p["unique_best_distinct_rcb_id"] for s in stages for p in s["pairs"]),
        "strong_same_file_name_overlap_candidates":sum(
            p["strong_same_file_name_overlap_candidate"]
            for s in stages for p in s["pairs"]),
        "stages":stages,
        "raw_assets_bones_hashes_matrices_or_vertices_exported":False,
        "skin_vertex_influences_decoded":False,
        "renderable_remote_human_body_verified":False,
        "actual_animation_pose_player_verified":False,
        "caveat":"RCB parent tree/lookup hashes plus same-file MODEL bone-name hash overlap are necessary metadata evidence, NOT skinning/animation/ally ownership or live BLACK renderer."
    }

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--owner-root",type=pathlib.Path,required=True)
    parser.add_argument("--report",type=pathlib.Path,required=True)
    args=parser.parse_args()
    summary=audit(args.owner_root)
    args.report.write_text(json.dumps(summary,indent=2,sort_keys=True)+"\n",encoding="utf-8")
    print("BO_RCB_OWNER_HIERARCHY_META_PASS: bounded RCB and MODEL overlap, NO animation or game actor")

if __name__=="__main__":
    try:main()
    except (OSError,ValueError) as err:
        print("BO_RCB_OWNER_HIERARCHY_META_BLOCKED: "+type(err).__name__)
        raise SystemExit(1)
