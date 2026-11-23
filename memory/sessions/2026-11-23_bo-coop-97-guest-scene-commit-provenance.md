# BO-Coop Session 97 — 2026-11-23

## Code actually published
- `6cdd624cc757a0b5e2d8aa8e84f364be93aaf71b`: require `STORY_GUEST_COMMITTED` specifically in guest event stream. Previously guest `STORY_HOST_COMMITTED` was treated as substitute when guest commit was absent; a forged/relayed host event plus guest contracts could pass the log preflight.
- `551289eb72783d52c102c858f5f2d92fa652f9cd`: regression rejects complete host contracts and complete guest contracts with only host commit in guest stream.

## Evidence and verification limits
- Read branch AGENTS, restore, state, plan, memory, handoff and session96 context. Direct live Actions/CL artifact API retrieval not available; previous known queued runs: source 45131600544, UDP 45131600475, Android 45131600446; no fresh result verified. GitHub connector confirmed published commit SHAs but did not execute tests. Earlier paired CL probes show separate original levels. No APK claimed playable.
- This is acceptance-preflight hardening only; host extra physical actors, independent weapons, shared AI/doors/objectives, guest original-engine application and automatic menu close remain unimplemented/unverified.

## Next priority
- Verify fresh Actions and Android build when accessible; avoid repeated native pushes during long build. Implement safe original host player2 lifecycle and original physics/weapon/AI ownership, then guest host-authoritative scene consumers and START auto-close. One major owner two-phone trial only after true integration. Never touch main/CL/biso/XBE/ISO/saves.
