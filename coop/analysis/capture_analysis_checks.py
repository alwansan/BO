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

    def test_correlations_are_aggregates_not_actor_identifiers(self):
        def rec(event,epoch,id_,local=False):
            return {
                "event":event, "epoch":epoch, "id":id_, "thread":12345678,
                "local":local, "position_valid":event!="animation",
                "health_valid":event=="player", "position":[1234.5,6,7],
                "vtable":9988123, "anim_name":"PRIVATE_ANIMATION_NAME"
            }
        p=self.fixture([HEADER,
            rec("player",1,1,True), rec("lod",1,1), rec("animation",1,1),
            rec("lod",1,2), rec("animation",1,2),
            rec("player",2,1,True), rec("lod",2,3),
            {"event":"end","reason":"time_or_record_limit","records":7,
             "attempts":14,"read_failures":0}
        ])
        out=summarize(p)
        self.assertEqual(out["correlation_overlap_counts"],{
            "player_lod":1, "player_animation":1,"lod_animation":2,
            "all_three":1,"local_player_lod":1,"local_player_animation":1,
            "player_animation_without_lod":0,
            "lod_animation_without_player":1
        })
        self.assertFalse(out["correlation_overlap_is_actor_proof"])
        self.assertEqual(out["world_epoch_count"],2)
        exported=json.dumps(out)
        for private in ("PRIVATE_ANIMATION_NAME","9988123","1234.5",
                        "12345678"):
            self.assertNotIn(private,exported)
        self.assertNotIn('"id"',exported)
        self.assertNotIn('"epoch"',exported)

    def test_nonoverlap_cannot_claim_missing_actor(self):
        p=self.fixture([HEADER,
            {"event":"player","epoch":1,"id":1,"thread":100,
             "position_valid":True,"health_valid":True,"local":True},
            {"event":"lod","epoch":1,"id":2,"thread":100,
             "position_valid":True,"health_valid":False,"local":False},
            {"event":"animation","epoch":2,"id":1,"thread":100,
             "position_valid":False,"health_valid":False,"local":False}
        ])
        out=summarize(p)
        self.assertTrue(all(x==0 for x in out["correlation_overlap_counts"].values()))
        self.assertFalse(out["runtime_actor_factory_proven"])
        self.assertFalse(out["correlation_overlap_is_actor_proof"])
        self.assertFalse(out["in_game_multiplayer_proven"])

    def test_probe2_mediastore_txt_pattern_is_privacy_safe(self):
        # Synthetic coverage inspired by categories seen in an actual opt-in
        # Android capture; it contains NO copied owner samples or addresses.
        entries=[HEADER]
        entries.append({"event":"player","epoch":1,"id":1,"thread":999008,
                        "ms":200,"local":True,"position_valid":True,
                        "health_valid":True,"position":[888.333,1,2],
                        "resource_valid":False,"render_valid":False,
                        "clip_hash":0})
        entries.append({"event":"animation","epoch":1,"id":1,"thread":999008,
                        "ms":900,"local":True,"position_valid":False,
                        "health_valid":False,"resource_valid":False,
                        "render_valid":False,"clip_hash":101011,
                        "anim_name":"NEVER_EXPORT_PRIVATE_NAME"})
        for i in range(2,15):
            entries.append({"event":"lod","epoch":1,"id":i,"thread":999008,
                            "ms":1100+i,"local":False,"position_valid":True,
                            "health_valid":False,"resource_valid":True,
                            "render_valid":True,"resource320":991001,
                            "render35c":881122})
            entries.append({"event":"animation","epoch":1,"id":i,"thread":999008,
                            "ms":139502,"local":False,"position_valid":False,
                            "health_valid":False,"resource_valid":False,
                            "render_valid":False,"clip_hash":303333,
                            "anim_name":"NEVER_EXPORT_PRIVATE_NAME"})
        p=self.fixture(entries)
        copy=p.with_suffix(".txt")
        copy.write_bytes(p.read_bytes())
        report=summarize(copy)
        self.assertEqual(report["events"],{"player":1,"lod":13,"animation":14})
        self.assertEqual(report["correlation_overlap_counts"]["player_animation_without_lod"],1)
        self.assertEqual(report["correlation_overlap_counts"]["lod_animation_without_player"],13)
        self.assertEqual(report["correlation_overlap_counts"]["player_lod"],0)
        self.assertEqual(report["valid_resource_count"]["lod"],13)
        self.assertEqual(report["valid_render_count"]["lod"],13)
        self.assertEqual(report["nonzero_animation_clip_hash_count"]["animation"],14)
        self.assertEqual(report["sample_observation_span_seconds"],139)
        self.assertEqual(report["distinct_thread_count"],1)
        self.assertTrue(report["capture_is_partial_snapshot"])
        self.assertIn("capture_end_not_seen",report["diagnostic_flags"])
        self.assertFalse(report["runtime_actor_factory_proven"])
        self.assertFalse(report["in_game_multiplayer_proven"])
        output=json.dumps(report)
        for secret in ("NEVER_EXPORT_PRIVATE_NAME","888.333","991001",
                       "881122","999008","101011","303333","139502"):
            self.assertNotIn(secret,output)

    def test_position_variation_clip_changes_never_export_raw_values(self):
        def rec(kind,id_,point=None,clip=0):
            r={"event":kind,"epoch":2,"id":id_,"thread":440044,
               "local":kind=="player","position_valid":point is not None,
               "health_valid":False,"ms":9000,
               "clip_hash":clip,"render_valid":kind=="lod",
               "resource_valid":kind=="lod"}
            if point is not None:r["position"]=point
            return r
        data=[HEADER,
              rec("player",1,[99900.7,10,20]),
              rec("player",1,[99900.9,10,20]),
              rec("animation",1,clip=101010),
              rec("animation",1,clip=101011),
              rec("lod",2,[0,10,20]),rec("lod",2,[0.11,10,20]),
              rec("animation",2,clip=888888),
              rec("animation",2,clip=888889),
              rec("lod",3,[2,3,4]),rec("lod",3,[2.05,3,4]),
              rec("animation",3,clip=888888),
              rec("animation",3,clip=888888),
              {"event":"end","reason":"time_or_record_limit",
               "records":12,"attempts":24,"read_failures":0}]
        report=summarize(self.fixture(data))
        self.assertEqual(report["correlation_motion_heuristics"],{
            "local_player_correlation_position_variation":1,
            "lod_correlation_position_variation":1,
            "lod_animation_multi_clip_correlation":1
        })
        self.assertFalse(report["motion_is_actor_lifecycle_proof"])
        output=json.dumps(report)
        for sentinel in ("99900.7","99900.9","440044","101010","888888"):
            self.assertNotIn(sentinel,output)

    def test_integer_and_bool_guards(self):
        p=self.fixture([HEADER,
          {"event":"player","epoch":1,"id":1,"thread":5,"position_valid":1,"health_valid":True,"local":True},
          {"event":"lod","epoch":1,"id":65,"thread":5,"position_valid":True,"health_valid":True,"local":True},
          {"event":"player","epoch":1,"id":2,"thread":5,"position_valid":False,"health_valid":False,"local":False}])
        r=summarize(p)
        self.assertEqual(r["unknown_or_malformed_line_count"],2)
        self.assertEqual(r["events"]["player"],1)

if __name__=="__main__":unittest.main()
