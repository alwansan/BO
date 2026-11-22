"""Stage a strictly read-only original Xbox kernel FILE OPEN witness.

Only patches the pinned SDK's runtime/kernel/files.cpp in an EPHEMERAL
checkout. Never touches owner game assets, game memory, save files, or main.
Reports fixed resource-open categories (no file paths or bytes).
"""
from pathlib import Path

HERE=Path(__file__).resolve().parent
HEADER=HERE/"stage_resources"/"level_file_open_contract.hpp"
REL="runtime/kernel/files.cpp"
HEADER_REL="runtime/kernel/coop_stage_resource_contract.hpp"

def exactly_once(text,old,new):
    if text.count(old)!=1:raise ValueError("Pinned Xbox kernel original file-open anchor changed")
    return text.replace(old,new)

def patched(original):
    s=original
    s=exactly_once(s,'#include "kernel/exports.h"',
                   '#include "kernel/exports.h"\n#include "kernel/coop_stage_resource_contract.hpp"')
    s=exactly_once(s,'    LOG_FIRST(400, "open %s", name.c_str());\n    return kSuccess;\n}\n\n}  // namespace',
'''    // Original XBE asked to OPEN an actual disc file; this is NOT stage loaded.
    blackcoop::OriginalStageResource seen;
    if(blackcoop::g_originalStageFileOpen.ObserveSuccessfulOpen(name,seen))
        LOGR("BO_STAGE_RESOURCE_OPEN_LEVEL_%02d_TYPE_%d_NOT_SCENE",seen.folder,seen.resource);
    LOG_FIRST(400, "open %s", name.c_str());
    return kSuccess;
}

}  // namespace''')
    # Disc path always uses OpenOnDisc above. Files on extracted host tree
    # use normal successful NtCreateFile/NtOpenFile paths instead.
    s=exactly_once(s,'''    f->delete_on_close = options & 0x1000;  // FILE_DELETE_ON_CLOSE
    W32(handle_out, NewHandle(f));
    IoStatus(iosb, kSuccess, info);
    LOG_FIRST(400, "open %s", name.c_str());''',
'''    f->delete_on_close = options & 0x1000;  // FILE_DELETE_ON_CLOSE
    W32(handle_out, NewHandle(f));
    IoStatus(iosb, kSuccess, info);
    // Host extracted files behave identically to original mounted ISO.
    blackcoop::OriginalStageResource opened;
    if(blackcoop::g_originalStageFileOpen.ObserveSuccessfulOpen(name,opened))
        LOGR("BO_STAGE_RESOURCE_OPEN_LEVEL_%02d_TYPE_%d_NOT_SCENE",opened.folder,opened.resource);
    LOG_FIRST(400, "open %s", name.c_str());''')
    s=exactly_once(s,'''    (void)options;
    W32(handle_out, NewHandle(f));
    IoStatus(iosb, kSuccess, 1);
    return kSuccess;
}

// I/O completes before the call returns''',
'''    (void)options;
    W32(handle_out, NewHandle(f));
    IoStatus(iosb, kSuccess, 1);
    blackcoop::OriginalStageResource opened;
    if(blackcoop::g_originalStageFileOpen.ObserveSuccessfulOpen(name,opened))
        LOGR("BO_STAGE_RESOURCE_OPEN_LEVEL_%02d_TYPE_%d_NOT_SCENE",opened.folder,opened.resource);
    return kSuccess;
}

// I/O completes before the call returns''')
    return s

def prepare(sdk):
    sdk=Path(sdk).resolve()
    file=sdk/REL
    header=sdk/HEADER_REL
    if not file.is_file() or header.exists():raise ValueError("Missing pinned SDK or existing header")
    original=file.read_text(encoding="utf-8")
    fixed=patched(original)
    new_header=HEADER.read_text(encoding="utf-8")
    if "ORIGINAL_LEVEL_RESOURCE_OPEN_GATE_PASS" in new_header:raise ValueError("Unexpected header content")
    # No write until ALL anchors and safety checks succeeded.
    header.write_text(new_header,encoding="utf-8")
    file.write_text(fixed,encoding="utf-8")
    return [REL,HEADER_REL]

if __name__=="__main__":
    import argparse
    p=argparse.ArgumentParser()
    p.add_argument("--sdk",required=True)
    args=p.parse_args()
    print("STAGED ORIGINAL RESOURCE OPEN WITNESS ONLY",prepare(args.sdk))
