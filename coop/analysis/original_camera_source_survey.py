#!/usr/bin/env python3
"""Read-only pinned-owner camera metadata. No instructions/game bytes exported.

This is a bounded source investigation, NOT camera ABI/runtime acceptance.
Uses the existing private ELF/objdump helper; all temporary code remains private.
"""
import argparse
import collections
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

OWNER_SHA1 = 'c5bfed25449c8212d7a976f15246e87ead57713f'
HELPER = Path(__file__).with_name('owner_xbe_static_slices.py')
WINDOWS = ('2b830:2ba5f', 'd2f40:d30ea', '2db60:2dc14', '2dc20:2dd40', '2d1a0:2d33a', '2d9f0:2da70')
ROW = re.compile(r'\s*([0-9a-f]+):\s+(\w+)\s*(.*)')
FIELD = re.compile(r'(0x[0-9a-f]+)?\(%(eax|ebx|ecx|edx|esi|edi|esp|ebp)\)')


def rows(owner, window):
    env = dict(os.environ, BLACK_OWNER_XBE=str(owner))
    result = subprocess.run([sys.executable, str(HELPER), window], env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            text=True, timeout=30, check=True)
    for line in result.stdout.splitlines():
        match = ROW.match(line)
        if match:
            yield int(match[1], 16), match[2], match[3]


def survey(owner):
    if hashlib.sha1(owner.read_bytes()).hexdigest() != OWNER_SHA1:
        raise ValueError('requires exact pinned owner image; no foreign image accepted')
    result = {'schema': 1, 'ownerSha1': OWNER_SHA1,
              'scope': 'bounded static candidate metadata only; not runtime or camera ABI acceptance',
              'windows': {}, 'clipSetterDirectCallCandidates': []}
    for window in WINDOWS:
        fields = collections.defaultdict(set)
        calls, returns = [], []
        for address, op, operands in rows(owner, window):
            if op.startswith('call'):
                target = re.match(r'0x([0-9a-f]+)\b', operands)
                if target:
                    calls.append(int(target[1], 16))
            if op.startswith('ret'):
                returns.append(address)
            for offset, base in FIELD.findall(operands):
                fields[base].add(int(offset or '0', 16))
        result['windows'][window] = {'fieldOffsetsByRegister': {k: sorted(v) for k, v in fields.items()},
                                      'directCalls': calls, 'returnAddresses': returns}
    callback = list(rows(owner, '2b830:2ba5f'))
    # Derive the true entry stack offset, rather than confusing a post-push
    # ESP displacement with the original callback argument convention.
    prologue = len(callback) >= 2 and callback[0][1].startswith('push') and callback[0][2].strip() == '%ebp' and callback[1][1].startswith('mov') and callback[1][2].replace(' ', '').replace('\t', '') == '%esp,%ebp'
    camera_load = any(op.startswith('mov') and re.fullmatch(r'0xc\(%ebp\),\s*%esi', operands) for _, op, operands in callback)
    result['cameraCallbackStaticFacts'] = {
        'framePointerPrologueRecognized': prologue,
        'cameraLoadEbpPlus12Recognized': camera_load,
        'derivedCameraEntryStackOffset': 8 if prologue and camera_load else None,
        'directSetTransformCallCount': sum(op.startswith('call') and re.match(r'0x26aae0\b', operands) is not None for _, op, operands in callback),
        'plainReturnAtExpectedEnd': any(address == 0x2BA5E and op.startswith('ret') and not operands.strip() for address, op, operands in callback)}
    # Stream the full bounded executable survey without retaining its code.
    env = dict(os.environ, BLACK_OWNER_XBE=str(owner))
    with subprocess.Popen([sys.executable, str(HELPER), '11000:2964e0'], env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True) as proc:
        history = collections.deque(maxlen=8)
        for line in proc.stdout:
            match = ROW.match(line)
            if not match:
                continue
            address, op, operands = match.groups()
            target = re.match(r'0x(2db60|2dc20)\b', operands) if op.startswith('call') else None
            if target:
                nearby = sorted({(base, int(offset or '0', 16))
                                 for text in history for offset, base in FIELD.findall(text)})
                result['clipSetterDirectCallCandidates'].append({
                    'caller': int(address, 16), 'target': int(target[1], 16),
                    'nearbyCandidateFields': nearby})
            history.append(operands)
        if proc.wait(timeout=30):
            raise RuntimeError('private metadata extraction failed')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--owner-xbe', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if args.output.resolve() == args.owner_xbe.resolve():
        parser.error('output must not replace owner image')
    result = survey(args.owner_xbe.resolve())
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('CAMERA_STATIC_METADATA_WRITTEN: candidate facts only; no camera/runtime acceptance')


if __name__ == '__main__':
    main()
