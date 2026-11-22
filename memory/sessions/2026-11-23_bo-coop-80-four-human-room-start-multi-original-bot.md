# BO-COOP session80: START for 2–4 and all remote BOT LOOK avatars (2026-11-23)

## What user requires
One HOST presses START in lobby with up to three other humans (2–4 total). The ORIGINAL BLACK mission level_00/stg_0001 starts automatically on every actual phone and EVERY player plays its ORIGINAL first-person camera/weapon. Each human sees EACH other as an original ally/hostile BOT soldier visual, not original first-person hands. ALL humans' actions/attacks change the SAME host enemy AI, world objects, health, objective and next original campaign mission. The test is NOT acceptable until genuinely in same authoritative original game world.

## This session's inspected evidence and actual commits
- Inspected failed Android compile #45068110154; original remote-bot hooks.cpp lacked g_originalStageFileOpen. Fixed [39df2567](https://github.com/alwansan/BO/commit/39df2567fc197fc5a8406cd6b7cbde60a026b732). Native source correct with no direct cross-translation-unit symbols.
- Inspected original pinned Android \`src/game/layout.h\` and \`src/game/exit_menu.cpp\`, original MIT PC native menu. Distinguish ORIGINAL guest ui manager \`R32(0x2D199C)\` vs GAME STATE \`R32(game+0x21070)==game+0x20220\`. Wrong comparison caused persistent invisible auto-start block, fixed [ba34d965](https://github.com/alwansan/BO/commit/ba34d965bd905660d24d71ec38e0513691dd5d72), and dropped unjustified EBP==ui on nested difficulty wrapper [4c131fca](https://github.com/alwansan/BO/commit/4c131fca34443dc47914eb71f08c478986776929). Still original page transitions/Confirm are **UNTESTED ON PHONE**.
- Start now accepts exactly 2–4 humans (1–3 genuine TLS peers) with PREPARE tickets 3/7/15, stage mission0/stage1, host+all guests native auto frontend, UI button enabled 2–4; [3d0ca55](https://github.com/alwansan/BO/commit/3d0ca55afa922a4ed9d1be52307bc30a27648b10).
- New four-human encrypted socket Start test (NO XBE) plus native contract slot3/4 [07554692](https://github.com/alwansan/BO/commit/07554692a559291d2c7557001d665ced65b82162), CI #45070918389 SUCCESS.
- Replaced two-human-only native body source with canonical 1–3 other humans batch. Host uses original feet and trusted TLS input for owned placeholders; guests take only fresh host world snapshots; roster/ticket/350ms bounds. Java [d9108bb](https://github.com/alwansan/BO/commit/d9108bb7257d6f49bf5ec8b1fd19e0015909cdbd) (real TLS 45071151487 SUCCESS); native bounded feed [9bf7ffdb](https://github.com/alwansan/BO/commit/9bf7ffdb60e1643599f1ddc4315e48d6780fe152); original bot visual array per peer [4a8d90af](https://github.com/alwansan/BO/commit/4a8d90af47c856f265a1051a61582ccb44cf7ce3) C++ story sanitizers 45071316506 SUCCESS; retry source eligibility once/2sec BEFORE first original clone memory writes only [3b99e615](https://github.com/alwansan/BO/commit/3b99e615daa75381ce59fae95ea7e0ab26cabe43). These are BOT-LOOK remote soldier visuals only. **DO NOT imply REAL host engine actors or shared combat**.
- Latest Android APK workflow [45071574359](https://github.com/alwansan/BO/actions/runs/45071574359) launched from 3b99e615; status last checked queued. Next task query actual success/failure and CI logs.
- Original 2006 BLACK XBE from owner biso EXACT matches MIT PC known source reference SHA256 (private CI 45067068552 SUCCESS), but Android guest ABI/render visible not proven.
- Earlier owner two-phone host+guest CL event logs read; old first-stage start mismatch fixed last session with stage1.

## Remaining P0 — do NOT declare success
1. Actual live original XBE scene auto-start on every phone needs Android runtime proof. Host/guest source-packets only are not scene launch.
2. Per-peer soldier BOT-LOOK engine-memory code added but not runtime proven visible; safeguard blank slots, fake NPC AI prohibited, host protagonist keeps original first-person.
3. Real original host-owned actor for each authenticated remote human including independent physics, weapon, collisions, health, aiming, ability to trigger objectives and receive NPC aggro; original BLACK is singleplayer, not provided by one visual source.
4. HOST ONLY enemy/quest/mission authoritative world; guests must not run independent authoritative NPC AI and quest progress. Use original native game engine data/state and bounded indexed network snapshots; no fake separate world.
5. Meaningful single phone test only once all four above implemented. Until then all APKs are experimental and not acceptable as shared story even if build SUCCESS.

## Resume instructions
Read \`AGENTS.md\`, \`prompt/RESTORE_SESSION_AR.md\`, \`docs/CLOUD_HANDOFF_AR.md\`, \`memory/MEMORY.md\`, \`memory/STATE.json\`, \`memory/PLAN_AR.md\`, this session. Verify HEAD of \`alwansan/BO:experiment/black-coop\` and newest GitHub Actions, analyze new CL/black log if uploaded by owner. Only commit on branch experiment/android-? NO: writable ONLY \`experiment/black-coop\`; no edits to CL or biso or upstream source. Preserve user-supplied original XBE/ISO, protect saves.

