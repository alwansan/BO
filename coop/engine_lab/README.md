### Session120 genuine original PC 2-player motion and world-grid acceptance source proof — 2026-11-23

- [Actual original Windows PC engine run 45233723518](https://github.com/alwansan/BO/actions/runs/45233723518) finished after 1071250 ms with **real original mission_ticks=1221**, **P2 ready=768**, independent actor=1216, physics proxy=768, weapon=768, P1/P2 original movements=4527/6363 mm (their physical bodies same), original ammo-shoot witness=10/10, 8 input phases complete, 0 epoch/proxy/invalid failures. This proves a source-authentic 2-player original PC engine mission, NOT original Android 2–4 human networked co-op.
- Why old `passed=false`? The only failed evaluator check was `p1_context_not_restored`. The old strict `memcmp` over all original actor bytes counted **1215** changes, but all **1215** were P1 **world-owned grid node head 0x20..0x2F only**. Exact P1 actor outside world node matched **1215/1215**; grid cell +0x30..0x3B and neighbor links +0x3C..0x57 changed **0**, other actor/owner/physics/model/weapon/motion/scope differences **0**. Pinned MIT PC `black_p2_find_links/black_p2_swap` specifically refuses to swap world-owned P1 grid-node bytes, so whole-actor byte equality is an inappropriate *P1 context* condition.
- `lab.py original_p1_actor_context_restored` now requires >=720 unchanged P1 non-world-node samples, exact consistency of old restore-failure counts with only world-head changes, **zero** scope/motion/weapon/model/tail/non-grid differences, **zero** original grid cell/link changes. The old full-actor counters remain in JSON unmodified for audit; the new rule is source-defined, not a blanket ignore of mismatches. Fail-closed tests corrupt each forbidden counter separately and test real-run sanitized numeric result. This re-evaluates already observed aggregate data, **not** a retroactive claim that the historical GitHub run turned green. New real PC engine run is required for an end-to-end CI PASS.
- Android P2 guest stage direct writer bypass was closed in commit [3fe6ab69](https://github.com/alwansan/BO/commit/3fe6ab69d6fc861983e950fe18404e380db0ab17), and read-only world-grid link witness integrated with `OriginalHostPlayerStage` in [ccd754be](https://github.com/alwansan/BO/commit/ccd754be59c0caddae43571ebe7ab5cb6d0a1fcd). [Source preflight 45238513295](https://github.com/alwansan/BO/actions/runs/45238513295) SUCCESS, [Source preflight 45239115392](https://github.com/alwansan/BO/actions/runs/45239115392) SUCCESS. No original Android P2 engine actor activated yet, no host-canonical NPC/doors/objectives guest apply, no two-phone game world. No phone trial.

### Session118 — 2026-11-23 — original PC grid-node, physics, and scripted-intro source diagnosis

- **Authoritative real original PC engine**: [run 45227027742](https://github.com/alwansan/BO/actions/runs/45227027742) FAILS gameplay acceptance, *but* source authentic story-loop `mission_ticks=249` and two original Xbox USB pad ports `6316/6253` confirmed. Both original physical proxy/body pointers were present when sampled (`p1_proxy_missing_ticks=p1_body_missing_ticks=p2_proxy_missing_ticks=p2_body_missing_ticks=0`), original pool had >=2 free slots (`proxy_pool_low_ticks=0`), but `scripted_control_ticks=244` of 244 distinct actor ticks, `p2_ready_ticks=0`, `phases_completed=0`; zero proven movement or weapon effects. Do not wrongly claim player2 lacks a physical proxy, nor that a player2 playable world exists. Exact independent proxy/body identity and original cutscene source states are still not proven; a present pointer alone is insufficient.
- All **243 of 243 strict original P1 restore mismatches** were isolated to *actor+0x20..+0x57*, the original world-owned grid node. `restore_grid_other_failures=0`, all motion/weapons/model/tail/scope differences zero. Pinned MIT PC `portingpete/Black-2006-Recomp@b2512fafd3` `black_p2_find_links/black_p2_swap` intentionally protect world-grid links from P1/P2 swap. Thus a strict whole-actor byte compare may be overinclusive for a live grid link, but integrity is NOT yet proven, so original full restore acceptance remains unchanged and strict.
- Code [3fd362da](https://github.com/alwansan/BO/commit/3fd362da61d33cb1c9e2875e954e741413356073) adds **12 new aggregate-only, privacy allowlisted** PC lab metrics: P1 stable-non-grid restore vs grid-only change, three node offset subregions, distinct P2 physics/body witness independent of cinematic readiness, original P2 setup-vs-playing phase and four scripted owner/actor hold statuses. No guest RAM writes, actor pointers/bytes, private game or save paths exported. Does NOT make the real game playable or change `evaluate()` strict pass criteria. C native observer fixture and Python privacy+nonpass test passed in [run 45233723518](https://github.com/alwansan/BO/actions/runs/45233723518) harness-checks; real original-engine job queued at check.
- **Duration problem:** prior Windows lab `300s` bound gave only 249 mission ticks, below mandatory `720` even if the whole game were playable. Increased the bounded original-engine workflow push fallback AND manual default from `300` to `1200` seconds, within validated 60..1800 input, keeping 120-minute Windows job timeout, private summary allowlist and cleanup, and all strict original 720-tick gameplay gates. This is testing headroom, not evidence that the intro will complete or multiplayer will pass.
- Android [45227692658](https://github.com/alwansan/BO/actions/runs/45227692658) **COMPLETED SUCCESS**, pinned C++ SDK/room APK compile/build only, definitely **not original 2–4 player co-op**; original Android guest actor2 setup, live input, original weapon/animation/audio context + engine-safe teardown, host canonical NPC/doors/objectives and guest apply are not yet done. Do not ask owner for minor phone tests. Work only `experiment/black-coop`; do not change main/CL/biso, owned XBE/ISO/saves/raw logs. NEXT: inspect fresh original PC lab grid subregion/physical independence/cinematic controls and investigate original game-thread actor2 setup and guest world apply without ungrounded writes.

### Session116 — 2026-11-23 — original PC engine P2 proxy blocker and grid-node restoration source proof

- Real original BLACK PC runner [45222127349](https://github.com/alwansan/BO/actions/runs/45222127349) FAILED gameplay strict after 300234 ms, while HDD path/sector4, original USB0=5650 USB1=5589, original frontend and 176 mission-loop samples were verified. New counters isolate **all 170 P1 restoration mismatches to actor 0x000..0x07F grid region**; 0 scope/motion/weapon/model/tail mismatches, 0 verified distinct proxy and 0 P2 ready. Runner summary category `mission_not_reached` means strict mission **720 distinct ready samples** not reached, NOT that the original engine never entered its mission loop. No 2-phone co-op is established.
- Source: pinned MIT `portingpete/Black-2006-Recomp@b2512fafd3` `black_p2_find_links` reserves actor+0x20..+0x57 world-owned grid node links and dynamically detects other linked actor words; `black_p2_swap` skips such links. `black_p2_take_proxy` requires a valid P1 physical proxy body, original physics pool with at least 2 free of 20 slots, and real DF400 linkage; `black_p2_setup` only calls it after actual original 1E0E40 setup. Missing real proxy cannot be inferred from a join notice.
- New private PC lab observer adds 9 **integer-only** fixed privacy-allowlisted diagnostic counters for grid 0x20..0x57 vs the other grid bytes, missing P1/P2 proxy/body, low/unreadable original physics pool and held scripted controller. No original game addresses, bytes, source dumps or identifying paths; **all original gameplay acceptance and P1 full-actor restoration criteria remain unchanged**. Native C fixture covers grid/proxy/pool/scripted gates and Python checks strict nonpass. New actual original PC runner must confirm first; fixture alone cannot prove cause or performance.
- Session115 owned-buffer staging source SDK and sanitizers [45225086457](https://github.com/alwansan/BO/actions/runs/45225086457) SUCCESS, UDP [45225086329](https://github.com/alwansan/BO/actions/runs/45225086329) SUCCESS. The Android candidate [45225086434](https://github.com/alwansan/BO/actions/runs/45225086434) still compiling at check; do not disturb long Android build via `coop/room_network` commits. Only PC lab and docs in this session. No physical P2 game actor, original guest canonical NPC/doors/mission apply, or 2–4-player story ready.
- NEXT: use private real-run counters to decide if original P1 grid mismatch is the legitimate shared node (+0x20..+0x57) or other grid corruption; independently diagnose why original P2 DF400 proxy not attached and when the 1E0E40 setup finishes. Keep full acceptance strict. Android next requires safe original P1/P2 context, animation/weapon/audio, proxy/AI, and host-owned canonical original world/guest apply. Do not request phone mini-trials; only branch `experiment/black-coop`, no main/CL/biso or original game XBE/ISO/saves/logs.

### Session112 — 2026-11-23 — categorize original PC P1 restoration failures without exposing game data

- [Private original PC engine 45213250944](https://github.com/alwansan/BO/actions/runs/45213250944) verified virtual HDD created/written/read, original frontend and 5245/5183 original USB pad reports. Multiplayer acceptance FAILED: 167 original mission-loop ticks (required 720), 161 restore failures, P2 ready/proxy=0, phase 0, ~300 seconds CPU rendering. Do not claim accepted original mission or any Android phone-storage issue.
- New `coop/engine_lab/observer.inc` observes P1 restoration discrepancies as **six aggregate numeric buckets only**: original scene/core/owner/P2-in scope, actor 0x000-0x07F grid, 0x080-0x17F motion, 0x180-0x2FF weapons, 0x300-0x3FF model and 0x400-0x8FF tail. No original guest bytes, addresses or strings are logged. Strict `evaluate()` acceptance is unchanged; original game RAM is read-only for this diagnostic. Added a native C synthetic failure fixture and Python privacy/contract assertions. A green harness is not proof of P1 restoration.
- Session111 source preflight [45220093377](https://github.com/alwansan/BO/actions/runs/45220093377) SUCCESS and UDP [45220093294](https://github.com/alwansan/BO/actions/runs/45220093294) SUCCESS. Full Android candidate [45220093146](https://github.com/alwansan/BO/actions/runs/45220093146) still compiling at check. Session112 edits PC engine-lab source and documentation only, not Android source, so it avoids superseding the long Android run.
- Next: read new *real* private PC restore-region counters; source-verify isolated 0x200 model workspace for P2 without borrowing P1; implement native Android original P2–4 full context/weapon/audio/animation and physical proxy/AI, host-owned NPC/doors/objectives and guest original-engine apply. No playable co-op or small owner phone trials. Write only `experiment/black-coop`; protect main, CL, biso, owner XBE/ISO/saves and raw logs.

# Original BLACK engine lab

This job builds the original Xbox mission as a Windows x64 static recompilation
from the owner's exact XBE and complete disc, then attempts a **real two-player
mission** with its original physics and player update functions. It does not
verify Android execution, online transport, enemy targeting, full mission
completion, or three/four players. The native Android experiment remains separate.

Run **BLACK original engine private two-player lab** on `experiment/black-coop`.
`run_seconds` bounds engine execution to 60–1800 seconds (default 300).
The job fails until it observes all acceptance counters; a successful compiler,
fixture test, boot, menu, join log, or actor allocation cannot pass it.
No successful original-engine run has been established by the harness checks.

## Immutable inputs and disposable changes

- PC host source: `portingpete/Black-2006-Recomp` at
  `b2512fafd3e181e4febfb298f297245b845ef628`.
- XboxRecomp runtime: `e4b621706a3b5e58c084838d1f27547548cf27a8`.
- DSP56300: `aab3649fc452daf03da8b5e36fdc0c308d874101`.
- Owner XBE SHA-256:
  `df2739c372d254a90aeeccf5971d12097f22d89fbb90db021fda96dd4deb68f6`.

`lab.py` uses the existing BO manifest restoration and bounded XDVDFS walker to
extract every owner file into the runner's disposable `engine/game/` directory.
It leaves the disc layout intact. It refuses an existing game directory, dirty
host sources, differing source pins and changed patch anchors. It changes
host `src/recomp_manual.c`, adds the observer/scheduler beside it, observes
`src/black_native_menu.inc`, and instruments runtime `src/usb/usb_gamepad.c` in
the disposable checkout. It also inserts fixed booleans before the original
HAL firmware-return/shutdown exits in `src/kernel/kernel_hal.c`, preserving
all original parameters and `ExitProcess(0)` behaviour. Session109 additionally
instruments **pinned, disposable Windows-only kernel files** `src/kernel/kernel_path.c`
and `src/kernel/kernel_file.c` to classify `Partition0.img` image creation,
partition-table write, exact original `NtCreateFile` partition0 open outcome,
original `NtReadFile` at sector4 offset `0x800` and fixed magic validity, and
FileFsSizeInformation inquiry. These markers never print paths, guest bytes,
file handles, error codes or the private Xbox disk data, and capture the actual
Win32 `GetLastError` before diagnostic stderr output. No exit is suppressed or converted into gameplay. No original game bytes
or generated game-function bodies are patched. Generation follows the pinned
production script; compilation uses Ninja with the active x64 MSVC developer environment and
production CMake definitions, with two build jobs. This supports the runner
image's Visual Studio 2022 or 2026 toolchain without hardcoding an unavailable
Visual Studio year. The MSVC major and backend are included as safe metadata. Python uses capstone 5.0.7; Rust is 1.98.1.


### Session109: fail-closed Xbox virtual-HDD diagnostics

Original owner-engine run [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768)
reported Dashboard Type1/Reason1 ERROR with parameter0=2 = Invalid Harddrive,
not a successful playable mission or any evidence about Android device storage.
Windows source instrumentation now emits *only* predetermined Boolean fields
in the private report's `boot_progress` dictionary, so cause may be narrowed
to table-file creation/write, partition0 open, and sector4 read (or excluded
from those exact branches). Missing flags are NOT proof that Xbox code never
attempted a different interface. See `docs/ORIGINAL_PC_HDD_PARTITION_PROBE_AR.md`.
The private Windows kernel patches are never committed to pinned upstream.
Intermediate [45210316015](https://github.com/alwansan/BO/actions/runs/45210316015)
failed due to a test-exposed Windows/POSIX anchor-count mismatch; corrected
and regression-tested via [e440e3b3](https://github.com/alwansan/BO/commit/e440e3b3348478fe5ce992a59b4b6377a1faf903).
Real original gameplay still fails until original mission, independent player2
and valid two-controller/weapon results are witnessed.

## Inputs, observations and failure semantics

The input driver substitutes the **20-byte host pad report**, then uses the real
OHCI bus, Xbox input polling, game controller and original player update paths.
It does not write guest actor state or bypass the frontend. The fresh-menu
navigation sends short A pulses every two seconds and short START pulses every twelve
seconds throughout premission navigation. These do not overlap; START can submit
the original profile keyboard after A enters characters. This is a
bounded attempt using default menu selections, **not verified navigation**. If a
fresh profile/name/save dialog blocks it, the job fails `mission_not_reached`.
The lab stops menu pulses as soon as the original mission-running field is
observed. It waits for the scripted intro to release camera/player control and
for P2 to have its own actual proxy before advancing gameplay inputs.

Eight phases of 96 original player updates each: neutral baseline, P1 strafe,
neutral, P2 opposite strafe, neutral, P1 fire, neutral, P2 fire. The phase clock is
simulation-driven; input reports are port-specific. No prerecorded game memory,
save or snapshot is supplied. CPU-only rendering is selected, and no framebuffer
window is created. Windows WARP is not used: this pinned renderer explicitly
rejects software DXGI adapters. The game still runs its original world and player
simulation; it can be slow without a GPU.

The same translation unit observer records only bounded integer counters:

- original mission-running state, separate shadow actor, actual distinct proxy
  and body-motion objects, and separate current weapons;
- full 0x900-byte P1 actor and camera-owner equality before/after each P2 pass,
  with P2 swapped out (this is a narrow update-pass restoration check);
- actor and physics-body horizontal displacement during each player's own
  movement phase, compared with that player's neutral drift;
- stable current-weapon/magazine ammunition decreases during that player's fire
  phase and separately counted delivery of movement/fire USB reports;
- mission epoch changes, invalid observations and proxy/control loss.

Movement must exceed 100 mm and twice the neutral drift plus 50 mm; both physics
bodies must also move at least 100 mm. Both weapons must lose ammunition in their
own fire phases. At least 720 ready/distinct/restored samples are required and
all eight phases must finish. Context mismatch, missing physical ownership,
control loss after readiness, epoch changes, timeout, crash, absent or malformed
aggregate evidence fails the job. Ammunition loss is a weapon effect observation,
not proof that a bullet hit an enemy. Input timing plus neutral comparison is
not a full determinism or collision correctness proof.

The manual workflow option `restart_after_phases` (CLI: `--restart-after-phases`)
adds a native lifecycle check after those phases. Port 0 uses normal USB
input to open the pause menu and select **Restart Mission → Yes**. The pinned
PC menu consumes that input headlessly only on the two restart pages. The
lab freezes the gameplay measurements at phase eight and requires the
original restart request, observed removal from the world list during unload,
and a fresh map and world link for the same unit role, authored unit index
and row count at the same level and stage. This does not require the same
world allocation. The first running frame also rechecks the mapped objects
in the terminating list. `restart_lifecycle_passed` requires zero
foreign-thread calls or mapping/list failures. `unit_lifecycle_only` (CLI:
`--unit-lifecycle-only`) starts this test after the original intro releases
control and initial membership is verified, without the two-player input
phases. Its summary scope is `original_PC_engine_unit_lifecycle_only`. The
default run still exits after phase eight. The restart test exercises the
Windows lab; its result does not establish Android co-op.

The objective diagnostic samples the live table on both sides of the original
`001F1080` update, before the PC second-player pass. Every live record must
point to its corresponding `LevelDat` descriptor, with the same running
scene and table at both boundaries. Only aggregate counts are retained:
verified/rejected frames, maximum matched records, changes, counter advances
and entries reaching raw state 3. The update window does not cover every
objective callback and cannot attribute a change to a particular input.
The lab also observes the original `001BA430` objective state writer. Its
entry ESI must be an exact row in the verified live table on the frame
thread. The original runs once, and only a before/after change of that
same row is counted. Absent, unmapped, replaced, or foreign-thread rows
do not count as a verified transition. These observations remain separate
from door or collision state.

`summary.json` reports the exact fail-closed category and counters. A build failure
is reported by stage (`runtime_dependency_checkout_failed`, `dsp_dependency_checkout_failed`,
`original_code_generation_failed`,
`engine_build_failed`, etc.). Compiler/guest output is privately drained and never
printed or retained. Configure/build failures retain only fixed allowlisted
error categories and up to sixteen numeric MSVC/linker/Rust error codes. A small
allowlist of public host/lab source basenames may include a line number; generated
game-source names and all diagnostic message text/private paths are discarded. This trades detailed diagnostics for keeping original-code
snippets and guest addresses out of public CI logs. The native frontend update also observes only the original bounded page-name
field at UI+0x3840, then exports a fixed integer category: 0 empty/unavailable,
1 initial title, 2 progressed title, 3 options, 4 difficulty, 5 profile/name/keyboard
prefix, 6 pause prefix, 7 other. Only the category, sample count and category
change count leave the runner, never raw page strings or addresses. This callback
is read-only and does not request a page or write menu input/state.

Fixed boot-progress booleans distinguish XBE load, the guest entry call and first
USB poll. Fixed runtime markers additionally distinguish normal guest entry
return, guest-requested firmware return/shutdown, CPU renderer startup, USB
controller startup, audio-emulation startup and graphics/audio device failure.
Only booleans are retained, plus bounded elapsed runtime in milliseconds; neither
error text, guest addresses nor the firmware routine argument is exported. These
markers never satisfy gameplay acceptance. With USB polling observed but
no mission-running samples, `mission_not_reached` identifies the unverified
frontend navigation/startup path; earlier blockers are `original_boot_incomplete`
or `usb_input_not_polled`.

## Privacy and licenses

Only `safe-engine-lab/summary.json` is uploaded, using an explicit file path.
No game bytes, XBE, assembly, generated C, binaries, saves, screenshots, memory,
crash dumps or raw logs are artifacts. Child stdout/stderr is drained privately;
no generated-source compiler errors reach the job log. Temp aggregate files are
removed; the workflow also removes the disposable engine and extracted owner
inputs. No caching is configured. Do not add broad artifact globs or raw log
printing to diagnose a failure.

The lab's own source is MIT licensed (see `LICENSE`). PC host code is
MIT; XboxRecomp's xemu-derived APU/Kelvin files remain LGPL-2.1-or-later, with
Unlicense/Mesa notices on interpreter components. DSP56300 is MIT. Preserve the
pinned projects' LICENSE/NOTICE files. Their licenses do not grant redistribution
rights to original BLACK data or generated game bodies.

## Local harness verification

```sh
python3 -m unittest discover -s coop/engine_lab -p 'test_*.py' -v
cc -std=c11 -Wall -Wextra -Werror coop/engine_lab/test_schedule.c -o /tmp/schedule-test
/tmp/schedule-test
cc -std=c11 -Wall -Wextra -Werror -Wno-unused-function coop/engine_lab/test_observer.c -lm -o /tmp/observer-test
/tmp/observer-test
```

These test the lab's filtering, patch drift rejection, schedule/report encoding,
and observer accounting under explicit stubs. They cannot establish original
engine gameplay. The actual job requires Windows x64/AVX2, owner assets, network
access, Visual Studio 2022/CMake/Python/Rust and substantial runner disk space.
The current ARM64 owner workspace cannot directly execute this Windows engine.

The first actual CI attempt, run `45102832832` at BO `a122d765`, passed harness
checks, extracted 574 owner files with the correct XBE hash, fetched the exact
dependencies and generated original code. It failed at CMake configure; the
public setup log identifies VS18/MSVC14.52 while the first lab requested the
VS17/2022 generator. The Ninja change addresses that observed toolchain mismatch.
A successful configure/build/mission run has not yet been established.

The later actual run `45113568182` at BO `44a5369` built the engine and exited
with code zero after calling guest entry, but reached neither the frontend UI
marker nor a USB pad poll. Its aggregate blocker was
`frontend_and_usb_input_not_polled`, with no mission/P2 observations. That is a
pre-frontend runtime failure, not evidence that A/START menu navigation failed.
The following fixed HAL/normal-return markers and elapsed duration narrow that
exit on the next real run; the already discarded runtime output cannot establish
retrospectively whether guest entry returned, firmware exit occurred, or graphics
initialization failed. No speculative game-state fix has been applied.

## Session103 real original firmware exit evidence

Real Windows engine run [45186010290](https://github.com/alwansan/BO/actions/runs/45186010290) verified owner XBE, extracted 574 files and built/executed the original PC engine, but `HalReturnToFirmware` exited after ~219 ms before a pad report, original menu tick, mission or P2. It FAILED gameplay acceptance despite process code zero. The private instrumented HAL now emits only five fixed mode marker categories for routine values 0/1/2/3/other, preserving ExitProcess and all privacy restrictions. A matching source-only test checks no unknown code or owner game log is forwarded. CI runtime is still required to establish which mode is seen. Do not suppress firmware exit or claim gameplay from this diagnostic.
