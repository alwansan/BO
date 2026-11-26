#!/usr/bin/env python3
# Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
"""Offline bounded objective chunks: C++ values -> Java assembly -> C++ values.

No socket, Android SDK, game engine, game assets or native memory is used.
The default Java target is --release 8. --java-current-runtime is an explicit
host-only alternative and does not establish Java 8/Android compatibility.
"""
from __future__ import annotations
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys
import tempfile
import time

CPP_SOURCE='coop/room_network/test_room_objective_chunks_interop.cpp'
JAVA_BASE='coop/lobby/java/com/racas/black/coop/'
JAVA_SOURCES=[JAVA_BASE+n for n in (
    'RoomWire.java','RoomStoryWire.java','RoomObjectiveChunks.java')]
JAVA_TEST='coop/lobby/test/RoomObjectiveChunksTest.java'

def digest(path:Path)->str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main()->int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-root',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--java-current-runtime',action='store_true')
    parser.add_argument('--disable-leak-check',action='store_true')
    args=parser.parse_args()
    root=args.source_root.resolve();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy()
    env['ASAN_OPTIONS']=('detect_leaks=0' if args.disable_leak_check else 'detect_leaks=1')+':halt_on_error=1'
    env['UBSAN_OPTIONS']='halt_on_error=1:print_stacktrace=1'
    report={
        'scope':'Offline synthetic body codec/assembler and C++ interoperability; no RoomWire packet type, sockets, TLS, Android or engine',
        'started_at_utc':datetime.now(timezone.utc).isoformat(),
        'platform':platform.platform(),'python':sys.version,
        'source_root':str(root),'java_compile_target':'current runtime' if args.java_current_runtime else '--release 8',
        'java8_target_requested':not args.java_current_runtime,
        'cpp_sanitizers':{k:env[k] for k in ('ASAN_OPTIONS','UBSAN_OPTIONS')},
        'commands':[],'source_sha256':{},'errors':[],'success':False,
    }
    dependencies={CPP_SOURCE,*JAVA_SOURCES,JAVA_TEST}
    def visit(relative):
        p=root/relative
        for inc in re.findall(r'^\s*#include\s+"([^"]+)"',p.read_text(),re.M):
            child=str(Path(relative).parent/inc)
            if child not in dependencies:
                dependencies.add(child);visit(child)
    def run(name,command,timeout=60):
        started=time.monotonic();record={'name':name,'command':command,'exit_code':None,'stdout':'','stderr':''}
        try:
            p=subprocess.run(command,cwd=out,env=env,text=True,capture_output=True,timeout=timeout)
            record.update(exit_code=p.returncode,stdout=p.stdout,stderr=p.stderr)
        except (OSError,subprocess.TimeoutExpired) as failure:
            record['error']=str(failure)
        record['seconds']=round(time.monotonic()-started,3);report['commands'].append(record)
        if record['exit_code']!=0:raise RuntimeError(name+' failed; see captured command record')
        return record
    try:
        visit(CPP_SOURCE)
        report['source_sha256']={p:digest(root/p) for p in sorted(dependencies)}
        report['checker_sha256']=digest(Path(__file__).resolve())
        compiler=shutil.which('g++') or shutil.which('c++')
        java=shutil.which('java');javac=shutil.which('javac')
        if not compiler or not java:raise RuntimeError('C++20 compiler and Java compiler/runtime required')
        java_compiler=[javac] if javac else [java,'-m','jdk.compiler/com.sun.tools.javac.Main']
        report['java_compiler_selection']='javac' if javac else 'jdk.compiler module'
        run('cpp_version',[compiler,'--version']);run('java_version',[java,'-version'])
        run('java_compiler_version',java_compiler+['-version'])
        # Never allow a previous invocation's classes or guest files to satisfy
        # this run. A fresh owned directory also avoids deleting caller files.
        run_dir=Path(tempfile.mkdtemp(prefix='run-',dir=out))
        report['run_directory']=str(run_dir)
        classes=run_dir/'classes';classes.mkdir()
        fixtures=run_dir/'fixtures';fixtures.mkdir()
        guests=fixtures/'guests';guests.mkdir(exist_ok=True)
        executable=run_dir/'objective_chunks_interop'
        run('cpp_compile',[compiler,'-std=c++20','-O1','-g','-Wall','-Wextra','-Werror','-pedantic','-pthread',
            '-fsanitize=address,undefined','-fno-omit-frame-pointer',str(root/CPP_SOURCE),'-o',str(executable)],120)
        java_target=[] if args.java_current_runtime else ['--release','8']
        run('java_compile',java_compiler+java_target+['-Xlint:all,-options','-Werror','-encoding','UTF-8',
            '-classpath',str(classes),'-d',str(classes)]+[str(root/p) for p in JAVA_SOURCES+[JAVA_TEST]],120)
        host=fixtures/'host-records.bin'
        write=run('cpp_write_fixture',[str(executable),'write',str(host)])
        if 'OBJECTIVE_CHUNKS_CPP_FIXTURE_WRITE_PASS records=4096 bytes=65536' not in write['stdout']:
            raise RuntimeError('C++ fixture success marker missing')
        java_test=run('java_codec_assembler',[java,'-cp',str(classes),
            'com.racas.black.coop.RoomObjectiveChunksTest',str(host),str(guests)])
        if 'OBJECTIVE_CHUNKS_JAVA_PASS' not in java_test['stdout']:
            raise RuntimeError('Java test success marker missing')
        verify=run('cpp_verify_guests',[str(executable),'verify',str(guests)])
        if 'OBJECTIVE_CHUNKS_CPP_INTEROP_PASS guests=3 records=4096' not in verify['stdout']:
            raise RuntimeError('C++ guest verification marker missing')
        fixture_paths=[host]+[guests/('guest-'+str(i)+'.bin') for i in (2,3,4)]
        if any(p.stat().st_size!=65536 for p in fixture_paths):raise RuntimeError('Fixture byte length changed')
        fixture_hashes={str(p.relative_to(out)):digest(p) for p in fixture_paths}
        if len(set(fixture_hashes.values()))!=1:raise RuntimeError('Guest record bytes differ from C++ host')
        report['synthetic_fixture_sha256']=fixture_hashes
        report['synthetic_fixture_bytes']=65536;report['records_per_guest']=4096;report['guests']=3
        match=re.search(r'OBJECTIVE_CHUNKS_JAVA_PASS[^\n]*scenarios=(\d+)',java_test['stdout'])
        report['java_scenarios']=int(match.group(1)) if match else None
    except (OSError,ValueError,RuntimeError) as failure:
        report['errors'].append(type(failure).__name__+': '+str(failure))
    finally:
        after={p:digest(root/p) for p in sorted(dependencies) if (root/p).is_file()}
        report['source_sha256_after']=after
        report['sources_unchanged']=bool(report['source_sha256']) and report['source_sha256']==after
        if not report['sources_unchanged']:report['errors'].append('Source bytes absent or changed during execution')
        report['success']=not report['errors'] and len(report.get('synthetic_fixture_sha256',{}))==4
        report['finished_at_utc']=datetime.now(timezone.utc).isoformat()
        (out/'results.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    for record in report['commands']:
        if record['name'] in ('cpp_write_fixture','java_codec_assembler','cpp_verify_guests'):
            print(record['stdout'].rstrip())
    print('OBJECTIVE_CHUNKS_INTEROP_'+('PASS' if report['success'] else 'FAIL'))
    print(out/'results.json')
    return 0 if report['success'] else 1

if __name__=='__main__':
    raise SystemExit(main())
