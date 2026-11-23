# Session105 — BLACK private original-engine quick reboot provenance

Date: 2026-11-23. Source branch: `experiment/black-coop`; no writes to main, CL, biso, original ISO/XBE/saves or private data.

The real Windows original-engine lab [45193373405](https://github.com/alwansan/BO/actions/runs/45193373405) finished **FAILURE** with 574 owner files, verified original XBE, built/executed engine, `firmware_kind_2`, `firmware_return`, runtime 202ms, no USB poll, no frontend, no mission or P2. Pinned Xbox enum confirms kind2 is `HalQuickRebootRoutine` (not a native crash). It does NOT prove another XBE was intended or launched.

Implemented [476410fb1c025075f32c98171305f0f9b4e8b136](https://github.com/alwansan/BO/commit/476410fb1c025075f32c98171305f0f9b4e8b136): bounded allowed launch page hints from the original pinned private kernel log, no raw path/title id/guest words/stack. Captures presence/absence, type category, path-is-present, first word coarse category. Three regression tests include actual private reader. The first word is intentionally labeled without inferring semantic boot reason.

Prior source probe [45195066451](https://github.com/alwansan/BO/actions/runs/45195066451) SUCCESS for original P2 proxy ownership quarantine. Last observed room-only Android [45195066473](https://github.com/alwansan/BO/actions/runs/45195066473) SUCCESS, still explicitly NO GAMEPLAY CO-OP. New original-engine [45197279738](https://github.com/alwansan/BO/actions/runs/45197279738) queued/pending at initial check. Do not claim new original runtime classification verified until this run finishes.

Next P0: inspect original-engine new launch metadata; diagnose source-backed fast reboot cause; fix boot only with proof, then real host original actor2–4 full independent ownership (animation/camera/weapon/audio/physics/AI), authoritative shared NPC/objects/objective and guest original engine consumer; one substantial 2+ phone trial only after real integration. Update all restore/plan/state automatically; use CAS against fresh remote.
