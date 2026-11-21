# BO-COOP Session39 — RoomSession story handoff (2026-11-22)

Branch: **alwansan/BO:experiment/black-coop** ONLY. Never change main, ISO, saves, licensed assets, original player-camera/hands. Owner goal: one host-owned BLACK campaign world; each phone keeps ORIGINAL first-person locally, sees OTHER real humans as independent full-body ALLY visuals without AI brains.

## Verified starting point
- Live branch HEAD before work **68bab3e1bc1f56df01accfe17935f4a849af6f53**.
- Latest prior true real TLS JVM run **44953176046 SUCCESS**, Android29 synthetic TLS run **44953176054 SUCCESS**, source-preflight **44953176101 SUCCESS**, lobby-only game APK **44953176093 SUCCESS**. No real mission/actor/quest gameplay.
- Owner supplied nine screenshots in chat: host and guest both display **2/4** in BLACK LAN lobby (older Arabic/PIN and English UI variants); none show two remote player actors or shared mission.
- Re-read latest CL safe room logs: host `black log/logs/BLACK-Room-1795294900585-events.txt` shows HOST_DISCOVERY_READY, ROOM_HOSTING, then two `ERROR_BINDEXCEPTION` on fast re-host and later successful ROOM_HOSTING; guest `black log/من الهاتف الثاني/BLACK-Room-1795294904769-events.txt` shows scan, multiple `JOIN_CONNECTED`/`ROOM_JOINED`. This is **old device evidence** predating the new fixed-port retry build; it cannot verify that phone bind failure is now fixed.
- Current owner-tested player model probe still 8/8 primary arms-only, 44 bones; do not request repeat. Protocol v3 intentionally rejects owner-tested v2 APK.

## Source commits made this session
1. **86faa0148210d188f7a24496fdcd1626225f9e2c** — `coop/lobby/java/com/racas/black/coop/RoomSession.java`: add `bindStoryGameThread()`, `storyStatus()`, `prepareStoryFromGameThread()`, `reportSceneLoadedFromGameThread()`, `cancelStoryFromGameThread()`. Bind is reset on new/stop room generation; socket/UI worker is rejected after game thread binds. This is a **Java thread ownership guard**, not proof a hook is the real game thread or that BLACK scene was loaded. Only the host can PREPARE, guest READY requires explicit independently verified callback. The actual engine never calls these yet.
2. **5370dcaed04fab156f807a63dc86dd827d5ae16f** — `coop/lobby/test/SessionTest.java`: exercise new facade with real localhost pinned TLS host/guest, original mission index0 and stage0, wrong stage/epoch, rejected non-game thread, no PREPARE before thread bind, no auto READY on JOIN, host-waits-for-guest barrier, duplicate READY, ABORT and new ticket lifecycle. Still **synthetic stage-loaded callbacks** and no actual game actor/mission.

## Verification
- Actions spawned on test commit 5370dcae: real TLS core **44954581676**, Android29 platform **44954581718**, Android title lobby APK **44954581624**, source staging **44954581651** (SUCCESS initially seen), isolated probe **44954581696** (SUCCESS initially seen). Check latest statuses and logs before recording PASS.
- No real BLACK gameplay code was patched this session. No new owner-accepted physical-phone APK or shared-story acceptance.

## Next tasks (required; not yet done)
1. Verify CI outputs and repair any failures, ensure v3 source staging in new Android APK.
2. Audit pinned `src/game/difficulty.cpp` candidate `sub_00124CF0` and scene epoch/loading through exact owner c5 image; find a game-thread observation without altering mission unlock/progression and without synthesizing a game stage.
3. Connect verified engine callback to these Java TLS APIs through narrow JNI (never network thread to X86Context). Must guard room generation, host mission selection, actual scene loaded/unloaded, guest stage matches, reset and disconnection.
4. Find a licensed/owner-provided full-body ALLY rig for *remote human-only render*, prove independent ownership, no NPC AI brains, then human input/state, world/AI/combat/objective sync.
5. Accept on two real phones in the exact SAME host campaign stage only after actor, movement, combat/quest prove; expand to 4 later.

Never claim `2/4`, socket TLS, synthetic `READY` or Android APK build equals finished co-op.
