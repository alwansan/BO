# BO-COOP — Session46: pinned owner stride36 lane statistics and bounded synthetic AllyBody skin-math
**Date:** 2026-11-22. **Only writable branch:** `alwansan/BO:experiment/black-coop`. User said Continue. Continue CI-first without owner phone tests until a **large visible real co-op milestone**: host-chosen original story mission, second human visible as separate fullbody ALLY (not NPC), automatic safe logs, basic movement/combat/world proof. No file writes to owner BO/main, CL, ISO, saves, or owner game assets.

## Start / live status
- Session opening HEAD `0a450b1192182e9449728476c8f712861b74faaa`. Last Session45: original 3 `StLevel.bin` stages 12 body MODEL records using 36 stride vertex buffers; 9 serialized RCBs / 3 unique identity contexts, 22 RCB-linked BNKS, 675 clip occurrences and 61,615 track records (metadata only). Last successful room-only Android APK run `44962611024`, artifact14730766256; synthetic host input fence ASAN/UBSAN `44963681576 SUCCESS`. No real BLACK stage-loaded callback, no human remote fullbody animation, no gameplay transport or host world sync.
- Read `AGENTS.md`, RESTORE, Session45, `docs/OWNER_RCB_ANIMATION_LINK_AUDIT_AR.md`, STATE and scope constraints before changes. Only working branch experiment/black-coop.

