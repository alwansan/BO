# Private original audio-object witness

This extends the existing disposable original-engine lab. It is not an audio
implementation or an initialized P2 bank. Source contracts are in
`docs/ORIGINAL_HOST_AUDIO_BANK_LIFECYCLE_AR.md`.

The main `lab.py execute` now integrates this automatically **after original
code generation and before compilation**. For a manually prepared disposable
checkout, run:

```sh
python coop/engine_lab/audio_lab.py /path/to/disposable/pinned/pc
```

Then use the existing private lab build/run. Set `BO_LAB_AUDIO_AGGREGATE` to a
private temporary JSON path before launching. The integration leaves the original input schedule unchanged, sets the private
path automatically, and reads optional audio counters through
`audio_lab.aggregate()` into the existing safe `summary.json`. Missing audio
output adds no evidence; oversized, malformed or duplicate-key output fails
closed. The workflow runs both sanitized C calibrations before any private build.
Do not copy raw logs, generated bodies, guest memory or game assets out.

The patcher verifies the PC source pin and previous disposable lab preparation,
requires exactly one definition for each real original body `7EF50`/`7EDB0`,
renames those definitions and wraps all direct and indirect calls. Each wrapper
executes the original body exactly once. It does not create or release audio
resources itself, change a guest register, or write guest memory. The existing
PC experiment may create its normal P2 resources; this witness only watches.

The bounded 128-entry internal identity table records only caller-provided
embedded objects of type `2C2C38`. The publication walker follows the real
`81030` list topology, with at most 256 publication lists and 4096 entries.
Neither object addresses, native thread IDs nor names appear in output.

`after_take_checks` examines the PC take operation **after** it has restored
the original FPV bytes. `published_identity_changes` counts published objects
whose type/engine/data/embedded-storage bit now differs from creation. This is
an observation requiring investigation, not an automatic corruption verdict.
Other valid streaming callbacks may move data as well.

`foreign_thread_observations` means calls arrived on a thread different from
the first observed constructor/release thread. `overlapping_call_observations`
also includes same-thread reentrancy; it is not by itself evidence of parallel
execution. Neither a zero nor nonzero count proves Android thread exclusion.
Android's pinned SDK can run queued guest stream callbacks from its fallback
dsound host thread, outside the private voice mutex.

If no original audio calls were observed, if the mission/P2 was not reached,
or if any reads were invalid/dropped, the result does not establish cleanup.
Clean release requires observed original releases, no surviving publication
references afterward, and an independent successful original-engine run. A
full comparison against the Android SDK is still required before live writes.

Local calibration (not original-engine proof):

```sh
python -m unittest discover -s coop/engine_lab -p test_audio_lab.py -v
cc -std=c11 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined \
  coop/engine_lab/test_audio_publication.c -o /tmp/audio-publication
/tmp/audio-publication
mkdir -p /tmp/bo-audio-lab-include
cp coop/engine_lab/audio_publication.h /tmp/bo-audio-lab-include/bo_audio_publication.h
cc -std=c11 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined \
  -I/tmp/bo-audio-lab-include coop/engine_lab/test_audio_observer.c -o /tmp/audio-observer
/tmp/audio-observer
```

Integration verification also constructs a temporary source tree to exercise the
real preparation mutations: production output is `src/recomp/gen`, not a root
`recomp/gen` directory. Only each exact original function definition is renamed;
call sites and dispatch table references keep the wrapper names, preserving
direct and indirect interception. The fixture is not initialized game audio.
