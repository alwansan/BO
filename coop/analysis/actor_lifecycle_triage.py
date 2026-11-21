#!/usr/bin/env python3
"""Read-only ACTOR-001A observation coverage, NOT actor lifetime proof.

Input is a PRIVATE HOOK001 JSONL capture. Output contains aggregate counts,
never guest pointers, IDs, labels, coordinates, raw times, or input paths.
Snapshot gaps, epoch changes and reuse candidates DO NOT prove NPC death,
despawn, resource ownership, or actor factory behavior.
"""
import argparse
import collections
import json
from pathlib import Path
import sys

MAX_BYTES=2*1024*1024
MAX_LINES=8192
MAX_LINE=4096
MAX_MS=1800000
KINDS=("player","lod","animation")

def uint(v,low,high):
    if type(v) is not int or not low<=v<=high: raise ValueError("invalid record")
    return v

def analyze(path):
    if path.stat().st_size>MAX_BYTES: raise ValueError("oversize input")
    groups={}
    per_epoch=collections.defaultdict(set)
    epoch_times=collections.defaultdict(lambda:[None,None])
    obs_start=None
    obs_end=None
    seen_end=False
    header=False
    count=collections.Counter()
    malformed=0
    previous_ms=None
    with path.open("rb") as fh:
        for line_no,line in enumerate(fh,1):
            if line_no>MAX_LINES or len(line)>MAX_LINE:
                raise ValueError("capture limits")
            try:
                record=json.loads(line.decode("utf-8"))
            except (UnicodeDecodeError,json.JSONDecodeError):
                malformed+=1;continue
            if type(record) is not dict:
                malformed+=1;continue
            kind=record.get("event")
            if kind=="start":
                if (header or line_no!=1 or
                    record.get("probe")!="HOOK001" or
                    record.get("scope")!="read_only_no_multiplayer"):
                    raise ValueError("invalid capture start")
                header=True
                continue
            if not header: raise ValueError("missing capture start")
            if kind=="end":
                seen_end=True
                continue
            if kind not in KINDS or seen_end:
                malformed+=1;continue
            try:
                epoch=uint(record.get("epoch"),1,0xffffffff)
                actor=uint(record.get("id"),1,64)
                ms=uint(record.get("ms"),0,MAX_MS)
                uint(record.get("thread"),1,0xffffffff)
                if type(record.get("local")) is not bool:
                    raise ValueError("invalid local")
            except ValueError:
                malformed+=1;continue
            # Do not infer actor lifecycle across source log resets, even if
            # the numeric (epoch,id) value happens to match a previous run.
            key=(epoch,actor)
            g=groups.setdefault(key,{"first":ms,"last":ms,"samples":0,
                    "kinds":set(),"long_gaps":0,"last_by_kind":{}})
            # Out-of-order timestamps are ignored, not used as negative ages.
            if previous_ms is not None and ms<previous_ms:
                malformed+=1;continue
            previous_ms=ms
            if g["samples"] and ms-g["last"]>=10000:
                g["long_gaps"]+=1
            g["samples"]+=1
            g["last"]=ms
            g["kinds"].add(kind)
            g["last_by_kind"][kind]=ms
            per_epoch[epoch].add(actor)
            epoch_range=epoch_times[epoch]
            epoch_range[0]=ms if epoch_range[0] is None else min(epoch_range[0],ms)
            epoch_range[1]=ms if epoch_range[1] is None else max(epoch_range[1],ms)
            obs_start=ms if obs_start is None else min(obs_start,ms)
            obs_end=ms if obs_end is None else max(obs_end,ms)
            count[kind]+=1
    if not header: raise ValueError("missing header")
    lod=[g for g in groups.values() if "lod" in g["kinds"]]
    anim_lod=[g for g in lod if "animation" in g["kinds"]]
    # These are potential SAMPLING gaps, not object deaths or respawns.
    any_gap=sum(g["long_gaps"]>0 for g in lod)
    total_gaps=sum(g["long_gaps"] for g in lod)
    if obs_start is None or obs_end is None:
        delayed=early_last=0
    else:
        delayed=sum(g["first"]-obs_start>=10000 for g in lod)
        early_last=sum(obs_end-g["last"]>=10000 for g in lod)
    epoch_count=len(epoch_times)
    return {
        "schema":"BO-COOP-ACTOR001A-coverage-v1",
        "scope":"read_only_coverage_not_lifetime",
        "capture_end_seen":seen_end,
        "capture_is_partial":not seen_end,
        "events":{x:count[x] for x in KINDS},
        "observed_epoch_count":epoch_count,
        "multiple_epoch_boundaries_observed":epoch_count>1,
        "temporary_correlation_count":len(groups),
        "lod_correlations":len(lod),
        "lod_with_animation_correlation":len(anim_lod),
        "lod_groups_first_seen_10s_after_first_sample":delayed,
        "lod_groups_last_seen_10s_before_last_sample":early_last,
        "lod_groups_with_observation_gaps_ge_10s":any_gap,
        "lod_observation_gaps_ge_10s_total":total_gaps,
        "malformed_or_rejected_lines":malformed,
        "sampling_gaps_are_actor_deaths":False,
        "epoch_changes_prove_respawn":False,
        "npc_lifetime_proven":False,
        "actor_factory_proven":False,
        "remote_player_spawn_proven":False,
        "next_evidence":["explicit_world_or_scene_transition","npc_death_and_reappearance",
                         "generation_or_deallocation_owner_validation"],
    }

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("private_capture",type=Path)
    p.add_argument("--output",type=Path)
    args=p.parse_args()
    report=analyze(args.private_capture)
    output=json.dumps(report,sort_keys=True,indent=2)+"\n"
    if args.output:
        args.output.write_text(output,encoding="utf-8")
        print("Sanitized ACTOR-001A coverage only; do not publish raw capture.")
    else:
        print(output,end="")

if __name__=="__main__":
    try:main()
    except (OSError,ValueError):
        print("ACTOR-001A private capture rejected; no raw data disclosed.",file=sys.stderr)
        sys.exit(1)
