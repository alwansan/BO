# Session38 — Host story and remote ally-body visual plan (2026-11-22)

Branch: alwansan/BO experiment/black-coop, ONLY writable branch. This plan supersedes Session36's prohibition on using an ally model for other human players.

## New owner directive
- Every phone controls its ORIGINAL LOCAL first-person player: own hands, weapon, camera, HUD and input stay untouched.
- Every OTHER connected human, including host from a guest phone, is drawn as an independently-owned FULL-BODY ALLY VISUAL. The human controls their body; never inherit the AI or a live NPC/ally brain.
- Host alone selects and starts campaign mission. Guests must load the same stage/scene and share one progressing world. 2 separate solo games or 2/4 lobby humans are NOT co-op.
- Preserve up to 4 peers, single-player, saves, and later AI/combat/quests/late-join/revive/friendly-fire options.

## New owner evidence from CL
- Verified earlier APK run44946597973/artifact14724535514 was tested. Latest uploaded primary capture: black log/logs/BLACK-Probe-20261121-235407-267/coop_probe.txt, blob a9ade07f8ffecb051969d3dc00a092c5459bf2ad, 675 events.
- Exactly 8/8 default_player_model samples: valid=true, complete=true, has_model=true, bone_count=44, buffer_count=2, draw_list_mask=32 (draw-list5), arms=true, head=false, torso=false, legs=false, body_candidate=false. This is only local actor+0x320 wrapper[0] PRIMARY model, not every asset; consistent with first-person arms.
- Primary Android15 Xiaomi23013PC75G log and second Android10 Huawei JSN-L22 log both load compatible owner image c5bfed25449c8212d7a976f15246e87ead57713f, package org.bocoop.black.defaultplayer. Huawei has no uploaded matching default_player_model capture. The first phone provides sufficient evidence for design pivot, no re-request.
- Old PIN lobby 2/4 proven on two phones in CL; automatic names-only variant has further joined/discovery event codes but NOT shared campaign.
- CL JPG images with defaultplayer names at ~23:55-23:58 indexed; available GitHub connector only returns UTF-8, binary JPEG inspection failed. No invented visual screenshot analysis. Safe evidence: memory/evidence/DEFAULT-PLAYER-owner-20261122-235407-safe-summary.json.

## Gates and responsibilities
1. EVIDENCE-38 DONE: record new owner test, update older Session36 misleading "no phone capture / no ally" wording.
2. VIEW-001 SOURCE PASS: coop/bridge/remote_ally_roster.hpp, test_remote_ally_roster.cpp — value-only host and guest viewpoint awareness, no local-self clone, authenticated snapshot prerequisite, scene epoch and generation lifetime guard, stale timeout; host must be visible as ally on guest, guest visible on host.
3. MISSION-001 SOURCE PASS: coop/story/mission_barrier.hpp and test — host-generated mission ticket/nonce/session, stage, scene epoch, expected roster; only authenticated READY with matching actually-loaded scene can issue one COMMIT; peer leave/stale replay invalidates.
4. SOURCE CI PASS: 44951425780, Linux C++20 ASAN/UBSAN; no actual game API/Android APK, no real TLS mission messages yet.
5. WIRE-001 OPEN: real RoomWire/RoomTls mission PREPARE/READY/COMMIT, protected by TLS, host-only, bounded frame and roster revision. Avoid ever calling BLACK game API on socket worker.
6. ENGINE-001 OPEN: actual host mission menu selection, authoritative stage prepare/load/ready on game thread and verified loading on every phone. Prevent independently starting separate missions.
7. BODY-001 OPEN: identify an available friendly ALLY asset/rig; validate materials/skin/bones/animation and ownership. Render a dedicated human-controlled proxy for EVERY OTHER player, including host visible to guests. Original local player's hands stay local. No AI brain, NPC slot or same actor pointer reused.
8. INPUT-001 OPEN: real character transform/animation/weapon state into game-thread inbox; host-authoritative snapshots and interpolation/reconnect across 2-4 phones.
9. WORLD-001 OPEN: host governs enemy AI targeting/damage, objectives, resources, doors, checkpoints, scene transition, late join and save scope. Complete one level end-to-end with two physically tested phones, then four.
10. COOP-ACCEPT OPEN: demonstrate host starts actual story, guest sees/moves/shoots/interacts with host and shares same progressing mission. 2/4 alone is NOT proof.

## Safety
- Local player original vtable draws via 1D25D0 writing outside NPC stride 0x3C0. NPC uses 1C6150, not interchangeable. NPC membership +0x250, brain +0x324, controller +0x328 ownership and cleanup are unproven for human remote. Do not shallow-copy ally actors or register null-brain actor vulnerable to damage.
- Candidate ally full-body meshes from owner source: 271 files, 4286 MODEL occurrences, 76 candidates / 10 unique keys. Metadata are NOT verified skinning, friendly faction, rig or visual identity. Do not export original copyrighted assets.
- Portingpete/Black-2006-Recomp pinned b2512fafd3e181e4febfb298f297245b845ef628 is MIT reference with experimental PC split-screen; it is not Android story sync. If copying licensed code, preserve attribution/LICENSE. No copying Peterson binary-only TDM code/assets.
- Owner already supplied source (BO split ISO and biso); no re-upload. Preserve main, game saves and single-player. Avoid percentage-complete claims until gates accepted.
