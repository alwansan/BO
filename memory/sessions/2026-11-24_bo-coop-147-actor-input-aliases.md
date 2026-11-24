# Session147 — 2026-11-24

### Session147 (2026-11-24): original actor input alias isolation

- Code 1afe0e3aa8efa0654f4c789b159a75e10f2785cb adds source-reviewed exact donor input/pad alias repair from pinned PC black_p2_update inside OriginalPlayerLinkExchangePlan. Extra human input references are changed only in unprotected active extra actor words; inactive P1 remains untouched and restored. Protected world-owned alias conflict refuses entry before writes. Original human tick now uses separate EnterOwnedOriginalHumanContext, validates owned pad and donor input lifetime before actor exchange; original setup path is unchanged. No bot AI changes.
- Fixture covers donor aliases, unchanged unrelated data, protected world-link rejection with no writes, and round-trip P1 restoration plus retained P2 aliases. Existing human update mock adapts to distinct human entry. New CI results unverified; no production activation or actual Android gameplay claimed.
- Continue parallel work on original allocation lifetime, audio/weapon/presentation construction, physics and soldier bodies, original guest canonical world apply. Keep original enemy/ally AI and targeting untouched, no additional world step. No phone microtests. Only experiment/black-coop writable; preserve main, CL, biso, original XBE/ISO/saves/private logs.

