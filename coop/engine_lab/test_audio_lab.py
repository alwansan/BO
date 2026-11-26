import json
from pathlib import Path
import tempfile
import unittest

import audio_lab


class AudioLabTests(unittest.TestCase):
    def test_private_fields_and_duplicates_are_rejected(self):
        data = dict.fromkeys(audio_lab.FIELDS, 0)
        self.assertEqual(audio_lab.parse(json.dumps(data)), data)
        for key, value in (("object_address", 10), ("create_calls", True),
                           ("create_calls", -1), ("create_calls", 100000001)):
            invalid = dict(data)
            invalid[key] = value
            with self.assertRaises(ValueError):
                audio_lab.parse(json.dumps(invalid))
        with self.assertRaises(ValueError):
            audio_lab.parse('{"create_calls":1,"create_calls":2}')

    def test_original_bodies_execute_once_and_drift_is_atomic(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "recomp_0.c"
            original = ("void sub_0007EF50(void)\n{ ORIGINAL_CREATE(); }\n"
                        "void sub_0007EDB0(void)\n{ ORIGINAL_DESTROY(); }\n")
            path.write_text(original)
            audio_lab.instrument_generated(directory)
            result = path.read_text()
            self.assertEqual(result.count("ORIGINAL_CREATE();"), 1)
            self.assertEqual(result.count("ORIGINAL_DESTROY();"), 1)
            self.assertEqual(result.count("bo_audio_original_sub_0007EF50();"), 1)
            self.assertEqual(result.count("bo_audio_original_sub_0007EDB0();"), 1)
            path.write_text(original.replace("sub_0007EDB0", "unknown"))
            before = path.read_bytes()
            with self.assertRaises(ValueError):
                audio_lab.instrument_generated(directory)
            self.assertEqual(path.read_bytes(), before)
            path.write_text(original + original)
            with self.assertRaises(ValueError):
                audio_lab.instrument_generated(directory)
