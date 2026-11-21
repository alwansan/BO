# CURRENT OVERRIDE — Session38 (2026-11-22)
Old Session36 no-ally visual ban below is superseded by owner. Local remains original first-person; OTHER real human players should be drawn with independent full-body ALLY look (not AI allies), including host visible to guests. Host alone starts one shared mission/world. Owner has already uploaded the primary MODEL test, no longer pending. This older plan remains an archival basis; active plan docs/superpowers/plans/2026-11-22-host-story-remote-ally.md. Source-only CI44951425780 PASS, game integration open.

---

# Default player linkage implementation plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans. Owner explicitly requests autonomous execution and a real phone experiment; no additional design approval is pending.

**Owner steering:** use the game's original protagonist on each phone, no NPC/ally replacement models. Preserve the factory diagnostic as a tool, but do not make an NPC-factory capture the next requested test. The existence of each phone's local gameplay actor is confirmed; complete world-visible body, independent remote ownership and shared mission are not automatically established.

**Immediate testable gate:** observe the model actually bound to the running original local actor, using the existing original player-update hook. This settles the default-body visibility question with runtime facts and no unsafe cloned ownership. Read relocated model metadata and bounded bone-name category tags plus draw-list mask; export only booleans/counts, no names/meshes/pointers/poses. Separate package, no NPC factory hook, no actors created, no save changes. Keep automatic room transport so the owner can test names-only2/4 separately.

- [x] RED test valid relocated default-player model, wrong actor, malformed bounds, missing model, first-person draw-list and full-body candidate classification, unchanged output and unchanged guest bytes.
- [x] Implement bounded read-only `default_player/model_contract.hpp` and actual observer include, <=8records, <=1Hz, safe reads, host-state preservation.
- [x] Atomic `prepare_room_player.py` on prepare_room_network only; separate package, original protagonist hook only, no1DF440 hook. Stage and native sanitizer tests; independent review.
- [x] Build/verify actual APK in Actions and update owner steering in STATE/RESTORE/AGENTS.
- [x] **OWNER DEVICE TEST RECEIVED**: CL BLACK-Probe-20261121-235407-267/coop_probe.txt includes exactly 8/8 complete original PRIMARY-model observations: 44 bones, 2 buffers, draw-list mask32 (list5), arms=true, head/torso/legs=false, body_candidate=false. The primary model appears first-person-only; do not extrapolate to all game assets. No repeat test required.

**Next implementation:** host-authoritative mission prepare/load/ready, real local-player state published into bounded room messages and game-thread remote-state inbox, then independent original player contexts/visible representation verified against this gate. No shared Start or rendered second human is claimed by lobby or metadata success. A default actor on each phone must still be connected to one authoritative world; do not ship two independent missions as co-op.

Verified build:44946597973/artifact14724535514, code8bc7f92, APK SHA25694400c2d650d772293ab5f3a1d0c72e91d1974dea64a3004d18404e90ccb02d0. Device capture pending, no gameplay completion claim.
