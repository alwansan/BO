# BO-COOP — Session43 | World writer control-flow and source-only original game witness
Date: 2026-11-22. Authorized write target: **alwansan/BO:experiment/black-coop ONLY**. Never overwrite main or CL/ISO, don't touch user saves.

## Owner intent / device policy
User said Continue from Session42. Prior binding instruction: rely on GitHub Actions/JVM/Android emulator/source analysis; **do not request phone trials for tiny issues**. Reserve a single substantial two-phone owner validation until the game can start a real host-selected shared campaign with human-controlled remote full-body ally representation. At that time capture all small diagnostics automatically as fixed safe codes, not raw private data.

## Initial check
Branch HEAD `d4d989103de74c0d5ac35806d9d3eb4ae8083582` at opening. Room-only Android APK `44961110208` subsequently **SUCCESS** at source commit `367052097`, but this is lobby/TLS only and still NOT real BLACK gameplay. Android29 TLS `44961110132 SUCCESS`, source preflight `44961110163 SUCCESS`, JVM TLS `44961110123 SUCCESS`.

## Major findings: the ORIGINAL c5 XBE writer paths
- Added `coop/analysis/world_writer_flow.py` commit `19fdcdea`, `test_world_writer_flow.py` `69ab864c`, wired into the pinned owner's original XBE private workflow via `52b95155`.
- **GitHub Actions `44961768279 SUCCESS`**, synthetic markers `BO_WORLD_WRITER_FLOW_SYNTHETIC_PASS`, private original XBE markers `BO_WORLD_WRITER_FLOW_OWNER_PASS`. Only sanitized artifact **14729429674** `bo-world-xrefs-safe-summary`, no XBE or code bytes/disassembly exported. Original XBE SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f` matched.
- The two exact `kWorld=0x002D45D4` store candidates are only 9 bytes apart. `0x0020BF48` writes from **EAX**, `0x0020BF51` from **EBX**, both in same linear executable section. One nearby branch `0x0020BF3F → 0x0020BF51` targets the second writer directly; another `0x0020BF4F → 0x0020BF57` bypasses it. 17 total direct branch targets into bounded local window; 0 direct CALL targets in that window (not proof none exist, indirect edges/real function entry unknown). 12 instructions before second writer include a possible EBX zero setter, BUT branch path may bypass preceding code, so no assertion EBX=0 on all paths. First writer EAX value from call not established. These may be assign/reset paths but **function shape, ABI, scene epoch, mission ID and game-thread identity remain UNPROVEN**.
- Detailed no-overclaim evidence saved in `docs/BLACK_WORLD_POINTER_XREF_AUDIT_AR.md`. Do not inject a hook at those two locations until validated.

## Implemented actual source-only game-event observers (NO co-op gameplay yet)
- New `coop/room_network/world_tick_contract.hpp` (atomic value-only fixed-code witness) and `room_world_tick.hpp` (JNI code bridge called only on first/changed opaque world pointer), with `test_world_tick_contract.cpp` and workflow `.github/workflows/black-coop-world-tick-contract.yml`.
- In **disposable source staging only** `coop/prepare_room_network.py` now extends the original `src/game/cheats.cpp::sub_001E5FA0` *local player update* wrapper and `src/game/difficulty.cpp::sub_00124CF0` *mission-launch attempt* wrapper. Both original guest functions are still called normally; no game pointer writes, save mutation, NPC AI takeover or remote player creation.
- `BlackActivity.coopRecordWorldTick(int)` delivers only three fixed codes to bounded async RoomDiagnostics: `WORLD_MISSION_LAUNCH_ATTEMPT` (max 8 times and NOT loaded), `WORLD_LOCAL_PLAYER_UPDATE_SEEN` (original first-person player tick observed at nonzero world), `WORLD_POINTER_CHANGED_UNVERIFIED` (a later tick uses different opaque pointer; NOT scene epoch). No IPs, names, guest addresses, raw coordinates, XBE bytes, stage values or PINs exported. No automatic `RoomSession.reportSceneLoadedFromGameThread` or `STORY_READY` from these events.
- Existing source-only world witness CI `44962030766 SUCCESS`, updated contract run `44962215874 SUCCESS`; source staging `44962231552 SUCCESS`, newer `44962275672 SUCCESS`. Native scene wrapper code itself needs real complete APK build verification, and actual device callback is UNTESTED.
- `RoomSession.java` comment corrected at `d4dfc962`: guest scene report returns **READY queued**, not sent/received. No live BLACK engine hooked to it.
- Latest Android APK workflow `44962275637` still **in_progress** on current last inspection (build source d4dfc962); inspect live and repair if failed. It is still a title TLS room-only APK with source-observation bridge, **not full co-op**. Do not request owner install.

## What remains
1. Verify complete Android APK build, fix any Java/C++/JNI compile errors without asking for phone testing. Repeat source checks and sanitizers.
2. Deeper proven engine dataflow: EAX construction path before writer `0x20BF48` and EBX status on all branch arrivals to `0x20BF51`. Locate actual authoritative campaign mission ID, stage load completion, scene lifetime and safe game-thread hook. Avoid presuming mere local player update signals full load.
3. Connect only a verified *actual* scene loaded callback to `RoomSession` story PREPARE/READY/COMMIT; do not start independent solo world on guest.
4. Create actual human-owned full-body AllyBody renderer for other players on each phone; retain local original 1st person hands+weapon; then synchronize input, movement, weapons, AI, damage and objectives in host authoritative world.
5. Update AGENTS, STATE, PLAN, ISSUES, RESTORE, CLOUD_HANDOFF each session; owner one large physical test only once meaningful shared-world/human actor milestone ready.

## Restoration
Read `AGENTS.md`, `prompt/RESTORE_SESSION_AR.md`, `docs/CLOUD_HANDOFF_AR.md`, `memory/STATE.json`, this Session43, Session42/41, `docs/BLACK_WORLD_POINTER_XREF_AUDIT_AR.md`, `docs/BLACK_STORY_ENTRY_AUDIT_AR.md`, `docs/MAJOR_DEVICE_TEST_GATE_AR.md`. Verify current BO HEAD and live Actions; only write experiment/black-coop.

## APK CI packaging allowlist follow-up
- [Run44962275637](https://github.com/alwansan/BO/actions/runs/44962275637) failed during **Unit tests and prepare isolated app packaging**, NOT Android/JNI C++ compilation. All 19 staging unit tests PASS, but the Windows packaging workflow's old strict native/gameplay file allowlist omitted the newly intentionally staged, read-only `src/game/difficulty.cpp`; its added `src/coop/room_world_tick.hpp` and `src/coop/world_tick_contract.hpp` were also absent from the strict additional-file whitelist. Exact reported actual diff: cheats/hooks/options/exit/**difficulty**.
- Commit **561eacc0362ec2a1a730e7d85f68511e0c8d5b82** updated ONLY those strict explicit allowlists in `.github/workflows/black-coop-room-network-apk.yml`, retaining fail-closed rejection for any other native modifications, player/actor write or unknown file. New room-only candidate [run44962611024](https://github.com/alwansan/BO/actions/runs/44962611024) was `in_progress` on latest check; verify final result and inspect compiler errors. No phone trial.
- User's major test gate `docs/MAJOR_DEVICE_TEST_GATE_AR.md` now records the three fixed-code native witness events as automatic evidence and explicitly forbids treating them as real scene loaded.
