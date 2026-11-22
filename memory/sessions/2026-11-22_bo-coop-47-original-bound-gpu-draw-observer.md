# BO-COOP Session47 — Bound real original D3D8 draw source observer
Date: 2026-11-22. Only writable `alwansan/BO:experiment/black-coop`. No owner mini phone tests. Read AGENTS.md, RESTORE, STATE, CLOUD_HANDOFF, this session and Session46.

## Real changes
- Start HEAD `fb7860e146af448cb660e2ee35d235f8facc8c76`; code `2ef9fd504107a19848783b5fd00c28908c373e50` adds safe pure `ClassifyBoundDraw` for original D3D8. Requires original parsed declaration stride36 **and** nonzero currently bound vertex buffer of stride36, plus a positive-count original DrawVertices or DrawIndexedVertices with backend. Packed declarations at offsets +12 and +20 must appear on the SAME active stream. Never log shader IDs, vertex data, actor pointers, gameplay names, or assets.
- Existing `gpu_decl_witness.hpp` now reads `gpu::State` only; codes 10 `GPU_V36_BOUND_INDEXED_DRAW`, 11 `GPU_V36_BOUND_ARRAY_DRAW`, 12 `GPU_V36_BOUND_SAME_STREAM_12_20`, each once. `prepare_room_network.py` inserts observations immediately before the original `g_backend->Draw` in staged `src/d3d/d3d8.cpp`; original rendering and game actor semantics unchanged. Only main array/indexed draw paths instrumented; DrawUP/push not covered. Same strict native file allowlist as Session46.
- `2fd7fe5bb29072d351e3ad705d3cc05bf7d38e0e` strengthens fail-closed source assertions, `ac156ea76315ea0738a790e4aab8ba3b6f78a75b` fixes introduced Python multiline string SyntaxError discovered in CI; no owner game files mutated.

## CI verified
- [C++ ASAN/UBSAN 44967390703](https://github.com/alwansan/BO/actions/runs/44967390703) **SUCCESS**.
- Initial source preflights `44967390711` and `44967416619` **FAILED** due the malformed Python source anchor, **not** engine compile; corrected.
- [Final source preflight 44967448413](https://github.com/alwansan/BO/actions/runs/44967448413) **SUCCESS**; [isolated UDP 44967448396](https://github.com/alwansan/BO/actions/runs/44967448396) **SUCCESS**.
- [Full Windows Android room-only run 44967448414](https://github.com/alwansan/BO/actions/runs/44967448414) **CHECK LIVE**, was in progress. Older 44966638815 CANCELLED via concurrency after later commits (not an APK compiler failure). Do NOT claim latest Android build success before result.

## Unfinished critical original game objectives
Even when runtime observes a bound D3D draw, this does NOT identify the owner's original human MODEL (BC1_MAT/TOM, etc.) using it, identify weights/indices or original RCB matrix convention, show an independently rendered remote ally, or confirm host-selected campaign scene. No c5 true game-thread stage-loaded hook/mission epoch, no authenticated gameplay snapshot/input/wire, host-world AI/combat/quest replication. These are required for real co-op. Never emit STORY_READY from shaders, world pointer xrefs or a local-player tick.
Next: verify 44967448414 and repair errors; associate actual MODEL resource to real bound D3D draw safely; independently verify original c5 host/guest scene-loaded game-thread epoch; then human-controlled remote fullbody ALLY, original local first-person untouched and host shared mission. One big two-phone owner acceptance only when meaningful gameplay integrated, automatically safe logs for all minor observations. No main/CL/ISO/saves/third-party proprietary assets changes.

Restore: AGENTS, prompt/RESTORE_SESSION_AR.md, STATE, this Session47, docs/ORIGINAL_GPU_DECL_SAFE_WITNESS_AR.md, Session46, docs/OWNER_VERTEX36_SKIN_RESEARCH_AR.md, docs/BLACK_WORLD_POINTER_XREF_AUDIT_AR.md and live HEAD/Actions.
