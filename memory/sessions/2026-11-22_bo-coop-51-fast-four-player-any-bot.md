# Session51 — owner shifts BO-COOP to playable shared story ASAP, ANY BOT LOOK
Date 2026-11-22. New user directive: temporary remote player2/3/4 can use ANY original BLACK bot body (enemy OR ally, same model okay), postpone exact ally search, rig/animation, friendly reticle and cosmetics until humans can join the **same host-launched original story** and see/control distinct remote humans. Preserve original local FPS. Do not borrow NPC AI or shallow-copy live NPC actors.
## Work actually pushed
- `086e8b92` RoomWire version4 + bounded `RoomHumanInputWire`, test/staging.
- `c34ee733` host TLS receives authenticated per-peer inputs only after story COMMIT, 250ms freshness, rate/replay/epoch guard, bounded queue & `RoomSession` game-thread send/drain.
- `2c60cbee`, `97b3333c`: real Java TLS integration host + 3 independent guest inputs with synthetic stage attestations; source [44971688698 SUCCESS](https://github.com/alwansan/BO/actions/runs/44971688698).
- `c528b20c`: `TemporaryBotVisualPolicy`, arbitrary verified source bot mesh permitted for remote human, `ownsAiBrain=false`, sanitized C++ story contract [44971624673 SUCCESS](https://github.com/alwansan/BO/actions/runs/44971624673). Corrected APK wire metadata v4/profile4.
- `72d74cd7`: human-input safe diagnostics only first input per story, not every movement frame.
## Still NOT done
No real original BLACK scene-loaded callback/mission selected + guest stage launch, game-input JNI/game-thread sampling, actual independently rendered remote actor body, host-authoritative gameplay/AI/quest replication. Source CI uses synthetic/mock scene-loaded; not proof of phone co-op. Full Android room-only run44971762426 CHECK LIVE, cannot ask owner to install lobby-only.
## Next source work
Top P0 engine real mission loader/scene epoch, guest start same mission, actual movement/snapshot + ANY bot visual. Once usable, one comprehensive two-phone trial with automatic safe logs. Owner only writes BO experiment/black-coop, no BO main/CL/biso/ISO/saves. Restore from AGENTS, RESTORE, STATE, this session, docs/FAST_SHARED_STORY_TRIAL_AR.md; update continuously.
