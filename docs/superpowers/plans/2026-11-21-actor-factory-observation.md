# Actor factory observation implementation plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement task by task. The owner explicitly opted for autonomous work until a necessary phone test; no design approval question is pending.

**Goal:** Resolve the actual original actor/controller/resource creation contract before creating independent human bodies or enabling a shared story Start. Owner's new old-APK two-phone screenshots and logs prove HOSTING/JOINED 2/4 on LAN. Keep the automatic room simplification; do not confuse lobby acceptance with gameplay.

**Design:** An isolated opt-in factory diagnostic layers on the room-network preparer, preserving its original output. Observe the original c5 function1DF440 once at entry/return; call original exactly once and never invoke a factory from observer/network code. Capture descriptor provenance, finite pose, controller pool membership and post-call actor/controller backlinks and resource/serial membership. Log bounded scalar/boolean/index facts, not pointers, coordinates, raw descriptor dumps, names or game assets. Keep up to32calls/process; use safe reads and preserve host state. Failed observations do not change guest state. Factory null result is observation, not a permission to spawn. The wrapper still executes the original even if diagnostics fail.

**Why this step:** Static analysis1AF880 consumes controller first and returns the controller, whereas1DF440 returns the actor; resource exhaustion can return null before caller dereferences actor. Descriptor+40/+44 and+50/+54 feed AI/action state. We must not replay mission-owned descriptors or borrow324. Existing Probe4 did not capture this contract; repeating its pool trace is unnecessary.

## Task1 — bounded contract reader and real observer
- [x] Add failing tests for entry/return provenance, null/exhaustion, invalid resource/controller, identity changes, unchanged output, no raw addresses and strict observation budget.
- [x] Implement `coop/probe5/factory_contract.hpp` as safe read-only entry/return reader and `factory_observer.inc` as bounded logging wrapper. No actor admission/action implementation is claimed.
- [x] Run strict C++20 ASAN/UBSAN tests; test actual observer with GuestReader and in-memory log sink.

## Task2 — source and generated-code gates
- [x] Add `prepare_room_factory.py`, atomic stage on `prepare_room_network`, separate package `org.bocoop.black.factoryroom`; integrate original hook with saved entry values and post-result, diagnostic prompt and scalar export.
- [x] Add source transformation tests, original hook checks, synthetic guard drift tests. Reject missing/duplicate/trapping1DF440 original and wrong register/ret contract in generated output before linking.
- [x] Independent review, fix findings, publish to experiment/black-coop only.

## Task3 — real build and necessary owner test
- [ ] Build isolated ARM64 APK using pinned source/SDK and c5 owner image in Actions; no ISO/XBE/keys in artifacts. Verify package/JNI/image/signature/hash as room-network workflow does.
- [ ] Inspect artifact and safe metadata, update STATE/MEMORY/PLAN/ISSUES/RESTORE with real run results and old-APK two-phone success.
- [ ] Ask owner for targeted capture: enable diagnostic before loading a mission, play to first NPC spawn, one checkpoint reload, export logs. This answers new factory/resource lifetime questions; it does not claim a second player or ask to repeat accepted UI/old lobby testing.

**After this gate:** Reserve independent actor/controller ownership without stealing NPCs; verify full-body rig asset; game-thread spawn/update/remove; host-authoritative mission prepare/load/ready barrier and entity snapshots; only then enable shared Start with truthful capability state. Preserve original single-player and all quest/save data. Full combat/AI/downed/revive/story transitions remain required.

New evidence: see docs/REFERENCE_BLACK_PC_COOP_AR.md and model inventory. Re-evaluate player-context vs factory replay using this MIT reference; the diagnostic does not commit us to replaying mission-owned NPC descriptors. Do not claim full-body protagonist identity from skeletal candidate tags.
