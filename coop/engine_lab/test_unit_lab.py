import json
from pathlib import Path
import unittest

import lab
from unit_lab import FIELDS, parse


class UnitLabTests(unittest.TestCase):
    def test_report_has_only_consistent_counts(self):
        counts = dict.fromkeys(FIELDS, 0)
        self.assertEqual(parse(json.dumps(counts)), counts)
        for invalid in (
            dict(counts, unit_address=10),
            dict(counts, poll_calls=1, unit_publications=1),
            dict(counts, ordinal_maps_verified=True),
            dict(counts, same_slot_unit_republications=1),
            dict(counts, link_calls=1, world_membership_verified=1),
            dict(counts, unlink_calls=1, world_removal_verified=1),
        ):
            with self.assertRaises(ValueError):
                parse(json.dumps(invalid))

    def test_wrappers_keep_generated_calls(self):
        source = '\n'.join((
            'static int black_p2_banks_make(void)\n{',
            '    black_p2_banks_take();',
            '}',
            lab.MANUAL_ANCHOR,
        ))
        patched = lab.patch_text(source, 'manual')
        for name in ('sub_001EB7F0', 'sub_001EB1F0', 'sub_001EB440'):
            self.assertIn(f'\nvoid {name}(void)\n', patched)
            self.assertEqual(patched.count(f'    {name}_gen();'), 1)
        self.assertIn('bo_unit_bind_frame_thread();\n    sub_001F1080_gen();', patched)
        self.assertTrue(Path(lab.HERE, 'unit_observer.inc').is_file())
