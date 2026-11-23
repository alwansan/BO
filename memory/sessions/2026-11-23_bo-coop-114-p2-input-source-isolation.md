# BO-COOP Session114 — 2026-11-23

### Session114 — 2026-11-23 — original P2 controller source isolated in detached snapshot, not gameplay

- Actual code [54cc9b55](https://github.com/alwansan/BO/commit/54cc9b553dc6cfd0d1930f1af64e3678e3f6d89f) extends `room_original_host_player_shadow_plan.hpp` with independently copied 0x40-byte input source. Pinned MIT original PC `black_p2_update` discovers P1 source via `GUEST32(GUEST32(core+0x354)+0xA8)`, copies it and replaces exact matching P2 actor donor references with independent P2 input storage; actual second controller binding is still a future game-tick operation.
- Added fail-closed source-manager, committed-range, overlapping allocation, failed read and before/after source identity guards. `InputIsolated()` distinguishes a verified standalone controller SOURCE from `Ready()`; a missing source during a cutscene is NOT ready. No guest writes, no actual Android player2, no weapons/animation/audio/proxy/AI or same original mission. Synthetic native tests cover source clone/rebase, boundaries, no P1 mutation, rollback and wrong pointers. Compile/CI on this commit must be checked; do not claim Android gameplay.
- Previous source [45222569021](https://github.com/alwansan/BO/actions/runs/45222569021) and UDP [45222569507](https://github.com/alwansan/BO/actions/runs/45222569507) SUCCESS. Session112 Windows original private lab [45222127349](https://github.com/alwansan/BO/actions/runs/45222127349) still running at last check; previous full Android [45222569240](https://github.com/alwansan/BO/actions/runs/45222569240) was also building. Current commit automatically triggers a newer Android candidate; inspect live statuses, older runs may be cancelled by CI concurrency. No fake success.
- Next: verified full original host scene and safe game-thread native actor2 materialization/independent animation, sound, weapon and camera context + P1 restoration, tracked proxy/AI, authoritative host NPC/doors/objectives and guest original engine apply. Do not send small phone APKs. Only write `experiment/black-coop`; no main/CL/biso or original XBE/ISO/saves/raw logs modification.


## Verified source mapping

The upstream `black_p2_update` consults original `GUEST32(GUEST32(core+0x354)+0xA8)` for a controller source, validates 0x40 bytes and copies them into independent P2 source. Its actor array then replaces exact donor-address words with the P2 independent input address. Here the entire preparation is deliberately DETACHED host C++ memory (no guest mutation) so this is not a second controller playing an original mission.

Subsequent original in-engine setup needs live game-thread scene identity, real packet input, full owner/actor/weapon/animation/audio and reversible context/physics transitions. AI and canonical host mission state on all guests remain unimplemented.

The next real Windows lab counter categories may identify source of P1 restore failures; do not relax acceptance or leak private game data.
