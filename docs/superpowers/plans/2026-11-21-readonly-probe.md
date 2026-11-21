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
- [ ] Write failing C++ tests for guarded real self-reads, PROT_NONE/cross-page/overflow, finite values, bounded registry/rate/output formatter.
- [ ] Implement core and pass local/CI sanitizer tests.
- [ ] Implement Android sampler and optional UI/package preparer; test fail-closed source anchors and actual pinned-source preparation.
- [ ] Build probe APK on CI, verifying original generated hook targets, exact native image, package/ABI/signature/assets, and allowlisted source deltas.
- [ ] Independent review; fix Important findings with failing regression tests.
- [ ] Download/recheck artifact; update recovery and ask for concrete mission/nearby-NPC/reload capture. No actor or co-op claim.

## Urgent handoff
User interrupted implementation to move work to another session. Core tests passed locally; Android sampler is WIP, unintegrated, unreviewed as a whole, and unbuilt. See session08 for precise remaining tasks/known issues. Baseline phone test already succeeded.
