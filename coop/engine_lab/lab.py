#!/usr/bin/env python3
"""Private original-engine runner. Only allowlisted aggregate JSON may leave it."""
import argparse
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import threading
import time
from audio_lab import instrument_generated, parse as parse_audio
from unit_lab import parse as parse_units, restart_lifecycle_observed

PC_PIN = 'b2512fafd3e181e4febfb298f297245b845ef628'
RUNTIME_PIN = 'e4b621706a3b5e58c084838d1f27547548cf27a8'
DSP_PIN = 'aab3649fc452daf03da8b5e36fdc0c308d874101'
XBE_SHA256 = 'df2739c372d254a90aeeccf5971d12097f22d89fbb90db021fda96dd4deb68f6'
HERE = Path(__file__).resolve().parent
FIELDS = frozenset('mission_ticks p2_ready_ticks phases_completed distinct_actor_ticks distinct_proxy_ticks distinct_weapon_ticks p1_body_move_mm p2_body_move_mm restored_ticks restore_failures proxy_failures restore_scope_failures restore_actor_grid_failures restore_actor_motion_failures restore_actor_weapon_failures restore_actor_model_failures restore_actor_tail_failures restore_grid_node_failures restore_grid_other_failures p1_proxy_missing_ticks p1_body_missing_ticks p2_proxy_missing_ticks p2_body_missing_ticks proxy_pool_low_ticks proxy_pool_unknown_ticks scripted_control_ticks restored_immutable_ticks restore_grid_only_ticks grid_region_head_ticks grid_region_cell_ticks grid_region_links_ticks p2_distinct_physical_ticks p2_setup_wait_ticks p2_playing_ticks scripted_owner_ticks scripted_actor_ticks scripted_both_ticks scripted_unheld_ticks p1_move_mm p2_move_mm p1_idle_mm p2_idle_mm p1_shots p2_shots pad0_reports pad1_reports input0_move_reports input1_move_reports input0_fire_reports input1_fire_reports epoch_changes invalid_samples phase control_ready ui_ticks ui_category ui_category_changes'.split())
FIELDS |= frozenset((
    'objective_verified_frames', 'objective_rejected_frames', 'objective_max_records',
    'objective_changed_records', 'objective_counter_advances', 'objective_entries_reaching_state_3',
))
MANUAL_ANCHOR = '''void sub_001F1080(void)
{
    sub_001F1080_gen();
    black_p2_tick();
}'''
HAL_RETURN_ANCHOR = '''        "HalReturnToFirmware: routine=%u (exiting)", Routine);
    ExitProcess(0);'''
HAL_SHUTDOWN_ANCHOR = '''    xbox_log(XBOX_LOG_INFO, XBOX_LOG_HAL, "HalInitiateShutdown (exiting)");
    ExitProcess(0);'''
MENU_ANCHOR = '''    black_native_menu_tick(ui);
    sub_00124330_gen();'''
USB_ANCHOR = '''    out[0] = 0;
    out[1] = 20;
    if (pad) {'''
MENU_OVERLAY_ANCHOR = '    if (m_page >= 0 && !m_overlay_assumed && (!m_overlay_seen || GetTickCount64() - m_overlay_seen > 1000)) {'


def parse_aggregate(text):
    if len(text) > 8192:
        raise ValueError('oversized aggregate')
    def reject_duplicate_keys(pairs):
        obj = {}
        for key, value in pairs:
            if key in obj:
                raise ValueError('duplicate aggregate field')
            obj[key] = value
        return obj
    data = json.loads(text, object_pairs_hook=reject_duplicate_keys)
    if not isinstance(data, dict) or set(data) != FIELDS:
        raise ValueError('non-aggregate field')
    if any(type(v) is not int or not 0 <= v <= 100000000 for v in data.values()):
        raise ValueError('non-aggregate value')
    return data


def original_p1_actor_context_restored(data):
    """The engine owns P1's core+0x30 spatial-list node (+0x20..+0x57).

    Pinned MIT PC black_p2_find_links/black_p2_swap do NOT exchange that
    world-owned node. Only its opaque +0x20..+0x2F head is observed to change
    in real PC run 45233723518. Any actor bytes outside the node, any node
    cell/neighbor link, any original owner/core swap or inconsistent count
    MUST fail original P1 context restoration. The legacy whole-actor
    memcmp counters remain in the output for independent auditing.
    """
    count = data.get('restored_immutable_ticks', 0)
    grid = data.get('restore_grid_only_ticks', 0)
    exact = data.get('restored_ticks', 0)
    return (count >= 720 and
            exact + grid == count and
            data.get('restore_failures', 0) == grid and
            data.get('restore_actor_grid_failures', 0) == grid and
            data.get('restore_grid_node_failures', 0) == grid and
            data.get('grid_region_head_ticks', 0) == grid and
            all(data.get(k, 0) == 0 for k in (
                'restore_scope_failures', 'restore_grid_other_failures',
                'restore_actor_motion_failures', 'restore_actor_weapon_failures',
                'restore_actor_model_failures', 'restore_actor_tail_failures',
                'grid_region_cell_ticks', 'grid_region_links_ticks')))


