# Session93 — 2026-11-23 — private engine aggregate integrity

Branch: experiment/black-coop only.

## Actual changes
- e721c588f9aa87b9130f7d3f1f7a4f32511c542a — parse_aggregate now rejects repeated JSON keys via object_pairs_hook, rather than Python's silent last-value-wins behavior.
- 8a887bb686a083b69e0e68fc9e5882e83a5d2db2 — regression requires duplicate-key rejection even when both values agree and preserves normal exact-field parsing.
- Session documents and STATE updated. This guards integrity of original-engine lab evidence, not original gameplay.
- Session92 records original-engine CI 45102832832 generated original code but failed engine_configure; do not infer build or game runtime success. Follow-up runner evidence still required.

## Remaining P0
Host-owned original extra actor with safe lifecycle, independent physics/weapon, AI targeting, authoritative enemy/door/objective state and guest native application. Two phones loading the same original stage are still independent worlds. No minor phone test; no playable co-op APK link.

## Boundaries
No writes to main, CL, biso, XBE, ISO or saves. No owner-private data published.
