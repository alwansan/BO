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
    valid_position=collections.Counter()
    valid_health=collections.Counter()
    local=collections.Counter()
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
            valid_position[event]+=int(rec["position_valid"])
            valid_health[event]+=int(rec["health_valid"])
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
        "local_player_sample_count":local["player"],
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
