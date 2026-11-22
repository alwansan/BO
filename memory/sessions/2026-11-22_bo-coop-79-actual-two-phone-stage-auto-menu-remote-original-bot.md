# BO-COOP Session79: user-provided first 2-phone Android START results and actual engine integration (2026-11-22)

**Repo:** alwansan/BO, **writable branch ONLY** experiment/black-coop; never edit main, CL, biso, owner original game ISO/XBE/saves. User expects autonomous implementation and meaningful co-op demo, not repeated tiny tests.

## Phone experiment — read from owner records
- Host \`CL:main/black log/logs/BLACK-Room-1795378738622-events.txt\` and guest \`CL:main/black log/من الهاتف الثاني/BLACK-Room-1795378725623-events.txt\`. Host Android 15 POCO F5 Pro, guest Android 14 Xiaomi 22081283G. Both game ISO original XBE SHA1 c5bfed25449c8212d7a976f15246e87ead57713f.
- Both phones create and join authentic TLS room; START on host queues true story PREPARE and guest frontend observes it. But neither menu auto-starts, each owner manually starts the actual original BLACK mission, both see separate normal single-player world and no remote player. Room connection persists across offline local missions.
- Host source actual level0 **stage1**, compared with START's previous 0/0. This caused a false STAGE_MISMATCH_ABORT followed by second PREPARE. Host original frame 0/1 after manual select; guest original match 0/1, both READY/COMMIT, input/eye sent. COMMIT is NEVER proof of one shared AI/combat/objectives world.
- biso extracted 8 level directories (00, 01, 03, 04, 05, 06, 07, 08) and each has stg_0001; opening mission is \`levels/level_00/stg_0001\`.

## Actual source changes committed in this session
- \`5161e8177292678bbcb44819998e14b544a3a2ab\`: START fixed 0/1 and strict same-ticket no spurious stage abort, Java integration tests; TLS 45066912205 SUCCESS.
- \`d238a5b419e4b30cba1af40d93cca47498e09bef\`: exact private owner XBE SHA256 compared with MIT PC prototype original supported XBE, 45067068552 SUCCESS MATCH, no original bytes published.
- \`45e37699255076174d4731627c034999189ba035\`: source-authenticated two-human host authoritative other-body-feet into original game thread, 350ms freshness, proper scene ticket/identity, C++ native + Java/TLS tests, SUCCESS CI 45067312273/45067312245/45067312242/45067312271.
- \`0c2e8a04b17ea94eefb1e5f78910b4af3d659374\`, \`116e13235f10703daeb6c5dc63652b31b338d69b\`: original BLACK native experimental visual-only soldier bot cloning, reusing empty original 32-slot character pool and own model/part buffers, original model init/pose/draw direct original guest ABI with memory checks, never copy donor AI, title/scene and host-TLS COMMIT gated. Hooks in original sub_000D1500 draw. Safe source diag codes 90/91 (not on-phone proof). Windows compile still to be confirmed.
- \`de08bf18d71ac645d0583220991161b0f1cda860\`: host and guest authenticated FIRST-STAGE auto frontend ticket; Java/TLS PASS #45068013752.
- \`43350f6d8088a3788e6b4b0b943344da7c9cf25a\` and \`e2f00dd0eccac5fcfb0e66f768a17b29dc249bb9\`: original BLACK frontend GameThread game native UI request to page Difficulty and press CONFIRM once per host-signed ticket, relying on same XBE native menu layout from MIT PC source, not direct unknown stage loader. Native C++ synthetic gate test and JNI/preflight, Windows APK build newly scheduled.
- Native CI original story contracts #45068110223 SUCCESS. Source preflight #45068110178 SUCCESS. Full Windows ARM64 build #45068110154 last seen in progress, check/fix live.

## What STILL DOES NOT work
- True one-world coop: host-owned original actor for Player2 with independent original weapon/physics/damage; guest only sends TLS input so no actual original Player2 brain/actuator yet. Separate original enemy AI/mission state currently run on two phones. Original PC prototype milestone3 (enemies target P2) UNSOLVED even on PC; cannot claim owner request done.
- Android visual clone code not phone-tested, and auto menu stage not tested on phone; compile success ≠ gameplay.
- Do not ask user to install another APK unless meaningful 2-phone attempt auto-launch/visibility and ideally host original second actor/combat integrated.
- Original first-person local view must remain unchanged, no NPC AI stealing, no missing donor original model, guard scene change, donor/blank slot ownership and lag; dynamic runtime safety.

## Owner-requested next P0
1. Verify Windows original C++ APK CI #45068110154 and real TLS integration; fix compilation, default UI game menu flow via private program/test source.
2. Port MIT PC original **P2 actor** into Android host as a real second independent player controlled by authenticated remote input (not placeholder), original collision/weapons. Never just blindly copy split-screen PC x86 global registers; Android uses X86Context/CallGuest and different runtime threading.
3. Make both original physical phones see positions/weapon actions, with host sole world AI/objectives/damage; authoritative state replication to guest, correct aggro against BOTH. This is true co-op.
4. Only after integrated implementation, do one large phone trial with safe logs + video; no micro-tests.

## Resume
Read AGENTS.md, prompt/RESTORE_SESSION_AR.md, memory/STATE.json, memory/PLAN_AR.md, docs/CLOUD_HANDOFF_AR.md, this session, latest Actions, live exact CL logs. Update every session unprompted.