## New ORIGINAL OWNER DATA experiment: 36B vertex lanes
- Created `coop/analysis/ally_vertex36_layout_probe.py` (first commit `612ca545`), `test_ally_vertex36_layout_probe.py` (`43d30a3a`) and `.github/workflows/black-coop-ally-vertex36-layout.yml` (`66da6763`). Source-only Python parser reads one of 12 fullbody MODEL records from original owned `biso@9ceb6a65` `level_00`, `level_07`, `level_08` StLevel.bin, read-only on CI. Samples at most 2048 *distinct referenced* vertices per buffer, validates bounds, classifies six post-position word lanes (+12,+16,+20,+24,+28,+32) via COUNT ONLY, never raw byte values, model transforms, bone IDs, shader code, vertex position, texture or original game resources. Fixed number of input file paths; no cross-repo writes.
- CI `44965770507` synthetic passed but owner-private failed from an unidentified ValueError (very likely initial 64KiB report size limit; cause wasn't recorded by first code). Corrected **max report budget to 256KiB** and added fixed non-sensitive reason labels, commit `35990b81`. **New [CI 44965837041](https://github.com/alwansan/BO/actions/runs/44965837041) SUCCESS** both synthetic and owner-private. Safe metadata artifact **14731657226** `bo-owner-vertex36-statistics-safe`, materialized as `/mnt/data/bo-owner-vertex36-lane-safe.zip` (verified in this session).
- Actual report: **12/12 candidates accepted, 0 layouts rejected**. Total **38,917 sampled indexed vertex buffer entries** (NOT 38,917 unique whole-game vertices), at each of six offsets. Aggregate lane summary:

| Offset in stride36 | Four-byte sum≈255 | All u8 less than 25 (model bone count) | f32 bits interpreted in [0,1] |
|---|---:|---:|---:|
| +12 | 83.63% | 0% | 95.65% |
| +16 | 0% | 17.68% | 100% |
| +20 | 0% | 90.85% | 100% |
| +24 | 0.25% | 0.03% | 26.06% |
| +28 | 0.39% | 0% | 100% |
| +32 | 0.22% | 0% | 0.01% |

- In first mission level_00, SOL 2758 sampled (+12 sum255 80.5%; +20 four bytes each <25 85.2%), LRG 3061 (81.3%,78.9%), BC1_MAT 3380 (88%,88.4%), BC1_TOM 3402 (87.1%,88.3%). All candidate MODEL buffers 36 bytes/vertex previously proven. This **narrows where to investigate packed weights/indices**, BUT **DO NOT assert** +12 is weight or +20 is bone index or +28,+32 are UV. Raw nonfloat words may misleadingly look like tiny positive floats; requires actual title vertex shader declaration and backend interpretation before deciding. Detailed `docs/OWNER_VERTEX36_SKIN_RESEARCH_AR.md`.
- Read pinned original source `RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806` `src/d3d/d3d8.cpp::D3DDevice_CreateVertexShader` actual call to `gpu::ParseDeclaration(*vs)`; verified SDK renamed repo `TGP482/XboxXBERecomp` `runtime/gpu/vertex.cpp` supports D3DCOLOR 0x40, PBYTE4 0x44, packed normal 0x16, FLOAT2 0x22 and FLOAT3 0x32. These are **generic format definitions**, not evidence BLACK body shader actually binds those to these offsets. Next safe run-time witness should capture *fixed categorical* declaration/offset codes into existing bounded diagnostics only when a future major physical test occurs; do not print shaders/vertex bytes or request separate phone test.

## Added bounded SYNTHETIC ONLY CPU pose/skin reference
- `coop/bridge/synthetic_ally_skin_contract.hpp`, `test_synthetic_ally_skin_contract.cpp`. Custom row-major mathematical convention, NOT BLACK RCB/MODEL matrix semantics. `SyntheticRig` validates up to 64 acyclic parent joint indices (including child before parent), finite local/inverse bind matrices, synthetic parent composition, and up to four normalized float influence weights and indices, rejects duplicates/NaNs/out-of-bounds. No actual game actors or guest memory, rendering, shader or original data.
- Extended `.github/workflows/black-coop-story-contracts.yml` to compile with `-std=c++20 -Wall -Wextra -Werror -pedantic` under ASAN+UBSAN. [**Run 44966105790 SUCCESS**](https://github.com/alwansan/BO/actions/runs/44966105790) emitted `SYNTHETIC_ALLY_POSE_SKIN_CONTRACT_PASS`, along with previous `HOST_HUMAN_INPUT_GATE_PASS`. This is foundational **synthetic reference only**, NOT a decoded original model or moving avatar.

## State and critical missing work
- **No** real BLACK fullbody remote human AllyBody renderer, no identified skin attributes in owner's MODEL, no original animation curve evaluator/posing, no material shader mapping. Reference CPU rig uses synthetic matrices and cannot be fed original RCB data without interpretation checks.
- **No** real c5 BLACK game-thread scene loaded callback, mission/stage/epoch; 0x00124CF0 is launch candidate only and local player tick not true scene loaded. **Never issue STORY_READY** from these hints; only verified actual game loader callback may drive story barrier. No shared world/AI/quest/combat replication, no authenticated live input stream.
- **No owner device test** requested; Actions runs were fully automatic.
- Newly detailed documentation: `docs/OWNER_VERTEX36_SKIN_RESEARCH_AR.md`.

## Next high-value engineering
1. Prove actual game's GPU vertex declarations for stride36 and bind to actual character MODEL instance. From original pinned `src/d3d/d3d8.cpp` after `ParseDeclaration` read-only, **bounded fixed code** telemetry could be added to room-only staged source and built in CI, without requiring phone trial for isolated little diagnostics. Do not publish raw shader or game data. Combine all with future large two-phone session.
2. Only after this, decode true original vertex blend weight/index offsets and RCB bind-pose semantics, test synthetic->real cross-validation without guessing 4B patterns. Then isolated human AllyBody renderer, not NPC AI, preserve local original FPS.
3. In parallel original c5 XBE scene/stage loaded game-thread callback/epoch verification; no fake READY; then live authenticated input/snapshot and host-authoritative world/AI/combat/objectives.
4. Update RESTORE/STATE/MEMORY/AGENTS/CLOUD_HANDOFF on each Continue, avoid needless phone installs. One major meaningful phone test only when two actual humans share story and see one another.

## Restoration order
`AGENTS.md`, `prompt/RESTORE_SESSION_AR.md`, `docs/CLOUD_HANDOFF_AR.md`, `memory/STATE.json`, **Session46**, Session45/44, `docs/OWNER_VERTEX36_SKIN_RESEARCH_AR.md`, `docs/OWNER_RCB_ANIMATION_LINK_AUDIT_AR.md`, `docs/BLACK_WORLD_POINTER_XREF_AUDIT_AR.md`, `docs/MAJOR_DEVICE_TEST_GATE_AR.md`. Verify live HEAD/Actions before modifying.
