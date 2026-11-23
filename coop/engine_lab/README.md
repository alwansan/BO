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
host sources, differing source pins and changed patch anchors. It changes only
host `src/recomp_manual.c`, adds the observer/scheduler beside it, and observes the native frontend wrapper in `src/black_native_menu.inc`, and changes
runtime `src/usb/usb_gamepad.c` in the disposable checkout. Two fixed markers
are also inserted before the existing HAL firmware-return/shutdown exits in
`src/kernel/kernel_hal.c`; their original log calls, parameters and `ExitProcess(0)`
behavior are preserved. No exit is suppressed or converted into gameplay. No original game bytes
or generated game-function bodies are patched. Generation follows the pinned
production script; compilation uses Ninja with the active x64 MSVC developer environment and
production CMake definitions, with two build jobs. This supports the runner
image's Visual Studio 2022 or 2026 toolchain without hardcoding an unavailable
Visual Studio year. The MSVC major and backend are included as safe metadata. Python uses capstone 5.0.7; Rust is 1.98.1.

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
