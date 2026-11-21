## Session33 automation/build status correction
As of 2026-11-21 20:46 Baghdad, the existing hourly conditional automation ID 6b01c6efa2d481919e50c7255644ec2f was **successfully re-enabled and updated** to watch the latest room-net APK workflow attempts starting with https://github.com/alwansan/BO/actions/runs/44931616418, ignoring cancelled superseded runs; next check scheduled for 21:30:35 Baghdad. The earlier note that the old watcher was disabled describes only the state BEFORE this update. Source and Java preflight [44931616369](https://github.com/alwansan/BO/actions/runs/44931616369) SUCCESS; Android29 TLS+Dialog Back regression [44931616420](https://github.com/alwansan/BO/actions/runs/44931616420) SUCCESS; JVM TLS core [44931616384](https://github.com/alwansan/BO/actions/runs/44931616384) SUCCESS; Windows/NDK game APK [44931616418](https://github.com/alwansan/BO/actions/runs/44931616418) IN PROGRESS as checked. Do NOT claim new APK artifact or two-device join before real verification.

# Session33 addendum 2026-11-21: owner two-phone feedback repair

Start with memory/sessions/2026-11-21_bo-coop-33-owner-two-phone-lan-feedback.md, then AGENTS.md, STATE, MEMORY, PLAN, ISSUES, docs/ROOM_NETWORK_DEVICE_TEST_AR.md. Only writable branch alwansan/BO experiment/black-coop. Owner tested older APK run44924346601/artifact14715302629 on TWO Android phones; no 2/4 authenticated join proven, Back left game, form awkward and no LAN discovery. Both load matching c5bfed XBE. CL JPG filenames read but image pixels not accessible through GitHub text-only connector: don't claim visual inspection. Fixed source: RoomSession UDP1510 advertisement with complete fingerprint, discovery browser, Android Dialog inside game Activity with landscape two-column HOST/JOIN, separate leave/back buttons, no standalone Activity; first connection requires PIN plus host fingerprint verified out-of-band. Source also records safe independent event codes and one-shot Downloads export from both phones without Probe.

Android29 emulator RoomOverlay KEYCODE_BACK regression https://github.com/alwansan/BO/actions/runs/44931363100 SUCCESS with TLS_PLATFORM_PASS and host Activity surviving Back; not owner-phone runtime. Source preflight run44931616369 SUCCESS, latest full Windows APK attempt run44931616418 IN PROGRESS at record time. Check LIVE newest workflow runs, then verify APK artifact/signature/image and get second-owner-phone HOSTING+JOINED+2/4 evidence. Story/player actor+324/combat/quest sync remains incomplete. Newest code through ad0be5af41382a95e4183e4fb0ffa8aa5d6ad44e. Auto build watcher created for OLD run44924346601 was found disabled; don't claim it's watching newer builds.

---


# BO-COOP — Latest restore Session33 (2026-11-21)

**Writable branch ONLY: alwansan/BO experiment/black-coop.** Read AGENTS.md and memory/sessions/2026-11-21_bo-coop-33-owner-two-phone-lan-feedback.md, STATE/MEMORY/PLAN/ISSUES, docs/ROOM_NETWORK_DEVICE_TEST_AR.md, and earlier Session32 before editing. Check live HEAD and newest Actions; no force push.

The owner DID test roomnet APK on TWO real Android phones on same Wi-Fi (CL img/logs), but COULD NOT join; no confirmed 2/4 roster. Both phones load the same c5bfed25449c8212d7a976f15246e87ead57713f XBE; first Android15 SM8475/Adreno730, second Android14 MT8781/Mali-G57. Second exported black_log.txt can be empty; a separate file contains FEAudio quit warning unrelated to proven TLS. JPG filenames were read but binary pixels not accessible through this GitHub text connector, so rely on owner's UI description.

Old APK workflow 44924346601 SUCCESS, artifact14715302629, was the one tested. User disliked Android default form; Back quit entire app; LEAVE did not return; manual IP/port and room join failed. Source root causes: RoomLobbyActivity opened as separate Activity, LEAVE only stopped session, LAN discovery existed but never started by RoomSession. Fix committed: same-Activity BLACK-styled RoomOverlay Dialog instead of secondary Activity; back closes modal safely, LEAVE stops sockets; Flash rows HOST ROOM/FIND LAN ROOM; auto UDP 1510 host announce and JOIN scan showing full 32-byte TLS fingerprint with trust confirmation, PIN remains private; sanitized coop_room_events.log on both phones. First-time full SHA-256 fingerprint comparison is mandatory; no unsafe automatic trust or PIN bypass. Overlay still not pixel-perfect original Flash; actual gameplay co-op still absent.