def evaluate(data):
    """This evaluates observations; no unit test or join log is engine evidence."""
    checks = [
        ('mission_not_reached', data.get('mission_ticks', 0) >= 720),
        ('p2_not_ready', data.get('p2_ready_ticks', 0) >= 720),
        ('input_phases_incomplete', data.get('phases_completed', 0) >= 8),
        ('actor_identity_unproven', data.get('distinct_actor_ticks', 0) >= 720),
        ('physical_proxy_identity_unproven', data.get('distinct_proxy_ticks', 0) >= 720 and data.get('proxy_failures', 0) == 0),
        ('weapon_identity_unproven', data.get('distinct_weapon_ticks', 0) >= 720),
        ('physical_motion_unproven', data.get('p1_body_move_mm', 0) >= 100 and data.get('p2_body_move_mm', 0) >= 100),
        ('p1_context_not_restored', original_p1_actor_context_restored(data)),
        ('mission_epoch_changed', data.get('epoch_changes', 0) == 0),
        ('invalid_observation', data.get('invalid_samples', 0) == 0),
        ('p1_input_not_delivered', data.get('input0_move_reports', 0) >= 10 and data.get('input0_fire_reports', 0) >= 10),
        ('p2_input_not_delivered', data.get('input1_move_reports', 0) >= 10 and data.get('input1_fire_reports', 0) >= 10),
        ('p1_input_motion_unproven', data.get('p1_move_mm', 0) >= 100 and data.get('p1_move_mm', 0) > 2 * data.get('p1_idle_mm', 0) + 50),
        ('p2_input_motion_unproven', data.get('p2_move_mm', 0) >= 100 and data.get('p2_move_mm', 0) > 2 * data.get('p2_idle_mm', 0) + 50),
        ('p1_original_weapon_effect_unproven', data.get('p1_shots', 0) >= 1),
        ('p2_original_weapon_effect_unproven', data.get('p2_shots', 0) >= 1),
    ]
    failures = [name for name, ok in checks if not ok]
    return {'passed': not failures, 'blocker': failures[0] if failures else 'none', 'failed_checks': failures}


