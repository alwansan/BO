import json
from pathlib import Path
import unittest

import lab
from unit_lab import FIELDS, parse, restart_lifecycle_observed


class UnitLabTests(unittest.TestCase):
    def test_restart_requires_each_boundary_and_no_mapping_contradiction(self):
        progress = dict.fromkeys(('restart_armed', 'restart_gamecode_queued',
            'restart_unload_removal', 'restart_same_unit_relinked', 'restart_lifecycle_observed'), True)
        units = dict.fromkeys(FIELDS, 0)
        units.update(ordinal_maps_verified=2, world_membership_verified=2, world_removal_verified=1)
        self.assertTrue(restart_lifecycle_observed(progress, units))
        self.assertFalse(restart_lifecycle_observed(progress, None))
        for marker in progress:
            self.assertFalse(restart_lifecycle_observed(dict(progress, **{marker: False}), units))
        for field in ('ordinal_maps_verified', 'world_membership_verified', 'world_removal_verified'):
            self.assertFalse(restart_lifecycle_observed(progress, dict(units, **{field: 0})))
        for field in ('other_thread_calls', 'ordinal_map_failures', 'world_membership_failures',
                      'world_removal_failures'):
            self.assertFalse(restart_lifecycle_observed(progress, dict(units, **{field: 1})))

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
        source_tick = patched.split('void sub_001F1080(void)\n{', 1)[1]
        self.assertLess(source_tick.index('bo_unit_bind_frame_thread();'), source_tick.index('bo_objective_begin();'))
        self.assertLess(source_tick.index('bo_objective_begin();'), source_tick.index('sub_001F1080_gen();'))
        self.assertLess(source_tick.index('sub_001F1080_gen();'), source_tick.index('bo_objective_end();'))
        self.assertTrue(Path(lab.HERE, 'unit_observer.inc').is_file())

    def test_headless_overlay_is_scoped_to_one_pinned_guard(self):
        patched = lab.patch_text(lab.MENU_OVERLAY_ANCHOR, 'menu_overlay')
        self.assertIn('&& !bo_lab_restart_headless_menu(page) && !m_overlay_assumed', patched)
        with self.assertRaises(ValueError):
            lab.patch_text(lab.MENU_OVERLAY_ANCHOR * 2, 'menu_overlay')
