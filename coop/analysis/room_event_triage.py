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
    # App-emitted safe codes previously counted as unknown. Still NO raw lines,
    # names, PINs, IPs, certs, keys, timestamps; producer safeCode regex applies.
    "STORY_DIAGNOSTICS_BEGIN","STORY_DIAGNOSTICS_END",
    "STORY_GUEST_READY_QUEUED","STORY_GUEST_READY_QUEUE_FULL","STORY_GUEST_READY_SEND_FAILED",
    "STORY_HOST_ABORTED","STORY_GUEST_ABORTED",
    "ROOM_FIRST_STAGE_START_REQUESTED","ROOM_FIRST_STAGE_START_BLOCKED",
    "ROOM_SAFE_LOG_EXPORT_VERIFIED","MENU_GUEST_FIRST_STAGE_AUTO_DISMISSED",
    "ORIGINAL_TWO_PHONE_AUTO_MENU_CONFIRM_SENT","ORIGINAL_TWO_PHONE_AUTO_MENU_DIFFICULTY_PAGE_REQUESTED",
    "PRESENCE_READY","PRESENCE_UNAVAILABLE","HOST_INPUT_ERROR","JOIN_INPUT_ERROR",
    "ERROR_SOCKETTIMEOUTEXCEPTION",
    # Read-only world/original observation codes (NO shared-world claim).
    "WORLD_MISSION_LAUNCH_ATTEMPT","WORLD_LOCAL_PLAYER_UPDATE_SEEN","WORLD_POINTER_CHANGED_UNVERIFIED",
    "ORIGINAL_CORE_LEVEL_STAGE_CONFLICT_STREAM_SOURCE_HINT_NOT_READY",
    "ORIGINAL_CORE_LEVEL_STAGE_MATCH_STREAM_SOURCE_HINT_NOT_READY",
    "ORIGINAL_CORE_LEVEL_STAGE_UNKNOWN_SOURCE_HINT_NOT_READY",
    "ORIGINAL_CORE_PHASE_1C_RESOURCE_STREAM_STABLE_SOURCE_HINT_NOT_READY",
    "ORIGINAL_CORE_STREAM_STAGE_IDENTITY_CACHED_NOT_SCENE_READY",
    "ORIGINAL_GUEST_GAMEPAD_TLS_SEND_REJECTED",
    "ORIGINAL_GUEST_HOST_NPC_VALUES_FIRST_TLS_RECEIVED_NOT_SHARED_AI",
    "ORIGINAL_GUEST_HOST_OBJECT_EVENT_INBOX_NOT_NATIVE_DOOR_APPLIED",
    "ORIGINAL_GUEST_HOST_STAGE_CHANGED_ON_FRONTEND_THREAD_NO_LOADER",
    "ORIGINAL_GUEST_HOST_STAGE_ON_FRONTEND_ENGINE_THREAD_NO_LOADER",
    "ORIGINAL_GUEST_RUNNING_LEVEL_MATCHES_HOST_STAGE_NO_SHARED_WORLD",
    "ORIGINAL_GUEST_RUNNING_LOCAL_SCENE_TLS_READY_NO_SHARED_AI",
    "ORIGINAL_GUEST_SELECTED_LEVEL_CONFLICTS_AUTH_HOST_STAGE_NO_READY",
    "ORIGINAL_GUEST_SELECTED_LEVEL_MATCHES_AUTH_HOST_STAGE_NO_READY",
    "ORIGINAL_GUEST_SOURCE_FORWARD_YAW_NOT_AVAILABLE_USING_NEUTRAL",
    "ORIGINAL_HOST_LOCAL_PHYSICS_PROXY_NOT_VERIFIED",
    "ORIGINAL_HOST_NPC_VALUES_FIRST_TLS_QUEUED_NO_SHARED_AI",
    "ORIGINAL_HOST_PHYSICS_POOL_INSUFFICIENT_RESERVED_FREE_SLOTS","ORIGINAL_HOST_PHYSICS_POOL_UNREADABLE_NO_ACTOR",
    "ORIGINAL_HOST_PHYSICS_PROXY_CAPACITY_SOURCE_ELIGIBLE_NO_NEW_ACTOR",
    "ORIGINAL_HOST_RUNNING_LOCAL_SCENE_TLS_READY_NO_SHARED_AI",
    "ORIGINAL_HOST_STAGE_TLS_PREPARE_QUEUED_NOT_SCENE_READY",
    "ORIGINAL_HOST_EXTRA_ACTOR_GATE_BAD_AUTHENTICATED_ROSTER","ORIGINAL_HOST_EXTRA_ACTOR_GATE_MISSING_FRESH_HUMAN_INPUT",
    "ORIGINAL_HOST_EXTRA_ACTOR_GATE_NO_COMMITTED_INPUT_EPOCH","ORIGINAL_HOST_EXTRA_ACTOR_GATE_NO_MATCHED_SOURCE",
    "ORIGINAL_HOST_EXTRA_ACTOR_GATE_NO_SAFE_AI_TARGET_CAPACITY","ORIGINAL_HOST_EXTRA_ACTOR_GATE_NO_SAFE_PHYSICS_CAPACITY",
    "ORIGINAL_HOST_EXTRA_ACTOR_GATE_NO_SOLDIER_BODY_CAPACITY","ORIGINAL_HOST_EXTRA_ACTOR_GATE_SOURCE_ELIGIBLE_NO_GAMEPLAY_ACTOR",
    "ORIGINAL_AI_TARGET_LOCAL_PLAYER_RECORD_BOUND_READ_ONLY",
    "ORIGINAL_AI_TARGET_POOL_ONE_FREE_SLOT_NO_ACTOR","ORIGINAL_AI_TARGET_POOL_THREE_OR_MORE_FREE_SLOTS_NO_ACTOR",
    "ORIGINAL_AI_TARGET_POOL_TWO_FREE_SLOTS_NO_ACTOR","ORIGINAL_AI_TARGET_POOL_UNREADABLE_NO_ACTOR",
    "ORIGINAL_AI_TARGET_POOL_ZERO_FREE_SLOTS_NO_ACTOR",
    "ORIGINAL_HUMAN_LOOK_NOT_ENOUGH_SAFE_BLANK_SLOTS","ORIGINAL_HUMAN_LOOK_NO_ORIGINAL_BOT_TEMPLATE",
    "ORIGINAL_HUMAN_LOOK_ONE_RIG_PROPOSED_NO_ACTOR","ORIGINAL_HUMAN_LOOK_PROPOSAL_INVALIDATED_NO_ACTOR",
    "ORIGINAL_HUMAN_LOOK_THREE_RIGS_PROPOSED_NO_ACTOR","ORIGINAL_HUMAN_LOOK_TWO_RIGS_PROPOSED_NO_ACTOR",
    "ORIGINAL_POOL_BOT_LOOK_BUT_ZERO_BLANK_RIG_SLOTS_NOT_READY","ORIGINAL_POOL_BOT_LOOK_ONE_BLANK_RIG_SLOT_NOT_READY",
    "ORIGINAL_POOL_BOT_LOOK_THREE_OR_MORE_BLANK_RIG_SLOTS_NOT_READY","ORIGINAL_POOL_BOT_LOOK_TWO_BLANK_RIG_SLOTS_NOT_READY",
    "ORIGINAL_POOL_NO_ELIGIBLE_BOT_LOOK_SOURCE_NOT_READY",
    "ORIGINAL_LAUNCH_DIFFICULTY_BLACK_OPS_NOT_MISSION","ORIGINAL_LAUNCH_DIFFICULTY_EASY_NOT_MISSION",
    "ORIGINAL_LAUNCH_DIFFICULTY_HARD_NOT_MISSION","ORIGINAL_LAUNCH_DIFFICULTY_NORMAL_NOT_MISSION",
    "ORIGINAL_LAUNCH_DIFFICULTY_UNAVAILABLE",
    "ORIGINAL_LOCAL_CAMERA_EYE_AND_BODY_FEET_VALIDATED","ORIGINAL_NON_FRONTEND_PLAYER_STABLE_UNVERIFIED_MISSION",
    "ORIGINAL_POST_1F1080_GAME_FRAME_STABLE_SOURCE_ONLY_NO_PLAYER2",
    "ORIGINAL_UNIT_ORDINALS_WORLD_LIST_MEMBERSHIP_OBSERVED",
    "ORIGINAL_UNIT_EARLY_TEARDOWN_WORLD_LIST_REMOVAL_OBSERVED",
    "ORIGINAL_SAME_UNIT_WORLD_LIST_RELINK_OBSERVED",
    "OBJECTIVE_HOST_FIRST_PART_SENT","OBJECTIVE_GUEST_STARTED",
    "OBJECTIVE_GUEST_COMPLETED","OBJECTIVE_GUEST_REJECTED","ORIGINAL_OBJECTIVE_SCENE_REVOKED",
    "ORIGINAL_GUEST_OBJECTIVE_ACTIVATION_READBACK_VERIFIED",
    "ORIGINAL_GUEST_OBJECTIVE_ACTIVATION_READBACK_FAILED",
    "ORIGINAL_GUEST_SIMPLE_OBJECTIVE_COMPLETION_READBACK_VERIFIED",
    "ORIGINAL_GUEST_SIMPLE_OBJECTIVE_COMPLETION_READBACK_FAILED",
    # Original remote-visual / human-actor gate observations (still NOT visual proof).
    "ORIGINAL_REMOTE_HUMAN_BOT_DRAW_CALLED_NOT_VISUAL_PROOF","ORIGINAL_REMOTE_HUMAN_BOT_VISUAL_SLOT_INITIALIZED_UNTESTED",
    "ORIGINAL_REMOTE_VISUAL_DRAW_METHOD_UNAVAILABLE","ORIGINAL_REMOTE_VISUAL_NO_SAFE_SOLDIER_DONOR_OR_BLANK_SLOT",
    "ORIGINAL_REMOTE_VISUAL_NO_TRUSTED_HOST_PEER_FRAME","ORIGINAL_REMOTE_VISUAL_ORIGINAL_ANIMATION_FUNCTION_MISSING",
    "ORIGINAL_REMOTE_VISUAL_ORIGINAL_BOT_POOL_UNREADABLE","ORIGINAL_REMOTE_VISUAL_ORIGINAL_GRID_DRAW_ENTERED_NOT_DISPLAY_PROOF",
    "ORIGINAL_REMOTE_VISUAL_ORIGINAL_GRID_WORLD_MISMATCH","ORIGINAL_REMOTE_VISUAL_ORIGINAL_RENDER_THREAD_MISMATCH",
    "ORIGINAL_REMOTE_VISUAL_PEER_BODY_MISSING_OR_STALE","ORIGINAL_REMOTE_VISUAL_SLOT_RECLAIMED_OR_CONSTRUCTOR_REJECTED",
    "ORIGINAL_REMOTE_VISUAL_SOLDIER_MODEL_RESOURCE_UNSAFE",
    "FIRST_AUTHENTICATED_HOST_WORLD_ON_ORIGINAL_GUEST_GAME_THREAD_NO_RENDERER",
    "FIRST_AUTHENTICATED_HUMAN_INPUT_ON_ORIGINAL_GAME_THREAD_NO_ACTOR",
    "FIRST_ORIGINAL_GUEST_TOUCH_INPUT_QUEUED_NOT_GAMEPLAY","FIRST_SOURCE_ORIGINAL_GUEST_MERGED_INPUT_QUEUED_NOT_GAMEPLAY",
    # Fixed read-only GPU/original stride evidence.
    "GPU_V36_DECL_CANDIDATE","GPU_V36_SLOT12_PACKED_DECL","GPU_V36_SLOT16_PACKED_DECL","GPU_V36_SLOT20_PACKED_DECL",
    "GPU_V36_SLOT24_NORMPACK_DECL","GPU_V36_SLOT28_FLOAT2_DECL","GPU_V36_BOUND_ARRAY_DRAW","GPU_V36_BOUND_INDEXED_DRAW",
    "GPU_V36_BOUND_SAME_STREAM_12_20",
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
    # App `RoomDiagnostics.exportContents` prepends a safe ASCII header
    # (schema + package/version + base APK sha256 + blank line) then events.
    # Skip ONLY these exact known header keys + blank lines; never emit them.
    HEADER_PREFIXES=("BO_ROOM_DIAGNOSTICS_SCHEMA=","PACKAGE=","VERSION_NAME=",
                     "VERSION_CODE=","BASE_APK_SHA256=")
    for line in lines:
        if not line:
            continue
        if line.startswith(HEADER_PREFIXES):
            continue
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