def patch_text(text, kind):
    text = text.replace('\r\n', '\n')
    if kind == 'manual':
        if text.count(MANUAL_ANCHOR) != 1:
            raise ValueError('manual source drift')
        audio_anchor = 'static int black_p2_banks_make(void)\n{'
        if text.count(audio_anchor) != 1 or text.count('    black_p2_banks_take();') != 1:
            raise ValueError('audio source drift')
        text = text.replace(audio_anchor, '#include "bo_audio_observer.inc"\n' + audio_anchor)
        text = text.replace('    black_p2_banks_take();',
                            '    black_p2_banks_take();\n    bo_audio_after_take();')
        unit_hooks = ('sub_001EB7F0', 'sub_001EB1F0', 'sub_001EB440')
        if any(re.search(r'\bvoid\s+' + name + r'\s*\(', text) for name in unit_hooks):
            raise ValueError('unit observer source drift')
        return text.replace(MANUAL_ANCHOR, '''#include "bo_unit_observer.inc"
#include "bo_objective_observer.inc"
#include "bo_engine_lab.inc"
extern void sub_001EB7F0_gen(void);
extern void sub_001EB1F0_gen(void);
extern void sub_001EB440_gen(void);

/* ECX is the stream poll descriptor; unlink receives it at entry ESP+4.
 * Define the wrappers in recomp_manual.c so generate.py finds both names. */
void sub_001EB7F0(void)
{
    const uint32_t unit = g_ecx;
    const int slot = bo_unit_enter(BO_UNIT_POLL, unit);
    const uint32_t before = slot < 0 ? 0 : GUEST32(unit);
    sub_001EB7F0_gen();
    bo_unit_after_poll(slot, unit, before, g_eax);
}

void sub_001EB1F0(void)
{
    const uint32_t unit = black_pc_ram_span(g_esp, 8u) ? GUEST32(g_esp + 4u) : 0;
    const int slot = bo_unit_enter(BO_UNIT_UNLINK, unit);
    const int linked = slot >= 0 && GUEST8(unit + 0x38u);
    const uint32_t index = linked ? GUEST32(unit + 8u) : 0;
    const unsigned removals = bo_lab_unit_count(BO_UNIT_WORLD_UNLINK_VERIFIED);
    BoUnitObjects before = {0};
    if (linked && bo_unit_mapping(unit, &before) > 0 &&
        bo_unit_world_members(before) != (int)before.count) before.count = 0;
    sub_001EB1F0_gen();
    if (linked) {
        bo_unit_after_unlink(slot, unit, index, before);
        bo_lab_restart_after_unlink(removals);
    }
}

void sub_001EB440(void)
{
    const uint32_t unit = black_pc_ram_span(g_esp, 8u) ? GUEST32(g_esp + 4u) : 0;
    const int slot = bo_unit_enter(BO_UNIT_LINK, unit);
    const int linking = slot >= 0 && GUEST32(unit) == 0x1Cu && !GUEST8(unit + 0x38u);
    sub_001EB440_gen();
    if (linking) bo_unit_after_link(slot, unit);
}

void sub_001F1080(void)
{
    bo_unit_bind_frame_thread();
    const int owner_thread = InterlockedCompareExchange(&bo_unit_frame_thread, 0, 0) == (LONG)GetCurrentThreadId();
    if (owner_thread) bo_objective_begin();
    sub_001F1080_gen();
    if (owner_thread) bo_objective_end();
    bo_lab_before();
    black_p2_tick();
    bo_lab_after();
}''')
    if kind == 'hal':
        if text.count(HAL_RETURN_ANCHOR) != 1 or text.count(HAL_SHUTDOWN_ANCHOR) != 1:
            raise ValueError('HAL exit source drift')
        mode_lines = (
            '    if (Routine == 0) fputs("BO_ENGINE_FIRMWARE_KIND_0\\n", stderr);\n'
            '    else if (Routine == 1) fputs("BO_ENGINE_FIRMWARE_KIND_1\\n", stderr);\n'
            '    else if (Routine == 2) fputs("BO_ENGINE_FIRMWARE_KIND_2\\n", stderr);\n'
            '    else if (Routine == 3) fputs("BO_ENGINE_FIRMWARE_KIND_3\\n", stderr);\n'
            '    else fputs("BO_ENGINE_FIRMWARE_KIND_OTHER\\n", stderr);\n'
        )
        text = text.replace(HAL_RETURN_ANCHOR, HAL_RETURN_ANCHOR.replace(
            '    ExitProcess(0);', mode_lines +
            '    fputs("BO_ENGINE_FIRMWARE_RETURN\\n", stderr);\n    ExitProcess(0);'))
        text = text.replace(HAL_SHUTDOWN_ANCHOR, HAL_SHUTDOWN_ANCHOR.replace(
            '    ExitProcess(0);', '    fputs("BO_ENGINE_FIRMWARE_SHUTDOWN\\n", stderr);\n    ExitProcess(0);'))
        return '#include <stdio.h>\n' + text
    if kind == 'menu':
        focus_anchor = '    if (active) {\n        /* Hidden once the page is loaded'
        request_anchor = '    GUEST32(reg + 0x370u) = 1u;\n    fprintf(stderr, "[MENU] gamecode'
        if any(text.count(anchor) != 1 for anchor in (MENU_ANCHOR, focus_anchor, request_anchor)):
            raise ValueError('frontend source drift')
        text = text.replace(focus_anchor, '''    bo_lab_restart_menu_tick(page, pc_menu_test_focus(),
        ready && active && GUEST32(ui + 0x3884u) == 0u && GUEST32(ui + 0x388Cu) == 0u);
''' + focus_anchor)
        text = text.replace(request_anchor, request_anchor.replace(
            '    fprintf', '    bo_lab_restart_gamecode(name);\n    fprintf'))
        return text.replace(MENU_ANCHOR, '''    black_native_menu_tick(ui);
    if (ui && ui == GUEST32(0x002D199Cu) && black_pc_ram_span(ui, 0x3960u)) {
        char bo_lab_page[40];
        bnm_get_string(ui + 0x3840u, bo_lab_page, sizeof(bo_lab_page));
        bo_lab_ui_page(bo_lab_page);
    }
    sub_00124330_gen();''')
    if kind == 'usb':
        if text.count(USB_ANCHOR) != 1:
            raise ValueError('USB source drift')
        text = text.replace(USB_ANCHOR, '''    out[0] = 0;
    out[1] = 20;
    if (bo_lab_pad_report(pad, out)) {
        if (!pad) pc_input_apply_report(out, 1);
        return 20;
    }
    if (pad) {''')
        return 'extern int bo_lab_pad_report(int, unsigned char *);\n' + text
    if kind == 'menu_overlay':
        if text.count(MENU_OVERLAY_ANCHOR) != 1:
            raise ValueError('pinned native menu overlay source drift')
        return 'extern int bo_lab_restart_headless_menu(const char *page);\n' + text.replace(
            MENU_OVERLAY_ANCHOR,
            MENU_OVERLAY_ANCHOR.replace('&& !m_overlay_assumed',
                '&& !bo_lab_restart_headless_menu(page) && !m_overlay_assumed'))
    if kind == 'hdd_file':
        # Pin the Windows backend only, never touch POSIX/XBE/guest code.
        checks = (
            '#if defined(_WIN32)\n/* ====================  Win32 backend',
            '    /* A partition device opened as a directory.',
            '    if (h == INVALID_HANDLE_VALUE) {\n        DWORD err = GetLastError();',
            '    *FileHandle = h;\n    if (IoStatusBlock) {',
            '    if (result || GetLastError() == ERROR_HANDLE_EOF) {',
            '    XBOX_TRACE(XBOX_LOG_FILE, "NtClose(handle=%p)", Handle);',
            '    switch (FsInformationClass) {',
        )
        # NtClose and filesystem-query anchors also exist in POSIX code.
        # We patch the FIRST (Windows) occurrence only.
        counts = (1, 1, 1, 1, 1, 2, 2)
        if any(text.count(anchor) != count for anchor, count in zip(checks, counts)):
            raise ValueError('pinned Xbox Windows harddrive I/O source drift')
        text = '#include <wchar.h>\n' + text
        text = text.replace('#if defined(_WIN32)\n/* ====================  Win32 backend',
            r'''#if defined(_WIN32)
static HANDLE bo_hdd_partition0_handle = INVALID_HANDLE_VALUE;
/* ====================  Win32 backend''')
        text = text.replace('    /* A partition device opened as a directory.',
            r'''    /* BO: private lab only; original NTSTATUS/guest bytes unchanged. */
    const BOOL bo_hdd0_path = wcsstr(win_path, L"\\Partition0.img") != NULL;
    if (bo_hdd0_path) {
        fputs("BO_ENGINE_HDD_PART0_OPEN_TRIED\n", stderr);
        if (CreateOptions & XBOX_FILE_DIRECTORY_FILE)
            fputs("BO_ENGINE_HDD_PART0_DIRECTORY_OPEN\n", stderr);
    }
    /* A partition device opened as a directory.''')
        text = text.replace('    if (h == INVALID_HANDLE_VALUE) {\n        DWORD err = GetLastError();',
            r'''    if (h == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (bo_hdd0_path)
            fputs("BO_ENGINE_HDD_PART0_OPEN_FAILED\n", stderr);''')
        text = text.replace('    *FileHandle = h;\n    if (IoStatusBlock) {',
            r'''    *FileHandle = h;
    if (bo_hdd0_path) {
        bo_hdd_partition0_handle = h;
        fputs("BO_ENGINE_HDD_PART0_OPEN_OK\n", stderr);
    }
    if (IoStatusBlock) {''')
        text = text.replace('    if (result || GetLastError() == ERROR_HANDLE_EOF) {',
            r'''    const DWORD bo_hdd_read_error = result ? ERROR_SUCCESS : GetLastError();
    if (bo_hdd_partition0_handle != INVALID_HANDLE_VALUE &&
        FileHandle == bo_hdd_partition0_handle && ByteOffset &&
        ByteOffset->QuadPart == 0x800 && Length >= 512) {
        fputs("BO_ENGINE_HDD_PART0_SECTOR4_READ\n", stderr);
        if (result && bytes_read >= 512 &&
            memcmp(Buffer, "****PARTINFO****", 16) == 0)
            fputs("BO_ENGINE_HDD_PART0_SECTOR4_VALID\n", stderr);
        else
            fputs("BO_ENGINE_HDD_PART0_SECTOR4_INVALID\n", stderr);
    }
    if (result || bo_hdd_read_error == ERROR_HANDLE_EOF) {''', 1)
        # Preserve the pre-instrumentation last-error diagnostic after stderr I/O.
        text = text.replace(
            'FileHandle, Length, GetLastError());',
            'FileHandle, Length, bo_hdd_read_error);', 1)
        text = text.replace('    XBOX_TRACE(XBOX_LOG_FILE, "NtClose(handle=%p)", Handle);',
            r'''    if (Handle == bo_hdd_partition0_handle)
        bo_hdd_partition0_handle = INVALID_HANDLE_VALUE;
    XBOX_TRACE(XBOX_LOG_FILE, "NtClose(handle=%p)", Handle);''', 1)
        text = text.replace('    switch (FsInformationClass) {',
            r'''    if (FsInformationClass == XboxFileFsSizeInformation)
        fputs("BO_ENGINE_HDD_FS_SIZE_QUERY\n", stderr);
    switch (FsInformationClass) {''', 1)
        return text
    if kind == 'hdd_path':
        checks = (
            '    DWORD written;\n',
            '    if (h == INVALID_HANDLE_VALUE)\n        return;',
            '    if (SetFilePointerEx(h, off, NULL, FILE_BEGIN))\n'
            '        WriteFile(h, sector, sizeof(sector), &written, NULL);',
        )
        if any(text.count(anchor) != 1 for anchor in checks):
            raise ValueError('pinned Xbox partition-table source drift')
        text = text.replace('    DWORD written;\n', '    DWORD written = 0;\n')
        text = text.replace('    if (h == INVALID_HANDLE_VALUE)\n        return;',
            r'''    if (h == INVALID_HANDLE_VALUE) {
        fputs("BO_ENGINE_HDD_TABLE_CREATE_FAILED\n", stderr);
        return;
    }
    fputs("BO_ENGINE_HDD_TABLE_CREATE_OK\n", stderr);''')
        text = text.replace(
            '    if (SetFilePointerEx(h, off, NULL, FILE_BEGIN))\n'
            '        WriteFile(h, sector, sizeof(sector), &written, NULL);',
            r'''    if (SetFilePointerEx(h, off, NULL, FILE_BEGIN))
        WriteFile(h, sector, sizeof(sector), &written, NULL);
    if (written == sizeof(sector))
        fputs("BO_ENGINE_HDD_TABLE_WRITE_OK\n", stderr);
    else
        fputs("BO_ENGINE_HDD_TABLE_WRITE_FAILED\n", stderr);''')
        # The pinned BLACK PC main passes save_dir="save". Windows
        # SHCreateDirectoryExW requires an ABSOLUTE pathname. Its failure
        # prevented Partition0.img from ever being created (real lab evidence).
        # Do not alter owner files or use a network supplied path.
        save_anchor = '        SHCreateDirectoryExW(NULL, s_save_dir, NULL);'
        if text.count(save_anchor) != 1:
            raise ValueError('pinned Xbox save-path source drift')
        text = text.replace(save_anchor, r'''        WCHAR bo_absolute_save[MAX_PATH];
        DWORD bo_save_length = GetFullPathNameW(s_save_dir, MAX_PATH, bo_absolute_save, NULL);
        if (bo_save_length > 0 && bo_save_length < MAX_PATH) {
            wcscpy_s(s_save_dir, MAX_PATH, bo_absolute_save);
            fputs("BO_ENGINE_HDD_SAVE_PATH_NORMALIZED\n", stderr);
        } else {
            fputs("BO_ENGINE_HDD_SAVE_PATH_NORMALIZE_FAILED\n", stderr);
        }
        {
            int bo_mkdir = SHCreateDirectoryExW(NULL, s_save_dir, NULL);
            if (bo_mkdir == ERROR_SUCCESS || bo_mkdir == ERROR_FILE_EXISTS ||
                bo_mkdir == ERROR_ALREADY_EXISTS)
                fputs("BO_ENGINE_HDD_SAVE_DIR_CREATED\n", stderr);
            else
                fputs("BO_ENGINE_HDD_SAVE_DIR_FAILED\n", stderr);
        }''')
        return text
    raise ValueError('unknown patch kind')


