# Session129 — source freshness and CI regression

### Session129 (2026-11-24): fix CI freshness regression and replay-renewal bug

- Inspected HEAD 4afb5f06. Source CI 45266674268 and parallel original guest-world job in 45266674076 failed at canonical barrier test line 25: test expected expiry at t=1104, only 97 ms after last unique source frame t=1007 while contract allows 100 ms. Correct boundary test to 1108.
- Found additional real issue: repeated Observe of unchanged host frames renewed lastObservedAt_ even though stable quorum did not advance. Change timestamp renewal to only newer authenticated host world/NPC ticks. Add regression with replay at t=1050 and expiry at t=1108. Source-only, does not enable original Android actors, guest NPC/door writes, or multiplayer readiness.
- Android lobby APK 45266674118 SUCCESS, artifact 14863104326 named NO-GAMEPLAY-COOP; UDP 45266674179 SUCCESS. Prior original Windows P1/P2 mission 45239818055 SUCCESS only for PC. No Android shared mission verified. Recheck new CI after this single batched commit. No phone microtests; do not alter main/CL/biso/XBE/ISO/saves.

