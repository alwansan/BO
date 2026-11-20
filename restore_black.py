#!/usr/bin/env python3
import hashlib, json, pathlib, shutil, sys
base = pathlib.Path(__file__).resolve().parent
out = pathlib.Path(sys.argv[1] if len(sys.argv)>1 else base.parent/'black-restored').resolve()
if out == base or base in out.parents or out.exists():
    raise SystemExit('Choose a new output directory outside the checkout.')
m = json.loads((base/'BLACK_FILE_MANIFEST.json').read_text())
out.mkdir(parents=True)
for x in m['files']:
    dest = out/x['path']; dest.parent.mkdir(parents=True, exist_ok=True)
    h = hashlib.sha256(); n = 0
    with dest.open('wb') as w:
        for p in x['parts']:
            with (base/p).open('rb') as f:
                for block in iter(lambda:f.read(1024*1024), b''):
                    w.write(block); h.update(block); n += len(block)
    if n != x['bytes'] or h.hexdigest() != x['sha256']:
        dest.unlink(); raise SystemExit('Verification failed: '+x['path'])
    print('OK', x['path'], flush=True)
for p in m.get('empty_dirs', []): (out/p).mkdir(parents=True, exist_ok=True)
print('SUCCESS: verified',len(m['files']),'files in',out)
