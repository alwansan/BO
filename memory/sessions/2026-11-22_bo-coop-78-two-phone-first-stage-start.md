### Session78 trial package and CI gate (2026-11-22)
- User explicitly accepts one **meaningful integrated 2-phone trial** to accelerate. Documented precise host/guest setup, START first mission, manual original mission loading, combined video+safe logs for both phones in `docs/OWNER_TWO_PHONE_START_TRIAL_AR.md`. Do not claim remote bot body visible or guest auto launch.
- Stage 0 host START code fbfeedff: network real TLS + native preflight + Android29 all SUCCESS (#45054921382, #45054921402, #45054921375). Full Android room-only build #45054921483 was still compiling original engine at last observation; check live before download/test.
- Read `docs/OWNER_TWO_PHONE_START_TRIAL_AR.md` and latest Action before asking for trial; do NOT ask owner for repeated tiny tests.

---

# BO-COOP Session78 — User accelerated two-phone Start trial focus

Date: 2026-11-22. Only write alwansan/BO:experiment/black-coop. Do not write main, CL, biso or original saved assets.

## User's exact goal
Host creates room, second real Android joins, host START visible/enabled, selected first mission (level0/stage0 default) begins on BOTH devices, each first-person player sees the other as ANY BLACK original BOT. Whole story same host-authoritative world. Owner now permits a *single meaningful integrated two-phone test* when needed, with combined automatic logs rather than repeated tiny tests.

## Implemented right now
Commit fbfeedffcc9c85dad6eb44084e58797c723ca2d0:
- RoomOverlay: real visible START button in room panel. Enabled only HOSTING + exactly two persons + no current story; pressing it calls RoomSession.requestTwoPhoneFirstStageFromLobby. On success closes lobby to original BLACK game frontend and shows Toast explaining that manual first mission selection on BOTH phones is still required until true auto navigation is source-proven.
- RoomSession: refuses start without real second connected player; sends real host TLS STORY_PREPARE ticket with mission0/stage0, revision/participants/positive epoch, not a fake UI claim. Keeps original read-only core/stream/player source attestation as the only code allowed to emit READY and COMMIT; wrong original host stage aborts.
- RoomOriginalHostStagePrepareGate: distinct first-level host request epoch allocator while preserving auto source stage epochs.
- Real two-person TLS regression RoomTwoPhoneFirstStageStartTlsTest: ensures no ticket without guest; accepted first PREPARE only once; host source wrong stage abort; separate fresh room, synthetic native scene matching, actual encrypted handshake + both READY/COMMIT, host sends two positions and guest movement accepted, abort purges. Explicit limitation: test JNI/native callbacks are synthetic JVM—not live gameplay.
- Triggers source/real TLS/Android platform/Windows APK CI runs #45054921382, #45054921402, #45054921375, #45054921483 (running when authored).

## Not implemented and next critical tasks
1. Actual original frontend auto-selection/launch for host and guest with source-verified engine ABI. Existing original mission start sub_00124CF0 is only static candidate; entry-rooted CFG source audit #45054097462 green but no safe call ABI.
2. Visually instantiate non-AI independent original bot body for remote human on both phones; every original char slot must be independently owned and rechecked; original game renderer/pose ABI still unknown.
3. Synchronize real original AI and objectives/combat/shot effects under host; currently host world "positions" are placeholder values from existing real TLS source tests; separate original offline worlds are not a real shared campaign.

## Next agent step
Read repo live HEAD, Actions results and test failure logs, fix immediately. Keep AGENTS.md, memory/MEMORY.md, memory/STATE.json, memory/PLAN_AR.md, prompt/RESTORE_SESSION_AR.md, docs/CLOUD_HANDOFF_AR.md and this session current. No fake game-ready claim. Aim meaningful 2-phone test with a new Android artifact when significant feature works.
