# Session107 — original BLACK early Dashboard error classification

Date 2026-11-23. Writable branch experiment/black-coop only. Goal remains one **original BLACK** host-authoritative mission with 2–4 separate human actors, synchronized NPCs/combat/doors/objectives, not four private worlds.

## New real private engine evidence

Original Windows private two-player [45197279738](https://github.com/alwansan/BO/actions/runs/45197279738) **FAILED gameplay**, but built/recomped the pinned original game and verified 574 owner files/sha. `firmware_kind_2` = HalQuickRebootRoutine, runtime ~484ms; `launch_page_present=true`, `launch_type_1=true`, `launch_path_empty=true`, `launch_word0_1=true`, no USB input poll, frontend or mission and no P2.
- Public Xbox LaunchDataPage [documentation](https://xboxdevwiki.net/Kernel/LaunchDataPage) specifies Type1 = switch to Dashboard, first payload word = reason, reason1 = ERROR, third payload word = parameters[0]. Error kinds 1 invalid XBE, 2 invalid harddrive, 3 region, 4 parental control, 5 bad media type. **This proves the guest requested the original Xbox Dashboard due to ERROR**, not just a vague quick restart or a path-directed next XBE. It does NOT yet prove which failure caused this decision. Audio output unavailable was observed but not shown causal.

## New source code

- [dcc9fc76](https://github.com/alwansan/BO/commit/dcc9fc760e3b6be0b6ceffca20310abca39490f9) began bounded original DVD and file-open boot probes. A generated string replacement error inadvertently made this intermediate commit's Python unparsable; [45201462925](https://github.com/alwansan/BO/actions/runs/45201462925) caught it promptly in harness. [926fc93a](https://github.com/alwansan/BO/commit/926fc93a8c8436ada7026ad024a4d73527b8d839) added three regression tests; it was also based on this temporary syntax failure, **not a working release**.
- [95c01225](https://github.com/alwansan/BO/commit/95c01225497bcbdcbbb8bc756157582a94f92078) repaired the Python function boundary and private reader. [45201724219](https://github.com/alwansan/BO/actions/runs/45201724219) harness checks **SUCCESS**, original-engine job pending/in progress after that check.
- [83d7d566](https://github.com/alwansan/BO/commit/83d7d566e4520ada649b4eeb3114ebcb70e76b2c) extended `collect_launch_hints`: only when Type1 and reason1, classify parameters[0] as fixed `dashboard_error_invalid_xbe`, `harddrive`, `region`, `parental_control`, `media_type`, or `other`. Three tests cover all category enums, unrelated launch types and private-log filtering. Never print actual parameter/launch path/titleid or any guest stack. New real engine run [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) pending on first review.
- Source parser also emits bounded `dvd_device_opened`, `dvd_mode_sense_seen`, `dvd_other_ioctl_seen`, `file_open_failed_seen` from **existing pinned runtime log**; optional file misses alone are not causal evidence.

## Acceptance status and safety

No real Android multiplayer yet. The room-only Android build [45199349662](https://github.com/alwansan/BO/actions/runs/45199349662) succeeded but is **NO GAMEPLAY**, and cross-adapter original proxy ownership sanitizer [45199349609](https://github.com/alwansan/BO/actions/runs/45199349609) succeeded, neither represents original game integration.

P0 next: read real bounded `dashboard_error_*` after new Windows lab. Fix proven original media/region/harddrive/XBE path without spoofing success; then integrate original host 2–4 actors with camera/animation/weapon/audio/physics/AI and stable native scene lifecycle, host canonical NPC/objects/objectives and guest original scene consumption. Ask owner to run one significant combined multi-phone trial only when real original engine shared-world work is ready.

Never touch main, CL, biso, original game archives, saves, or private logs. Source pins and CI privacy remain hard constraints.
