"""Synthetic owner-log triage validation: no identities or raw content in reports."""
import json
from pathlib import Path
import tempfile
import unittest

from room_event_triage import summarize,compare,read_file,MAX_BYTES_PER_FILE

def write(directory, ident, codes):
    d=Path(directory)
    d.mkdir(parents=True,exist_ok=True)
    p=d/("BLACK-Room-"+str(ident)+"-events.txt")
    p.write_text("".join("1795294904769 "+x+"\n" for x in codes),encoding="ascii")
    return p

class TwoPhoneEventTests(unittest.TestCase):
    def setUp(self):
        self.root=tempfile.TemporaryDirectory()
        self.addCleanup(self.root.cleanup)
        self.a=Path(self.root.name)/"first"
        self.b=Path(self.root.name)/"second"
    def test_detects_repeat_discovery_after_join_and_host_bind_error(self):
        write(self.a,1,["HOST_REQUEST","ROOM_HOSTING",
             "ROOM_LEFT","HOST_REQUEST","ERROR_BINDEXCEPTION"])
        write(self.b,1,["SCAN_STARTED","JOIN_REQUEST","ROOM_JOINED"]+
              ["SCAN_STARTED","SCAN_RESULTS_1"]*12+["SESSION_STOPPED"])
        out=compare(self.a,self.b)
        self.assertEqual(out["first_device_bind_exceptions"],1)
        self.assertEqual(out["second_device_join_success_events"],1)
        self.assertEqual(out["second_device"]["scans_after_join_within_same_log"],12)
        self.assertTrue(out["second_device"]["high_redundant_discovery_evidence"])
        self.assertFalse(out["shared_black_world_proven"])
        self.assertFalse(out["needs_live_owner_phone_test_now"])
        self.assertNotIn("1795294904769",json.dumps(out))
        self.assertNotIn("BLACK-Room-",json.dumps(out))
    def test_unknown_safe_codes_are_counted_but_not_printed(self):
        write(self.a,2,["HOST_REQUEST","PIN_98765432"])
        write(self.b,3,["ROOM_JOINED","SCREEN_NAME_ALWAN"])
        out=compare(self.a,self.b)
        self.assertEqual(out["first_device"]["unknown_event_codes_not_exported"],1)
        self.assertEqual(out["second_device"]["unknown_event_codes_not_exported"],1)
        result=json.dumps(out)
        self.assertNotIn("98765432",result)
        self.assertNotIn("ALWAN",result)
    def test_connected_scans_stop_counting_on_leave(self):
        write(self.a,2,["ROOM_HOSTING"])
        write(self.b,3,["ROOM_JOINED","SCAN_STARTED","ROOM_LEFT"]+
              ["SCAN_STARTED"]*20)
        out=compare(self.a,self.b)
        self.assertEqual(out["second_device"]["scans_after_join_within_same_log"],1)
        self.assertFalse(out["second_device"]["high_redundant_discovery_evidence"])
    def test_malformed_oversized_nonascii_and_symlink_rejected(self):
        item=write(self.a,1,["ROOM_JOINED"])
        self.assertEqual(len(read_file(item)),1)
        for text in ["1795294904769 LEAK_127.0.0.1\n", "1795294904769 LEAK-SECRET\n",
                     "Raw/PRIVATE\n", ""]:
            item.write_text(text)
            with self.assertRaises(ValueError):read_file(item)
        item.write_bytes(b"x"*(MAX_BYTES_PER_FILE+1))
        with self.assertRaises(ValueError):read_file(item)
        item.write_bytes(b"1795294904769 ROOM_JOINED\n\xff")
        with self.assertRaises(ValueError):read_file(item)
        item.unlink()
        (self.a/"BLACK-Room-2-events.txt").symlink_to(Path(self.root.name)/"unknown")
        with self.assertRaises(ValueError):summarize(self.a)

if __name__=="__main__":
    unittest.main()
