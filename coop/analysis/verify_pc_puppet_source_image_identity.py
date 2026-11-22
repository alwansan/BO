#!/usr/bin/env python3
"""Private exact owner BLACK XBE fingerprint vs public MIT puppet-reference identity.
No bytes, hex dumps, disassembly, paths or hashes of *unknown* inputs emitted.
"""
import argparse
import hashlib
import sys
from pathlib import Path

OWNER_SHA1="c5bfed25449c8212d7a976f15246e87ead57713f"
PC_REFERENCE_SHA256="df2739c372d254a90aeeccf5971d12097f22d89fbb90db021fda96dd4deb68f6"
MAX_INPUT=5_000_000

class IdentityError(ValueError):pass
def inspect(data):
    if not isinstance(data,bytes) or not 64<=len(data)<=MAX_INPUT or data[:4]!=b"XBEH":
        raise IdentityError("Not a bounded XBE image")
    if hashlib.sha1(data).hexdigest()!=OWNER_SHA1:
        raise IdentityError("Not exact pinned owner image")
    return hashlib.sha256(data).hexdigest()==PC_REFERENCE_SHA256

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--owner-xbe",type=Path,required=True)
    a=parser.parse_args()
    if a.owner_xbe.is_symlink() or not a.owner_xbe.is_file() or a.owner_xbe.stat().st_size>MAX_INPUT:
        raise IdentityError("Invalid source file")
    same=inspect(a.owner_xbe.read_bytes())
    print("BO_OWNER_PC_PUPPET_REFERENCE_IMAGE_IDENTITY_"+("MATCH" if same else "DIFFERENT"))
    print("BO_ORIGINAL_PUPPET_ENGINE_ABI_AND_ACTOR_STILL_UNVERIFIED")
if __name__=="__main__":
    try:main()
    except (IdentityError,OSError) as e:
        print("BO_OWNER_XBE_IDENTITY_PRIVATE_CHECK_BLOCKED:"+type(e).__name__,file=sys.stderr)
        sys.exit(1)
