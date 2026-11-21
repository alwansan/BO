# Title CO-OP Entry Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement task-by-task.

**Goal:** Add CO-OP below Options and above Quit in the native BLACK title menu, opening the existing room page directly and returning to the same title.

**Architecture:** Extend the pinned exit-menu clip/ring implementation; share a one-shot route with options_menu.cpp. Reuse its dynamic string lookup and room page, preserving the stock settings entry. No ISO change is necessary for this menu. Networking and actual playable actors remain separate unfinished subsystems.

**Tech Stack:** C++20, pinned Black-Android v1.5.0, Python staged source preparer, GitHub Actions NDK28.

**Spec:** docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md and owner request 2026-11-21; screenshots CL black log/img examined directly. Owner repeatedly authorizes inline implementation and Actions, so no additional design approval requested.

## Global Constraints
- BO branch experiment/black-coop only; no main changes.
- Original APK, ISO and saves preserved; separate org.bocoop.black.roomui package.
- HOST/JOIN must remain clearly unavailable until real networking/actors work; no fake connected players.
- No game assets or raw owner captures in commit/artifact.

## Review Focus
- Main title variants retain their own stock rows and quit confirmation.
- Cancelled/failed transitions must not redirect later ordinary Options visits.
- Room Back returns directly to the originating title, without trapping focus.
- Clip duplication is idempotent; Quit stays below the new row.
- Bad source anchors/preexisting additions fail before any checkout writes.

## Task 1 — route and staged UI integration
- [x] Write native one-shot routing tests and observe missing-header RED.
- [x] Implement room_route.hpp and route wrappers in existing options source.
- [x] Add real dynamic label and CO-OP clip/ring in exit_menu.cpp; use OptionsMenuXbox as the actual movie with direct room mode.
- [x] Add preparer checks for atomic failure, isolated packaging, intact original settings and route placement; run route tests and full pinned preparer suite.

## Task 2 — independent Android build
- [x] Add separate workflow based on verified Probe4 build; allowlist the two UI changes and exact new headers.
- [x] Review diff independently; fix material findings and rerun checks.
- [x] Push with expected-head lease, wait for Actions, inspect logs and artifact metadata/hash.
- [x] Update STATE/RESTORE with owner solo-only clarification, ISO/APK distinction and true remaining actor/network gates.

## Next subsystems (not completed by a UI build)
Native independent actor/controller acquisition (null-brain registered actor rejected), two-phone authenticated host/join and snapshots, host-authoritative combat and AI target integration, mission/checkpoint transitions and full player bodies including host third-person representation. NPC allies/enemies are not remote players and are preserved by default; AI multiplayer behavior needs verification rather than assuming it adapts automatically.

Ledger: source bf9a5c8; Windows CI first failed on raw CRLF source anchor. Reproducer RED→GREEN; normalize new title source only. Preparer13/13 and route PASS; native build retry required.

Final: retry44913654405 SUCCESS on96f69b6; Linux+NDK logs read. Artifact14710883570 downloaded, ZIP/APK hashes independently match. Phone UI navigation is pending; overall co-op goal remains unfinished.
