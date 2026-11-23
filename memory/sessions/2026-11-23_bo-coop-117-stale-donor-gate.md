### Session117 CI proof — 2026-11-23

- [Source preflight 45227692500](https://github.com/alwansan/BO/actions/runs/45227692500) SUCCESS for P1 donor freshness and original independent P2 stage (synthetic native ASan/UBSan and pinned Android SDK header compatibility), [UDP 45227692651](https://github.com/alwansan/BO/actions/runs/45227692651) SUCCESS.
- [Android candidate 45227692658](https://github.com/alwansan/BO/actions/runs/45227692658) is still compiling at last check; [original PC engine 45227027742](https://github.com/alwansan/BO/actions/runs/45227027742) is still running after harness PASS. No original Android P2 actor/physics/AI/shared world, despite green preflight. No phone mini-trial.

# BO-COOP Session117 — 2026-11-23

### Session117 — 2026-11-23 — fail closed on stale original P1 donor before extra actor guest materialization

- [Code 6968f2d7](https://github.com/alwansan/BO/commit/6968f2d7d3f3a9dcf91b08bbde58b9785ebde1f6): `OriginalHostPlayerShadowPlan` now retains an UNCHANGED internal 0x900-byte original P1 actor source and 0x16C0-byte camera-owner source BEFORE P2 pointer rebasing. `SourceStillCurrent` rechecks original scene/world/owner/physics/NPC and input-manager source identity, then compares untouched original actor (including 0x20..0x57 world-owned grid), owner, 0x70+0x3C0 parts, 0x200 model and 0x40 input snapshots against current guest bytes. The result is only Boolean; no raw donor bytes, addresses, game strings or state exported. This is targeted to the proven grid risk, and prevents preparing a second actor across an intervening P1/AI/world update.
- `OriginalHostPlayerStage::MaterializeOwned` calls `SourceStillCurrent` read-only **BEFORE** `BeginOriginalSetup`/guest writes; stale donor reports `Incomplete` and still allows safe constructor-only release. `CopyIntoOwned` repeats the freshness check immediately before the five exact original-P2 allocations are written; any uncertainty AFTER engine-touched quarantines cleanup, never shallow frees. Native tests exercise six independently mutated sources with stable root addresses (P1 grid, P1 position, owner control, parts, model, input) and assert zero P2 writes and exact constructor-only free, plus original positive and partial-failure scenarios.
- This is an opt-in helper **not yet invoked in Android original gameplay**. It does not create a human actor2/AI/physical proxy, swap P1, supply weapons or animation/audio, or apply canonical host campaign scene on guests. Do not infer working multiplayer from C++ tests or an Android APK build. Need safe original engine actor setup/teardown first.
- [Session116 Windows private original lab 45227027742](https://github.com/alwansan/BO/actions/runs/45227027742) **harness CHECKS SUCCESS** and original-engine job running at source check; [Session115 Android 45225086434](https://github.com/alwansan/BO/actions/runs/45225086434) completed SUCCESS as build-only (before new source). This code 6968f2d7 triggered new source Android SDK preflight [45227692500](https://github.com/alwansan/BO/actions/runs/45227692500) and full Android candidate [45227692658](https://github.com/alwansan/BO/actions/runs/45227692658); results pending at time of update. [UDP 45227692651](https://github.com/alwansan/BO/actions/runs/45227692651) SUCCESS.
- NEXT: read private Session116 original engine grid-node/rest-of-grid and P1/P2 proxy/body/pool/intro counters; address actual original setup 1E0E40, reversible context swap including weapon/animation/audio & static world lists and P2 proxy 0xDF400, add AI target and host NPC/doors/objective -> guest original engine apply, then one substantive 2–4-device co-op trial ONLY after integration. Only branch `experiment/black-coop`; do not modify main/CL/biso/user game ISO/XBE/saves/private logs.


## Fail-closed precedence

1. Require separate P2 model and input donor bytes plus same original scene.
2. Before changing any tracked P2 guest allocation, ensure the ORIGINAL actor's world-grid node and every required source buffer still match the read-only prepared donor state.
3. A stale source is `Incomplete`, not `EngineCleanupRequired`: no original P2 memory write occurred and constructor-only cleanup is safe.
4. AFTER `BeginOriginalSetup`, every uncertain write failure is quarantined and cannot call shallow `FreeSystem`; full game-entity teardown remains unavailable. This module is deliberately not production-invoked.

Note: the production native hook for real original P2 setup is not implemented; no claim of gameplay or 2–4 player co-op. The first gameplay trial should test HOST START from the same room and have all humans in the same source-authentic campaign world only after actual actor/proxy/AI/mission state are integrated.
