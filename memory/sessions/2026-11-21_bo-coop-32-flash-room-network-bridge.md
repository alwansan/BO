# BO-COOP Session32 — Flash HOST/JOIN source bridge to Android TLS, isolated network APK

Date 2026-11-21. Only writable `alwansan/BO:experiment/black-coop`; never main. Read `AGENTS.md`, STATE, Session31, this file, RESTORE and live Actions before writes.

## Proven from latest CI
- Real Android29 EC-P256 AndroidKeyStore/pinned TLS `TLS_PLATFORM_PASS` 44922935535.
- Standalone Android Host/Join/Leave form and RoomSession host/join runtime compilation/instrumentation `TLS_PLATFORM_PASS` **44923426931 SUCCESS**, job136904270536.
- JVM RoomSession real sockets `ROOM_SESSION_PASS` 44923264389 SUCCESS.
- Source-only game bridge [44924220185](https://github.com/alwansan/BO/actions/runs/44924220185) SUCCESS: Python atomic fixture tests `ROOM_NET_STAGE_PASS` + Android javac 8 `ROOM_NET_JAVA_PASS`. Fix predecessor failed test44924134306: must patch read-only UpdateRow guard, not ApplyRowInput.
- Full Windows/NDK game APK [44924346601](https://github.com/alwansan/BO/actions/runs/44924346601) running when documented; NO successful APK claim yet.

## New code/source changes and isolation
- `a043245d79cbfe768e86a05ae1ec44db3d956bd7`: `RoomAppRooms.java` process room owner (session persists after dismissing the form) and `room_net_ui.hpp` game-thread JNI calls, cached text.
- `09c82527d3413434c02b6f66af50300deda1cc8e`: initial `prepare_room_network.py`, negative/atomic tests.
- `47c3f957a3b9fa4557f175e9e6011e4841076a1a`: fix read-only room row short-circuit for HOST/JOIN confirms only. Source-only CI passed.
- `ead146ffc1f3f6fca1ee540330cbaaea02c9a3c0`: separate full Windows APK workflow `.github/workflows/black-coop-room-network-apk.yml`, retains matching owner XBE c5 SHA1, no assets in output, package `org.bocoop.black.roomnet`, label `BLACK Co-op NETWORK TEST`, network permissions only in new app, accepted `roomui` untouched.
- Flash UI confirmation -> `BlackActivity.openCoopRoom(row)` -> `RoomLobbyActivity` (real TLS host/join); status rows display phase and authenticated human counts via `RoomAppRooms.statusLine`. No network workers call Flash; do not equate roster peers with game actors.

## Next
Check newest full APK Windows run logs. If build passed, verify SHA, artifact, permissions, JNI and emulator/physical device feedback before calling in-game bridge functional. No two-physical-phone test, game actor body/brain+324 ownership or combat/story sync. Previously accepted CO-OP button and Probe4 are NOT to be retested; private saves, original ISO, original base APK, main untouched.