def head(path):
    return subprocess.check_output(['git', '-C', str(path), 'rev-parse', 'HEAD'], text=True, stderr=subprocess.DEVNULL).strip()


def prepare(pc):
    if head(pc) != PC_PIN or head(pc / 'third_party/xboxrecomp') != RUNTIME_PIN or head(pc / 'third_party/dsp56300') != DSP_PIN:
        raise ValueError('source pin disagreement')
    if subprocess.check_output(['git', '-C', str(pc), 'status', '--porcelain', '--untracked-files=no'], text=True).strip():
        raise ValueError('source checkout is not disposable and clean')
    sources = [(pc / 'src/recomp_manual.c', 'manual'),
               (pc / 'src/black_native_menu.inc', 'menu'),
               (pc / 'third_party/xboxrecomp/src/video/pc_menu.c', 'menu_overlay'),
               (pc / 'third_party/xboxrecomp/src/usb/usb_gamepad.c', 'usb'),
               (pc / 'third_party/xboxrecomp/src/kernel/kernel_hal.c', 'hal'),
               (pc / 'third_party/xboxrecomp/src/kernel/kernel_file.c', 'hdd_file'),
               (pc / 'third_party/xboxrecomp/src/kernel/kernel_path.c', 'hdd_path')]
    patched = [(path, patch_text(path.read_text(encoding='utf-8'), kind)) for path, kind in sources]
    for path, text in patched:
        path.write_text(text, encoding='utf-8')
    shutil.copyfile(HERE / 'observer.inc', pc / 'src/bo_engine_lab.inc')
    shutil.copyfile(HERE / 'audio_observer.inc', pc / 'src/bo_audio_observer.inc')
    shutil.copyfile(HERE / 'unit_observer.inc', pc / 'src/bo_unit_observer.inc')
    shutil.copyfile(HERE / 'objective_observer.inc', pc / 'src/bo_objective_observer.inc')
    shutil.copyfile(HERE / 'schedule.h', pc / 'src/schedule.h')


