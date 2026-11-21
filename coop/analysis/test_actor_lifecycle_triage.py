import json
import pathlib
import tempfile
import unittest
from actor_lifecycle_triage import analyze

START={"event":"start","probe":"HOOK001","scope":"read_only_no_multiplayer"}
def sample(kind,epoch,actor,ms,**kwargs):
    return {"event":kind,"epoch":epoch,"id":actor,"thread":9988123,"ms":ms,
            "local":kind=="player","vtable":11123990,
            "position":[1234.456,2,3],"anim_name":"PRIVATE_ANI_LABEL",**kwargs}
class ActorCoverageTests(unittest.TestCase):
    def setUp(self):
        self.d=tempfile.TemporaryDirectory()
        self.addCleanup(self.d.cleanup)
    def capture(self,records):
        p=pathlib.Path(self.d.name)/"private-actors.jsonl"
        p.write_text("".join(json.dumps(r)+"\n" for r in records),encoding="utf8")
        return p
    def test_single_epoch_partial_and_sampling_gap_is_not_lifetime(self):
        log=self.capture([START,sample("player",1,1,1000),
            sample("lod",1,2,2000),sample("animation",1,2,3000),
            sample("lod",1,2,14000),sample("lod",1,3,25000),
            sample("animation",1,3,26000),sample("player",1,1,27000)])
        out=analyze(log)
        self.assertEqual(out["observed_epoch_count"],1)
        self.assertEqual(out["events"],{"player":2,"lod":3,"animation":2})
        self.assertEqual(out["lod_correlations"],2)
        self.assertEqual(out["lod_with_animation_correlation"],2)
        self.assertEqual(out["lod_groups_first_seen_10s_after_first_sample"],1)
        self.assertEqual(out["lod_groups_last_seen_10s_before_last_sample"],1)
        self.assertEqual(out["lod_groups_with_observation_gaps_ge_10s"],1)
        self.assertTrue(out["capture_is_partial"])
        self.assertFalse(out["npc_lifetime_proven"])
        self.assertFalse(out["sampling_gaps_are_actor_deaths"])
        text=json.dumps(out)
        for secret in ("9988123","11123990","1234.456","PRIVATE_ANI_LABEL"):
            self.assertNotIn(secret,text)
        self.assertNotIn('"id"',text)
        self.assertNotIn('"epoch"',text)
    def test_multiple_epochs_never_proves_respawn(self):
        log=self.capture([START,sample("lod",1,2,100),
            sample("animation",1,2,200),
            sample("lod",2,2,300),
            sample("animation",2,2,400),
            {"event":"end","reason":"time_or_record_limit","records":4}])
        out=analyze(log)
        self.assertEqual(out["observed_epoch_count"],2)
        self.assertEqual(out["temporary_correlation_count"],2)
        self.assertTrue(out["multiple_epoch_boundaries_observed"])
        self.assertFalse(out["epoch_changes_prove_respawn"])
        self.assertFalse(out["remote_player_spawn_proven"])
        self.assertTrue(out["capture_end_seen"])
    def test_invalid_and_privacy(self):
        p=self.capture([{"event":"start","probe":"wrong","scope":"read_only_no_multiplayer"}])
        with self.assertRaises(ValueError):analyze(p)
        p.write_bytes(b"x"*(2*1024*1024+1))
        with self.assertRaises(ValueError):analyze(p)
    def test_no_lod_is_not_absence_proof(self):
        out=analyze(self.capture([START,sample("player",1,1,55)]))
        self.assertEqual(out["lod_correlations"],0)
        self.assertFalse(out["actor_factory_proven"])
        self.assertFalse(out["npc_lifetime_proven"])
if __name__=="__main__": unittest.main()
