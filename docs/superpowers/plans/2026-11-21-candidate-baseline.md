# Candidate Baseline Implementation Plan

> **For agentic workers:** Use superpowers:executing-plans inline; user already requested continuous execution until a device test is required.

**Goal:** Build an isolated Android baseline from the exact owner image now confirmed in the phone log, before attempting read-only hooks or co-op actors.
**Architecture:** Reuse pinned v1.5.0 and SDK, restore the supplied ISO in a disposable Actions runner, regenerate guest code from its actual XBE, compile a separate debug-signed app. Keep runtime image verification intact and original game/save/signature untouched.
**Tech Stack:** Python stdlib, Windows Actions, clang-cl, NDK28.2.13676358, CMake3.31.6, JDK17/Gradle.
**Spec:** docs/COOP_REQUIREMENTS_AR.md; this is the prerequisite baseline only, not the co-op deliverable.

## Constraints and decisions
- Owner confirms current Arabic APK enters missions; provided log identifies f3754d6/c5bfed... and copied ISO SHA256 equals manifest f6d15f.... Original matching82d058 image/source unavailable.
- Build candidate from c5bfed image; do not relabel it as original82d058 baseline or weaken hash checking. Static review supports investigation but full semantics/runtime remain unproven.
- Use applicationId org.bocoop.black.baseline and explicit original Java activity; namespace/JNI class remains com.racas.black. Label NO CO-OP, independent app-specific saves.
- Modify packaging/build resource limit/updater guard only; no native/src/config gameplay changes.
- Artifacts: APK and metadata only after actual build, package/ABI/signature/embedded-image checks; never ISO/XBE/generated sources, logs from owner, signing credentials or saves.
- Device acceptance remains required before HOOK/ACTOR work. Original Arabic modifications are not reconstructed by this upstream baseline.

## Task1: Prepare isolated packaging
- [x] Add failing unittest for separate ID/label/activity, preserved namespace/native source, and fail-closed unknown upstream anchors without partial writes.
- [x] Run RED; implement coop/prepare_baseline.py; run entire Python unittest suite GREEN.

## Task2: Build and verify candidate on Actions
- [x] Add bounded workflow: manifest restore, pinned tools, regenerate XBE, apply packaging, Android compile with two native compile jobs.
- [x] Validate YAML/inline Python locally; run actual CI and diagnose failures from logs.
- [x] Verify final APK package/ARM64/debug signature and embedded kImageSha1 equals input. Confirm no original game assets bundled; archive only APK+metadata.
- [x] Record run/code/artifact hashes and precise scope in memory/state/plan/issues/session/recovery.
- [x] Provide exact install/import/first-mission/log instructions for the new baseline; do not claim co-op or working rebuilt engine before phone test.

## Review focus
Unknown source anchors must fail before mutation; app identity must resolve original JNI class; native gameplay files must remain unchanged; build must bind to real input hash; copied owner logs and game bytes must stay out of artifacts.

Review fixed: private import destination plus removal of broad storage permissions, and no metadata/APK upload before verification. Storage-permission test observed RED then GREEN; full suite4/4 PASS. Actual pinned-source packaging preview and syntax checks PASS.

Completed baseline build deliverable: run44845637359, artifact14682519899; APK SHA/ZIP SHA and native image independently rechecked after download. Phone acceptance remains explicitly pending before HOOK/ACTOR; full co-op goal incomplete.
