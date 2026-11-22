# Session57 — Original BLACK mission field B read BEFORE and AFTER original launch
Date 2026-11-22. **Only writable branch** alwansan/BO experiment/black-coop.

## Issue and exact source correction
Original source width CI [44977857722](https://github.com/alwansan/BO/actions/runs/44977857722) proved 0x002D0D68 is WORD (16-bit) and original launch sub_00124CF0 WRITES it inside the call. Previous diagnostic captured B only BEFORE the original call, so a stale candidate might be mislabeled as launch selection. New source stage preserves the original call and captures A/B_BEFORE/C from read-only R16 beforehand, B_AFTER via R16 after original function return, with an atomic per-original-launch episode guard. Pending, stale, out-of-order or duplicate call completions fail closed.

Four fixed 81-code families now 100..423: A, B_BEFORE, B_AFTER, C. Emitted only after actual two original resource opens for the same episode AND 30 original non-front-end player observations. Java logs contain only source label, 0..7/OTHER, allowed folder, and UNPROVEN; no original bytes, paths, guest memory writes or mission identity claims. Prior builds' B event is pre-call only; do not compare with B_AFTER.

## Tests and status
Input HEAD 752e1679affd611c409eebd7a5a4ed206612af85. CI [44978058718](https://github.com/alwansan/BO/actions/runs/44978058718) source PASS, [44978058696](https://github.com/alwansan/BO/actions/runs/44978058696) four-peer real TLS MOCKED READY PASS, [44978058836](https://github.com/alwansan/BO/actions/runs/44978058836) Android29 platform PASS, APK [44978058766](https://github.com/alwansan/BO/actions/runs/44978058766) was IN_PROGRESS at first check. New source must pass [native preflight workflow](https://github.com/alwansan/BO/actions/workflows/black-coop-room-network-preflight.yml); don't call it passing without checking live results.

**Not implemented:** verified selected campaign index, safe original guest level loader, true game scene-loaded callback and READY, independently rendered remote human bot, host-authoritative AI/combat/objectives. Owner wants actual shared BLACK campaign on 2–4 phones with ANY enemy/friend bot appearance temporarily and normal original first person locally. No small phone tests; one substantial two-phone trial only after real gameplay exists, all details via bounded logs.

Next: actual source-verified original stage loader and authoritative host-selected scene proof; do not auto-load a guessed folder.

## Verified later this session
- [C++ + pinned stage + Java8 preflight 44978712336 SUCCESS](https://github.com/alwansan/BO/actions/runs/44978712336). Actual job log contains `ORIGINAL_MISSION_PRE_POST_WORD_PASS` (synthetic contract), `ROOM_NET_STAGE_PASS` and `ROOM_NET_JAVA_PASS`; this verifies code compilation/staging, not game device runtime.
- [UDP CI44978712333 SUCCESS](https://github.com/alwansan/BO/actions/runs/44978712333).
- [Full Android APK44978712319](https://github.com/alwansan/BO/actions/runs/44978712319) and [Android29 TLS44978712299](https://github.com/alwansan/BO/actions/runs/44978712299) IN_PROGRESS when checked; always recheck live. No actual shared BLACK campaign or phone trial.