Latest code commits through af14d855c612747d77f555de5f7598ef64f30790; current official HEAD must be fetched, not assumed. New network preflight 44930785583 SUCCESS and TLS JVM 44930785371 SUCCESS on preceding d78e3d9. Check newer runs triggered by b89fc9d and af14d855. Latest full candidate workflow is black-coop-room-network-apk.yml, with concurrency cancel-in-progress=true at code 8a42b42 so obsolete Windows runs can be cancelled; no new APK success may be claimed before artifact verification.

Update memory after each batch, protect single-player/main, don't publish licensed original assets or private keys. Next repair latest APK CI, distribute test safely, then independently confirm real LAN 2/4 and Back/Leave via both devices. No second actor/body/AI/combat/story synchronization yet.

---


# BO-COOP — Latest restore Session32 (2026-11-21)

**Only writable GitHub branch `alwansan/BO:experiment/black-coop`.** Read `AGENTS.md`, `memory/STATE.json`, `memory/sessions/2026-11-21_bo-coop-32-flash-room-network-bridge.md`, then Session31/MEMORY/PLAN/ISSUES and `docs/ROOM_DIRECT_JOIN_AR.md`. Verify LIVE HEAD and Actions before editing; CAS commits only, no force push.

## Session32 latest
- Android TLS/EC P-256 opaque AndroidKeyStore: [44922935535](https://github.com/alwansan/BO/actions/runs/44922935535) **SUCCESS `TLS_PLATFORM_PASS`**. Standalone Host/Join Android form and RoomSession runtime [44923426931](https://github.com/alwansan/BO/actions/runs/44923426931) **SUCCESS `TLS_PLATFORM_PASS`**. Real JWT? no; actual pinned TLS loopback Android29 only, not two phones.
- Java RoomSession JVM test [44923264389](https://github.com/alwansan/BO/actions/runs/44923264389) SUCCESS `ROOM_SESSION_PASS`.
- New separate game Flash -> Android TLS room bridge `coop/prepare_room_network.py`, `RoomAppRooms.java`, `room_net_ui.hpp`. `org.bocoop.black.roomnet` package independently staged; accepted `roomui` and Probe4 untouched. Flash HOST/JOIN confirmed buttons open Android form, status rows show authenticated roster count; process-owned session stays active when returning from form, connection workers do not touch game actors/Flash.
- Source atomic staging plus Android javac gate [44924220185](https://github.com/alwansan/BO/actions/runs/44924220185) **SUCCESS** `ROOM_NET_STAGE_PASS`/`ROOM_NET_JAVA_PASS`. Code source `47c3f957a3b9fa4557f175e9e6011e4841076a1a`.
- New full Windows/NDK game APK workflow [44924346601](https://github.com/alwansan/BO/actions/runs/44924346601) **IN PROGRESS when documented**, code `ead146ffc1f3f6fca1ee540330cbaaea02c9a3c0`. Do NOT claim APK exists before outcome/artifact/signed SHA checked. Original XBE image gate stays; no ISO, private keys, derived assets in GitHub.
- **No playable co-op, no second player actor/body, combat, mission or quest sync, nor two-phone test.** Need actor+324 ownership/factory/rig research; owner accepted previous UI and Probe4, no retest.
- Next inspect live APK workflow; fix and rerun build if it fails. Update RESTORE+STATE/MEMORY/PLAN/ISSUES after results. Preserve main, story saves, original ISO, accepted APK.

---

# BO-COOP — Latest restore Session31 (2026-11-21)

**Only writable branch:** `alwansan/BO:experiment/black-coop`. Read `AGENTS.md`, `memory/STATE.json`, `memory/sessions/2026-11-21_bo-coop-31-android-tls-session-forms.md`, then MEMORY, PLAN, ISSUES, `docs/ROOM_DIRECT_JOIN_AR.md`, `docs/COOP_INTEGRATION_ROADMAP_AR.md` and earlier Session30. Check live HEAD and Actions before writing.

### Session31 latest verified facts
- **Android TLS platform PASS** on Android29 emulator in [44922935535](https://github.com/alwansan/BO/actions/runs/44922935535), job136902689543, explicit `TLS_PLATFORM_PASS`. EC P-256 opaque AndroidKeyStore and real pinned TLS room host/client/roster/heartbeat/close. It is not a real phone pair or gameplay.
- Added `RoomSession.java` bounded async lifecycle, cancel, immutable roster. Source `e8c0aece75bc466644e3a9cd775b8f5c79ce4105`, core [44923264389](https://github.com/alwansan/BO/actions/runs/44923264389) **SUCCESS** with `ROOM_SESSION_PASS`.
- Added isolated Android `RoomLobbyActivity.java` Host/Join/Leave direct IPv4+port/full fingerprint/PIN and roster; source `9ab240262f5d327203053b6ca7d2c820360b72a8`. Android emulator platform [44923426931](https://github.com/alwansan/BO/actions/runs/44923426931) **PENDING when recorded**. Previous run44923264409 APK `pm install` hung exit124 after adb push; diagnose/retry bounded, not a proven TLS regression.
- **No Flash-to-Activity wiring yet. No second human gameplay actor, body, AI/combat or mission sync.** Owner accepted previous UI and Probe4; do NOT ask to redo those. Pin/key/private assets never committed. `main` and `biso` unchanged.
- Next: inspect latest Actions, ensure Android form gate PASS, then stage actual game Android/Flash Host/Join integration and a separate APK, plus independent ACTOR+324/factory/full-body ownership audit. Update RESTORE and memory after every batch.

---

# BO-COOP — Latest restore Session30 (2026-11-21)
**Only writable GitHub branch `alwansan/BO:experiment/black-coop`.** Start with `AGENTS.md`, `memory/STATE.json`, `memory/sessions/2026-11-21_bo-coop-30-directjoin-android-keystore.md`, then `memory/MEMORY.md`, `memory/PLAN_AR.md`, `memory/ISSUES_AR.md`, `docs/ROOM_DIRECT_JOIN_AR.md`, `docs/superpowers/plans/2026-11-21-real-lobby.md`, and `docs/COOP_INTEGRATION_ROADMAP_AR.md`. Verify HEAD and Actions live before writing.

## Session30 new work — keep all session29 advances
- User linked another ChatGPT shared chat, but exact URL was unavailable via web; progress was recovered **from newer live GitHub commits and session29**, not fabricated from the chat itself. The UI CO-OP under Options and Probe4 were owner accepted; do not retest them. Four humans including phone host remains target, but second actor/brain/mission/combat sync are **still absent**.
- Real TLS core RoomTls/RoomWire on JVM and UDP room discovery existed in session29. New `RoomDirect.java` validates canonical IPv4:port, full SHA256 fingerprint, rejects wrong PIN/cert on real TLS; source8df7a683 and CI [44920763536](https://github.com/alwansan/BO/actions/runs/44920763536) **SUCCESS**. Still no Android/Flash UI bridge for this.
- Emulator script had ADB hang; bounded calls with `timeout -k` (code a954306c). Subsequent Android29 simulator **booted and test APK installed**, actual TLS failed with `SSLHandshakeException: Connection closed by peer`. Added `DIGEST_NONE` in AndroidKeyStore (code650d50eb) but not enough.
- Sanitized no-secrets server diagnostics (code903bb225, run [44921892101](https://github.com/alwansan/BO/actions/runs/44921892101)) revealed `SSLHandshakeException/SSLProtocolException`, requested key type `EC` while old server X509KeyManager only served RSA. Replaced test identity with new opaque AndroidKeyStore **EC P-256** alias `BLACK_COOP_TLS_EC_V1` code `af67599d8cbe7db7f99bf36f1ebc139f0ec4e0c0`. **Check current platform run [44922289967](https://github.com/alwansan/BO/actions/runs/44922289967) before claiming Android PASS.** No RSA-as-EC misuse, no disabling pin or trust.
- The old title UI APK `org.bocoop.black.roomui` built in run44913654405 and owner accepted; Probe4 pool/instance read-only capture verified; refer to Session29 full handoff below for actor ABI and ownership caveats. `biso` owner ISO already decomposed/pinned; Peterson Windows TDM external source restricted and binary-only, never copy private code/animations.
- Don't ask the owner to redo accepted baseline/UI/Probe4, or upload an XBE. Preserve story saves and main. Approval for separate experimental `biso` branch remains only if modifying ISO becomes genuinely necessary.
- Next: verify Android EC identity/TLS test, then connect Android host/join form and immutable room roster to game Flash in separate package; **not** equal to actual co-op. Independently audit actor+324 ownership, allocator/factory and safe full-body rig before writing guest memory. Keep project memory updated, expected-SHA CAS commits, no force-push.

---
## Prior session29 detail retained
# BO-COOP — استعادة أحدث حالة، جلسة29، 2026-11-21

## الهدف والفرع
تطوير story co-op حقيقي لـBLACK Android، وليس TDM أو محاكاة شبكة. المتطلبات الكاملة في `docs/OWNER_PROJECT_OBJECTIVE_AR.md`. **لم يكتمل co-op بعد**: لا لاعب ثانٍ حقيقي أو COMBAT/AI/quest sync مقبول، ولا ISO معدل جديد. التطويرعلى `alwansan/BO:experiment/black-coop`؛وأجازالمالكفيجلسة28فرعاًتجريبياًجديداًفيbisoلتعديلISOعندالحاجة.mainوالمراجعالأخرىللقراءةفقط،لاforcepush. المالك يجيز البحث والتنفيذ وActions حتى الحاجة لاختبار هاتف.

ابدأ بـAGENTS.md ثمmemory/STATE.json و`memory/sessions/2026-11-21_bo-coop-29-real-tls-core.md`،ثمMEMORY/PLAN_AR/ISSUES_AR. تحقق منHEAD وActions الحيين؛لا تستعد حالة تاريخية قديمة بدلاً منالحالة هنا.

## الخطوة التالية الآن
**المالك قبل تشغيل اللعبة وظهور القوائم، والصور الجديدة رُئيت فعلياً. لا تطلب إعادة اختبار UI/Probe4.** صورCL الخمس تؤكد الزر في القائمتين وصفحة الغرفة. السجل الجديد173522-325قُرئوفُحص:140/140pool و692/692links صالحة،نحو141s،scopeواحد،دونend. الأدلة memory/evidence/ROOM-UI-owner-173522-safe-summary.json. خطأFEAudioواحدفيConfirmWarning؛السببغيرمثبتولاcrashمبلغ،لاfixتخميني.

**العمل الجاري: غرفة اتصال حقيقية.** RoomWire.java وRoomTls.java تحتcoop/lobbyتعملبـTLSsocketsفعليةعلىJVM(مضيف+3عملاء)،fullcertificatepin وPINمشفّر،توافقالصورة/profile،رفضامتلاء/نسخةخاطئة،روسترومعرفاتحيةوheartbeatوإغلاقموارد. اختباراتمحليةPASSومراجعةمستقلةأُصلحتنتائجهاباختباراتRED→GREEN. [run44917272041](https://github.com/alwansan/BO/actions/runs/44917272041) **SUCCESS** على591372ba4819fd001ebb354dced27179e97ac548؛job136885459965قُرئسجله. أضيفRoomIdentityواختبارAndroid29emulatorمستقلبلاgameassets؛افحصworkflowblack-coop-lobby-android-platformحياً،run44917717978/job136886809488فشلبعدJava/Androidcompileلغيابrgفيrunner؛أضيفfind/grepfallbackوفحصclassinventory. retry44917922246/job136887416486على8125aae7تجاوزpackagingلكنadbwaitفشل120sقبلجهازالمحاكي. أضيفemulatorlog/accelcheckللتشخيص؛run44918736731/job136889884196أثبتأنKVMيعملوأنemulatorلايجدAVDblack_tls؛أضيفAVDregistryموحدومسارصريحوفحصlist-avdsقبلadb. RoomDiscoveryالجديدBCLN-v2نجحمحلياًبـUDPوأعدادأعضاءTLSالحقيقية،وليسAndroidLAN. افحصCIالقادم؛Keystore/TLSruntimeعلىAndroidلميثبتبعد. التاليAndroidKeyStore/تحققالبصمةواكتشافLAN/directIPوforms/JNIإلىFlash،ثمAPKغرفةفعليوتجربةهاتفينثقيلة. **لم يربطTLSبـAndroidأوباللعبةبعد،والأجساموالقتالومزامنةالقصةغيرمكتملة.** الخطة docs/superpowers/plans/2026-11-21-real-lobby.md. لا تقف عندTLSوحدهكتحقيقللهدف.

**مرجع APK الواجهة المقبولة الآن:** زر CO-OP داخل Flash تحت Options، يفتح الغرفة mode6 مباشرة، وBack يعيد إلى نفس title. حزمة `org.bocoop.black.roomui`، اسم **BLACK Co-op UI TEST**. [run44913654405](https://github.com/alwansan/BO/actions/runs/44913654405) **SUCCESS** على source `96f69b6c8e912760fedaf5d7add52928f3b212f7`؛ Linux136874536833 وWindows/NDK136874579612 نجحا وقُرئت logs. APK21753039bytes،SHA256 `af133c9b2a74a9714d62ca943bdd209ffc552267a053951085980ad7addfebfe`. [artifact14710883570](https://github.com/alwansan/BO/actions/runs/44913654405/artifacts/14710883570)،ينتهي2026-12-05؛metadata14710784028. نُزل وفُحص ZIP/APK مستقلاً؛الأدلة `memory/evidence/ROOM-UI-apk-44913654405.json`. نسخة محلية `/home/a/Desktop/black/RoomUI-44913654405/black-room-ui-NO-GAMEPLAY-COOP.apk`.

مرجع اختبار الواجهة `docs/ROOM_UI_DEVICE_TEST_AR.md`؛المالك قبلها الآن،لا تطلب إعادة الاختبار. HOST/JOIN غير متاحين،ولا أجسام بشر متصلين أو co-op قابل للعب بعد. ISO لم تتغير؛هذه الإضافة من APK. اختبارات13preparer و12rootPython وnative routing/sanitizers ومراجعة مستقلة نجحت. runالأول44913282838 فشل قبلAPKcompile بسبب CRLF؛أُعيد الفشل RED وأُصلح ثم نجح runالحالي. خريطة الربط الفعلي `docs/COOP_INTEGRATION_ROADMAP_AR.md`،وخطة الواجهة `docs/superpowers/plans/2026-11-21-title-coop-entry.md`.

**توضيح المالك:** جميعاختباراتالهاتفالسابقةفردية،والشخصياتالأخرىبوتات. يريدأجساماًمستقلةللبشرالحقيقيينومزامنةانتقالالمراحلواختيارالمهمةللمضيف. أجازفرعاًتجريبياًفيbisoوتعديل/تجميعISOعندالحاجة؛لاmain. الزرالحاليينفذمنAPKولايحتاجISOمعدلة. رابطالمحادثةالمشتركةتعذرفتحه؛صورCLرُئيتفعلياً.

**وصل سجل Probe4 وجرى تحليله فعلياً، والمالك يؤكد عدم وجود مشاكل.** لا تطلب إعادة الاختبار نفسه. المصدر `alwansan/CL:main/black log/logs/BLACK-Probe-20261121-163248-271/coop_probe.txt`؛ 239490bytes، blobSHA `b9771b23aec3dc6af350818915da46e2ef9af946`، SHA256 `3bda968f49790ff2f7254a6d8f860eb4891367edf849fc941be83b8b83496d0c`. الملخص الآمن `memory/evidence/PROBE4-owner-163248-safe-summary.json`: 111/111 pool و548/548 instance links صالحة، 437 NPC و111 local، صفر rejects. المدة115107ms،scopeواحد،لاend؛لاreloadproof أوallocationownership أوco-op.

**التالي تجربة مجسم محلي حقيقي مؤقت، بعد حسم سلامة lifecycle.** اقرأ جلسة27: full factory1DF440 لا يقبل null controller (1D21F0 يكتب +8C دون guard). 1801B0 يحرك spatial tree؛cleanup1DA1A0 لا يصلح لشخصية غير مسجلة. 1CE1F0 يهيئ world list وspatial links، **مسار الإصابة1E7753 يقرأ324 ثم+90 دونnullguard؛رُفض تصميمnull-brainالمسجل.** التاليتدقيق1AF880(12stackargs+ECX/EDI)والـfreelistglobal2D1988+22CDCلاكتساب324مستقلعبرالمحرك،معفحصquest/spawnIDsوسعاتالحوض. لايكفيdamagehookواحد. لا تكتب guest memory أو تستدع factory قبل حسمها، ولا توسع mock network بدلاً من مجسم فعلي. APK الواجهة الجديدة موصوف أعلاه؛لا APK لاعب ثانٍ بعد.

- APKrun44876183662 **SUCCESS**،source `1b27ecc26e5417b91eb84208479e039e9b792521`؛LinuxASAN/UBSAN وWindows/NDK/APKنجحا وقرئتlogs.
- package `org.bocoop.black.probe4`،labelBLACK Probe4 READ ONLY،21749755bytes،SHA256 `bfd7968b04851d8c814bdb88563b1efb8e8b9b88695d2c161c077111bd3c9897`.
- [artifact14696342069](https://github.com/alwansan/BO/actions/runs/44876183662/artifacts/14696342069)،metadata14697085027؛artifactالتطبيقينتهي2026-12-05. نُزلZIPوفُحصZIP/APKhashمستقلاً؛الأدلة `memory/evidence/PROBE4-apk-44876183662.json`. لاsignedURLsفيالمستودع.
- نسخةمحلية `/home/a/Desktop/black/Probe4-44876183662/black-probe4-candidate-READ-ONLY-NO-COOP.apk` ومعهاmetadata/NOTICES،ولايفترضوجودهافيجلسةسحابيةجديدة.
- أحدثanalyzerCI44908343745 **SUCCESS** على `399cf730f95dfb6a52c76f02cca30b353545cadd`؛تحديثPythonفقط،لايغيّرAPKالمذكور. لاCIمطلوبإعادةبدئهلمجردالاستعادة.

## ما أثبته سجل المالك السابق
وصلسجلProbe3 `alwansan/CL:main/black log/logs/BLACK-Probe-20261121-090753-249/coop_probe.txt` وقرئفعلياً. 89203bytes،SHA256 `d95bddaedf2edcbe4d670f3aa776060fabc622982e6a17862574477a4e803e0e`،blobSHA `02ab58b296a4ae09f27f8ef173e244936423a7ba`. لاتنسخالخامإلىBO.
196JSONrecords تشمل56/56poolصالحة وzero rejects؛رُصدت17slot/serialcombinations وتبدلاserialفيالخانةنفسهامعظهورهاoccupied. الانتقالاتالمشاهدة0→1عدد13،1→2عدد9،2→3عدد8،1→0عدد1. هذايدعمإعادةالتهيئةالمرصودة،وليسcallbacksدقيقةلـspawn/death. اللقطةحوالي59.7ثانيةبـscopeواحدودونend،فلاreloadproofداخلنافذتها. ملخصآمن `memory/evidence/PROBE3-owner-090753-safe-summary.json` وجلسة24.
resource320مشتركبينعدةشخصيات(حتى4قيمغيرصفريةفياللقطة)،و35Cقيمةواحدةمشتركة؛ليساإثباتmodelinstance. **لا تطلبProbe3نفسهمجدداً.** baseline وProbe2قبلهيعملانعلىهاتفالمالك؛لا تكررطلبXBEالأصلي82d058غيرالمتوفر.

## معرفة المحرك التي يبنى عليها العمل
اقرأ `docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md` و`docs/ACTOR_INSTANCE_OWNERSHIP_AR.md` قبلأيguestwrite أوfactorycall.
- الصورةالمطابقةc5bfed25449c8212d7a976f15246e87ead57713f. globalmanager2D1980؛32NPCslotsبداية+A0،stride3C0. vtableNPC28C1E8 وlocal28CFE0. أولvtableليسdestructorمفترضاً.
- containers79A0/free،79B0/live-like،79C0/phase1،79D0/phase2؛+4capacity/+8occupied/+Cpointer،arraysبهاholes. actor+250membership،+374serial،+384state. reset1D5E00ينجحAL1ويعيدcounter8524إلى1؛الفشلAL0لايعدreset. لاهويةدائمةبعنوانpointerأوserialمنفرد.
- factory-like1DF440وinit1DA4F0مشروحانثابتاً؛لايستدعيهماالرصد. زيادةlivecapacityوحدهاقدتكسرfree-slotinvariant؛لاتستدعfactoryبـdescriptor أو324مفترضين.
- **actor328هوcontrollerمرشححقيقي**:17E420يربطactor328↔controller.owner0؛runtime54/binding50 وscratch34C/350يستخدمهاdraw1C6150. Probe4يفحصbacklinkوهويةالشخصيةقبلوبعدالقراءة؛حتى4NPC+localكلثانية،حد300kernelcopies،دونguestwrites/calls.
- instance_schema1 وinstances tuple `[local,slot,serial,valid,controller,runtime,binding,scratchA,scratchB]`. validيعنيbacklinkوالحقولالمختارةاستقرتفيالقراءة؛**لايثبتreadabletargets أوallocationownership أوuniqueness أوatomicity/ABA**. الفشلpointeroutputsصفر. بعددليلالهاتفانتقللتجربةbody/proxyحقيقيةبخيطاللعبةوحمايةscope/slot/serial؛لا تستبدلهابتوسيعmocknetwork.

## الأدوات والكود
`coop/analysis/pool_capture.py`يحللProbe3/4ويخرِجaggregatesفقط؛10pooltests،19analysis test_*.py و11legacyprivacychecksمحلياًPASS ومراجعةمستقلةواضحة. `coop/probe4/instance_graph.hpp`وpool_observer.inc هماالرصدالجديد؛prepare_probe4.pyيعملفيstageمؤقتويحافظعلىProbe3. الخطة `docs/superpowers/plans/2026-11-21-probe3-evidence-and-instance.md`.
NET/SceneInbox/RemoteProxyRoster وLANPOSIXالقديمةنجحتضمنحدودloopback/synthetic/NDKفقط؛لاauthأوAndroidLANruntimeأوgameactorintegrationمقبول. زر CO-OP العلوي نُفذ في APK الواجهة الحالية،لكن قبول موضعه على الهاتف معلق؛HOST/JOIN الفعليان غير منفذين.

## المصادر والمراجع الإلزامية
- ownerISOموجودمجزأفيBO ومفككفي `alwansan/biso@9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`؛`docs/OWNER_BISO_SOURCE_AR.md` وrestore_github_upload.py. لاتطلبإعادةرفعالأصول،ولاتنشُرISO/XBE/generatedsource/keysفيcommitsأوartifacts.
- Black-Androidbaseline `f3754d6985a8c35204e7df071546e586b10f1806` وSDK `99078b119b5d5e0d1d6afb1390e29437c0c984c0` للقراءةفقط. يعادcompileلـc5المطابق؛لاbypassلـimagegateولاادعاءمطابقةمصدرالتعديلاتالعربيةالكامل.
- مرجعالمالك `Peterson047/BLACK-XBEMultiplayer@0ee3ae6411c7f0989c62cbe36b224d3f55043923` مازالheadنفسهعندجلسة24. اقرأ `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`:WindowsTDMbinary-only،nativeBLACK-Coopالمشارإليه404،لاLICENSEشامل،body.bcaمقيدبذلكالمود. استفدمنأفكارالبنيةواكتبكوداًمستقلاً؛لاتنسخassets/binaries/scriptsغيرالمرخصة.

## الذاكرة والتسليم
بعدكلدفعة/قرار/CIحدّثSTATE/MEMORY/PLAN/ISSUESحسبالحاجة والجلسة وRESTOREوادفعنفسالفرعبـCASدونforce. احفظSHAالكودالمفحوصوRunIDs،لاSHAcommitالتوثيقنفسهداخله. المستودعالحيهوالمصدرالمرجعي.
محلياًBO-activecheckoutقديمHEADولهملفاترفعتعبرGitHubAPI؛لا تفترضأنgitstatusالمحليفروقمستجدة. فيجلسةجديدةاستنسخالفرعالحينظيفاًبـsparsecheckoutلـcoop/docs/memory/prompt/.githubبدلاًمنجلبكلالأصول. **لاتعملresetأوحذف لـBO-coopالقديم** ولهتعديلاتمحليةمسبقة. تاريخالجلساتمحفوظفيmemory/sessions؛لايُفهمحفظالذاكرةكعملفيالخلفيةخارجالجلسة.
