"""Disposable PRIVATE c5 audio witness patcher; no generated code is exported."""
import argparse
from pathlib import Path
import re
import shutil
import subprocess

HERE = Path(__file__).resolve().parent
TAKE = '    black_p2_fpv_at = fpv;\n}'
LOOKUP = 'recomp_func_t recomp_lookup_manual(uint32_t xbox_va)\n{'
TARGETS = {'0007EF50': 'bo_audio_original_create', '0007EDB0': 'bo_audio_original_release'}
FIELDS = frozenset('creates create_failures releases foreign_thread_observations overlapping_call_observations dropped_observations invalid_publication_reads publication_refs_before_release publication_refs_after_release published_identity_changes after_take_checks'.split())


def manual(text):
    text = text.replace('\r\n', '\n')
    if text.count(TAKE) != 1 or text.count(LOOKUP) != 1 or 'bo_audio_after_take' in text:
        raise ValueError('pinned audio/manual anchor drift or repeated patch')
    text = 'static void bo_audio_after_take(void);\n' + text
    text = text.replace(TAKE, '    black_p2_fpv_at = fpv;\n    bo_audio_after_take();\n}', 1)
    return text.replace(LOOKUP, '#include "bo_audio_observer.inc"\n' + LOOKUP + '''
    if (xbox_va == 0x0007EF50u) return sub_0007EF50;
    if (xbox_va == 0x0007EDB0u) return sub_0007EDB0;''', 1)


def generated(sources):
    pending = dict(sources)
    for suffix, replacement in TARGETS.items():
        pattern = re.compile(r'\bvoid\s+sub_' + suffix + r'\s*\(\s*(?:void)?\s*\)\s*\{')
        matches = [(name, match) for name, text in pending.items() for match in pattern.finditer(text)]
        if len(matches) != 1:
            raise ValueError('missing/duplicate original audio body ' + suffix)
        name, match = matches[0]
        pending[name] = pending[name][:match.start()] + match.group().replace('sub_' + suffix, replacement) + pending[name][match.end():]
    return pending


def aggregate(text):
    import json
    def unique(pairs):
        result = {}
        for key, value in pairs:
            if key in result:
                raise ValueError('duplicate audio aggregate')
            result[key] = value
        return result
    if len(text) > 4096:
        raise ValueError('audio aggregate size')
    result = json.loads(text, object_pairs_hook=unique)
    if not isinstance(result, dict) or set(result) != FIELDS or any(type(v) is not int or not 0 <= v <= 100000000 for v in result.values()):
        raise ValueError('non-aggregate audio data')
    return result


def prepare(pc):
    # Run ONLY after existing pinned lab.prepare and original code generation.
    if subprocess.check_output(['git', '-C', str(pc), 'rev-parse', 'HEAD'], text=True).strip() != 'b2512fafd3e181e4febfb298f297245b845ef628':
        raise ValueError('PC source pin disagreement')
    source = pc / 'src/recomp_manual.c'
    original = source.read_text(encoding='utf-8')
    if '#include "bo_engine_lab.inc"' not in original or not (pc / 'src/bo_engine_lab.inc').is_file():
        raise ValueError('requires existing disposable original engine lab preparation')
    patched = manual(original)
    paths = sorted((pc / 'src/recomp/gen').rglob('*.c'))
    changed = generated({p: p.read_text(encoding='utf-8') for p in paths})
    for name in ('bo_audio_observer.inc', 'bo_audio_publication.h'):
        if (pc / 'src' / name).exists():
            raise ValueError('preexisting audio witness addition')
    # ALL anchors and destinations validated before any writes.
    source.write_text(patched, encoding='utf-8', newline='\n')
    for path, text in changed.items():
        path.write_text(text, encoding='utf-8', newline='\n')
    shutil.copyfile(HERE / 'audio_observer.inc', pc / 'src/bo_audio_observer.inc')
    shutil.copyfile(HERE / 'audio_publication.h', pc / 'src/bo_audio_publication.h')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('disposable_pc', type=Path)
    prepare(parser.parse_args().disposable_pc.resolve())
