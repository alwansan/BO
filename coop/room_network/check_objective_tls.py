#!/usr/bin/env python3
# Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
"""Exercise objective snapshots over real loopback TLS using synthetic records.

Production Java is compiled with --release 8 by default. Tests use the current
host JDK, matching the existing lobby CI. --java-current-runtime opts production
into that host JDK too; it does not establish Java 8 or Android compatibility.
No Android SDK, game image, native engine, external host or real user data is used.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import tempfile
import time


JAVA_ROOT = Path('coop/lobby/java/com/racas/black/coop')
TEST_ROOT = Path('coop/lobby/test')
TESTS = (
    ('WireTest', 'PASS strict room wire', False),
    ('RoomStoryWireTest', 'STORY_WIRE_PASS', False),
    ('RoomObjectiveTlsTest', 'OBJECTIVE_REAL_TLS_PASS', True),
    ('RoomObjectiveSessionTlsTest', 'OBJECTIVE_SESSION_REAL_TLS_PASS', True),
    ('RoomStoryTlsTest', 'STORY_TLS_PASS', True),
    ('RoomWorldSnapshotTlsTest', 'WORLD_TLS_FANOUT_PASS', True),
    ('RoomNpcWorldTlsTest', 'ORIGINAL_NPC_WORLD_REAL_TLS_PASS', True),
    ('RoomObjectEventTlsTest', 'OBJECT_EVENT_REAL_TLS_PASS', True),
    ('RoomHumanInputTlsTest', 'HUMAN_INPUT_TLS_PASS', True),
    ('RoomCameraEyeTlsTest', 'ORIGINAL_EYE_AND_BODY_FEET_TLS_PASS', True),
)


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--java-current-runtime', action='store_true')
    args = parser.parse_args()
    root = args.source_root.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    production = sorted(p.relative_to(root) for p in (root / JAVA_ROOT).glob('*.java'))
    tests = sorted({TEST_ROOT / (name + '.java') for name, _, _ in TESTS} |
                   {TEST_ROOT / 'TlsRoomTest.java'})
    dependencies = sorted(set(production + tests))
    checker = Path(__file__).resolve()
    report = {
        'scope': 'Actual loopback TLS and synthetic objective records; no Android or engine',
        'started_at_utc': datetime.now(timezone.utc).isoformat(),
        'platform': platform.platform(), 'python': sys.version,
        'source_root': str(root),
        'production_java_target': 'current runtime' if args.java_current_runtime else '--release 8',
        'test_java_target': 'current runtime; existing tests use host-JDK language features',
        'java8_production_requested': not args.java_current_runtime,
        'source_sha256': {}, 'commands': [], 'tests': [], 'errors': [], 'success': False,
    }

    def save() -> None:
        (output / 'results.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')

    # Invalidate any previous success before even reading the requested sources.
    # This also makes failures during discovery/hashing reviewable.
    save()

    def run(name: str, command: list[str], timeout: int = 90) -> dict:
        start = time.monotonic()
        record = {'name': name, 'command': command, 'exit_code': None,
                  'stdout': '', 'stderr': ''}
        try:
            result = subprocess.run(command, cwd=output, env=os.environ.copy(),
                                    text=True, capture_output=True, timeout=timeout)
            record.update(exit_code=result.returncode, stdout=result.stdout, stderr=result.stderr)
        except (OSError, subprocess.TimeoutExpired) as failure:
            record['error'] = str(failure)
            for stream in ('stdout', 'stderr'):
                value = getattr(failure, stream, None)
                if value:
                    record[stream] = value.decode(errors='replace') if isinstance(value, bytes) else value
        record['seconds'] = round(time.monotonic() - start, 3)
        report['commands'].append(record)
        save()
        if record['exit_code'] != 0:
            raise RuntimeError(name + ' failed; see captured command record')
        return record

    try:
        if not production:
            raise RuntimeError('No production Java sources found')
        report['source_sha256'] = {str(p): sha256(root / p) for p in dependencies}
        report['checker_sha256'] = sha256(checker)
        java = shutil.which('java')
        javac = shutil.which('javac')
        keytool = shutil.which('keytool')
        if not java or not keytool:
            raise RuntimeError('Java compiler/runtime and keytool are required')
        compiler = [javac] if javac else [java, '-m', 'jdk.compiler/com.sun.tools.javac.Main']
        report['java_compiler_selection'] = 'javac' if javac else 'jdk.compiler module'
        run('java_version', [java, '-version'])
        run('compiler_version', compiler + ['-version'])

        # Owned fresh outputs ensure an old class or test result cannot pass a
        # new invocation. The identity is an ephemeral test certificate only.
        work = Path(tempfile.mkdtemp(prefix='run-', dir=output))
        report['run_directory'] = str(work)
        classes = work / 'classes'
        classes.mkdir()
        identity = work / 'synthetic-room.p12'
        run('synthetic_tls_identity', [keytool, '-genkeypair', '-alias', 'room',
            '-keyalg', 'RSA', '-keysize', '2048', '-sigalg', 'SHA256withRSA',
            '-dname', 'CN=BLACK Co-op Synthetic Loopback Test', '-validity', '2',
            '-storetype', 'PKCS12', '-keystore', str(identity),
            '-storepass', 'test-only', '-keypass', 'test-only', '-noprompt'])
        target = [] if args.java_current_runtime else ['--release', '8']
        flags = ['-Xlint:all,-options', '-Werror', '-encoding', 'UTF-8',
                 '-classpath', str(classes), '-d', str(classes)]
        run('production_compile', compiler + target + flags + [str(root / p) for p in production])
        run('tests_compile', compiler + flags + [str(root / p) for p in tests])
        for name, marker, needs_identity in TESTS:
            command = [java, '-ea', '-cp', str(classes), 'com.racas.black.coop.' + name]
            if needs_identity:
                command.append(str(identity))
            result = run(name, command)
            if marker not in result['stdout']:
                raise RuntimeError(name + ' success marker missing')
            report['tests'].append({'name': name, 'passed': True,
                                    'real_tls': needs_identity, 'marker': marker})
            print(result['stdout'].rstrip(), flush=True)
            save()
        report['ephemeral_identity_packaged'] = False
    except (OSError, ValueError, RuntimeError) as failure:
        report['errors'].append(type(failure).__name__ + ': ' + str(failure))
    finally:
        after = {}
        for p in dependencies:
            try:
                after[str(p)] = sha256(root / p)
            except OSError as failure:
                report['errors'].append('Cannot hash source after run: ' + str(p) + ': ' + str(failure))
        report['source_sha256_after'] = after
        report['sources_unchanged'] = bool(report['source_sha256']) and report['source_sha256'] == after
        try:
            report['checker_unchanged'] = report.get('checker_sha256') == sha256(checker)
        except OSError as failure:
            report['checker_unchanged'] = False
            report['errors'].append('Cannot hash checker after run: ' + str(failure))
        if not report['sources_unchanged'] or not report['checker_unchanged']:
            report['errors'].append('Source/checker absent or changed during execution')
        report['success'] = not report['errors'] and len(report['tests']) == len(TESTS)
        report['finished_at_utc'] = datetime.now(timezone.utc).isoformat()
        save()
    print('OBJECTIVE_TLS_CHECK_' + ('PASS' if report['success'] else 'FAIL'))
    print(output / 'results.json')
    return 0 if report['success'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
