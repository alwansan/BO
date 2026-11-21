# Real Android Host / Join Lobby Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans task-by-task.

**Goal:** Replace unavailable Host/Join with a genuine bounded, authenticated LAN/VPN room, then carry its real human IDs into the native actor integration. A lobby alone does not satisfy the overall story-coop goal.

**Architecture:** Java JSSE TLS sockets shared between JVM integration tests and Android. Per-install AndroidKeyStore server identity; client certificate pin verified out of band, encrypted room PIN checked by host. Native Flash actions request Android input forms; a thread-safe immutable snapshot feeds the native room rows. No network-thread guest calls. Discovery remains untrusted until TLS + join verification. Native actor creation remains a separate required subsystem.

**Tech Stack:** Java 17 test compiler / Android min SDK from pinned source, standard JSSE and AndroidKeyStore, C++ Flash/JNI adapter, GitHub Actions NDK28.

**Spec:** docs/COOP_INTEGRATION_ROADMAP_AR.md; owner active goal accepts existing menus and authorizes continuous inline implementation/Actions until a necessary heavy phone test.

## Global Constraints
- BO experiment/black-coop; no main changes, no force push.
- Keep original APK/ISO/saves; separate lobby package. No ISO change needed for this subsystem.
- Four real humans including host. NPCs never count as connected peers.
- No embedded production keys, no trust-all TLS, no network-thread guest writes/calls.
- Report lobby-only state honestly until actual bodies, combat and story sync work.

## Review Focus
- Wrong peer identity/PIN/image/version must fail before accepting a player.
- Slow/stalled peers cannot block other participants or grow unbounded threads/queues.
- Disconnect, app destruction and failed connect release every socket/thread and clear stale roster.
- Cancelled Android forms cannot leave a later unwanted host/join action pending.
- Discovery duplicates, forged replies and VPN broadcast absence cannot turn into authenticated joins.

## Tasks
- [ ] Record accepted owner room screenshots and fresh capture aggregates; preserve existing FEAudio observation without guessing a fix.
- [ ] TDD strict binary room frames: UTF-8, bounded lengths, image/profile compatibility, roster IDs, malformed/trailing data.
- [ ] TDD actual loopback TLS host with three clients, verified pins, incorrect PIN/image, full room, disconnect and shutdown; no game simulation.
- [ ] Add AndroidKeyStore identity and explicit peer fingerprint verification; platform compile and JVM integration in Actions.
- [ ] Add bounded LAN discovery and direct IPv4 join; real socket tests and compatibility with BCLN-v1.
- [ ] Connect Flash Host/Join/Leave to Android forms and immutable live roster; isolated APK build, review and artifact verification.
- [ ] Only request heavy two-phone LAN/VPN connection/lifecycle test once a real networking APK is ready. Do not repeat the accepted UI-only test.
- [ ] Continue independently owned native actor/controller acquisition and main-character body/rig audit; lobby success does not close this requirement.

## Sources checked
Android Keystore: https://developer.android.com/privacy-and-security/keystore
SSLSocket: https://developer.android.com/reference/javax/net/ssl/SSLSocket
These document key isolation and TLS APIs; they do not prove our integration works on a phone.
