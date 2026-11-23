# BO-COOP Session122 — original post-frame identity and trusted input anti-rollback — 2026-11-23

## Live verification
- Branch at start: 38840eb76427a0306cbf2fabf221a2d38a1d8189. Scope: `experiment/black-coop` only; no concurrent head change allowed for CAS.
- Real original Android native APK build 45243553282: both `observer-core` and `candidate-probe` jobs completed SUCCESS. This is only original Android source compilation, APK assembly and signature; not a shared mission phone trial.
- Real original PC historical 45233723518: actual original P1 and P2 moved 4527/6363mm and fired 10 rounds each in the same stage; historical gate falsely rejected world-owned grid HEAD, kept FAIL. New runner 45239818055 original-engine still in progress at check; no fabricated PASS.

## Actual changed implementation (source)
1. On ORIGINAL post-1F1080 game-thread observer, read source-proven P1 vtable from actor +0 and require `0x28CFE0` as well as phase `0x1C`, actor kind2, committed core/owner/physics/NPC and 30 stable post-ticks. Reject cross-thread observe/Reset, retain protected owner state. Emit only existing fixed privacy-safe event 2130; DO NOT signal room READY, allocate a player, touch guest RAM or mutate stage.
2. In authenticated `OriginalHostHumanInbox`, block any lower scene epoch from rolling back a newer committed story epoch even with a larger sequence number and FIRE. Existing Reset on genuine room teardown remains usable. Native tests cover epoch43 / stale epoch42 and cross-thread post-frame.
3. CI source gate adds `-pthread` for the new postframe thread regression. Run source preflight and full native Android build after commit and check results honestly.

## Outstanding P0
The real Android host needs separately owned P2–P4 game actors with constructor `0x14380`, `0x1E0E40` staged setup, independently owned P1/P2 camera/animation/viewmodel/weapon/audio state, safe engine lifecycle/teardown, reversible world-grid-aware context, original DF400 collision proxy and AI awareness. TLS original pad ports 1–3 cannot become live before this. Guest phones must apply authoritative original host NPC, doors, objectives and mission state; no fake multiple solo worlds. Owner phone tests remain deferred until a substantial integrated gameplay milestone. Keep state/restore synchronized every session; preserve main/CL/biso, XBE/ISO/saves, all private logs.
