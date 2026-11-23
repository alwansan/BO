# Session95 — original engine boot input diagnostics (2026-11-23)

Branch: experiment/black-coop only.

## Fresh live CI
- [45104974473](https://github.com/alwansan/BO/actions/runs/45104974473): original-engine Windows job failed acceptance, **but engine compiled and exited successfully**. Aggregate: engine_built=true; engine_exit_success=true; owner_xbe_sha256_verified=true; owner_files_extracted=574; boot_progress.xbe_loaded=true; guest_entry_called=true; blocker=usb_input_not_polled. No mission ticks, no player2 actor or physics/weapon evidence. This corrects the stale assumption that configuration remains the latest blocker.
- [45107430108](https://github.com/alwansan/BO/actions/runs/45107430108): source-only shared-world acceptance guard success, **NO GAMEPLAY**.
- [45103454355](https://github.com/alwansan/BO/actions/runs/45103454355): Android room-only APK workflow success, **NO GAMEPLAY**.
- CL main host logs and guest folder inspected read-only; earlier two-device result remains separate original worlds.

## Code
- [a1641ced](https://github.com/alwansan/BO/commit/a1641ced280ac98502f3404cc43372f354a97c55): split private runtime blocker into frontend+USB unpolled versus USB-only unpolled, preserving fail-closed engine acceptance.
- [290e63f9](https://github.com/alwansan/BO/commit/290e63f942c93631fb14aa495bf5b2246b851061): once-per-process original frontend UI marker in disposable observer; no game memory changes or private bytes in artifact.
- [44a53691](https://github.com/alwansan/BO/commit/44a536918371b974daed1a4fdc5771066db21888): unit regression for frontend progress marker independent of USB.
- New harness CI is pending verification; do not assert green. No Android rebuild needed for engine-only harness source changes.

## Next
Inspect new engine-lab CI. If original UI marker absent, investigate native frontend hook and startup; if present but USB absent, inspect USB initialization/controller enumeration and Windows runtime configuration. Only after boot reaches original mission, validate P2 actor/proxy/weapon context and input, then implement host-authoritative 2–4 actor and guest scene consumption. Never substitute transport or synthetic test success for shared-world acceptance. One large owner phone trial only when substantive gameplay is ready. Do not modify main, CL, biso, owner XBE/ISO/saves.
