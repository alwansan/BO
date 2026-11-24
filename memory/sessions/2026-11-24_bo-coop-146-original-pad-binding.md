# Session146 — 2026-11-24

### Session146 (2026-11-24): original independent pad component binding

- Code b06bf2b1faff2c2c7e2c9543856d593b4161ee29 adds OriginalHumanPadBinding: source-reviewed original input +C, manager [2D45E8]+98, P2 stride 120 from pinned PC black_p2_update; original sub_000C5A80 confirms component vtable 28CE00, records +118, port +11C and 32-byte record kind 2. P3/P4 are stride candidates ONLY, independently required to pass actual component/port/shared-record checks. No static claim that their original addresses exist on Android.
- The SDK setup adapter now binds the independently owned input +C before original controller +A8 exchange. Checks owned allocation lifetime, exact source pointer, repeated topology, readback and refuses cross-player reuse. Only the extra input word is written, never P1 pad or bot AI. Fixture covers all three synthetic ports, mismatched port/record, repeat binding and reused input lifetime. No production activation; new CI results not verified.
- Remaining parallel native work: actual original allocator provenance, actor embedded pad/source alias repair under protected world-link mask, verified presentation/audio/weapon constructors, original physics and visible bodies, live guest NPC/door/objective apply. No change to enemy/ally AI or targeting, no second world tick. No phone microtests. Only experiment/black-coop writable; preserve main, CL, biso, XBE/ISO/saves/private logs.

