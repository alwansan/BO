# Session94 — 2026-11-23 — scene-scoped two-device evidence

Branch: experiment/black-coop only.

## Source changes
- 4b0efa719728e4207defa15417a48a2d741c9569: original_shared_world_acceptance.py rejects contract events emitted before story commit and clears evidence after abort, disconnect, leave, or repeated scene commit.
- a6c94b94721a7cbbb8610528af8229f4ccffd2d9: tests assert precommit, aborted and restarted log sessions cannot pass. Existing all-contract fixture now orders commit before evidence.
- These are evidence validation changes, not runtime multiplayer integration. Current original BLACK still lacks host player2 original actor/weapon/physics, AI target, unified object/objective and guest application.

## CI / devices
- Previous engine lab run 45102832832 failed configuration per session93. Latest Actions jobs/APK and latest CL device files could not be independently inspected with current connector endpoints; no new run number or success claimed.
- No owner phone test requested. The next owner trial remains one large shared-campaign trial only after real gameplay integration.

## Next
Implement safely owned original player2 actor lifecycle and native physics/weapon/AI; authoritative guest scene consumers; then rerun CI. Do not modify main, CL, biso or private assets.
