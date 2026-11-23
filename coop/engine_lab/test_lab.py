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

    def test_runtime_exit_markers_are_only_fixed_booleans(self):
        lab = self.module()
        progress = {}
        code = lab.private_call([sys.executable, '-c',
            "print('[KELVIN] NV2A engine on the CPU: started'); print('BO_ENGINE_FIRMWARE_RETURN'); print('Game returned. Cleaning up...'); print('PRIVATE_GUEST_ADDRESS')"],
            HERE, 5, progress=progress)
        self.assertEqual(code, 0)
        self.assertEqual(progress, dict(cpu_renderer_started=True, firmware_return=True, guest_entry_returned=True))
        self.assertNotIn('PRIVATE', str(progress))

    def test_hal_marker_preserves_original_exit_and_rejects_drift(self):
        lab = self.module()
        original = lab.HAL_RETURN_ANCHOR + '\n' + lab.HAL_SHUTDOWN_ANCHOR
        patched = lab.patch_text(original, 'hal')
        self.assertEqual(patched.count('BO_ENGINE_FIRMWARE_RETURN'), 1)
        self.assertEqual(patched.count('BO_ENGINE_FIRMWARE_SHUTDOWN'), 1)
        self.assertEqual(patched.count('ExitProcess(0);'), 2)
        with self.assertRaises(ValueError):
            lab.patch_text('unknown HAL', 'hal')

    def test_firmware_kind_is_bounded_and_never_raw_original_data(self):
        lab = self.module()
        original = lab.HAL_RETURN_ANCHOR + '\n' + lab.HAL_SHUTDOWN_ANCHOR
        patched = lab.patch_text(original, 'hal')
        for label in ('0', '1', '2', '3', 'OTHER'):
            self.assertEqual(patched.count('BO_ENGINE_FIRMWARE_KIND_' + label), 1)
        self.assertEqual(patched.count('ExitProcess(0);'), 2)
        progress = {}
        code = lab.private_call([sys.executable, '-c',
            "print('BO_ENGINE_FIRMWARE_KIND_2'); print('BO_ENGINE_FIRMWARE_RETURN'); print('PRIVATE_GAME_DATA')"],
            HERE, 5, progress=progress)
        self.assertEqual(code, 0)
        self.assertEqual(progress, dict(firmware_kind_2=True, firmware_return=True))
        self.assertNotIn('PRIVATE', str(progress))
        other = {}
        lab.private_call([sys.executable, '-c',
            "print('BO_ENGINE_FIRMWARE_KIND_OTHER'); print('BO_ENGINE_FIRMWARE_KIND_9999')"],
            HERE, 5, progress=other)
        self.assertEqual(other, dict(firmware_kind_other=True))

    def test_original_quick_reboot_launch_page_categories_exclude_raw_data(self):
        lab = self.module()
        progress = {}
        # Exact pinned runtime log shape; private paths, title IDs and values
        # must NEVER be materialized in public evidence.
        records = [
            "  [KERNEL] launch data page 0x00200000: type=2 titleid=0xAABBCCDD path='D:\\PRIVATE\\game.xbe'",
            "  [KERNEL]   launch data: 00000007 DEADBEEF CAFEBABE 00000000 00000000 00000000 00000000 00000000",
        ]
        for record in records:
            lab.collect_launch_hints(record, progress)
        self.assertEqual(progress, dict(launch_page_present=True, launch_type_2=True,
                                        launch_path_present=True, launch_word0_other=True))
        self.assertNotIn('PRIVATE', str(progress))
        self.assertNotIn('AABBCCDD', str(progress))
        self.assertNotIn('DEADBEEF', str(progress))

    def test_quick_reboot_launch_absence_and_malformed_lines_fail_closed(self):
        lab = self.module()
        progress = {}
        for bad in ("  [KERNEL] launch data page PRIVATE TYPE titleid PRIVATE",
                    "  [KERNEL]   launch data: 99999999", "PRIVATE_GAME_DATA",
                    "  [KERNEL] launch data page 0x00INVALID: type=1 titleid=0x00000000 path=''",
                    "  [KERNEL]   launch data: " + "FFFFFFFF " * 100):
            lab.collect_launch_hints(bad, progress)
        self.assertEqual(progress, {})
        lab.collect_launch_hints("  [KERNEL] no launch data page set", progress)
        self.assertEqual(progress, dict(launch_page_absent=True))

    def test_private_original_kernel_log_is_streamed_into_fixed_launch_flags(self):
        lab = self.module()
        progress = {}
        program = (
            "print('  [KERNEL] launch data page 0x000A1000: type=1 titleid=0x1234ABCD path=\\'\\\\\\\\PRIVATE\\\\\\\\xbe.xbe\\'');"
            "print('  [KERNEL]   launch data: 00000002 00000000 00000000 00000000 00000000 00000000 00000000 00000000');"
            "print('BO_ENGINE_FIRMWARE_KIND_2'); print('PRIVATE_RAW_BYTES')"
        )
        code = lab.private_call([sys.executable, '-c', program], HERE, 5, progress=progress)
        self.assertEqual(code, 0)
        self.assertEqual(progress, dict(launch_page_present=True, launch_type_1=True,
                                        launch_path_present=True, launch_word0_2=True,
                                        firmware_kind_2=True))
        self.assertNotIn('PRIVATE', str(progress))

    def test_private_original_dvd_media_breadcrumbs_are_bounded_only(self):
        lab = self.module()
        progress = {}
        for line in (
            r'  [FILE] \Device\CdRom0 -> synthetic DVD device handle',
            r'  [FILE] \??\Device\CdRom0 -> synthetic DVD device handle',
            '  [FILE] DVD MODE SENSE(10) page 0x3E, 32 bytes -> authentication page',
            '  [FILE] DVD device IOCTL 0x4D014 (in=44 out=0) -> STATUS_SUCCESS',
            r'  [FILE] NtCreateFile FAILED: C:\PRIVATE\game.bin (err=2)',
        ):
            lab.collect_boot_media_hints(line, progress)
        self.assertEqual(progress, dict(dvd_device_opened=True, dvd_mode_sense_seen=True,
                                         dvd_other_ioctl_seen=True, file_open_failed_seen=True))
        for private in ('PRIVATE', 'game.bin', '0x4D014', '0x3E', 'err=2'):
            self.assertNotIn(private, str(progress))

    def test_private_media_hints_reject_forged_paths_and_arbitrary_data(self):
        lab = self.module()
        progress = {}
        for line in (
            r'  [FILE] \PRIVATE\CdRom0 -> synthetic DVD device handle',
            r'  [FILE] \Device\CdRom0\SECRET -> synthetic DVD device handle',
            '  [FILE] DVD MODE SENSE(10) page PRIVATE, 30 bytes -> authentication page',
            '  [FILE] DVD device IOCTL INVALID (in=1 out=1) -> STATUS_SUCCESS',
            '  [FILE] DVD MODE SENSE(10) page 0x3E, 12 bytes -> authentication page: EXTRA',
            'NtCreateFile FAILED: ' + 'PRIVATE' * 2000,
        ):
            lab.collect_boot_media_hints(line, progress)
        self.assertEqual(progress, {})

    def test_private_call_classifies_media_before_quickreboot_without_leaks(self):
        lab = self.module()
        private_lines = [
            r'  [FILE] \Device\CdRom0 -> synthetic DVD device handle',
            '  [FILE] DVD MODE SENSE(10) page 0x3E, 32 bytes -> authentication page',
            r'  [FILE] NtCreateFile FAILED: C:\PRIVATE\internal.dat (err=2)',
            'BO_ENGINE_FIRMWARE_KIND_2',
        ]
        progress = {}
        script = 'for line in ' + repr(private_lines) + ': print(line)'
        code = lab.private_call([sys.executable, '-c', script], HERE, 5, progress=progress)
        self.assertEqual(code, 0)
        self.assertEqual(progress, dict(dvd_device_opened=True, dvd_mode_sense_seen=True,
                                         file_open_failed_seen=True, firmware_kind_2=True))
        self.assertNotIn('PRIVATE', str(progress))

    def test_original_dashboard_error_is_classified_without_guest_values(self):
        lab = self.module()
        page = ("  [KERNEL] launch data page 0x00123456: "
                "type=1 titleid=0xAABBCCDD path=''")
        for parameter, label in ((1, 'invalid_xbe'), (2, 'harddrive'),
                                 (3, 'region'), (4, 'parental_control'),
                                 (5, 'media_type'), (65535, 'other')):
            progress = {}
            lab.collect_launch_hints(page, progress)
            line = ("  [KERNEL]   launch data: 00000001 12345678 "
                    + format(parameter, '08X')
                    + " 00000000 00000000 00000000 00000000 00000000")
            lab.collect_launch_hints(line, progress)
            self.assertTrue(progress['dashboard_error_' + label])
            self.assertTrue(progress['launch_word0_1'])
            for private in ('AABBCCDD', '00123456', '12345678', format(parameter, '08X')):
                self.assertNotIn(private, str(progress))

    def test_dashboard_error_does_not_infer_from_unrelated_launch_types(self):
        lab = self.module()
        line = ("  [KERNEL]   launch data: 00000001 12345678 00000005 "
                "00000000 00000000 00000000 00000000 00000000")
        for kind in (0, 2, 3):
            progress = {}
            lab.collect_launch_hints(
                "  [KERNEL] launch data page 0x00123456: "
                + "type=" + str(kind) + " titleid=0xAABBCCDD path=''", progress)
            lab.collect_launch_hints(line, progress)
            self.assertFalse(any(k.startswith('dashboard_error_') for k in progress))
        progress = {}
        lab.collect_launch_hints(line, progress)
        self.assertFalse(any(k.startswith('dashboard_error_') for k in progress))

    def test_private_stream_dashboard_error_omits_owner_path_and_title_id(self):
        lab = self.module()
        lines = [
            "  [KERNEL] launch data page 0x00991111: type=1 titleid=0xABCDEF12 path=''",
            "  [KERNEL]   launch data: 00000001 FFFF1234 00000005 "
            "00000000 00000000 00000000 00000000 00000000",
            "BO_ENGINE_FIRMWARE_KIND_2",
        ]
        progress = {}
        script = 'for line in ' + repr(lines) + ': print(line)'
        code = lab.private_call([sys.executable, '-c', script], HERE, 5, progress=progress)
        self.assertEqual(code, 0)
        self.assertTrue(progress['dashboard_error_media_type'])
        self.assertTrue(progress['firmware_kind_2'])
        self.assertNotIn('ABCDEF12', str(progress))
        self.assertNotIn('FFFF1234', str(progress))

    def test_truncated_aggregate_cannot_pass(self):
        lab = self.module()
        with self.assertRaises(ValueError):
            lab.parse_aggregate('{"mission_ticks":')

if __name__ == '__main__':
    unittest.main()
