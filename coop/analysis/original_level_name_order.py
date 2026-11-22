#!/usr/bin/env python3
"""Owner-pinned BLACK XBE level-name pointer-order survey (read only).

Question: can the host's original 8-mission order be proven to match the
eight level folders WITHOUT guessing mission index from directory name?
Only export derived numeric identifiers and bounded counts. No copyrighted
source strings/bytes/paths/instructions, no mission/gameplay assertions.
"""
import argparse,hashlib,json,re,struct,sys
from pathlib import Path
from story_entry_static import OWNER_SHA1,MAX_FILE,UnsafeImage,word

KNOWN=(0,1,3,4,5,6,7,8)
LEVEL=re.compile(rb"(?i:(?:^|[^A-Za-z0-9_])level[_-]([0-9]{2})(?![0-9]))")
MAX_SECTIONS=96
MAX_TABLES=6

def sections(data):
    if len(data)<0x160 or data[:4]!=b"XBEH":raise UnsafeImage("Not XBE")
    base=word(data,0x104)
    count=word(data,0x11C)
    at=word(data,0x120)-base
    if base<0x10000 or not 1<=count<=MAX_SECTIONS or at<0x160 or at+56*count>len(data):
        raise UnsafeImage("Malformed original section headers")
    out=[]
    for i in range(count):
        _,va,vsize,raw,rsize,_=struct.unpack_from("<6I",data,at+i*56)
        if vsize==0 or rsize==0:continue
        size=min(vsize,rsize)
        if raw>len(data) or size>len(data)-raw or va<base or va+size>0xffffffff:
            raise UnsafeImage("Original section mapping invalid")
        out.append((va,raw,size))
    if not out:raise UnsafeImage("No mapped sections")
    return out

def audit(data,pinned=OWNER_SHA1):
    if not isinstance(data,(bytes,bytearray)) or not 0<len(data)<=MAX_FILE:
        raise UnsafeImage("Bad original XBE size")
    if not re.fullmatch("[0-9a-f]{40}",pinned or "") or hashlib.sha1(data).hexdigest()!=pinned:
        raise UnsafeImage("Original XBE hash mismatch")
    segs=sections(data)
    # Only valid known owner level folders; don't leak actual string contents.
    addresses={}
    counts={n:0 for n in KNOWN}
    for va,raw,length in segs:
        block=data[raw:raw+length]
        for match in LEVEL.finditer(block):
            level=int(match.group(1))
            if level not in KNOWN:continue
            addresses[va+match.start()+1 if block[match.start():match.start()+1] not in (b"l",b"L")
                      else va+match.start()]=level
            counts[level]+=1
    # Derive possible exact 8-index tables of absolute original string ptrs.
    # Any table can be incidental; not sufficient as selected-mission mapping.
    candidates=[]
    seen=set()
    for va,raw,length in segs:
        stop=length-32
        for offset in range(0,max(0,stop+1),4):
            ptrs=struct.unpack_from("<8I",data,raw+offset)
            seq=tuple(addresses.get(p,-1) for p in ptrs)
            if set(seq)==set(KNOWN) and seq not in seen:
                seen.add(seq)
                candidates.append(seq)
                if len(candidates)>=MAX_TABLES:break
        if len(candidates)>=MAX_TABLES:break
    return {
        "schema":"bo-owner-xbe-original-level-name-order-hints-v1",
        "pinned_original_identity_checked":True,
        "level_name_match_counts":{f"level_{i:02d}":counts[i] for i in KNOWN},
        "bounded_8_pointer_permutation_candidate_count":len(candidates),
        "bounded_possible_orderings":[list(x) for x in candidates],
        "level_index_and_original_menu_mission_mapping_proven":False,
        "real_host_scene_loader_proven":False,
        "actual_guest_auto_launch_proven":False,
        "raw_XBE_strings_and_bytes_exported":False,
        "caveat":"Static XBE string occurrence/pointer adjacency may be absent, coincidental, or not used by game. Even a permutation is NOT a verified mission index or runtime stage. No game memory writes or gameplay evidence."
    }

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--owner-xbe",required=True,type=Path)
    parser.add_argument("--report",required=True,type=Path)
    args=parser.parse_args()
    if args.owner_xbe.is_symlink() or not args.owner_xbe.is_file() or args.owner_xbe.stat().st_size>MAX_FILE:
        raise UnsafeImage("Invalid owner XBE input")
    result=audit(args.owner_xbe.read_bytes())
    result_bytes=json.dumps(result,indent=2,sort_keys=True).encode()
    if len(result_bytes)>4096:raise UnsafeImage("Metadata privacy budget")
    args.report.write_bytes(result_bytes+b"\n")
    print("BO_LEVEL_NAME_STATIC_HINT_PASS: exact pinned private XBE; no mission map claimed")
if __name__=="__main__":
    try:main()
    except (UnsafeImage,OSError) as exc:
        print("BO_LEVEL_NAME_STATIC_BLOCKED: "+type(exc).__name__,file=sys.stderr)
        sys.exit(1)
