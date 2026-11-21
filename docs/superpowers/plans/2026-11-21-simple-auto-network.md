# Simple automatic LAN/VPN room repair

> Execute inline with superpowers:executing-plans. Owner explicitly authorizes implementation and testing until a meaningful two-phone test is needed.

**Goal:** Host needs player/room names only. Join selects an automatically discovered host, or enters IPv4 only. No room password, fingerprint entry, trust confirmation, or port entry. List running-game devices even when idle. Preserve the actual story-coop objective; this repair does not implement bodies/combat/story sync.

**Evidence:** Owner CL 21:09/21:17 host screenshots show empty mandatory PIN; events repeatedly show HOST_INPUT_ERROR and never HOST_REQUEST. Guest 21:19 screenshot and JOIN_INPUT_ERROR show the same PIN/fingerprint gate. Thus sampled attempts fail before sockets; empty scans alone do not prove network isolation. Both phones load c5 image.

**Architecture:** Keep working EC AndroidKeyStore/TLS internally with automatic endpoint pin acquisition on trusted LAN/VPN; remove PIN from room wire protocol and user flows, bump compatibility version. Fixed default room port 24872, discovered automatically; direct IPv4 auto-appends default port. Add bounded UDP presence on1511 independent of hosting, active while game Activity lives; live devices advertise idle/host/join roles. Broadcast plus bounded VPN subnet probes and remembered/manual endpoints; no false promise to enumerate VPN accounts or networks which block peer traffic.

**User override:** Earlier mandatory out-of-band fingerprint/PIN policy is superseded by this explicit trusted-friends request. Retain automatic version/image/frame bounds and original XBE verification; do not change game protection or original files.

- [ ] Reproduce host and join with no PIN using actual socket tests; remove PIN payload and API, preserve malformed/version/capacity/close/heartbeat checks.
- [ ] Automatic direct certificate probe and discovered-room join with no user trust prompts; test actual TLS joins.
- [ ] Device presence independent of hosting; test two UDP endpoints idle→host, self exclusion, stale expiration/cancel and protocol bounds.
- [ ] Replace complex forms with names, nearby devices/rooms, automatic refresh and optional IP; preserve Back/Leave and diagnostics.
- [ ] Test host button with names only in Android emulator, actual session join2/4, Back and leave. Source preparer atomicity/JNI checks and full ARM64 APK in Actions.
- [ ] Review, persist evidence/current memory and deliver verified APK for both-phone LAN/VPN test. No claim of gameplay co-op.
