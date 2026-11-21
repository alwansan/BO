import json
import pathlib
import tempfile
import unittest
from tdm_reference_profile import STAGE,STAGE_PROFILE,ROOT,profile

def entry(path,size):
    return {"path":ROOT+path,"bytes":size}

def fixture():
    files=[entry(STAGE+p,n) for p,n in STAGE_PROFILE]
    for i in range(16):
        for suffix in (".db",".wdd"):
            files.append(entry(STAGE+"fpguns/bg1_w%02d"%i+suffix,100+i))
    return {"format":"github-auto-upload-v1","files":files}

class Preflight(unittest.TestCase):
    def test_reference_match_only_proves_asset_inventory(self):
        out=profile(fixture())
        self.assertEqual(out["required_stage_files_present_and_correct"],13)
        self.assertEqual(out["weapon_db_wdd_pairs"],16)
        self.assertTrue(out["matches_third_party_reference_asset_profile"])
        for key in ("tdm_runtime_proven","story_coop_actor_compatibility_proven",
                    "windows_binary_compatible_with_android","third_party_source_copied"):
            self.assertFalse(out[key])
    def test_missing_sizes_and_orphans_fail_closed(self):
        data=fixture()
        data["files"][0]["bytes"]+=1
        data["files"].pop(1)
        data["files"].pop()
        out=profile(data)
        self.assertEqual(out["missing_stage_files"],1)
        self.assertEqual(out["stage_file_size_mismatches"],1)
        self.assertEqual(out["unpaired_weapon_resources"],1)
        self.assertFalse(out["matches_third_party_reference_asset_profile"])
    def test_duplicate_and_traversal_rejected(self):
        data=fixture()
        data["files"].append(entry(STAGE+"UNIT_01.BIN",100))
        with self.assertRaisesRegex(ValueError,"collision"):profile(data)
        data=fixture()
        data["files"].append(entry("../BAD",1))
        with self.assertRaisesRegex(ValueError,"invalid item"):profile(data)
    def test_wrong_format_and_bool_byte_rejected(self):
        data=fixture();data["format"]="anything-else"
        with self.assertRaises(ValueError):profile(data)
        data=fixture();data["files"][0]["bytes"]=True
        with self.assertRaises(ValueError):profile(data)
    def test_output_never_includes_per_file_user_metadata(self):
        data=fixture()
        data["files"].append(entry("logs/private_1234567.txt",5))
        out=profile(data)
        self.assertNotIn("private_1234567",json.dumps(out))
if __name__=="__main__":unittest.main()
