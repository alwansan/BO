import unittest
from coop.analysis.original_shared_world_acceptance import (
    evaluate, events_from_text, REQUIRED_HOST, REQUIRED_GUEST,
)

def lines(names):
    return "".join(f"1795378511993 {name}\n" for name in names)

class AcceptanceTest(unittest.TestCase):
    def test_current_real_world_host_log_is_not_shared_gameplay(self):
        host = lines(["STORY_HOST_COMMITTED", "FIRST_AUTHENTICATED_HUMAN_INPUT_ON_ORIGINAL_GAME_THREAD_NO_ACTOR", "ORIGINAL_HOST_RUNNING_LOCAL_SCENE_TLS_READY_NO_SHARED_AI"])
        guest = lines(["STORY_HOST_COMMITTED", "MENU_OPEN"])
        result = evaluate(host, guest)
        self.assertEqual(result["status"], "not_ready")
        self.assertEqual(len(result["missing_host_contracts"]), len(REQUIRED_HOST))
        self.assertEqual(len(result["missing_guest_contracts"]), len(REQUIRED_GUEST))

    def test_single_device_claim_cannot_pass(self):
        self.assertEqual(evaluate(lines(REQUIRED_HOST | {"STORY_HOST_COMMITTED"}), lines(REQUIRED_GUEST))["status"], "not_ready")

    def test_all_contracts_is_only_preflight(self):
        result = evaluate(lines(["STORY_HOST_COMMITTED"]) + lines(REQUIRED_HOST), lines(["STORY_GUEST_COMMITTED"]) + lines(REQUIRED_GUEST))
        self.assertEqual(result["status"], "ready_for_combined_phone_validation")
        self.assertIn("not_runtime_proof", result["scope"])

    def test_host_commit_in_guest_log_cannot_impersonate_guest_commit(self):
        host = lines(["STORY_HOST_COMMITTED"]) + lines(REQUIRED_HOST)
        guest = lines(["STORY_HOST_COMMITTED"]) + lines(REQUIRED_GUEST)
        result = evaluate(host, guest)
        self.assertEqual(result["status"], "not_ready")
        self.assertFalse(result["host_commit_witnessed_both_devices"])

    def test_contracts_before_commit_cannot_pass(self):
        self.assertEqual(evaluate(lines(REQUIRED_HOST) + lines(["STORY_HOST_COMMITTED"]),
                                  lines(REQUIRED_GUEST) + lines(["STORY_GUEST_COMMITTED"]))["status"], "not_ready")

    def test_abort_invalidates_previous_evidence(self):
        host = lines(["STORY_HOST_COMMITTED"]) + lines(REQUIRED_HOST)
        guest = lines(["STORY_GUEST_COMMITTED"]) + lines(REQUIRED_GUEST)
        self.assertEqual(evaluate(host, guest + lines(["STORY_ABORTED"]))["status"], "not_ready")

    def test_new_session_cannot_reuse_old_contracts(self):
        host = lines(["STORY_HOST_COMMITTED"]) + lines(REQUIRED_HOST)
        guest = lines(["STORY_GUEST_COMMITTED"]) + lines(REQUIRED_GUEST)
        self.assertEqual(evaluate(host + lines(["STORY_HOST_COMMITTED"]), guest)["status"], "not_ready")

    def test_timestamp_rollback_invalidates_old_scene_contracts(self):
        host = lines(["STORY_HOST_COMMITTED"]) + lines(REQUIRED_HOST)
        guest = lines(["STORY_GUEST_COMMITTED"]) + lines(REQUIRED_GUEST)
        # A later line with an earlier clock value starts an untrusted capture segment.
        host += "1795378511000 ROOM_HOSTING\n"
        self.assertEqual(evaluate(host, guest)["status"], "not_ready")
        self.assertFalse(evaluate(host, guest)["host_commit_witnessed_both_devices"])

    def test_equal_timestamps_remain_accepted(self):
        host = lines(["STORY_HOST_COMMITTED"]) + lines(REQUIRED_HOST)
        guest = lines(["STORY_GUEST_COMMITTED"]) + lines(REQUIRED_GUEST)
        self.assertEqual(evaluate(host, guest)["status"], "ready_for_combined_phone_validation")

    def test_no_sensitive_line_content_is_returned(self):
        result = evaluate("1795378511993 STORY_HOST_COMMITTED secret=private-ip\n", "bad")
        self.assertNotIn("private-ip", str(result))
        self.assertEqual(events_from_text("STORY_HOST_COMMITTED\n"), set())

if __name__ == "__main__":
    unittest.main()
