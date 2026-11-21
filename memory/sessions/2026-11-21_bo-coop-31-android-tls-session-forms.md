# BO-COOP — Session31 Android secure room runtime and isolated forms (2026-11-21)

## Source of truth
Repository `alwansan/BO`, only writable branch `experiment/black-coop`. Session30 restored from `prompt/RESTORE_SESSION_AR.md` and live GitHub, not a synthetic reconstruction. Initial HEAD `1da051c02c300ef5c4a301ae2691d6ab5e8830f7`. Existing owner-approved title CO-OP UI and Probe4 remain accepted, no retest or reupload requested.

## Verified Android TLS resolution
- 44922289967 **FAILED** with `exit 124` during emulator install phase, not definitive EC failure; recent fix switched to `adb push` plus `pm install`.
- 44922935535 **SUCCESS**, job136902689543. Raw log explicit `INSTRUMENTATION_RESULT: stream=TLS_PLATFORM_PASS`, code -1. Opaque EC P-256 AndroidKeyStore identity preserved, pinned real TLS Android29 host/client, roster, heartbeat and shutdown.
- Later run44923264409 failed in emulator package-manager installation after `test.apk` push, exit124; unrelated JVM session tests passed. Keep platform reliability investigation open.

## Code delivered
1. `e8c0aece75bc466644e3a9cd775b8f5c79ce4105`: `RoomSession.java` and `SessionTest.java`; generation-guard cancellation, one bounded worker, host/join over real RoomTls/RoomDirect, immutable snapshot and close. [JVM CI44923264389](https://github.com/alwansan/BO/actions/runs/44923264389) SUCCESS; job136903679809 logged `ROOM_SESSION_PASS` and other TLS/discovery regressions PASS.
2. `9ab240262f5d327203053b6ca7d2c820360b72a8`: `RoomLobbyActivity.java` standalone Android form for HOST/JOIN/LEAVE (name, port, PIN, full out-of-band SHA-256 fingerprint, local addresses, actual TLS roster). Manifest and build script compile it into synthetic test APK. Android instrumentation also checks RoomSession host/join loopback. [Android CI44923426931](https://github.com/alwansan/BO/actions/runs/44923426931) pending when documented.

## Strict limits / next
The accepted in-game Flash CO-OP room still has HOST/JOIN unavailable; the new Activity is not yet linked to game; do not claim gameplay co-op, two-phone LAN, full body, mission or combat. Next read current CI and fix bounded emulator install if needed, then implement BlackActivity/Flash bridge in separate package and real on-device room. Independently inspect factory1DF440, component+324 freelist, rig assets and safe world registration before guest writes. Update memory docs and RESTORE after new CI; use expected-SHA CAS, no force push. Preserve saves and main.
