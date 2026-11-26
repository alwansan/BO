"""Read-only original audio lifetime observations in the disposable PC lab."""
import json
from pathlib import Path
import re

FIELDS = frozenset(("create_calls", "destroy_calls", "other_thread_calls",
                    "overlap_calls", "untracked_storage", "ledger_evictions",
                    "observation_busy", "restore_checks", "retained_fpv_storage"))


def parse(text):
    def unique(pairs):
        result = {}
        for key, value in pairs:
            if key in result:
                raise ValueError("duplicate audio field")
            result[key] = value
        return result
    if len(text) > 2048:
        raise ValueError("oversized audio aggregate")
    data = json.loads(text, object_pairs_hook=unique)
    if (not isinstance(data, dict) or set(data) != FIELDS or
            any(type(v) is not int or not 0 <= v <= 100000000 for v in data.values())):
        raise ValueError("invalid audio aggregate")
    return data


def instrument_generated(directory):
    """Keep original generated bodies intact and call each exactly once."""
    sources = {p: p.read_text(encoding="utf-8") for p in sorted(Path(directory).glob("*.c"))}
    changes = {}
    for kind, name in enumerate(("sub_0007EF50", "sub_0007EDB0")):
        anchor = re.compile(r"^void " + name + r"\(void\)\s*\{", re.M)
        matches = [(p, match) for p, text in sources.items() for match in anchor.finditer(text)]
        if len(matches) != 1:
            raise ValueError("original audio definition drift")
        path, _ = matches[0]
        text = changes.get(path, sources[path])
        text = anchor.sub("void bo_audio_original_" + name + "(void)\n{", text, count=1)
        text += ("\nextern uint32_t bo_audio_enter(unsigned);\n"
                 "extern void bo_audio_leave(unsigned, uint32_t);\n"
                 f"void {name}(void) {{\n"
                 f"    const uint32_t object = bo_audio_enter({kind}u);\n"
                 f"    bo_audio_original_{name}();\n"
                 f"    bo_audio_leave({kind}u, object);\n}}\n")
        changes[path] = text
    for path, text in changes.items():
        path.write_text(text, encoding="utf-8")
