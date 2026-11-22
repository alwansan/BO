#!/usr/bin/env python3
"""Private BIN/DB animation bank v8 -> RCB v21 track-boundary validator.

Inputs: only pinned owner StLevel files; report: bounded counts. Do not output
original bytes, key IDs, animation tracks, bone names, matrices or game assets.
Validating track boundaries does NOT prove pose interpolation, skinning,
animation state-machine or a playable full-body remote human in BLACK.
"""
import argparse
from collections import Counter
import json
import math
from pathlib import Path
import struct

from model_inventory import span, u32, u64
from ally_model_geometry_gate import LEVEL_FILES, MAX_FILE
from ally_rcb_hierarchy_gate import rcb_dictionary, read_skeleton

BNKS=0x555C57E557000000
SHAREDBNKS=0xB767793C8161D7C0
MAX_BANKS=96
MAX_CLIPS_PER_BANK=1024
MAX_JOINTS=256
MAX_TRACK_RECORDS_PER_CLIP=4096
MAX_TOTAL_TRACK_RECORDS_PER_STAGE=800000
MAX_FRAMES=4096
MAX_DEPTH=64
MAX_FIXUPS=200000
MAX_REPORT_BYTES=65536

class Blob:
    """Checked original serialized animation blob: B-relative fixup fields."""
    def __init__(self,data,base,limit,version,header):
        self.data=data;self.base=base
        if limit>len(data) or base<0 or base+header>limit or u32(data,base+4)!=version:
            raise ValueError("Animation version/limit")
        rel=u32(data,base)
        self.table=base+rel
        if rel<header or self.table+4>limit:raise ValueError("Relocation boundary")
        self.fields=set()
        at=self.table
        for _ in range(MAX_FIXUPS):
            if at+4>limit:raise ValueError("Fixup termination")
            field=u32(data,at);at+=4
            if field==0:break
            if field<4 or field+4>rel or field in self.fields:
                raise ValueError("Invalid relocation field")
            if u32(data,base+field)>rel:
                raise ValueError("Relocation target outside payload")
            self.fields.add(field)
        else: raise ValueError("Fixup count budget")
    def bounded(self,offset,size):
        if offset<self.base or size<0 or offset>self.table or size>self.table-offset:
            raise ValueError("Animation track crosses fixup table")
        return span(self.data,offset,size)
    def pointer(self,field):
        self.bounded(field,4)
        if field-self.base not in self.fields:raise ValueError("Missing relocation entry")
        at=self.base+u32(self.data,field)
        self.bounded(at,1)
        return at
    def array(self,field,count,stride):
        if count==0:return None
        at=self.pointer(field)
        self.bounded(at,count*stride)
        return at

def bank_dictionary(data,field,expected):
    """Only follow embedded DB roots; do not scan incidental GtIDs."""
    if u32(data,0)!=0x0A:raise ValueError("Wrong stage wrapper")
    db=u32(data,12)
    span(data,db,0x44)
    if u32(data,db+0x18)!=9:raise ValueError("Wrong DB wrapper")
    relative=u32(data,db+field)
    if relative==0:return []
    dic=db+relative
    span(data,dic,16)
    if u64(data,dic)!=expected:raise ValueError("Wrong bank dictionary")
    count=u32(data,dic+8)
    if count>MAX_BANKS:raise ValueError("Bank count budget")
    table=dic+u32(data,dic+12)
    span(data,table,count*16)
    values=[];seen=set()
    for i in range(count):
        entry=table+i*16
        value=dic+u32(data,entry+8)
        if value not in seen:
            values.append(value);seen.add(value)
    return values

