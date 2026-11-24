# Session148 — 2026-11-24

### Session148 (2026-11-24): independent original weapon manager preparation

- Code e23256548a608ed844edb9ed883198a94aadc09f implements OriginalWeaponManagerImage from pinned PC black_p2_banks_take. Detached 0xD0 manager preparation rebases internal record pointers +7C/+80, assigns independent heap pair, remaps two original sound-bank references, resets original load/weapon record fields. Requires initialized donor and original pool signature +130C=9; rejects aliases and unexplained bank/record pointers. Preserves original donor and output on failure.
- Fixture verifies two independent heap/bank bindings, pointer rebase, reset fields and no mutation on invalid input. New CI results not verified. This is detached data preparation, not original audio construction or a live manager install. Original P3/P4 heap IDs remain unproven. No production activation, no Android gameplay claim.
- Next parallel tracks: original audio constructor ABI 141AD0/141380 and verified in-place pool restoration, actual original allocator provenance, native presentation provider, independent physics and visible soldier bodies, guest canonical world apply. Original enemy/ally AI untouched. No phone microtests. Only experiment/black-coop writable; preserve main, CL, biso, XBE/ISO/saves/private logs.

