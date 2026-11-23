# BO-COOP Session113 — 2026-11-23

### Session113 — 2026-11-23 — detached original player2 first-person model source isolation (NO guest writes)

- Verified source reference: `portingpete/Black-2006-Recomp@b2512fafd3` defines `BLACK_P2_MODEL_BYTES=0x200`; `black_p2_swap` copies original actor+0x320 model into independently owned P2 `parts+0x430` when it still aliases the player1 model. Existing BO P2 shadow prepared only 0x430 parts bytes and left actor+0x320 pointing at the donor model; that is insufficient for a model-isolated candidate.
- Updated `OriginalHostPlayerShadowPlan` to prepare 0x630 total detached parts bytes, separately copy the source model's verified 0x200 bytes into +0x430 (when present), and rewrite only the DETACHED shadow actor+0x320 to its own independently tracked `t.parts+0x430`. `ModelIsolated()` explicitly distinguishes this extra prerequisite from general `Ready()`. Invalid/unowned-overlapping source model pointer, failed copy or scene/model/part source change causes fail-closed reset; absent model remains a partial preview (NOT model-ready). All bytes stay in C++ local snapshots; **zero writes to original guest RAM and no in-game P2 actor/model/physics/AI.**
- Native tests cover full first/last model words, unchanged P1, model source failures, invalid pointers, target overlaps, stage allocation/release and no stale data after Reset. Must verify new source preflight/Android SDK compile and Android APK before marking passed.
- [Session112 PC diagnostic 45222127349](https://github.com/alwansan/BO/actions/runs/45222127349) harness SUCCESS, original-engine still running at check; diagnostics for 161 previously observed P1 restore failures are read-only. [Prior Android 45220093146](https://github.com/alwansan/BO/actions/runs/45220093146) SUCCESS native compile and packaging, but **NO GAMEPLAY CO-OP**.
- NEXT: source-verify complete independent animation/audio/weapon state and actual safe native Android game-thread materialization/teardown before original player setup; then physics/AI and host NPC/doors/objectives -> guest original engine apply. Do not request small owner phone tests. Only branch `experiment/black-coop` may change; preserve main/CL/biso and owner XBE/ISO/saves/raw logs.


## Exact source relationship

Pinned MIT PC `black_p2_start` copies P1 actor and split parts while `black_p2_swap` has an additional `0x200` model-copy safeguard if P1 and P2 model pointers still alias. Stage113 now prepares that 0x200 segment in a detached C++ array and sets a separate `ModelIsolated` flag. All target allocations are tracked, 16-byte aligned, disjoint from P1/core/physics/NPC, and the donor source and original scene pointers are rechecked. The shadow still does NOT materialize anything into original RAM.

Complete original actor/weapon/animation/audio lifecycle plus in-game context swap and cleanup is still missing. A model pointer rebased in detached storage is **not** a rendered soldier or a true network co-op player. The host-owned canonical original mission and guest application are separately unimplemented.

Next substantial action: verified native game-thread original setup transactional gate, full bank/animation swap + cleanup witness, then physics/AI, original world authority, one meaningful 2+ phone trial when actually working.