def track_end(blob,at,hist,budget,depth=0):
    if depth>MAX_DEPTH or budget[0]>=MAX_TRACK_RECORDS_PER_CLIP:
        raise ValueError("Track recursion/record budget")
    head=blob.bounded(at,4)
    tag=head[0]
    count=struct.unpack_from("<H",head,1)[0]
    if tag>17:raise ValueError("Unknown encoding4 tag")
    hist[tag]+=1;budget[0]+=1
    if tag==1:
        cursor=at+4
        for _ in range(count):
            cursor=track_end(blob,cursor,hist,budget,depth+1)
        return cursor
    if tag==0:size=4+4*count
    elif tag in (2,10):size=16
    elif tag in (3,11):size=28
    elif tag in (4,12):size=40
    elif tag in (5,13):size=52
    elif tag in (6,14):size=4+12*count
    elif tag==7:size=28+((3*count+3)&~3)
    elif tag==8:size=28+((6*count+3)&~3)
    elif tag in (9,17):size=4
    elif tag==15:size=28+4*count
    elif tag==16:size=28+8*count
    else:raise ValueError("Invalid encoding4 tag")
    blob.bounded(at,size)
    if tag==2:
        for i in range(3):
            if not math.isfinite(struct.unpack_from("<f",blob.data,at+4+i*4)[0]):
                raise ValueError("Nonfinite constant vector")
    if tag==10:
        for i in range(4):
            off=at+4+i*3
            bits=bytes([0,*blob.data[off:off+3]])
            if not math.isfinite(struct.unpack("<f",bits)[0]):
                raise ValueError("Nonfinite packed quaternion")
    return at+size

def read_bank(data,base,limit,sk_by_id):
    bank=Blob(data,base,limit,8,0x3c)
    identity=u32(data,base+0x0c)
    if identity not in sk_by_id:raise ValueError("Bank references unverified/missing local RCB")
    joints=sk_by_id[identity]
    if not 1<=joints<=MAX_JOINTS:raise ValueError("Bank RCB joint count")
    clip_count=u32(data,base+0x2c)
    if clip_count>MAX_CLIPS_PER_BANK:raise ValueError("Clip count budget")
    clips=bank.array(base+0x30,clip_count,4)
    # Other categories are bounded but not interpreted as animation states.
    for count_offset,ptr_offset in ((0x24,0x28),(0x34,0x38)):
        count=u32(data,base+count_offset)
        if count>MAX_CLIPS_PER_BANK:raise ValueError("Bank object budget")
        bank.array(base+ptr_offset,count,4)
    records=0
    histogram=Counter()
    min_frames=0
    max_frames=0
    for i in range(clip_count):
        descriptor=bank.pointer(clips+i*4)
        bank.bounded(descriptor,0x30)
        frames=u32(data,descriptor+0x14)
        encoding=u32(data,descriptor+0x1c)
        if not 1<=frames<=MAX_FRAMES or (encoding&0x7FFFFFFF)!=4:
            raise ValueError("Unsupported clip encoding or frames")
        first=descriptor+0x30 if u32(data,descriptor+0x20)==0 else bank.pointer(descriptor+0x20)
        budget=[0];cursor=first
        for _ in range(joints*2):
            cursor=track_end(bank,cursor,histogram,budget)
        records+=budget[0]
        min_frames=frames if not min_frames else min(min_frames,frames)
        max_frames=max(max_frames,frames)
    return {"joint_count":joints,"clip_count":clip_count,
            "top_level_joint_tracks":clip_count*joints*2,
            "records_including_segments":records,
            "min_frame_samples":min_frames,"max_frame_samples":max_frames,
            "track_tag_counts":dict(sorted(histogram.items()))}

