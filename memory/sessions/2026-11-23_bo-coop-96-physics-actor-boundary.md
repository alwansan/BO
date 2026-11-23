### Session96 — 2026-11-23 — original physics pool actor ownership boundary
- Live run 45113568182 original-engine lab: harness SUCCESS, original-engine FAILED gameplay acceptance; aggregate engine_built=true, engine_exit_success=true, owner files 574, blocker frontend_and_usb_input_not_polled, mission and P2 still absent. Run 45113568213 isolated UDP SUCCESS; no new co-op APK. Do not treat successful engine build or TLS as shared-world gameplay.
- Code: reject fake kind-2 player actor headers overlapping the live original physics allocator 0x2974, including interior overlaps; add negative regression ensuring no original DF400 call. This is a defensive prerequisite for future independently owned actor2, NOT actor creation or gameplay.
- CL read-only host/guest latest probes 2026-11-23 05:13/05:14; independent worlds remain. Next: original boot/input unblock, construct original P2 independent storage+proxy+weapon, then host NPC/object authority and guest consumption. No phone microtest; only one major test when ready. Never change main/CL/biso/XBE/ISO/saves.

Scope: experiment/black-coop only. Tests pending CI verification; no claim of runtime player2. 
