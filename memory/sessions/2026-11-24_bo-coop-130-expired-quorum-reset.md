# Session130

### Session130 (2026-11-24): expire canonical quorum on source timeout

- HEAD ff084b53: source CI 45276818053, parallel CI 45276818179, UDP 45276818177 and Android lobby 45276818213 all SUCCESS. Artifact 14868487706 is NO-GAMEPLAY-COOP, not a shared original game APK.
- Found source freshness gap: SourceCoherent(now) expired after 100ms, but stable_ remained 3, allowing a single later host tick to restore readiness. Observe now resets quorum after a >100ms gap or clock rollback; added regression for 1/2/3 fresh post-timeout ticks.
- Android original P2-P4 actor creation, physics, weapons, NPC/door/mission application and automatic START are still unverified. No phone microtests; no changes to main, CL, biso, XBE/ISO or saves.

