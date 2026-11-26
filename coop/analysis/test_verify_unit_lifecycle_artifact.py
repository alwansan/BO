"""Synthetic GitHub receipts and reports; no original-engine evidence."""

import hashlib
import io
import json
import unittest
import zipfile

from coop.analysis.verify_unit_lifecycle_artifact import RESTART_MARKERS, verify
from coop.engine_lab.unit_lab import FIELDS


class UnitLifecycleArtifactTest(unittest.TestCase):
    run_id = 123
    commit = "0123456789abcdef0123456789abcdef01234567"

    def setUp(self):
        units = dict.fromkeys(FIELDS, 0)
        units.update(poll_calls=4, unlink_calls=1, link_calls=2, unit_publications=2,
                     ordinal_maps_verified=2, unit_unlinks=1, paired_unit_unlinks=1,
                     unit_link_transitions=2, world_membership_verified=2, world_removal_verified=1)
        self.report = dict(
            schema=1, scope="original_PC_engine_unit_lifecycle_only",
            unit_lifecycle_only_requested=True, restart_after_phases_requested=False,
            android_runtime_verified=False, four_players_verified=False, engine_built=True,
            owner_xbe_sha256_verified=True, bounded_run_finished=True, engine_exit_success=True,
            restart_lifecycle_passed=True, passed=True,
            boot_progress=dict.fromkeys(RESTART_MARKERS, True), unit_observations=units,
        )

    def bundle(self, extra=False):
        output = io.BytesIO()
        with zipfile.ZipFile(output, "w") as archive:
            archive.writestr("summary.json", json.dumps(self.report))
            if extra:
                archive.writestr("extra.txt", "synthetic")
        raw = output.getvalue()
        metadata = dict(id=456, name="original-engine-lab-aggregate", expired=False,
                        size_in_bytes=len(raw), digest="sha256:" + hashlib.sha256(raw).hexdigest(),
                        workflow_run=dict(id=self.run_id, head_sha=self.commit))
        return raw, metadata

    def test_accepts_same_logical_unit_without_same_slot_publication(self):
        result = verify(*self.bundle(), self.run_id, self.commit)
        self.assertTrue(result["same_unit_removal_and_relink_verified"])
        self.assertEqual(result["unit_observations"]["same_slot_unit_republications"], 0)

    def test_clean_exit_and_same_slot_publication_do_not_prove_relink(self):
        self.report["boot_progress"].pop("restart_same_unit_relinked")
        self.report["restart_lifecycle_passed"] = self.report["passed"] = False
        self.report["unit_observations"]["same_slot_unit_republications"] = 1
        result = verify(*self.bundle(), self.run_id, self.commit)
        self.assertTrue(result["clean_engine_exit"])
        self.assertFalse(result["same_unit_removal_and_relink_verified"])

    def test_rejects_thread_contradiction_despite_native_pass(self):
        self.report["unit_observations"]["before_frame_thread_calls"] = 1
        result = verify(*self.bundle(), self.run_id, self.commit)
        self.assertFalse(result["same_unit_removal_and_relink_verified"])

    def test_rejects_wrong_provenance_or_extra_zip_entry(self):
        raw, metadata = self.bundle()
        with self.assertRaisesRegex(ValueError, "mismatch"):
            verify(raw, metadata, self.run_id + 1, self.commit)
        with self.assertRaisesRegex(ValueError, "mismatch"):
            verify(raw, dict(metadata, digest="sha256:" + "0" * 64), self.run_id, self.commit)
        with self.assertRaisesRegex(ValueError, "summary.json"):
            verify(*self.bundle(extra=True), self.run_id, self.commit)
