# Read-only Character Probe Plan

> Execute inline with superpowers:executing-plans. User requested continuous implementation until another device test is needed.

**Goal:** Collect bounded local player/LOD/animation observations to investigate full-body proxies after the rebuilt baseline was tested successfully.
**Architecture:** Existing player/animation wrappers plus one verified LOD entry wrapper call a host-only sampler. Reads use process_vm_readv on this process with checked arithmetic and bounded copies; no direct guest-pointer dereference, guest writes, network transport, or unknown guest calls. Capture is off until a user button enables it.
**Tech Stack:** C++20, Linux/Android read syscall, Python preparation, Windows/NDK CI.
**Spec:** docs/ACTOR_FEASIBILITY_AR.md, docs/COOP_REQUIREMENTS_AR.md.

## Decisions / constraints
- Baseline user test and log identify package org.bocoop.black.baseline, exact c5bfed image without another-release, Exit0. Startup missing-image line predates completed import; quit FEAudio message remains known, not hidden.
- New package org.bocoop.black.probe1 prevents debug-key installation conflict and preserves both original and tested baseline. Private import/storage isolation inherited.
- User chooses capture at launch; disabled mode returns immediately. Local logs only, no gameplay modifications.
- Max64 observed pointer/vtable pairs per world epoch, max256 hook attempts/s, max32 records/s, max2048 records, max180s from first valid world, max1MiB output. IDs are within-epoch correlations, not proven lifetime IDs; same-address/vtable reuse can remain indistinguishable.
- Source-confirmed player ECX, animation first stack arg/owner+0; independently checked LOD entry0x1C55A0 has first stack argument and ret4. CI must prove generated original definition exists before linking wrapper.
- Positions from+0xA0 reject nonfinite/abs>1e7; animation owners are not assumed character objects. Root render pointers are provisional fields, not models/factories.
- record thread IDs; do not assert thread ownership before phone evidence. Faulted/partial reads and file errors skip/disable capture while original functions still run.
- Android15 AOSP SYSCALLS contains process_vm_readv and app/common blocklists omit it; actual device availability remains a test gate. No permissions bypass or root.

## Tasks
- [x] Write C++ tests for guarded real self-reads, PROT_NONE/cross-page/overflow, finite values, bounded registry/rate/output formatter. Passed Linux ASAN/UBSAN run 44852312025; negative cases tested.
- [x] Implement core and pass Linux CI sanitizers (44852312025); added one-shot enable and fail-closed file cleanup after exception.
- [x] Implement Android sampler and optional Java opt-in/package preparer; pinned-source and rejection tests PASS (44852312025, 44852419894). Android compilation/runtime still separately gated.
- [ ] Build probe APK on CI, verifying original generated hook targets, exact native image, package/ABI/signature/assets, and allowlisted source deltas. Windows run 44852419929 launched; results must be checked. No APK claim until success.
- [ ] Independent review of final APK/runtime integration; fix Important findings with failing regression tests. Source anchor guards and no-network permissions added, but device gate open.
- [ ] Download/recheck artifact; update recovery and invite a *new probe* device capture only after fully verified APK. Tested baseline must not be re-tested; no actor or co-op claim.

## Urgent handoff
User interrupted implementation to move work to another session. Core tests passed locally; Android sampler is WIP, unintegrated, unreviewed as a whole, and unbuilt. See session08 for precise remaining tasks/known issues. Baseline phone test already succeeded.

## Session09 progress note
- New owner upload: five ZIP parts verified and assembled locally; extracted ISO SHA256 matches BO repository manifest. See docs/OWNER_ISO_UPLOAD_AR.md. Sandbox copies are temporary, GitHub manifest copy is durable; no raw assets published.
- Linux ASAN/UBSAN and exact v1.5.0 preparer run 44852312025 completed SUCCESS. Follow-up 44852419894 completed SUCCESS. Pinned source preparer inherits accepted baseline private ISO import without touching original baseline, adds org.bocoop.black.probe1 and explicit Start capture UI; removes broad storage/network permissions. Source deltas confined to two observers plus new src/coop.
- Windows build run 44852419929 remains a separate acceptance gate; avoid assuming success from early steps. Device guide docs/PROBE_DEVICE_GUIDE_AR.md is for a verified probe candidate only.

## Session10 continuation (2026-11-21)
- [x] Complete original HOOK001 integration and Windows probe build [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) SUCCESS; **Android runtime not tested**.
- [x] Improve Registry player priority (local reservation) and negative tests; ASAN/UBSAN/pinned source [44854667613](https://github.com/alwansan/BO/actions/runs/44854667613) PASS.
- [ ] Wait for actual CI outcome of [44854667587](https://github.com/alwansan/BO/actions/runs/44854667587), then record hashes/artifact and choose latest candidate.
- [ ] First owner device probe run for runtime evidence; no request to repeat accepted baseline.
- [ ] Only then evaluate actor classifications; no spawn/factory claim or guest memory writes before evidence.

## Session11 continued
- [x] Prove previous Probe1 Android candidate 44854667587 passes all CI gates, artifact14687571586.
- [x] Fix local player promotion and per-slot serial after early NPC/LOD observation (f7abf7de); ASAN/preparer44855984868 PASS.
- [ ] Check completion 44855984997, new metadata/sha, then release only a verified candidate for first Probe1 phone run.
- [x] Add isolated NET-004 game-thread inbox and sanitizer tests (44856160545), not connected to game.
- [ ] Collect actual phone HOOK001 observations before ACTOR-001 spawn research changes.
