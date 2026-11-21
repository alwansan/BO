import json
import pathlib
import tempfile
import unittest
from summarize_probe import summarize

HEADER={"event":"start","probe":"HOOK001","scope":"read_only_no_multiplayer"}
def line(data):return (json.dumps(data,separators=(",",":"))+"\n").encode()

class SummaryChecks(unittest.TestCase):
    def fixture(self,items):
        d=tempfile.TemporaryDirectory()
        self.addCleanup(d.cleanup)
        p=pathlib.Path(d.name)/"private.jsonl"
        p.write_bytes(b"".join(line(i) for i in items))
        return p

    def test_no_raw_guest_data_leak(self):
        p=self.fixture([HEADER,
          {"event":"player","epoch":1,"id":7,"thread":9001,
           "position_valid":True,"health_valid":True,"local":True,
           "position":[456.789,-12,78], "vtable":222333444,"anim_name":"PRIVATE_SOLDIER","health":999},
          {"event":"lod","epoch":1,"id":8,"thread":9002,
           "position_valid":True,"health_valid":False,"local":False,
           "resource320":343434,"render35c":123456789},
          {"event":"animation","epoch":2,"id":1,"thread":9001,
           "position_valid":False,"health_valid":False,"local":False},
          {"event":"end","reason":"time_or_record_limit","records":3,"attempts":7,"read_failures":1}])
        result=summarize(p)
        serialized=json.dumps(result)
        for secret in ("PRIVATE_SOLDIER","456.789","222333444","123456789","9001","9002","343434"):
            self.assertNotIn(secret,serialized)
        self.assertEqual(result["events"],{"player":1,"lod":1,"animation":1})
        self.assertEqual(result["distinct_thread_count"],2)
        self.assertEqual(result["world_epoch_count"],2)
        self.assertEqual(result["max_distinct_correlations_in_epoch"],2)
        self.assertEqual(result["local_player_sample_count"],1)
        self.assertTrue(result["capture_end_seen"])
        self.assertFalse(result["in_game_multiplayer_proven"])
        self.assertEqual(result["diagnostic_flags"],[])

    def test_malformed_or_wrong_header(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/"log"
            p.write_bytes(b'{"event":"player"}\n')
            with self.assertRaisesRegex(ValueError,"missing start"):summarize(p)
            p.write_bytes(line({"event":"start","probe":"WRONG","scope":"read_only_no_multiplayer"}))
            with self.assertRaisesRegex(ValueError,"unexpected capture header"):summarize(p)

    def test_oversized_rejected_and_bad_lines_counted(self):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/"log"
            p.write_bytes(b"x"*(2*1024*1024+1))
            with self.assertRaisesRegex(ValueError,"too large"):summarize(p)
            p.write_bytes(line(HEADER)+b"not a JSON object\n")
            self.assertEqual(summarize(p)["unknown_or_malformed_line_count"],1)

    def test_untrusted_end_reason_type_cannot_crash_parser(self):
        p=self.fixture([HEADER, {"event":"end","reason":["not","hashable"]}])
        r=summarize(p)
        self.assertEqual(r["end_reasons"],{"other":1})

    def test_no_samples_and_reader_denial_are_safe_diagnostics_only(self):
        p=self.fixture([HEADER, {"event":"end", "reason":"world_read_unavailable",
                                 "records":0, "attempts":64, "read_failures":64}])
        result=summarize(p)
        self.assertEqual(result["diagnostic_flags"], [
            "no_samples","no_player_samples","no_lod_samples",
            "no_animation_samples","reader_unavailable"])
        self.assertFalse(result["runtime_actor_factory_proven"])
        self.assertFalse(result["in_game_multiplayer_proven"])

    def test_truncated_capture_has_incomplete_flag_without_private_data(self):
        p=self.fixture([HEADER, {"event":"player", "epoch":1,"id":1,"thread":99,
                                 "position_valid":True,"health_valid":True,"local":True,
                                 "position":[34,12,56],"vtable":9988123,"anim_name":"private"}])
        result=summarize(p)
        self.assertEqual(result["diagnostic_flags"], [
            "capture_end_not_seen","no_lod_samples","no_animation_samples"])
        self.assertNotIn("private",json.dumps(result))
        self.assertNotIn("9988123",json.dumps(result))

    def test_integer_and_bool_guards(self):
        p=self.fixture([HEADER,
          {"event":"player","epoch":1,"id":1,"thread":5,"position_valid":1,"health_valid":True,"local":True},
          {"event":"lod","epoch":1,"id":65,"thread":5,"position_valid":True,"health_valid":True,"local":True},
          {"event":"player","epoch":1,"id":2,"thread":5,"position_valid":False,"health_valid":False,"local":False}])
        r=summarize(p)
        self.assertEqual(r["unknown_or_malformed_line_count"],2)
        self.assertEqual(r["events"]["player"],1)

if __name__=="__main__":unittest.main()
