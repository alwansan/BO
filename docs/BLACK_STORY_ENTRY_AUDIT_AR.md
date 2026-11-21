# BO-COOP — candidate host mission entry audit, 2026-11-22

## Evidence and scope
Source inspected directly, pinned upstream RanggaCasper/Black-Android @ f3754d6985a8c35204e7df071546e586b10f1806, src/game/difficulty.cpp, src/game/exit_menu.cpp, src/game/options_menu.cpp, src/game/layout.h, src/game/flash.h, src/game/hooks.cpp. These comments and addresses are from the pinned game port source; they are NOT proof of full ABI semantics on the owner's c5bfed25449c8212d7a976f15246e87ead57713f XBE until an explicit source/translation verification passes. No guest writes performed.

## Specific mission-select candidates
- src/game/difficulty.cpp defines kMissions=8 and maps mission indices 0..7, not 1..8. First mission ID **0** is valid. The mission unlock table is described at 0x2D0D9E, 0x20 per mission, 0x100 per difficulty. It is a SAVE/progression unlock table; DO NOT rewrite those bytes to start a level.
- sub_0011B890 is described as level-select choice/mission ticks and available difficulty count; sub_00124F30 as difficulty-select bindings/ticks.
- sub_00124CF0 wrapper is described as "the mission starts on the difficulty chosen, nothing when it is not open there"; current wrapper calls Unlock() then original __imp__sub_00124CF0. This is an OBSERVATION/RESEARCH candidate to identify legitimate game-thread mission launch, NOT a proven reusable explicit Start(stage,mission) API.
- kDifficultySelection=0x2EC560 in the pinned source governs difficulty UI binding count, not proof of selected mission ID. The frontend Flash page numbers and loaded-movie pointer are not campaign scene epochs.
- src/game/exit_menu.cpp title startButton controls UI page navigation; its "start" and "page loaded" references are frontend movie state, not proof that mission world loaded or peers ready.

## Corrected safety preflight
- Earlier initial C++ mission_barrier.hpp wrongly rejected mission==0 and stage==0. Corrected after reading pinned source. Require session/nonce !=0, scene epoch !=0, mission<8, roster includes host and at least one actual remote; stage 0 allowed because stage numbering not independently mapped yet.
- Host sends PREPARE only for an actual mission choice from game-thread hook, with a new room generation+nonce; guest may send READY only AFTER exact scene actual loading is confirmed in engine, not when UI clicks Join.
- COMMIT/activation must only apply after every expected connected human is ready on the same host-selected scene, with ID->authenticated socket binding and no replay. A state-machine PASS does not prove engine/hook implementation. Network worker never calls X86Context/guest code.
- The local first-person player remains owner-controlled. For all OTHER humans, including host seen from guest, use independently owned Ally LOOK, not NPC/AI brain. Actors+0x250/0x324/0x328 and vtable/local draw offsets must be audited before writing.
- Next direct audit: inspect generated matched XBE function shapes and caller chain of 00124CF0/0011B890/00124F30 and authoritative stage state; build read-only bounded game-thread observation and lifecycle reset proof before any shared Start. Preserve solo saves and mission progression. Do not spoof complete campaign by launching solo mission simultaneously.

## Reference links
- https://github.com/RanggaCasper/Black-Android/blob/f3754d6985a8c35204e7df071546e586b10f1806/src/game/difficulty.cpp
- https://github.com/RanggaCasper/Black-Android/blob/f3754d6985a8c35204e7df071546e586b10f1806/src/game/exit_menu.cpp
- https://github.com/alwansan/BO/blob/experiment/black-coop/docs/ACTOR_INSTANCE_OWNERSHIP_AR.md
- https://github.com/alwansan/BO/blob/experiment/black-coop/docs/superpowers/plans/2026-11-22-host-story-remote-ally.md
