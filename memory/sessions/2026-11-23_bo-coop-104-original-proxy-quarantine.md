# Session104 — original HOST proxy partial-failure ownership quarantine

Date 2026-11-23. Original goal remains 2–4 humans inside one original BLACK mission, host-authoritative combat/world. No small owner phone tests.

- Base first verified: d5fda4f13f1b08ee168e272880500ab218d17047. Code commits 913aa472967751d85e73ab95f90baabf8cc89b71 + 1ae169ba5997a7b4cd04707051b27aa30b3df362. Source regression Actions [45195066451](https://github.com/alwansan/BO/actions/runs/45195066451) **SUCCESS** including original host proxy rollback test.
- Corrected unsafe retry when DF400/DBBE0 throws or leaves partial original engine state: per-adapter `uncertain_` quarantine before call, observed owner backlink, scene checks and strict rollback, explicit `RequiresEngineCleanup`. Tests verify no second native call after scope loss, bad backlink, thrown allocation or cleanup. Successful verified release clears quarantine.
- The guard is only PER INSTANCE. Reconstructing a C++ adapter could bypass it while original proxy is live. Real production must preserve adapter lifetime and add shared per-scene engine ownership ledger before actual P2 usage; document in `docs/ORIGINAL_HOST_PROXY_QUARANTINE_AR.md`.
- Last directly observed real original engine Windows lab [45193373405](https://github.com/alwansan/BO/actions/runs/45193373405) still running. Old [45186010290](https://github.com/alwansan/BO/actions/runs/45186010290) FAILED gameplay after 219ms firmware return with no pad/frontend. Android [45195066473](https://github.com/alwansan/BO/actions/runs/45195066473) still in progress, remains NO GAMEPLAY.
- P0 open: genuine original boot, P2-4 independent host actors, camera/controller/animation/weapon/audio, physics and AI, host canonical NPC/door/objective, guest original engine application and one major device test.
- Only experiment/black-coop writes; CAS protect parallel sessions. Do not alter or upload main, CL, biso, owner ISO/XBE/saves, raw logs or private game bytes.

