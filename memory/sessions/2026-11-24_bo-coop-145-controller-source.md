# Session145 — 2026-11-24

### Session145 (2026-11-24): original human controller source exchange

- Code 7a9c3ca6c4af4764e21b6a12f02ae6f085923afa implements OriginalHumanControllerSource, exact controller manager +A8 switch from pinned PC black_p2_update. Requires independent owned input and original controller/source lifetime proofs, rejects alias, preserves null scripted source, restores P1 before other contexts and quarantines uncertain restoration. SDK OriginalSetupKernelIO calls this helper before human controller/actor methods; provider must independently verify active player pad and interaction/FX contexts. No production activation.
- Fixture exercises unowned input rejection, successful switch/restore, duplicate entry, null scripted source and stale P1 input generation. New CI pending; Session144 push CI conclusion also unverified. Original enemy/ally AI untouched; no target modifications or additional world tick.
- Next in parallel: actual original game allocator provenance and per-player pad +C binding, audio constructors, native presentation provider, visible soldiers, independent physics and guest canonical host-world apply. No phone microtests. Only experiment/black-coop writable; preserve main, CL, biso, XBE/ISO/saves/private logs.

