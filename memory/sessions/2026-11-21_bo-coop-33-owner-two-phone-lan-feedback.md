# Session 33 — owner two-phone LAN test and room UI/discovery repair (2026-11-21)

## Actual owner evidence
- CL screenshots directory black log/img lists four roomnet screenshots at 19:33–19:34. GitHub exposed filenames but image bytes could not be viewed through available text-only connector, so the owner's visual report is the source for UI complaints, not an independent pixel inspection.
- Two separate phones verified by CL logs: POCO F5 Pro 23013PC75G Android15/Adreno730 at BLACK-Probe-20261121-193320-285/black_log.txt, and Xiaomi 22081283G Android14/Mali-G57 MC2 at black log/logs/black_log.txt. Both load XBE c5bfed25449c8212d7a976f15246e87ead57713f. No explicit TLS JOIN evidence. BLACK-Probe-20261121-194623-826/black_log.txt empty (0B). Second device reported quit ConfirmWarning FEAudio failed at ~294.6s; network causal link not established.
- First real room-net Android APK workflow 44924346601 SUCCESS; artifact14715302629. Owner tested the APK, not playable story co-op.

## Source fixes committed only to experiment/black-coop
- b418b83: RoomSession Host starts UDP discovery listener at 1510, RoomDiscovery v2 emits a full 32-byte certificate fingerprint + game profile and current roster size. Never transmit PIN. First connection requires trusted out-of-band full pin verification.
- 7500720 and 4daf69b: RoomOverlay in same BlackActivity (custom BLACK-styled Dialog) instead of separate Android RoomLobbyActivity. Android Back/LEAVE no longer routes via startActivity to another Activity; server and clients live in RoomAppRooms. Added capped scrubbed room event files on both devices.
- 9d4ae98/f1a3241/0900f6d: prepared Java source transition and regression checks. One preflight was red due to overly broad assertion matching existing storage picker startActivity; corrected.
- 24ab3d7 and 9302b95: distinguish TCP connect timeout/refusal, TLS identity mismatch, wrong PIN, record event reason codes in safe logs. d78e3d9: sensible default names, separate discovery error from empty result.
- 8a42b42: Windows APK CI cancels obsolete builds so latest code gets artifact. b89fc9d/af14d855: native Flash CO-OP row labels improved to HOST ROOM and FIND LAN ROOM; test assertions aligned.
- Existing accepted title CO-OP menu and Probe4 remain. Android overlay is styled and in same Activity, but not yet a 100% original Flash page; do not overstate.
- CI verified so far: 44930785583 room-net source+Android Java preflight SUCCESS, 44930785371 JVM TLS core SUCCESS. Later runs triggered by subsequent UI/CI changes need separate status verification. No new physical device PASS asserted.
- Docs: docs/ROOM_NETWORK_DEVICE_TEST_AR.md updated with actual two-phone source distinctions, LAN + TLS steps, Back/Leave expectations, and secret-safe logs.

## Next
1. Check HEAD and newest black-coop-room-network-preflight/real-tls/apk workflows, not old cancelled runs. Inspect any errors, fix with CAS.
2. Obtain artifact from new full Windows/NDK APK. Confirm identity/signature/metadata/no assets. Warn user about debug signing mismatch, don't remove older APK or ISO until backup.
3. Two-phone test of automatic LAN room discovery, full fingerprint comparison, PIN, real 2/4 roster and Back/Leave. Collect sanitized logs from both phones. No claim of successful join until actual evidence.
4. Finish original Flash/native game menus; independent guest body +324 ownership and safe actor spawning/mission+combat sync remain separate phases, not done.