def extract_owner(bo, pc, temp):
    sys.path.insert(0, str(bo / 'coop'))
    from source_audit import restore, ISO
    from biso_manifest_audit import read_xdvdfs, safe_path, CHUNK
    records = json.loads((bo / 'BLACK_FILE_MANIFEST.json').read_text(encoding='utf-8'))['files']
    record, = [r for r in records if r['path'] == ISO]
    iso = temp / 'owner-disc.iso'
    game = pc / 'game'
    if game.exists():
        raise ValueError('refuse to replace existing game directory')
    try:
        restore(bo, record, iso)
        with iso.open('rb') as source:
            entries = read_xdvdfs(source)
            if not entries or sum(n for _, n in entries.values()) > 8 * 1024**3:
                raise ValueError('invalid game inventory')
            folded = set()
            for name, (offset, size) in entries.items():
                safe_path(name)
                # Extract to Windows too: reject alternate streams, reserved suffixes and aliases.
                if ':' in name or any(p.endswith((' ', '.')) for p in name.split('/')) or name.casefold() in folded:
                    raise ValueError('unsafe Windows game path')
                folded.add(name.casefold())
                target = game / name
                target.parent.mkdir(parents=True, exist_ok=True)
                source.seek(offset)
                with target.open('xb') as out:
                    remaining = size
                    while remaining:
                        chunk = source.read(min(CHUNK, remaining))
                        if not chunk:
                            raise ValueError('truncated owner image')
                        out.write(chunk)
                        remaining -= len(chunk)
        if hashlib.sha256((game / 'default.xbe').read_bytes()).hexdigest() != XBE_SHA256:
            raise ValueError('XBE identity mismatch')
        return len(entries)
    finally:
        iso.unlink(missing_ok=True)


def windows_build_commands(pc, environment):
    # The public run used VS18/MSVC14.52; VS17 generators cannot select it.
    # Ninja uses the active x64 developer environment without guessing a VS year.
    major = environment.get('VisualStudioVersion', '').split('.')[0]
    if major not in ('17', '18'):
        raise ValueError('unsupported MSVC developer environment')
    configure = ['cmake', '-S', '.', '-B', 'build-lab', '-G', 'Ninja',
                 '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_C_COMPILER=cl']
    build = ['cmake', '--build', 'build-lab', '--target', 'black_xbox_recomp', '--parallel', '2']
    return configure, build, pc / 'build-lab/black_xbox_recomp.exe'


def collect_diagnostics(line, result):
    """Extract fixed categories/codes; never retain a diagnostic message or path."""
    if len(line) > 8192:
        return
    lowered = line.lower()
    patterns = {
        'visual_studio_instance_missing': ('could not find any instance of visual studio',),
        'cmake_generator_unavailable': ('could not create named generator',),
        'build_tool_missing': ('cmake_make_program is not set', 'unable to find a build program'),
        'compiler_missing': ('no cmake_c_compiler could be found', 'is not able to compile a simple test program'),
        'windows_sdk_missing': ('windows sdk version', 'was not found.'),
        'dsp_source_missing': ('pinned dsp56300 source is missing',),
        'cargo_missing': ('rust/cargo 1.98 or newer is required',),
        'generation_missing': ('supply your retail black xbox files',),
        'disk_full': ('no space left on device', 'not enough space on the disk'),
        'rust_dependency_failure': ('failed to get', 'as a dependency of package'),
    }
    categories = result.setdefault('categories', [])
    for category, fragments in patterns.items():
        if all(fragment in lowered for fragment in fragments) and category not in categories:
            categories.append(category)
    # These filenames are public host/lab source, never generated game bodies.
    sources = {'main.c', 'recomp_manual.c', 'black_native_menu.inc', 'bo_engine_lab.inc',
               'bo_audio_observer.inc', 'bo_unit_observer.inc', 'schedule.h'}
    match = re.search(r'(?i)\b(?:fatal\s+)?error\s+(C[0-9]{4}|LNK[0-9]{4}|MSB[0-9]{4})\b', line)
    rust = re.search(r'\berror\[(E[0-9]{4})\]', line)
    if not match and not rust:
        return
    entry = {'code': (match or rust).group(1).upper()}
    location = re.search(r'([^/\\(): ]+)\(([0-9]{1,7})(?:,[0-9]+)?\)\s*:', line)
    if location and location.group(1) in sources and '/recomp/gen/' not in lowered.replace('\\', '/'):
        entry.update(source=location.group(1), line=int(location.group(2)))
    errors = result.setdefault('compiler_errors', [])
    if len(errors) < 16 and entry not in errors:
        errors.append(entry)


