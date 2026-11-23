# Session106 — one host original scene shared physics ledger

Date: 2026-11-23. Allowed writable branch: `experiment/black-coop`. Goal unchanged: 2–4 independently controlled humans and original BLACK original mission/AI/weapons/objects in one host-authoritative world, not separate single-player sessions.

## Verified before changes

Branch was ce781faf2fc18ae7c912c953b8cb2b18323b3402. Original Windows engine test 45193624586 returned `firmware_kind_2=true` at 202ms, failed engine gameplay; new launch-data classifier in run [45197279738](https://github.com/alwansan/BO/actions/runs/45197279738) was still running at latest check. No phone gameplay trial requested.

## Native code

[81e623fc](https://github.com/alwansan/BO/commit/81e623fc1768e151e6671db9538eede91cc868de) implements `OriginalProxySceneLedger` in `coop/room_network/room_original_host_proxy_engine.hpp`. All original P2–P4 `OriginalHostProxyEngine` adapters must receive the SAME long-lived scene ledger explicitly; default construction deleted. Ledger binds one original scene/world/physics token for its lifetime, reserves an actor before native DF400, records at most 3 independent actor+proxy pairs, rejects duplicate actors/proxies and a fourth additional actor. A pending or failed/exception native transaction poisons subsequent acquisitions; verified rollback/release clears its slot. Adapter destruction while retaining an original proxy quarantines the shared ledger (cannot be silently forgotten by reconstructing the adapter).

[50b1d0e2](https://github.com/alwansan/BO/commit/50b1d0e2ec7ad91557798240508ca678907f3086) updates all native regression callers and adds 3 independent proxy actors in one host physics pool, full-capacity rejection, later slot re-use after validated release, lost adapter ownership and partial native allocation exception tests. Relevant sanitizer stage passed in [source preflight 45199349609](https://github.com/alwansan/BO/actions/runs/45199349609) SUCCESS.

## Strict remaining limits

- A C++ shared ledger is **not** a real second in-game BLACK actor or a live scene-generation witness. The caller must own it for the entire original-scene lifetime; never create a fresh ledger merely to bypass quarantine. Reuse across new original scenes requires independent engine-teardown proof; current ledger deliberately has no arbitrary reset.
- Original player2–4 initialization/weapon/audio/animation/physics AI hooks still missing, despite source guards.
- Android room TLS APK is still NO GAMEPLAY and cannot be distributed as multiplayer-ready. Prior run [45195066473](https://github.com/alwansan/BO/actions/runs/45195066473) SUCCESS room-only.
- Guest original-world apply, mission enemy/door/objective synchronization and host authority are not end-to-end.
- Do not modify or reupload protected main/CL/biso or original XBE/ISO/saves/raw device files or private logs.
- Real-game validation requires significant combined 2+ phone run only after actual original-engine integration; no trivial per-detail phone trials.

## Next

Inspect safe launch-page result of run 45197279738 when completed, repair original quick reboot according to native evidence (never fake entry), establish original scene lifetime hooks, connect shared P2–P4 ledger to actual source engine, implement host authoritative NPC/objective/object and guest original consumer; then request one comprehensive phone trial.
