#!/usr/bin/env python3
"""Produce a privacy-preserving summary of a private HOOK001 capture.

Never prints raw guest addresses, positions, animation names, raw JSON lines,
phone-specific paths, game files, or save data.
"""
import argparse
import collections
import json
import pathlib
import sys

MAX_BYTES=2*1024*1024
MAX_LINES=8192
MAX_LINE=4096
KINDS=("player","lod","animation")
END_REASONS={"output_limit","write_failed","open_failed","world_read_unavailable",
             "time_or_record_limit","exception"}

def bounded_uint(value,low=0,high=1<<32):
    if type(value) is not int or value<low or value>high:
        raise ValueError("bad bounded integer")
    return value

def summarize(path):
    if path.stat().st_size>MAX_BYTES: raise ValueError("capture too large")
    kinds=collections.Counter()
    threads=set()
    epochs={}
    # These are bounded, in-memory correlation keys only. Neither IDs nor
    # associated kinds are exported as raw values or treated as actor lifetimes.
    kinds_by_correlation={}
    local_player_correlations=set()
    valid_position=collections.Counter()
    valid_health=collections.Counter()
    valid_resource=collections.Counter()
    valid_render=collections.Counter()
    nonzero_clip_hash=collections.Counter()
    local=collections.Counter()
    sample_ms_min=None;sample_ms_max=None
    rejected=0
    start=0;end=0;reasons=collections.Counter()
    last_meta={}
    with path.open("rb") as stream:
        for index,raw in enumerate(stream,1):
            if index>MAX_LINES: raise ValueError("capture line limit exceeded")
            if len(raw)>MAX_LINE: raise ValueError("capture line too long")
            try:
                rec=json.loads(raw.decode("utf-8"))
            except (UnicodeDecodeError,json.JSONDecodeError):
                rejected+=1
                continue
            if not isinstance(rec,dict): rejected+=1;continue
            event=rec.get("event")
            if event=="start":
                if index!=1 or start or rec.get("probe")!="HOOK001" or rec.get("scope")!="read_only_no_multiplayer":
                    raise ValueError("unexpected capture header")
                start=1
                continue
            if not start: raise ValueError("missing start event")
            if event=="end":
                end+=1
                reason=rec.get("reason")
                if isinstance(reason,str) and reason in END_REASONS:
                    reasons[reason]+=1
                else:
                    reasons["other"]+=1
                for name in ("records","attempts","read_failures"):
                    try:last_meta[name]=bounded_uint(rec.get(name))
                    except ValueError:pass
                continue
            if event not in KINDS or end:
                rejected+=1;continue
            try:
                epoch=bounded_uint(rec.get("epoch"),1)
                id_=bounded_uint(rec.get("id"),1,64)
                thread=bounded_uint(rec.get("thread"),1)
                if type(rec.get("position_valid")) is not bool or type(rec.get("health_valid")) is not bool or type(rec.get("local")) is not bool:
                    raise ValueError("invalid boolean")
            except ValueError:
                rejected+=1;continue
            kinds[event]+=1
            threads.add(thread)
            epochs.setdefault(epoch,set()).add(id_)
            key=(epoch,id_)
            kinds_by_correlation.setdefault(key,set()).add(event)
            if event=="player" and rec["local"]:
                local_player_correlations.add(key)
            valid_position[event]+=int(rec["position_valid"])
            valid_health[event]+=int(rec["health_valid"])
            # Optional strictly-typed fields: older probe versions may omit
            # them. Never include raw resource IDs, clip hashes, or timestamps.
            if type(rec.get("resource_valid")) is bool:
                valid_resource[event]+=int(rec["resource_valid"])
            if type(rec.get("render_valid")) is bool:
                valid_render[event]+=int(rec["render_valid"])
            clip=rec.get("clip_hash")
            if type(clip) is int and 0<clip<=0xffffffff:
                nonzero_clip_hash[event]+=1
            ms=rec.get("ms")
            if type(ms) is int and 0<=ms<=1800000:
                sample_ms_min=ms if sample_ms_min is None else min(ms,sample_ms_min)
                sample_ms_max=ms if sample_ms_max is None else max(ms,sample_ms_max)
            local[event]+=int(rec["local"])
    if not start: raise ValueError("missing capture header")
    # Only diagnostic reason codes are exposed; never propagate raw payloads.
    # Absence of LOD/animation events NEVER proves missing models or factories.
    flags=[]
    if not end: flags.append("capture_end_not_seen")
    if not sum(kinds.values()): flags.append("no_samples")
    if not kinds["player"]: flags.append("no_player_samples")
    if not kinds["lod"]: flags.append("no_lod_samples")
    if not kinds["animation"]: flags.append("no_animation_samples")
    if reasons["world_read_unavailable"]: flags.append("reader_unavailable")
    if reasons["write_failed"] or reasons["open_failed"]: flags.append("local_output_error")
    # Overlap is only a within-epoch correlation of observed values. A shared
    # slot can be reused after respawn or promoted from a render hook; neither
    # overlap nor its absence proves that a player model/factory exists.
    overlap={
        "player_lod":0,
        "player_animation":0,
        "lod_animation":0,
        "all_three":0,
        "local_player_lod":0,
        "local_player_animation":0,
        "player_animation_without_lod":0,
        "lod_animation_without_player":0,
    }
    for key, kinds_seen in kinds_by_correlation.items():
        if "player" in kinds_seen and "lod" in kinds_seen:
            overlap["player_lod"]+=1
        if "player" in kinds_seen and "animation" in kinds_seen:
            overlap["player_animation"]+=1
        if "lod" in kinds_seen and "animation" in kinds_seen:
            overlap["lod_animation"]+=1
        if len(kinds_seen)==3: overlap["all_three"]+=1
        if key in local_player_correlations and "lod" in kinds_seen:
            overlap["local_player_lod"]+=1
        if key in local_player_correlations and "animation" in kinds_seen:
            overlap["local_player_animation"]+=1
        if "player" in kinds_seen and "animation" in kinds_seen and "lod" not in kinds_seen:
            overlap["player_animation_without_lod"]+=1
        if "lod" in kinds_seen and "animation" in kinds_seen and "player" not in kinds_seen:
            overlap["lod_animation_without_player"]+=1
    # An end event can be absent from periodic MediaStore snapshots. Use this
    # only to describe a partial observation window, not the actual session
    # duration or a game state/lifetime. No raw ms values leave this process.
    sampled_span_seconds=(sample_ms_max-sample_ms_min)//1000 if (
        sample_ms_min is not None and sample_ms_max is not None) else None
    # No epoch numbers, pointer IDs or thread IDs in the report.
    return {
        "diagnostic_flags": flags,
        "schema":"BO-COOP-HOOK001-safe-summary-v1",
        "scope":"read_only_no_multiplayer",
        "header_valid":True,
        "events":{k:kinds[k] for k in KINDS},
        "world_epoch_count":len(epochs),
        "max_distinct_correlations_in_epoch":max(map(len,epochs.values()),default=0),
        "distinct_thread_count":len(threads),
        "valid_position_count":{k:valid_position[k] for k in KINDS},
        "valid_health_count":{k:valid_health[k] for k in KINDS},
        "valid_resource_count":{k:valid_resource[k] for k in KINDS},
        "valid_render_count":{k:valid_render[k] for k in KINDS},
        "nonzero_animation_clip_hash_count":{k:nonzero_clip_hash[k] for k in KINDS},
        "sample_observation_span_seconds":sampled_span_seconds,
        "capture_is_partial_snapshot":end==0,
        "local_player_sample_count":local["player"],
        "correlation_overlap_counts":overlap,
        "correlation_overlap_is_actor_proof":False,
        "unknown_or_malformed_line_count":rejected,
        "end_event_count":end,
        "end_reasons":dict(sorted(reasons.items())),
        "end_metadata":last_meta,
        "capture_end_seen":end>0,
        "runtime_actor_factory_proven":False,
        "in_game_multiplayer_proven":False,
    }

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("private_capture",type=pathlib.Path)
    p.add_argument("--output",type=pathlib.Path)
    args=p.parse_args()
    report=summarize(args.private_capture)
    value=json.dumps(report,indent=2,sort_keys=True,ensure_ascii=False)+"\n"
    if args.output:
        args.output.write_text(value,encoding="utf-8")
        print("Wrote sanitized counts only; do not upload the original JSONL.")
    else:
        print(value,end="")

if __name__=="__main__":
    try:main()
    except (OSError,ValueError) as e:
        # Never include file paths or untrusted payloads in error reports.
        print("Capture rejected or unreadable (check size, header, encoding, bounds).",file=sys.stderr)
        sys.exit(1)
