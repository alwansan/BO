# BO-COOP — Original BLACK v6 camera-eye TLS and real story gate (2026-11-22)

Owner permits ANY original bot enemy/ally mesh as a prototype human remote body; don't delay actual co-op for visual polish. Maintain original local first-person player and human input. Work only on `experiment/black-coop`.

## Already implemented
- `RoomWire v6`: full-room TLS 4 humans, `HUMAN_INPUT` type12 (x/y/yaw/buttons), `WORLD_SNAPSHOT` type13 (authoritative host view), new `HUMAN_CAMERA_EYE` type14 (guest sends sampled original BLACK local camera eye x/y/z).
- Source-confirmed original `player+0x100` first-person *camera* XYZ read-only at max10Hz in pinned original game update, quantized/bounded, JNI on native game thread. `RoomSession` sends guest→host v6 `HUMAN_CAMERA_EYE` **after story COMMIT**, host then fans out own + valid recent guest eyes via `WORLD_SNAPSHOT`. Host identity of each guest derived from TLS socket, not the frame. Single latest-only queues, replay/epoch/roster/rate/freshness guards. No game writes.
- CI <https://github.com/alwansan/BO/actions/runs/44973843153> SUCCESS: host+3 real TLS loopback guests, camera frames and reverse world fanout, 4 human identities, cancel/restart, mocked scene READY. CI test positions synthetic — native sampled camera on real phone still unverified.
- CI <https://github.com/alwansan/BO/actions/runs/44973843154> SUCCESS Android TLS platform, <https://github.com/alwansan/BO/actions/runs/44973843200> SUCCESS native Android eye-source staging.
- Read-only conservative non-front-end candidate `room_nonfrontend_original_player.hpp` uses original `exit_menu.cpp` game state test plus 30 stable local player samples; fixed safe event ONLY, cannot send READY or prove a stage. Source CI <https://github.com/alwansan/BO/actions/runs/44974018224> check live.
- APK <https://github.com/alwansan/BO/actions/runs/44974018195> if successful still ONLY a diagnostic network build, **NOT multiplayer story**.

## Unblocked vs blocked
**Unblocked**: 4-player room TLS, movement frame schemas, original read-only camera-eye sample source, source-to-host camera stream, source-to-guests latest-only state, temporary ANY BOT LOOK selection policy.
**Still P0 blocked**: identify REAL original mission/stage chosen at host vs difficulty/level frontend candidates; guest invoke actual original stage loader safely, prove scene loaded and same host session, then host-only shared AI/combat/quest. Original camera is **not original body root**, and no remote original bot model is rendered in game. No user-facing claim that socket tests prove shared-world gameplay.

## Immediate next
Implement verified mission selection + guest launch integration, game-thread scene epoch/ready, original actor renderer using any existing bot mesh. Keep owner phone tests to one meaningful large milestone with all minor evidence in safe auto logs. Always update RESTORE/STATE/plan; never modify BO main, CL, biso, ISO or saves.