def collect_launch_hints(line, progress):
    """Allowlisted, bounded categories from the pinned private Xbox kernel log.

    Never store launch paths, title IDs, raw guest values, or call stacks.
    The first launch-data word is not always a dashboard reason; label it
    neutrally until the original source path is independently identified.
    """
    if len(line) > 8192:
        return
    line = line.rstrip('\r\n')
    if line == '  [KERNEL] no launch data page set':
        progress['launch_page_absent'] = True
        return
    m = re.fullmatch(
        r"  \[KERNEL\] launch data page 0x[0-9A-Fa-f]{8}: "
        r"type=(\d{1,10}) titleid=0x[0-9A-Fa-f]{8} "
        r"path='([^'\r\n]{0,63})'",
        line)
    if m:
        kind = int(m.group(1))
        progress['launch_page_present'] = True
        progress['launch_type_' + (str(kind) if kind <= 3 else 'other')] = True
        progress['launch_path_present' if m.group(2) else 'launch_path_empty'] = True
        return
    prefix = '  [KERNEL]   launch data:'
    if line.startswith(prefix):
        words = line[len(prefix):].split()
        if len(words) == 8 and all(re.fullmatch(r'[0-9A-Fa-f]{8}', w) for w in words):
            kind = int(words[0], 16)
            progress['launch_word0_' + (str(kind) if kind <= 3 else 'other')] = True
            # Original Xbox LaunchData type 1 is Dashboard; word0 == 1
            # means ERROR. The third payload word is parameter[0].
            # Only publish an allowlisted error class, never its raw value.
            if progress.get('launch_type_1') and kind == 1:
                category = {
                    1: 'invalid_xbe', 2: 'harddrive',
                    3: 'region', 4: 'parental_control', 5: 'media_type',
                }.get(int(words[2], 16), 'other')
                progress['dashboard_error_' + category] = True


def collect_boot_media_hints(line, progress):
    """Allowlisted BOOLEAN-only original disc/host-file boot diagnostics."""
    if len(line) > 8192:
        return
    line = line.rstrip('\r\n')
    prefix = '  [FILE] '
    suffix = ' -> synthetic DVD device handle'
    if line.startswith(prefix) and line.endswith(suffix):
        path = line[len(prefix):-len(suffix)]
        if path.casefold() in (r'\Device\CdRom0'.casefold(),
                              r'\??\Device\CdRom0'.casefold()):
            progress['dvd_device_opened'] = True
        return
    if re.fullmatch(r'  \[FILE\] DVD MODE SENSE\(10\) page 0x[0-9A-Fa-f]{2}, '
                    r'\d{1,10} bytes -> authentication page', line):
        progress['dvd_mode_sense_seen'] = True
        return
    if re.fullmatch(r'  \[FILE\] DVD device IOCTL 0x[0-9A-Fa-f]{1,8} '
                    r'\(in=\d{1,10} out=\d{1,10}\) -> STATUS_SUCCESS', line):
        progress['dvd_other_ioctl_seen'] = True
        return
    if re.search(r'\bNtCreateFile FAILED: [^\r\n]{1,512} \(err=\d{1,10}\)$', line):
        progress['file_open_failed_seen'] = True

    # Only markers emitted by our disposable, pinned original-PC kernel.
    # Do not forward any partition path, guest address, OS error or disk data.
    known = {
        'BO_ENGINE_HDD_SAVE_PATH_NORMALIZED': 'hdd_save_path_normalized',
        'BO_ENGINE_HDD_SAVE_PATH_NORMALIZE_FAILED': 'hdd_save_path_normalize_failed',
        'BO_ENGINE_HDD_SAVE_DIR_CREATED': 'hdd_save_dir_created',
        'BO_ENGINE_HDD_SAVE_DIR_FAILED': 'hdd_save_dir_failed',
        'BO_ENGINE_HDD_TABLE_CREATE_OK': 'hdd_table_created',
        'BO_ENGINE_HDD_TABLE_CREATE_FAILED': 'hdd_table_create_failed',
        'BO_ENGINE_HDD_TABLE_WRITE_OK': 'hdd_table_written',
        'BO_ENGINE_HDD_TABLE_WRITE_FAILED': 'hdd_table_write_failed',
        'BO_ENGINE_HDD_PART0_OPEN_TRIED': 'hdd_partition0_open_tried',
        'BO_ENGINE_HDD_PART0_DIRECTORY_OPEN': 'hdd_partition0_directory_open',
        'BO_ENGINE_HDD_PART0_OPEN_OK': 'hdd_partition0_open_ok',
        'BO_ENGINE_HDD_PART0_OPEN_FAILED': 'hdd_partition0_open_failed',
        'BO_ENGINE_HDD_PART0_SECTOR4_READ': 'hdd_partition0_sector4_read',
        'BO_ENGINE_HDD_PART0_SECTOR4_VALID': 'hdd_partition0_sector4_valid',
        'BO_ENGINE_HDD_PART0_SECTOR4_INVALID': 'hdd_partition0_sector4_invalid',
        'BO_ENGINE_HDD_FS_SIZE_QUERY': 'hdd_fs_size_query_seen',
    }
    if line in known:
        progress[known[line]] = True



