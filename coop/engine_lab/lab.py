#!/usr/bin/env python3
"""Private original-engine runner. Only allowlisted aggregate JSON may leave it."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import threading

PC_PIN = 'b2512fafd3e181e4febfb298f297245b845ef628'
RUNTIME_PIN = 'e4b621706a3b5e58c084838d1f27547548cf27a8'
DSP_PIN = 'aab3649fc452daf03da8b5e36fdc0c308d874101'
XBE_SHA256 = 'df2739c372d254a90aeeccf5971d12097f22d89fbb90db021fda96dd4deb68f6'
HERE = Path(__file__).resolve().parent
FIELDS = frozenset('mission_ticks p2_ready_ticks phases_completed distinct_actor_ticks distinct_proxy_ticks distinct_weapon_ticks p1_body_move_mm p2_body_move_mm restored_ticks restore_failures proxy_failures p1_move_mm p2_move_mm p1_idle_mm p2_idle_mm p1_shots p2_shots pad0_reports pad1_reports input0_move_reports input1_move_reports input0_fire_reports input1_fire_reports epoch_changes invalid_samples phase control_ready ui_ticks ui_category ui_category_changes'.split())
MANUAL_ANCHOR = '''void sub_001F1080(void)
{
    sub_001F1080_gen();
    black_p2_tick();
}'''
MENU_ANCHOR = '''    black_native_menu_tick(ui);
    sub_00124330_gen();'''
USB_ANCHOR = '''    out[0] = 0;
    out[1] = 20;
    if (pad) {'''


def parse_aggregate(text):
    if len(text) > 8192:
        raise ValueError('oversized aggregate')
    data = json.loads(text)
    if not isinstance(data, dict) or set(data) != FIELDS:
        raise ValueError('non-aggregate field')
    if any(type(v) is not int or not 0 <= v <= 100000000 for v in data.values()):
        raise ValueError('non-aggregate value')
    return data


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
        ('p1_context_not_restored', data.get('restored_ticks', 0) >= 720 and data.get('restore_failures', 0) == 0),
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
        return text.replace(MANUAL_ANCHOR, '''#include "bo_engine_lab.inc"
void sub_001F1080(void)
{
    sub_001F1080_gen();
    bo_lab_before();
    black_p2_tick();
    bo_lab_after();
}''')
    if kind == 'menu':
        if text.count(MENU_ANCHOR) != 1:
            raise ValueError('frontend source drift')
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
               (pc / 'third_party/xboxrecomp/src/usb/usb_gamepad.c', 'usb')]
    patched = [(path, patch_text(path.read_text(encoding='utf-8'), kind)) for path, kind in sources]
    for path, text in patched:
        path.write_text(text, encoding='utf-8')
    shutil.copyfile(HERE / 'observer.inc', pc / 'src/bo_engine_lab.inc')
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


def private_call(argv, cwd, timeout, env=None, progress=None):
    """Drain child output without publishing it or allowing an unbounded logfile."""
    proc = subprocess.Popen([str(a) for a in argv], cwd=cwd, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            creationflags=subprocess.CREATE_NEW_PROCESS_GROUP if os.name == 'nt' else 0)
    def drain():
        tail = b''
        markers = {b'XBE loaded:': 'xbe_loaded', b'Starting game...': 'guest_entry_called',
                   b'BO_ENGINE_PAD_STARTED': 'usb_pad_polled'}
        while True:
            chunk = proc.stdout.read(65536)
            if not chunk:
                break
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


def execute(bo, pc, out, seconds):
    report = {'schema': 1, 'scope': 'original_PC_engine_two_players_only',
              'android_runtime_verified': False, 'four_players_verified': False,
              'pc_pin': PC_PIN, 'runtime_pin': RUNTIME_PIN, 'dsp_pin': DSP_PIN,
              'passed': False, 'blocker': 'not_started', 'observations': {}}
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
        phase = 'engine_configure'
        if private_call(['cmake', '-S', '.', '-B', 'build-lab', '-G', 'Visual Studio 17 2022', '-A', 'x64'], pc, 300):
            raise RuntimeError('configure')
        phase = 'engine_build'
        if private_call(['cmake', '--build', 'build-lab', '--config', 'Release', '--target', 'black_xbox_recomp', '--parallel', '2', '--', '/nodeReuse:false', '/verbosity:minimal'], pc, 2400):
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
                   BO_LAB_AGGREGATE=str(temp / 'aggregate.json'))
        progress = {}
        code = private_call([pc / 'build-lab/Release/black_xbox_recomp.exe'], pc, seconds, env, progress)
        report['boot_progress'] = progress
        raw = temp / 'aggregate.json'
        if raw.is_file():
            report['observations'] = parse_aggregate(raw.read_text(encoding='ascii'))
        result = evaluate(report['observations'])
        report.update(result)
        if report['blocker'] == 'mission_not_reached':
            if not progress.get('guest_entry_called'):
                report['blocker'] = 'original_boot_incomplete'
            elif not progress.get('usb_pad_polled'):
                report['blocker'] = 'usb_input_not_polled'
        report['bounded_run_finished'] = code != -1
        report['engine_exit_success'] = code == 0
        if code != 0:
            report['passed'] = False
            if result['passed']:
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
    args = parser.parse_args()
    if not 60 <= args.seconds <= 1800:
        parser.error('run deadline must be 60..1800 seconds')
    return execute(args.bo.resolve(), args.pc.resolve(), args.out.resolve(), args.seconds)

if __name__ == '__main__':
    raise SystemExit(main())
