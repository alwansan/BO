# Session103 — private original firmware-exit diagnostic (2026-11-23)

- Verified source HEAD `10b4ae54cc4bbc9d9e9fe738025f57f630eba613`. Real original Windows engine lab [45186010290](https://github.com/alwansan/BO/actions/runs/45186010290) built the engine, verified XBE/extracted 574 owner files, but `HalReturnToFirmware` exited at ~219ms before first original mission, frontend update or USB poll. This is a FAILURE for co-op acceptance, despite exit code 0.
- New disposable and strict source-anchored HAL patch records only fixed firmware mode markers 0,1,2,3,OTHER. No raw call argument, guest bytes, addresses, game assets or private log are exported. Original firmware ExitProcess remains intact.
- Regression checks fixed mode classification and private output exclusion. CI still required; **this does not fix early firmware exit**.
- Outstanding P0: identify actual exit class, resolve original boot correctly; finish real player2 actor+camera+animation+audio+weapon+proxy+AI, host-authoritative combat/world state and actual guest original-world apply. Only request combined phone test after true implementation. Do not call room APK gameplay co-op.
- Writes only alwansan/BO:experiment/black-coop. Protect main, CL, biso and owner's ISO/XBE/save files.