def inspect_stage(data,relative):
    if not data or len(data)>MAX_FILE:raise ValueError("Owner stage size budget")
    skeletons={}
    invalid_skeletons=0
    for _,offset in rcb_dictionary(data):
        try:
            meta=read_skeleton(data,offset,len(data))
            ident=u32(data,offset+8)
            old=skeletons.get(ident)
            if old is not None and old!=meta["joints"]:
                raise ValueError("Duplicate RCB ID has conflicting joints")
            skeletons[ident]=meta["joints"]
        except (ValueError,struct.error):
            invalid_skeletons+=1
    output={
        "file":relative,
        "rcb_id_joint_count_consistently_read":len(skeletons),
        "invalid_rcb_records":invalid_skeletons,
        "bank_records":0,"banks_with_supported_encodings":0,
        "unsupported_or_unresolved_banks":0,
        "validated_clip_headers_and_tracks":0,"validated_track_records":0,
        "max_track_records_in_single_bank":0,
        "max_joint_count_of_bank_rcb":0,
        "clip_samples_min":0,"clip_samples_max":0,
        "track_tag_counts":{},
    }
    aggregate=Counter()
    for field,expected in ((0x2c,BNKS),(0x30,SHAREDBNKS)):
        for base in bank_dictionary(data,field,expected):
            output["bank_records"]+=1
            try:
                bank=read_bank(data,base,len(data),skeletons)
                output["banks_with_supported_encodings"]+=1
                output["validated_clip_headers_and_tracks"]+=bank["clip_count"]
                output["validated_track_records"]+=bank["records_including_segments"]
                output["max_track_records_in_single_bank"]=max(
                    output["max_track_records_in_single_bank"],bank["records_including_segments"])
                output["max_joint_count_of_bank_rcb"]=max(
                    output["max_joint_count_of_bank_rcb"],bank["joint_count"])
                if bank["clip_count"]:
                    output["clip_samples_min"]=min(
                        output["clip_samples_min"] or bank["min_frame_samples"],
                        bank["min_frame_samples"])
                    output["clip_samples_max"]=max(output["clip_samples_max"],bank["max_frame_samples"])
                aggregate.update(bank["track_tag_counts"])
            except (ValueError,struct.error,OverflowError):
                output["unsupported_or_unresolved_banks"]+=1
            if output["validated_track_records"]>MAX_TOTAL_TRACK_RECORDS_PER_STAGE:
                raise ValueError("Stage track budget exceeded")
    output["track_tag_counts"]={str(k):v for k,v in sorted(aggregate.items())}
    return output

def audit(root):
    root=Path(root)
    if root.is_symlink() or not root.is_dir():raise ValueError("Invalid owner path")
    stages=[]
    for path in LEVEL_FILES:
        p=root/path
        if p.is_symlink() or not p.is_file() or p.stat().st_size>MAX_FILE:
            raise ValueError("Invalid pinned stage")
        stages.append(inspect_stage(p.read_bytes(),path))
    data={
        "schema":"BO-COOP-owner-rcb-bank-clip-boundaries-v1",
        "files_read":len(stages),
        "rcb_contexts_found":sum(s["rcb_id_joint_count_consistently_read"] for s in stages),
        "bank_records":sum(s["bank_records"] for s in stages),
        "validated_banks":sum(s["banks_with_supported_encodings"] for s in stages),
        "unresolved_banks":sum(s["unsupported_or_unresolved_banks"] for s in stages),
        "validated_clips":sum(s["validated_clip_headers_and_tracks"] for s in stages),
        "validated_track_records":sum(s["validated_track_records"] for s in stages),
        "stages":stages,
        "original_game_bytes_or_hashes_exported":False,
        "original_animation_pose_playable":False,
        "bone_to_skin_weight_attributes_known":False,
        "remote_human_ally_rendering_implemented":False,
        "caveat":"RCB-linked bank v8/encoding4 track spans only, no interpolation/time/graph, skinning weights, body identity, or live BLACK actor."
    }
    if len(json.dumps(data))>MAX_REPORT_BYTES:raise ValueError("Unsafe report size")
    return data

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--owner-root",type=Path,required=True)
    parser.add_argument("--report",type=Path,required=True)
    args=parser.parse_args()
    result=audit(args.owner_root)
    args.report.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n",encoding="utf8")
    print("BO_RCB_BANK_OWNER_TRACK_BOUNDARY_PASS: private RCB-linked clips checked; NO game animation")

if __name__=="__main__":
    try:main()
    except (OSError,ValueError) as error:
        print("BO_RCB_BANK_TRACK_BLOCKED: "+type(error).__name__)
        raise SystemExit(1)
