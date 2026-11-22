### Session67 original owner XBE visual/animation/pose function private audit
- Added `coop/analysis/original_human_look_xbe.py`, synthetic positive/negative tests, and GitHub Action `black-coop-original-human-look-xbe.yml`. Reuses pinned owner c5 XBE read-only within the runner; inspects only 768 private bytes each at PC MIT candidate addresses `0x0017E420`, `0x0017E130`, `0x001801B0`. The workflow checks executable section mapping and bounded decode/field-reference count, never exports raw code, disassembly, guest pointers or owner private files. NO function boundaries, return/register ABI, safe guest call, engine-created remote character or drawable bot is asserted.
- Its next meaningful outcome is whether the 3 PC-source candidate offsets exist in this exact owner XBE; on success it narrows next *dynamic* ABI validation, not a safe permission to call these functions. Maintain one major owner test only when shared story works.

---

### CI fix for session67
[Source preflight 45032641407 FAILED](https://github.com/alwansan/BO/actions/runs/45032641407) at native C++ compile only: `room_original_bot_pool_contract.hpp:113` used unqualified `size_t` but did not explicitly include `<cstddef>`. Python stage/instrumentation source checks and earlier unit tests passed before that compile. Add the missing C++ standard header, push and require rerun; never claim full PASS before result.

---

## Session67 (2026-11-22) — concrete 1–3 remote-human ORIGINAL bot LOOK planning with live TLS host roster
- Staged a **read-only** original game-thread `OriginalHumanLookPlanner`: select a real source bot's verified visual template (enemy or ally), then propose 1–3 distinct blank visual rigs (indices from 31 down), strictly matching compact authenticated HOST TLS roster slots 2,3,4. Never use raw long-lived RoomWire.Player.id as the player slot and never borrow NPC-controlled slots. No model pointer/slot index logged.
- Plans are scoped to the original stream/source-matched CORE episode + world and exact room ordinal roster; all candidate slots reverified every 2s from original committed guest pages; refuse fake masks, missing visual buffers/source. If NPC reclaims a slot, original source disappears or roster changes, **latch invalidation until a new real original level launch**. NO guest writes, CallGuest, spawning, rendering, AI, physics, health, weapon or STORY_READY: room-only code, NOT playable coop.
- Native game-thread JNI gets host-verified active roster (only ordinal bitmask) via `BlackActivity.coopHostAppearancePeerMaskFromGameThread` → `RoomAppRooms` → `RoomSession`, which validates original game thread, HOSTING, authenticated Server roster, and rejects missing peer count. Source-match `coreStageIdentity.Matched()` required before proposing a look.
- Native C++ tests now cover 1/2/3 actual peer plan, enemy donor, NPC controller exclusion, pool slots, roster change and NPC takeover latch, mismatched mask; staged source + Java8 unit CI and Windows Android APK allowlist use ONLY existing installed header names.
- Preserve owner one-major-phone-test preference; only ask once real host story start, separate remote-human bot bodies, shared AI/combat/objectives can be accepted on original game. Never confuse this proposal with actor creation. This is the next major P0 interface toward an actor renderer/engine guest ABI, which remains unverified.
