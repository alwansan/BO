### Session115 CI verification — 2026-11-23

- Latest source [45225086457](https://github.com/alwansan/BO/actions/runs/45225086457) SUCCESS after owned-buffer writer and missing-phase guard, including pinned Android Xbox SDK C++ syntax and strict ASan/UBSan original P2 staging fixture; [UDP 45225086329](https://github.com/alwansan/BO/actions/runs/45225086329) SUCCESS.
- Full Android [45225086434](https://github.com/alwansan/BO/actions/runs/45225086434) still IN_PROGRESS at last check, candidate-probe compiling; previous Android build was cancelled by workflow concurrency, not a game failure. Do not mark APK as fully verified until job complete.
- Both source outcomes are compile/fixture-only: `MaterializeOwned` is NOT invoked from original Android game thread, needs separately implemented engine-safe teardown and full original P1/P2 camera/weapon/animation/audio context, physics/AI and canonical host NPC/door/objective guest apply. No playable co-op yet; no small owner test.

# BO-COOP Session115 — 2026-11-23

### Session115 — 2026-11-23 — separately owned ORIGINAL guest P2 materialization, permanently quarantined on partial writes

- Code [a7113623](https://github.com/alwansan/BO/commit/a7113623abf1dd627dd67c4a08a3c741e584f298) and [c31c58ae](https://github.com/alwansan/BO/commit/c31c58aed88cf4c809e163a1bac8bd0c914632ca) implement an **OPT-IN, NOT ACTIVE** owned-guest-memory path. `OriginalPlayerStorageKernelIO::WriteOwned` permits writes ONLY into separately tracked exact P2 allocation bases and sizes after checking committed pages. The detached shadow stores original scene and source-pointer identity, validates original mission phase 0x1C and stable world/owner/physics/NPC/model/parts/input pointers, then writes parts (0x630), input (0x40), zeroed weapons (0x40), rebased camera owner (0x16C0), then actor (0x900) LAST. Never writes original P1/core/physics/NPC or marks a network room/mission ready.
- `OriginalHostPlayerStage::MaterializeOwned` requires BOTH `InputIsolated` and `ModelIsolated` and original running scene/phase before touching allocations, calls `OriginalHostPlayerStorage::BeginOriginalSetup` BEFORE the first guest write and permanently requires proven engine cleanup after success OR any uncertain/partial write failure. No shallow `FreeSystem`, no silent retry, no original game `CallGuest` player setup/proxy/input transport. Constructor-only incomplete/phase-invalid path is rejected BEFORE quarantining. New native fixtures cover 5 exact P2-only writes, unchanged P1/core/NPC/physics, failed third write quarantines without free, missing source refuses without side effects, and phase-not-running rejects.
- Native original Android source SDK syntax/fixture CI must pass before claiming this stage is verified. No production caller; stage is not yet integrated into the original engine tick. **NOT playable co-op, NOT an active player2, NOT full cleanable lifecycle.** Current original PC lab still tests P1 restore and 2-controller real gameplay independently; do not relax its failure gates.
- P0 NEXT: source-backed reversible original engine teardown, animation/weapon/audio banks and P1/P2 context swap, real actor setup 0x1E0E40, tracked physical proxy AI and soldier representation, host-owned canonical NPC/door/mission state applied on guest original Android engine; only then one substantial 2–4 phone trial. Only branch `experiment/black-coop`; original user game assets/saves/raw logs and main/CL/biso untouched.



## Native memory ownership and still-missing engine lifecycle

`CopyIntoOwned` only copies staged data into five existing tracked extra-player allocations. It does **not** make the actor active in the engine, create a physics proxy, register an AI target, feed game input, initialize first-person animation/audio, or synchronize a second original BLACK world.

The original game's live owner constructor was already used in `Allocate`. Before changing any owned allocated bytes, `BeginOriginalSetup` marks storage engine-touched. With a partial failure, the state remains quarantined for separate engine-safe teardown. The current repo intentionally has no `ReleaseAfterSetup`, so production must not invoke `MaterializeOwned` yet. This prevents accidental shallow free/double ownership.

Regression tests confirm P1, original world, NPC and physics memory are untouched and five destination writes are exact. The real Android pinned runtime compiles template instantiations in source preflight, but synthetic tests cannot prove actual gameplay.

Next priority: native original P1/P2 context ownership contract, original setup/camera animation/audio banks and safe teardown, then proxy/NPCs/mission authoritative guest apply. Do not request tiny phone tests or misrepresent CI success as shared world.
