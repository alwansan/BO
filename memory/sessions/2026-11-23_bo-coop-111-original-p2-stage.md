# BO-COOP Session111 — 2026-11-23

### Session111 — 2026-11-23 — original PC virtual-HDD boots, P2 owned+shadow staging; NO playable co-op

- [Original PC engine run 45213250944](https://github.com/alwansan/BO/actions/runs/45213250944) FAILED strict multiplayer acceptance, BUT confirmed the specific Windows virtual HDD fix: normalized save path, created/written partition table, opened valid Partition0 sector4. Original BLACK frontend and USB controllers actually polled (5245/5183 reports); 167 original mission-loop ticks. Runtime reached ~300s timeout, below 720 acceptance ticks; P2 ready 0, proxy identity 0, phase 0 and P1 restore failures 161. Earlier Invalid Harddrive early exit no longer observed. Do not claim accepted mission or Android phone HDD defect.
- New `OriginalHostPlayerStage` connects `OriginalHostPlayerStorage::Allocate` (independent tracked camera/actor/weapon/parts storage with source-matched original camera constructor) to read-only `OriginalHostPlayerShadowPlan::Prepare`, cleans up only pristine constructor-owned resources after failure, and quarantines uncertain native side effects. Tests cover success, source unchanged, duplicate attempts, failed guest read, failed camera and foreign thread. **No original P2 actor written, gameplay setup, animation/audio/weapons/physics/AI or shared world.** CI for this new commit is unverified until a run passes.
- Prior source/SDK [45216969474](https://github.com/alwansan/BO/actions/runs/45216969474) SUCCESS only for previous detached plan. Android full candidate [45216969800](https://github.com/alwansan/BO/actions/runs/45216969800) remained compiling at observation; do NOT publish it as gameplay.
- NEXT: verify source and Android native build; inspect original PC mission intro and P1 restore context without loosening acceptance; implement complete original Android P2 setup/context swap, safe teardown, physics+AI+weapon and host canonical NPC/door/objective state applied inside original guest engine. Only ONE major two-plus phone acceptance. All writes only `experiment/black-coop`, protect main/CL/biso, original owner ISO/XBE/saves/raw logs.


## Evidence boundary

Windows original kernel now reads Partition0 and runs original input/front-end. 167 mission ticks is NOT the required 720. The P1 restore check presently fails on 161 samples; this requires source-backed investigation (possibly legitimate original P1 actor mutation during tick), not simply ignoring failures. P2 ready/proxy remain absent. Android SDK runtime differs from PC lab, which remains independent from phone operation.

## Implemented scope

The new `OriginalHostPlayerStage` guarantees the tracked allocation and detached P1 shadow are prepared by the SAME game-thread owner, with constructor-only rollback when safe, and no false fallback after uncertain native side effects. Native fixture asserts original P1 memory is untouched by staging. No production integration, no guest RAM staging or actor initialization, no canonical host mission/world or remote guest native application.

## Next implementation

1. Verify C++ source sanitizer and Android native syntax against pinned SDK; inspect full 45216969800 status.
2. Finish source-proven original player2 camera/animation/audio/weapon context transaction, separate physics proxy and AI target, and safe rollback/teardown; then activate inside original host scene thread.
3. Implement host-authoritative NPCs/objects/mission and genuine Android guest engine application; only one significant two-plus phone test after verifiable readiness.

Keep session/plan/restoration/STATE current; protect original game archives and other repositories.