def private_call(argv, cwd, timeout, env=None, progress=None, diagnostics=None):
    """Drain child output without publishing it or allowing an unbounded logfile."""
    proc = subprocess.Popen([str(a) for a in argv], cwd=cwd, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            creationflags=subprocess.CREATE_NEW_PROCESS_GROUP if os.name == 'nt' else 0)
    def drain():
        tail = b''
        pending = b''
        markers = {b'XBE loaded:': 'xbe_loaded', b'Starting game...': 'guest_entry_called',
                   b'BO_ENGINE_PAD_STARTED': 'usb_pad_polled',
                   b'BO_ENGINE_MENU_TICK': 'frontend_ui_polled',
                   b'BO_ENGINE_RESTART_ARMED': 'restart_armed',
                   b'BO_ENGINE_RESTART_GAMECODE_QUEUED': 'restart_gamecode_queued',
                   b'BO_ENGINE_RESTART_UNLOAD_REMOVAL': 'restart_unload_removal',
                   b'BO_ENGINE_RESTART_LIFECYCLE_OBSERVED': 'restart_lifecycle_observed',
                   b'Game returned. Cleaning up...': 'guest_entry_returned',
                   b'BO_ENGINE_FIRMWARE_KIND_0': 'firmware_kind_0',
                    b'BO_ENGINE_FIRMWARE_KIND_1': 'firmware_kind_1',
                    b'BO_ENGINE_FIRMWARE_KIND_2': 'firmware_kind_2',
                    b'BO_ENGINE_FIRMWARE_KIND_3': 'firmware_kind_3',
                    b'BO_ENGINE_FIRMWARE_KIND_OTHER': 'firmware_kind_other',
                    b'BO_ENGINE_FIRMWARE_RETURN': 'firmware_return',
                   b'BO_ENGINE_FIRMWARE_SHUTDOWN': 'firmware_shutdown',
                   b'[KELVIN] NV2A engine on the CPU: started': 'cpu_renderer_started',
                   b'OHCI: two controllers': 'usb_controller_started',
                   b'[BOOT] emulated APU up': 'audio_emulation_started',
                   b'D3D8: Failed to create D3D11 device': 'd3d_device_failed',
                   b'D3D8: Device created': 'd3d_device_started',
                   b'[XA2] CreateMasteringVoice failed': 'audio_output_unavailable'}
        while True:
            chunk = proc.stdout.read(65536)
            if not chunk:
                if pending and (diagnostics is not None or progress is not None):
                    remaining = pending.decode('utf-8', errors='replace')
                    if diagnostics is not None:
                        collect_diagnostics(remaining, diagnostics)
                    if progress is not None:
                        collect_launch_hints(remaining, progress)
                        collect_boot_media_hints(remaining, progress)
                break
            if diagnostics is not None or progress is not None:
                lines = (pending + chunk).split(b'\n')
                pending = lines.pop()[-8192:]
                for line in lines:
                    decoded = line.decode('utf-8', errors='replace')
                    if diagnostics is not None:
                        collect_diagnostics(decoded, diagnostics)
                    if progress is not None:
                        collect_launch_hints(decoded, progress)
                        collect_boot_media_hints(decoded, progress)
            if progress is not None:
                window = tail + chunk
                for marker, key in markers.items():
                    if marker in window:
                        progress[key] = True
                tail = window[-64:]
    thread = threading.Thread(target=drain, daemon=True)
    thread.start()
    try:
        result = proc.wait(timeout=timeout)
    except subprocess.TimeoutExpired:
        if os.name == 'nt':
            subprocess.run(['taskkill', '/PID', str(proc.pid), '/T', '/F'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=30)
        else:
            proc.kill()
        proc.wait(timeout=30)
        result = -1
    thread.join(timeout=5)
    if not thread.is_alive():
        proc.stdout.close()
    return result


def execute(bo, pc, out, seconds, restart=False):
    report = {'schema': 1, 'scope': 'original_PC_engine_two_players_only',
              'android_runtime_verified': False, 'four_players_verified': False,
              'pc_pin': PC_PIN, 'runtime_pin': RUNTIME_PIN, 'dsp_pin': DSP_PIN,
              'passed': False, 'blocker': 'not_started', 'observations': {},
              'restart_after_phases_requested': restart}
    out.mkdir(parents=True, exist_ok=True)
    phase = 'dependency_checkout'
    temp = Path(tempfile.mkdtemp(prefix='black-private-', dir=os.environ.get('RUNNER_TEMP')))
    try:
        if head(pc) != PC_PIN:
            raise ValueError('source pin disagreement')
        for module, category in [('third_party/xboxrecomp', 'runtime_dependency_checkout'),
                                 ('third_party/dsp56300', 'dsp_dependency_checkout')]:
            phase = category
            if private_call(['git', 'submodule', 'update', '--init', module], pc, 600):
                raise RuntimeError('pinned dependency checkout')
        phase = 'source_patch'
        prepare(pc)
        phase = 'owner_extraction'
        report['owner_files_extracted'] = extract_owner(bo, pc, temp)
        report['owner_xbe_sha256_verified'] = True
        phase = 'python_dependency'
        commands = [
            ([sys.executable, '-m', 'venv', '.venv'], 120),
            ([pc / '.venv/Scripts/python.exe', '-m', 'pip', 'install', '--disable-pip-version-check', 'capstone==5.0.7'], 240),
        ]
        for argv, budget in commands:
            if private_call(argv, pc, budget):
                raise RuntimeError('Python preparation')
        phase = 'original_code_generation'
        if private_call([pc / '.venv/Scripts/python.exe', 'scripts/generate.py'], pc, 1800):
            raise RuntimeError('generation')
        phase = 'audio_observer_generation'
        instrument_generated(pc / 'src/recomp/gen')
        phase = 'engine_toolchain'
        configure, build, executable = windows_build_commands(pc, os.environ)
        if not shutil.which('ninja') or not shutil.which('cl'):
            raise RuntimeError('active build tools missing')
        report['build_backend'] = 'ninja_msvc'
        report['msvc_major'] = int(os.environ['VisualStudioVersion'].split('.')[0])
        report['safe_build_diagnostics'] = {}
        phase = 'engine_configure'
        diagnostics = report['safe_build_diagnostics'].setdefault(phase, {})
        if private_call(configure, pc, 300, diagnostics=diagnostics):
            raise RuntimeError('configure')
        phase = 'engine_build'
        diagnostics = report['safe_build_diagnostics'].setdefault(phase, {})
        if private_call(build, pc, 2400, diagnostics=diagnostics):
            raise RuntimeError('build')
        report['engine_built'] = True
        phase = 'original_engine_run'
        env = {k: v for k, v in os.environ.items() if not k.startswith(('RECOMP_', 'BO_LAB_'))}
        env.update(RECOMP_AC97_READY='1', RECOMP_APU_DSP_ACK='0x83218810',
                   RECOMP_NV2A_KELVIN='1', RECOMP_VBLANK='1', RECOMP_VBLANK_KEEP_GRID='1',
                   RECOMP_USB='1', RECOMP_USB_PADS='2', RECOMP_KELVIN_GPU='0',
                   RECOMP_KELVIN_THREADS='2', RECOMP_SPLITSCREEN='1',
                   RECOMP_BLACK_INPUT_MODE='controller', RECOMP_BLACK_PROMPT_MODE='controller',
                   RECOMP_BLACK_SKIP_MOVIES='1', RECOMP_BLACK_SKIP_STARTUP_VIDEOS='1',
                   BO_LAB_AGGREGATE=str(temp / 'aggregate.json'),
                   BO_LAB_AUDIO_AGGREGATE=str(temp / 'audio-aggregate.json'),
                   BO_LAB_UNIT_AGGREGATE=str(temp / 'unit-aggregate.json'))
        if restart:
            env['BO_LAB_RESTART'] = '1'
        progress = {}
        started = time.monotonic()
        code = private_call([executable], pc, seconds, env, progress)
        report['runtime_elapsed_ms'] = min(2000000, int((time.monotonic() - started) * 1000))
        report['boot_progress'] = progress
        raw = temp / 'aggregate.json'
        if raw.is_file():
            report['observations'] = parse_aggregate(raw.read_text(encoding='ascii'))
        audio_raw = temp / 'audio-aggregate.json'
        report['audio_observations'] = parse_audio(audio_raw.read_text(encoding='ascii')) if audio_raw.is_file() else None
        report['audio_observer_executed'] = bool(report['audio_observations'] and
            report['audio_observations']['create_calls'] > 0 and
            report['audio_observations']['restore_checks'] > 0)
        unit_raw = temp / 'unit-aggregate.json'
        report['unit_observations'] = parse_units(unit_raw.read_text(encoding='ascii')) if unit_raw.is_file() else None
        result = evaluate(report['observations'])
        report.update(result)
        if report['blocker'] == 'mission_not_reached':
            if progress.get('firmware_return') or progress.get('firmware_shutdown'):
                report['blocker'] = 'guest_requested_firmware_exit'
            elif progress.get('d3d_device_failed'):
                report['blocker'] = 'graphics_device_initialization_failed'
            elif progress.get('guest_entry_returned'):
                report['blocker'] = 'guest_entry_returned_before_mission'
            elif not progress.get('guest_entry_called'):
                report['blocker'] = 'original_boot_incomplete'
            elif not progress.get('frontend_ui_polled') and not progress.get('usb_pad_polled'):
                report['blocker'] = 'frontend_and_usb_input_not_polled'
            elif not progress.get('usb_pad_polled'):
                report['blocker'] = 'usb_input_not_polled'
        report['bounded_run_finished'] = code != -1
        report['engine_exit_success'] = code == 0
        if restart:
            report['restart_lifecycle_passed'] = (
                code == 0 and restart_lifecycle_observed(progress, report['unit_observations']))
            if result['passed'] and not report['restart_lifecycle_passed']:
                report.update(passed=False, blocker='restart_lifecycle_unverified',
                              failed_checks=['restart_lifecycle_unverified'])
        if code != 0:
            report['passed'] = False
            if report['blocker'] == 'none':
                report['blocker'] = 'engine_timeout' if code == -1 else 'engine_exit_failure'
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError, KeyError, json.JSONDecodeError):
        report['blocker'] = phase + '_failed'
    finally:
        # No raw logs, guest memory, saves, generated sources or executables are retained.
        shutil.rmtree(temp, ignore_errors=True)
        (out / 'summary.json').write_text(json.dumps(report, sort_keys=True, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(report, sort_keys=True))
    return 0 if report['passed'] else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=['execute'])
    parser.add_argument('--bo', type=Path, required=True)
    parser.add_argument('--pc', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--seconds', type=int, default=300)
    parser.add_argument('--restart-after-phases', action='store_true')
    args = parser.parse_args()
    if not 60 <= args.seconds <= 1800:
        parser.error('run deadline must be 60..1800 seconds')
    return execute(args.bo.resolve(), args.pc.resolve(), args.out.resolve(), args.seconds,
                   args.restart_after_phases)

if __name__ == '__main__':
    raise SystemExit(main())
