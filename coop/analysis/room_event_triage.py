#!/usr/bin/env python3
"""Analyze existing owner-exported BLACK room event codes; no raw logs published.

Expected input: RoomDiagnostics safe event TXT files from BOTH phones in CL.
Reject malformed/oversized files and report only fixed, whitelisted aggregate
labels. Never output filenames, timestamps, names, PINs, addresses or raw lines.
An authenticated room is NOT proof of BLACK shared-world gameplay.
"""
from __future__ import annotations
import argparse
from collections import Counter
import json
from pathlib import Path
import re
import sys

MAX_FILES=128
MAX_BYTES_PER_FILE=131072
EVENT=re.compile(r"^[0-9]{13} ([A-Z0-9_]{1,70})$")
FIXED=frozenset({
    "MENU_OPEN","MENU_BACK_TO_GAME","HOST_REQUEST","HOST_DISCOVERY_READY",
    "HOST_DISCOVERY_DISABLED","HOST_BLOCKED_ALREADY_ACTIVE",
    "JOIN_REQUEST","JOIN_CONNECTED","JOIN_BLOCKED_ALREADY_ACTIVE",
    "ROOM_HOSTING","ROOM_JOINED","ROOM_IDLE","ROOM_ERROR","ROOM_LEFT",
    "ROOM_CONNECTING","SESSION_CONNECTING","SESSION_STOPPED",
    "ERROR_BINDEXCEPTION","SCAN_STARTED","SCAN_FAILED","SCAN_PAUSED_ACTIVE_ROOM",
    "EXPORT_REQUESTED","EXPORT_OK","EXPORT_FAILED",
    "STORY_HOST_PREPARED","STORY_HOST_SCENE_READY","STORY_HOST_PEER_READY",
    "STORY_HOST_COMMITTED","STORY_HOST_ABORTED","STORY_GUEST_PREPARED",
    "STORY_GUEST_READY_SENT","STORY_GUEST_COMMITTED","STORY_GUEST_ABORTED",
    "STORY_GUEST_ROSTER_INVALIDATED",
})
SAFE_EXPORT=tuple(sorted(FIXED))
SESSION_END={"ROOM_IDLE","ROOM_LEFT","SESSION_STOPPED","ROOM_ERROR","SESSION_CONNECTING"}

def read_file(path: Path) -> list[str]:
    if path.is_symlink() or not path.is_file() or path.stat().st_size>MAX_BYTES_PER_FILE:
        raise ValueError("invalid event file")
    raw=path.read_bytes()
    try:
        lines=raw.decode("ascii").splitlines()
    except UnicodeDecodeError as exc:
        raise ValueError("non-ASCII event data") from exc
    if not lines:
        raise ValueError("empty event file")
    codes=[]
    for line in lines:
        match=EVENT.fullmatch(line)
        if match is None:
            raise ValueError("unsafe or malformed event line")
        codes.append(match.group(1))
    return codes

def summarize(folder: Path) -> dict:
    if not folder.is_dir() or folder.is_symlink():
        raise ValueError("missing log directory")
    files=sorted(folder.glob("BLACK-Room-*-events.txt"))
    if not files or len(files)>MAX_FILES:
        raise ValueError("invalid number of room captures")
    observed=Counter()
    unknown=0
    scan_after_join=0
    worst_scan_after_join=0
    files_with_scan_after_join=0
    for path in files:
        joined=False
        count_after=0
        for code in read_file(path):
            if code.startswith("SCAN_RESULTS_") and code[13:].isdigit():
                # Discovery cardinality only, never room ID.
                idx=int(code[13:])
                if 0<=idx<=32:
                    observed["SCAN_RESULTS"]+=1
                else:
                    unknown+=1
            elif code in FIXED:
                observed[code]+=1
            else:
                unknown+=1
            if code in SESSION_END:
                joined=False
            if code=="ROOM_JOINED":
                joined=True
            if code=="SCAN_STARTED" and joined:
                count_after+=1
        scan_after_join+=count_after
        worst_scan_after_join=max(worst_scan_after_join,count_after)
        files_with_scan_after_join+=int(count_after>0)
    return {
        "files_checked":len(files),
        "known_events":{kind:observed[kind] for kind in SAFE_EXPORT if observed[kind]},
        "scan_results":observed["SCAN_RESULTS"],
        "unknown_event_codes_not_exported":unknown,
        "scans_after_join_within_same_log":scan_after_join,
        "largest_single_log_scans_after_join":worst_scan_after_join,
        "logs_with_scans_after_join":files_with_scan_after_join,
        "high_redundant_discovery_evidence":worst_scan_after_join>=10,
        "same_game_scene_proven":False,
        "remote_human_actor_proven":False,
        "story_and_world_sync_proven":False
    }

def compare(primary: Path, second: Path) -> dict:
    a,b=summarize(primary),summarize(second)
    return {
        "schema":"BO-COOP-two-device-room-log-triage-v1",
        "scope":"sanitized_historical_room_codes_not_live_gameplay",
        "first_device":a,
        "second_device":b,
        "first_device_bind_exceptions":a["known_events"].get("ERROR_BINDEXCEPTION",0),
        "second_device_join_success_events":b["known_events"].get("ROOM_JOINED",0),
        "needs_live_owner_phone_test_now":False,
        "shared_black_world_proven":False,
        "caveat":"Old room sessions may differ by APK/protocol; counts and intra-file order do not prove co-op gameplay or a matching cross-device session."
    }

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--first",required=True,type=Path)
    p.add_argument("--second",required=True,type=Path)
    p.add_argument("--report",required=True,type=Path)
    opts=p.parse_args()
    output=compare(opts.first,opts.second)
    opts.report.write_text(json.dumps(output,indent=2,sort_keys=True)+"\n",encoding="utf8")
    print("BO_CL_LOG_TRIAGE_PASS: sanitized counts only, no raw logs or identities exported")
    print("DISCOVERY_AFTER_JOIN_DETECTED="+str(output["second_device"]["high_redundant_discovery_evidence"]).lower())
    print("HOST_BIND_ERRORS_DETECTED="+str(output["first_device_bind_exceptions"]>0).lower())

if __name__=="__main__":
    try:
        main()
    except (OSError,ValueError) as e:
        print("BO_CL_LOG_TRIAGE_BLOCKED: input rejected ("+type(e).__name__+")",file=sys.stderr)
        sys.exit(1)
