# BO-COOP Session77 — ORIGINAL menu start entry-rooted control-flow candidates

**Date:** 2026-11-22. **Agent:** BO-COOP. **Write only** alwansan/BO:experiment/black-coop. Never touch main/CL/biso/owner private XBE or ISO, app saves.

## Source-backed work
- The prior stable project has verified room-only Android APK run 45042108516 (artifact 14762361067), 4-human real TLS test with simulated original native source 45042108523, and new original body pose *value-only* ASAN workflow 45053142029. Still not playable campaign.
- Pinned source RanggaCasper/Black-Android@f3754d6985: src/game/difficulty.cpp describes original sub_00124CF0 as the mission-start attempt on selected difficulty. Previous linear static private scan had a candidate direct CALL from 0x00124CF0 to 0x0011C3C0, and two candidate direct children of the latter 0x0011BC00 and 0x001B6EB0; no ABI or actual callback proven.
- **New code commit ffdb6d7a7b83356c6b299662f64c4b43841d5b4b** adds \`coop/analysis/original_guest_launch_cfg.py\`, \`test_original_guest_launch_cfg.py\`, \`.github/workflows/black-coop-original-guest-launch-cfg.yml\`. Verifies exact original c5 SHA1, x86 executable window identity, decodes only bounded 1024 bytes per known function, follows local direct conditional/unconditional branches and fallthrough from the actual candidate entry; cannot execute a game function or modify game RAM. Exports only aggregate counts/expected known-edge booleans. Synthetic fixture deliberately places a reachable direct CALL after an earlier linear RET on a separate path and an unreachable decoy CALL after RET.
- **CI #45054097462 SUCCESS** — synthetic job 137305394403 SUCCESS; pinned owner-private job 137305429033 SUCCESS. Logs: original menu launch source entry rooted instruction candidates=113, known reachable direct call candidates=1, unresolved edges=1. Direct callee rooted graph candidates=64, known child call candidates=2, unresolved=0.
- This reduces static uncertainty only; function-entry decoding/branches can be misleading, no indirect call completeness, no runtime ABI, no true Android auto launcher/loaded-scene callback/renderer/host synchronized original enemies/mission.

## Follow-up P0
1. Research actual original engine mission menu start ABI, selected level/stage binding, callsite register/stack and thread semantics using pinned source/owner XBE private strict checks. Never call function just from static hints.
2. Prove engine's authentic load-complete event; authenticated guest auto-start only with safe entry, exact latest host ticket, roster and abort fence. No fake READY.
3. Independent actual original BOT-look clone/pose/draw per remote human (host on guests), keeping local first person, no copying active NPC brain; then shared HOST world AI/damage/objectives.
4. Do not ask owner for tiny phone experiments. Only a future single substantial integrated 2–4-player story test with automatic bounded diagnostics.

## Handoff
Read this session, AGENTS, prompt/RESTORE_SESSION_AR.md, memory/STATE.json, memory/PLAN_AR.md, docs/CLOUD_HANDOFF_AR.md, latest Actions. Maintain updates continuously.
