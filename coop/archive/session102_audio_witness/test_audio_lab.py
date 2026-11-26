import json
import unittest
import tempfile
from pathlib import Path
from unittest.mock import patch
import audio_lab
from audio_lab import manual, generated, aggregate, FIELDS, TAKE, LOOKUP


class AudioLabTests(unittest.TestCase):
    def test_actual_body_wrappers_and_after_take_patch(self):
        output = manual(TAKE + '\n' + LOOKUP + '\nreturn 0;\n}')
        self.assertEqual(output.count('bo_audio_after_take();'), 1)
        self.assertIn('#include "bo_audio_observer.inc"', output)
        with self.assertRaises(ValueError):
            manual(output)
        original = {'a.c': 'void sub_0007EF50(void) { original(); }',
                    'b.c': 'void sub_0007EDB0(void) { original(); }'}
        patched = generated(original)
        self.assertIn('bo_audio_original_create(void)', patched['a.c'])
        self.assertIn('bo_audio_original_release(void)', patched['b.c'])
        self.assertIn('sub_0007EF50', original['a.c'])
        with self.assertRaises(ValueError):
            generated({'a.c': original['a.c']})
        with self.assertRaises(ValueError):
            generated(dict(original, duplicate=original['a.c']))

    def test_prepare_uses_actual_src_generated_tree_and_preserves_call_references(self):
        with tempfile.TemporaryDirectory() as temp:
            pc = Path(temp)
            gen = pc / 'src/recomp/gen'
            gen.mkdir(parents=True)
            (pc / 'src/bo_engine_lab.inc').write_text('/* existing lab */')
            (pc / 'src/recomp_manual.c').write_text('#include "bo_engine_lab.inc"\n' + TAKE + '\n' + LOOKUP + '\nreturn 0;\n}')
            original = 'void sub_0007EF50(void) { create_body(); }\nvoid sub_0007EDB0(void) { release_body(); }\nvoid caller(void) { sub_0007EF50(); sub_0007EDB0(); }\n'
            source = gen / 'calibration.c'
            source.write_text(original)
            with patch('audio_lab.subprocess.check_output', return_value='b2512fafd3e181e4febfb298f297245b845ef628\n'):
                audio_lab.prepare(pc)
            output = source.read_text()
            self.assertIn('bo_audio_original_create(void)', output)
            self.assertIn('bo_audio_original_release(void)', output)
            self.assertIn('sub_0007EF50(); sub_0007EDB0();', output)
            self.assertTrue((pc / 'src/bo_audio_observer.inc').is_file())
            self.assertTrue((pc / 'src/bo_audio_publication.h').is_file())

    def test_only_bounded_aggregate_counts(self):
        value = {key: 0 for key in FIELDS}
        self.assertEqual(aggregate(json.dumps(value)), value)
        for bad in (dict(value, raw_pointer=123456), dict(value, creates=True), dict(value, creates=-1)):
            with self.assertRaises(ValueError):
                aggregate(json.dumps(bad))


if __name__ == '__main__':
    unittest.main()
