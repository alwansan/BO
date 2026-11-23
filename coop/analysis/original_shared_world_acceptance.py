#!/usr/bin/env python3
"""Fail-closed BLACK original-world multiplayer acceptance from coarse phone event logs.

Only event names are parsed. Never print raw lines, paths, tokens, IPs or identities.
A green TLS handshake, scene ticket, bot draw, or APK build is not gameplay proof.
"""
import argparse
import json
import re
from pathlib import Path

EVENT = re.compile(r"^\s*\d{10,16}\s+([A-Z][A-Z0-9_]{2,100})(?:\s|$)")
# These names are future *acceptance contracts*, not existing game implementation.
REQUIRED_HOST = frozenset({
    "ORIGINAL_HOST_EXTRA_ACTOR_PHYSICS_VERIFIED",
    "ORIGINAL_HOST_EXTRA_ACTOR_WEAPON_DAMAGE_VERIFIED",
    "ORIGINAL_HOST_EXTRA_ACTOR_AI_TARGET_VERIFIED",
    "ORIGINAL_HOST_SHARED_OBJECT_AUTHORITY_VERIFIED",
    "ORIGINAL_HOST_SHARED_OBJECTIVES_VERIFIED",
})
REQUIRED_GUEST = frozenset({
    "ORIGINAL_GUEST_REMOTE_ACTOR_VISIBLE_VERIFIED",
    "ORIGINAL_GUEST_HOST_OBJECT_APPLIED_VERIFIED",
    "ORIGINAL_GUEST_HOST_OBJECTIVES_APPLIED_VERIFIED",
    "ORIGINAL_GUEST_START_AUTO_CLOSED_MENU_VERIFIED",
})
HOST_COMMIT = "STORY_HOST_COMMITTED"
GUEST_COMMIT = "STORY_GUEST_COMMITTED"

def events_from_text(text):
    """Accept only timestamped, bounded event names; reject arbitrary log prose."""
    found = set()
    for line in text.splitlines():
        match = EVENT.match(line)
        if match:
            found.add(match.group(1))
    return found

def evaluate(host_text, guest_text):
    host = events_from_text(host_text)
    guest = events_from_text(guest_text)
    missing_host = sorted(REQUIRED_HOST - host)
    missing_guest = sorted(REQUIRED_GUEST - guest)
    # A host commit must be witnessed on both devices, not inferred from the lobby.
    commits = HOST_COMMIT in host and (GUEST_COMMIT in guest or HOST_COMMIT in guest)
    return {
        "status": "ready_for_combined_phone_validation" if commits and not missing_host and not missing_guest else "not_ready",
        "host_commit_witnessed_both_devices": commits,
        "missing_host_contracts": missing_host,
        "missing_guest_contracts": missing_guest,
        "scope": "log_contract_preflight_only_not_runtime_proof",
    }

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host-events", required=True, type=Path)
    parser.add_argument("--guest-events", required=True, type=Path)
    args = parser.parse_args()
    result = evaluate(args.host_events.read_text(encoding="utf-8", errors="replace"),
                      args.guest_events.read_text(encoding="utf-8", errors="replace"))
    print(json.dumps(result, sort_keys=True))
    return 0 if result["status"] == "ready_for_combined_phone_validation" else 1

if __name__ == "__main__":
    raise SystemExit(main())
