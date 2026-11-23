# Session125 — three concurrently executing native C++ source CI jobs

### Session125 (2026-11-23): original co-op THREE PARALLEL CI development tracks

- Owner explicitly wants P2 actor original engine integration, P3/P4 expansion and canonical host-owned story state worked in parallel. Session124 code advanced all three. New `.github/workflows/black-coop-parallel-engine-tracks.yml` executes three independent simultaneous GitHub jobs: original P2 actor/camera/physics lifecycle, independent P2/P3/P4 Xbox/TLS control, and guest authoritative host NPC/object/scene guards. Existing Android APK and UDP workflows remain separate; all new jobs are ASan/UBSan source tests only.
- Session124 source CI 45251338131 completed SUCCESS; UDP 45251338270 SUCCESS. Its Android run 45251338108 observer-core SUCCESS, candidate-probe was IN_PROGRESS at last check. Original Windows game PC two-player lab 45239818055 SUCCESS; original Android 2–4 player world still NOT implemented. No claim that independent source tests create characters.
- Continue simultaneous native engine work: actor P2/P3/P4 constructor and `0x1E0E40` multi-frame setup, safe teardown, full P1 actor/context restoration across world-owned grid, independent viewmodel/weapon/animation/audio/effects, real physics/AI/soldier proxy; plus canonical host enemy/door/objectives applied to each original guest world. Keep native pad ports gated until all are true. No phone microtests.
- Write `experiment/black-coop` only. Update plan/restore/session/state and use CAS; preserve main, CL, biso, source ISO/XBE, saves and raw logs.


The CI jobs run concurrently, and they never attest working Android shared original BLACK story. Actual resource-owning guest actor setup and safe teardown remain P0.
