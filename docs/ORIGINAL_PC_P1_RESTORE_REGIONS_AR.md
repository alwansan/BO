# BLACK original P1 context restoration investigation — Session112

### Session112 — 2026-11-23 — categorize original PC P1 restoration failures without exposing game data

- [Private original PC engine 45213250944](https://github.com/alwansan/BO/actions/runs/45213250944) verified virtual HDD created/written/read, original frontend and 5245/5183 original USB pad reports. Multiplayer acceptance FAILED: 167 original mission-loop ticks (required 720), 161 restore failures, P2 ready/proxy=0, phase 0, ~300 seconds CPU rendering. Do not claim accepted original mission or any Android phone-storage issue.
- New `coop/engine_lab/observer.inc` observes P1 restoration discrepancies as **six aggregate numeric buckets only**: original scene/core/owner/P2-in scope, actor 0x000-0x07F grid, 0x080-0x17F motion, 0x180-0x2FF weapons, 0x300-0x3FF model and 0x400-0x8FF tail. No original guest bytes, addresses or strings are logged. Strict `evaluate()` acceptance is unchanged; original game RAM is read-only for this diagnostic. Added a native C synthetic failure fixture and Python privacy/contract assertions. A green harness is not proof of P1 restoration.
- Session111 source preflight [45220093377](https://github.com/alwansan/BO/actions/runs/45220093377) SUCCESS and UDP [45220093294](https://github.com/alwansan/BO/actions/runs/45220093294) SUCCESS. Full Android candidate [45220093146](https://github.com/alwansan/BO/actions/runs/45220093146) still compiling at check. Session112 edits PC engine-lab source and documentation only, not Android source, so it avoids superseding the long Android run.
- Next: read new *real* private PC restore-region counters; source-verify isolated 0x200 model workspace for P2 without borrowing P1; implement native Android original P2–4 full context/weapon/audio/animation and physical proxy/AI, host-owned NPC/doors/objectives and guest original-engine apply. No playable co-op or small owner phone trials. Write only `experiment/black-coop`; protect main, CL, biso, owner XBE/ISO/saves and raw logs.


## Test interpretation

The real original Windows engine restored the virtual-HDD boot path and advanced to original mission loop samples. The 161/162 P1 context restore failures may arise from incorrect P2 global context swaps, borrowed state, or a strict full-actor snapshot which includes legitimate per-pass changes. Neither possibility is proven by the old counters.

The read-only observer now counts failures in independent coarse regions. A frame can count in multiple regions; scope failures do not inspect potentially unrelated actor memory. No owner game data values, addresses, filenames, logs or generated original game source are uploaded. No acceptance threshold is relaxed. Compare fresh real engine aggregate, then investigate specific source mappings before changing original runtime.

Android TGP482/Xbox SDK is distinct and still has no active original P2 game actor. Keep a single substantive future phone trial only after host world and guest apply are genuinely integrated.
