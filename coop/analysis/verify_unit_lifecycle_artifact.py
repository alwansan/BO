"""Verify a downloaded unit-lifecycle ZIP against GitHub's artifact metadata."""

import argparse
import hashlib
import io
import json
from pathlib import Path
import re
import zipfile

from coop.engine_lab.unit_lab import parse, restart_lifecycle_observed


MAX_BYTES = 65536
RESTART_MARKERS = (
    "restart_armed",
    "restart_start_forwarded",
    "restart_pause_seen",
    "restart_pause_ready",
    "restart_confirm_ready",
    "restart_gamecode_queued",
    "restart_unload_removal",
    "restart_same_unit_relinked",
    "restart_lifecycle_observed",
)
MENU_MARKERS = ("restart_pause_ready", "restart_confirm_ready")
THREAD_COUNTERS = ("before_frame_thread_calls", "other_thread_calls")
FAILURE_COUNTERS = ("ordinal_map_failures", "world_membership_failures", "world_removal_failures")


def unique_keys(pairs):
    result = dict(pairs)
    if len(result) != len(pairs):
        raise ValueError("duplicate JSON key")
    return result


def verify(archive, artifact, run, commit):
    digest = hashlib.sha256(archive).hexdigest()
    if not re.fullmatch(r"[0-9a-f]{40}", commit) or run <= 0:
        raise ValueError("invalid expected run or commit")
    if (artifact.get("name") != "original-engine-lab-aggregate" or
            artifact.get("expired") is not False or
            artifact.get("size_in_bytes") != len(archive) or
            artifact.get("digest") != f"sha256:{digest}" or
            artifact["workflow_run"]["id"] != run or
            artifact["workflow_run"]["head_sha"] != commit or
            type(artifact.get("id")) is not int or artifact["id"] <= 0):
        raise ValueError("artifact metadata, run, commit or digest mismatch")
    if len(archive) > MAX_BYTES:
        raise ValueError("oversized aggregate ZIP")
    with zipfile.ZipFile(io.BytesIO(archive)) as bundle:
        if bundle.namelist() != ["summary.json"] or bundle.getinfo("summary.json").file_size > MAX_BYTES:
            raise ValueError("expected only a bounded summary.json")
        report = json.loads(bundle.read("summary.json"), object_pairs_hook=unique_keys)
    if (type(report.get("schema")) is not int or report["schema"] != 1 or
            report.get("scope") != "original_PC_engine_unit_lifecycle_only" or
            report.get("unit_lifecycle_only_requested") is not True or
            report.get("restart_after_phases_requested") is not False or
            report.get("android_runtime_verified") is not False or
            report.get("four_players_verified") is not False):
        raise ValueError("artifact is not a Windows unit-lifecycle run summary")

    units = report.get("unit_observations")
    if units is not None:
        units = parse(json.dumps(units))
    progress = report.get("boot_progress", {})
    if not isinstance(progress, dict) or any(type(value) is not bool for value in progress.values()):
        raise ValueError("invalid boot progress")
    clean_exit = report.get("engine_exit_success") is True and report.get("bounded_run_finished") is True
    native_pass = report.get("engine_exit_success") is True and restart_lifecycle_observed(progress, units)
    if report.get("restart_lifecycle_passed", False) is not native_pass or report.get("passed") is not native_pass:
        raise ValueError("reported lifecycle verdict does not match its observations")
    clean_counts = units is not None and all(units[key] == 0 for key in THREAD_COUNTERS + FAILURE_COUNTERS)
    menu_ready = all(progress.get(key) is True for key in MENU_MARKERS)
    paired_counts = (units is not None and units["ordinal_maps_verified"] >= 2 and
                     units["world_membership_verified"] >= 2 and units["world_removal_verified"] >= 1)
    same_unit_verified = (
        native_pass and clean_exit and clean_counts and menu_ready and paired_counts and
        report.get("owner_xbe_sha256_verified") is True and report.get("engine_built") is True
    )
    return {
        "run": run,
        "commit": commit,
        "artifact": artifact["id"],
        "artifact_sha256": digest,
        "artifact_entries": ["summary.json"],
        "clean_engine_exit": clean_exit,
        "restart_progress": {key: progress.get(key) is True for key in RESTART_MARKERS},
        "first_missing_restart_marker": next((key for key in RESTART_MARKERS
                                              if progress.get(key) is not True), None),
        "unit_observations": units,
        "same_unit_removal_and_relink_verified": same_unit_verified,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", required=True, type=int)
    parser.add_argument("--commit", required=True)
    parser.add_argument("--artifact-metadata", required=True, type=Path)
    parser.add_argument("archive", type=Path)
    args = parser.parse_args()
    try:
        with args.archive.open("rb") as source:
            archive = source.read(MAX_BYTES + 1)
        with args.artifact_metadata.open("rb") as source:
            metadata = source.read(MAX_BYTES + 1)
        if len(metadata) > MAX_BYTES:
            raise ValueError("oversized artifact metadata")
        result = verify(archive, json.loads(metadata, object_pairs_hook=unique_keys), args.run, args.commit)
    except (OSError, KeyError, TypeError, AttributeError, ValueError, zipfile.BadZipFile, RuntimeError):
        parser.exit(2, "BO_UNIT_LIFECYCLE_ARTIFACT_REJECTED\n")
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0 if result["same_unit_removal_and_relink_verified"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
