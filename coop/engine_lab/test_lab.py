"""Harness checks do not execute BLACK and cannot certify engine gameplay."""
import importlib.util
from pathlib import Path
import contextlib
import io
import sys
import unittest

HERE = Path(__file__).resolve().parent

class LabTests(unittest.TestCase):
    def module(self):
        spec = importlib.util.spec_from_file_location('engine_lab', HERE / 'lab.py')
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        return module

    def test_missing_original_runtime_evidence_cannot_pass(self):
        lab = self.module()
        self.assertFalse(lab.evaluate({})['passed'])
        self.assertEqual(lab.evaluate({})['blocker'], 'mission_not_reached')

    def test_join_message_and_motion_alone_are_insufficient(self):
        lab = self.module()
        evidence = dict(mission_ticks=1000, p2_ready_ticks=900, p1_move_mm=1000,
                        p2_move_mm=1000, p1_shots=3, p2_shots=3)
        self.assertFalse(lab.evaluate(evidence)['passed'])

    def test_context_corruption_and_missing_proxy_fail_closed(self):
        lab = self.module()
        for key in ('restore_failures', 'proxy_failures'):
            evidence = dict(mission_ticks=1000, p2_ready_ticks=900, phases_completed=8,
                            p1_move_mm=1000, p2_move_mm=1000, p1_shots=3, p2_shots=3,
                            distinct_actor_ticks=900, distinct_proxy_ticks=900,
                            restored_ticks=900, pad0_reports=1000, pad1_reports=1000)
            evidence[key] = 1
            self.assertFalse(lab.evaluate(evidence)['passed'])

    def test_aggregate_parser_rejects_extra_fields(self):
        lab = self.module()
        with self.assertRaises(ValueError):
            lab.parse_aggregate('{"guest_memory":"secret"}')
        with self.assertRaises(ValueError):
            lab.parse_aggregate('{}')
        with self.assertRaises(ValueError):
            lab.parse_aggregate('{"mission_ticks":true}')

    def test_patch_is_anchored_and_rejects_drift(self):
        lab = self.module()
        with self.assertRaises(ValueError):
            lab.patch_text('unexpected upstream body', 'manual')
        with self.assertRaises(ValueError):
            lab.patch_text('unexpected upstream body', 'usb')

    def test_every_acceptance_requirement_is_mandatory(self):
        lab = self.module()
        complete = dict(mission_ticks=1000, p2_ready_ticks=900, phases_completed=8,
            distinct_actor_ticks=900, distinct_proxy_ticks=900, distinct_weapon_ticks=900,
            restored_ticks=900, p1_move_mm=1000, p2_move_mm=1000,
            p1_body_move_mm=1000, p2_body_move_mm=1000, p1_shots=2, p2_shots=2,
            input0_move_reports=100, input1_move_reports=100,
            input0_fire_reports=100, input1_fire_reports=100)
        self.assertTrue(lab.evaluate(complete)['passed'])
        for field in complete:
            missing = dict(complete)
            missing[field] = 0
            self.assertFalse(lab.evaluate(missing)['passed'], field)
        drift = dict(complete, p1_idle_mm=600)
        self.assertFalse(lab.evaluate(drift)['passed'])

    def test_usb_driver_keeps_native_menu_filters(self):
        lab = self.module()
        patched = lab.patch_text(lab.USB_ANCHOR, 'usb')
        self.assertIn('pc_input_apply_report(out, 1)', patched)
        self.assertIn('if (pad) {', patched)
        self.assertEqual(patched.count('bo_lab_pad_report(pad, out)'), 1)

    def test_engine_output_is_never_forwarded(self):
        lab = self.module()
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            code = lab.private_call([sys.executable, '-c',
                "import sys; print('PRIVATE_GAME_BYTES'); print('PRIVATE_ASM', file=sys.stderr)"],
                HERE, 5)
        self.assertEqual(code, 0)
        self.assertEqual(output.getvalue(), '')

    def test_private_boot_progress_emits_only_fixed_booleans(self):
        lab = self.module()
        progress = {}
        code = lab.private_call([sys.executable, '-c',
            "print('XBE loaded: PRIVATE_BYTES'); print('Starting game...'); print('BO_ENGINE_PAD_STARTED')"],
            HERE, 5, progress=progress)
        self.assertEqual(code, 0)
        self.assertEqual(progress, dict(xbe_loaded=True, guest_entry_called=True, usb_pad_polled=True))

    def test_original_frontend_progress_is_distinct_from_usb_poll(self):
        lab = self.module()
        progress = {}
        code = lab.private_call([sys.executable, '-c',
            "print('Starting game...'); print('BO_ENGINE_MENU_TICK')"],
            HERE, 5, progress=progress)
        self.assertEqual(code, 0)
        self.assertEqual(progress, dict(guest_entry_called=True, frontend_ui_polled=True))
        self.assertNotIn('usb_pad_polled', progress)
        self.assertIn("frontend_and_usb_input_not_polled", (HERE / 'lab.py').read_text())

    def test_vs18_environment_uses_active_msvc_with_ninja(self):
        lab = self.module()
        configure, build, executable = lab.windows_build_commands(Path('engine'), {'VisualStudioVersion': '18.0'})
        self.assertIn('Ninja', configure)
        self.assertIn('-DCMAKE_BUILD_TYPE=Release', configure)
        self.assertNotIn('Visual Studio 17 2022', configure)
        self.assertNotIn('/nodeReuse:false', build)
        self.assertEqual(executable, Path('engine/build-lab/black_xbox_recomp.exe'))

    def test_safe_diagnostics_drop_source_text_and_generated_file_names(self):
        lab = self.module()
        errors = {}
        lab.collect_diagnostics('CMake Error: could not find any instance of Visual Studio. PRIVATE_BYTES', errors)
        lab.collect_diagnostics(r'C:/private/src/bo_engine_lab.inc(71): error C2065: PRIVATE_GAME_BYTES', errors)
        lab.collect_diagnostics(r'C:/private/src/recomp/gen/recomp_0001.c(33): error C2143: PRIVATE_ASM', errors)
        self.assertIn('visual_studio_instance_missing', errors['categories'])
        self.assertEqual(errors['compiler_errors'], [dict(code='C2065', source='bo_engine_lab.inc', line=71), dict(code='C2143')])
        self.assertNotIn('PRIVATE', str(errors))
        self.assertNotIn('recomp_0001', str(errors))

    def test_duplicate_aggregate_keys_rejected_even_if_values_agree(self):
        lab = self.module()
        fields = {key: 0 for key in lab.FIELDS}
        payload = __import__('json').dumps(fields)
        duplicate = payload[:-1] + ', "mission_ticks": 0}'
        with self.assertRaisesRegex(ValueError, 'duplicate aggregate field'):
            lab.parse_aggregate(duplicate)
        self.assertEqual(lab.parse_aggregate(payload), fields)

    def test_truncated_aggregate_cannot_pass(self):
        lab = self.module()
        with self.assertRaises(ValueError):
            lab.parse_aggregate('{"mission_ticks":')

if __name__ == '__main__':
    unittest.main()
