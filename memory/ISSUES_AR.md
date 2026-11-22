## Session45 resolved metadata / remaining critical gaps
- RESOLVED owner source hierarchy ambiguity: 9 serialized RCB records in 3 StLevel files correspond to 1 distinct RCB ID per file with consistent parent/lookup/matrix data; `44965062874 SUCCESS` artifact14731331749. All 12 body-model candidate records match 24/25 local skeleton hashes. **This does not prove vertex skinning or unique actual human ally model**.
- RESOLVED offline clip span validation: `44964889359 SUCCESS` 22 RCB-linked bank v8 resources, 675 clips and 61,615 encoding4 track records bounded; no runtime animation/player/pose. Asset metadata only.
- RESOLVED target stream selection: `44965229133 SUCCESS` all 12 body candidates use three stride36 vertex streams with 3 distinct raw format flags/model and one texture-name slot; **bone weights/indices/normals/UV/material meanings remain UNKNOWN**.
- OPEN CRITICAL: real game-thread stage-loaded hook/actual mission ID/host scene epoch; independent original fullbody human visual body and skin weight/bind matrices/animation pose/material shader; authenticated gameplay snapshots, world AI/combat/quest. Original first-person hands/camera must remain local; no NPC AI takeover and no fake story READY. All Actions pass only metadata and value-only contracts. No owner phone trial yet.

---

## Session44 new BODY evidence and limitations — 2026-11-22
Owner MODEL geometry private Actions44964028143 SUCCESS: 12 head/torso/leg name-tagged source candidates from 3 StLevel files, all 12 pass indices/finite XYZ, no raw model data exported; first mission keys BC1_MAT/BC1_TOM/LRG/SOL. Closed only geometry candidate issue, **NOT** verified friendly ALLY render, actual pose/skin weights/texture material, animation hierarchy, actor identity/AI ownership, or engine hooks. Derived buffer-index pairs != true vertex count and triangle strips != rendered mesh triangles. RCB skeleton parent data and MODEL skin influence mapping remain next blockers, do not copy restricted third-party animations.
Actual c5 scene-loaded callback + host shared story/quest/combat/player sync STILL OPEN. Synthetic host human input generation gate44963681576 PASS, APK room-only44962611024 PASS, owner-phone trial not needed until substantial true shared world. See docs/OWNER_ALLY_GEOMETRY_SAFETY_AR.md.

---

## Session44 remaining verified blockers
CLOSED: room-only Android staging build `44962611024 SUCCESS` (artifact14730766256), previous Windows strict source allowlist CI44962275637 failure repaired by commit561eacc0. C++ HOST_HUMAN_INPUT_GATE_PASS on `44963681576 SUCCESS`, including reentrant scene-switch/peer-leave stale commands. These are CI/synthetic only, no phone.
OPEN (critical): actual owner c5 BLACK game-thread mission and scene-loaded evidence/epoch is missing; world xref assignments EAX/EBX alone cannot prove loading, native witness event codes cannot assert READY. RoomWire v3 has no authenticated high-frequency gameplay input/snapshot channel; value-only host input gate must NOT accept raw unauthenticated UDP and cannot mutate BLACK game objects. Remote fullbody human-controlled ally renderer, original owner asset skinning/lifetime, AI/combat/quest/host world remain not implemented. Local first-person hands untouched. Keep source-only vs actual co-op distinction and owner CI-first one major device trial policy.

---

## Session43 APK packaging + external reference status
- CI44962275637 FAILURE was strict Windows packaging diff-list missing newly staged read-only `src/game/difficulty.cpp` and two `src/coop` header names, NOT compilation/gameplay or unit failure. Corrected exact finite whitelist in workflow commit561eacc0. New CI44962611024 check live.
- Peterson047 public TDM reference is binary-only; linked BLACK-Coop source 404 on attempted read. Do not claim reused source, player renderer or mission sync. Third-party `body.bca` animation file permission limited to original mod; must NOT copy. See docs/REFERENCE_BLACK_XBEMULTIPLAYER_20261122_AR.md.
- Actual host selected BLACK story, guest scene-loaded proof and world/AI/quest sync and human remote full-body visuals still OPEN. CI-first, no phone requests.

---

## Session43 research and verified limitations
- Real c5 XBE private metadata44961768279 PASS: both kWorld write sites in one executable section, 9B apart with EAX/EBX sources, direct branch to second; no validated function boundaries/ABI, no known mission ID, stage-loaded callback or scene epoch. EBX possible zero-instruction in previous linear path is NOT proof of zero across branch. Keep actual story integration blocked.
- Staging native source observer at existing original player update and difficulty wrapper added; C++ tests44962215874 and source preflight44962275672 PASS. Diagnostic codes are safe + bounded, do not indicate actual game loads, and are not connected to STORY_READY. Android full room-only APK44962275637 check live before claiming compile success. Never ask owner to install this just for these markers.
- OPEN: human-controlled remote full-body ally rendering/animation/ownership, host authoritative one shared mission/world, camera local hands unchanged, real AI/combat/objectives and replay-safe state sync. Other synthetic TLS lobby tests are NOT the missing gameplay.
- CI-first owner policy, one major two-phone acceptance only once substantial real gameplay ready.

---

## Session42 actual CI status — 2026-11-22
Closed for synthetic CI: async room diagnostics first Android failure44960759104 was a pre-flush test assumption; second44960934419 was process logger name accidentally matching bo-room network thread leak prefix. Both corrected without relaxing network teardown assertions. Android29 latest44961110132 **SUCCESS** with ROOM_LOG_ASYNC_PASS, TLS_PLATFORM_PASS, STORY_READY_ASYNC_ANDROID_PASS, ROOM_LOG_ROTATION_PASS; source44961110163 PASS. No physical device test warranted.
Unresolved: world global 0x2D45D4 sites 0020BF48 and 0020BF51 are x86 static register-source write candidates, NOT proven scene-loaded callbacks. Original game mission/stage identity, verified game-thread hook, real remote ALLY body model ownership/renderer, input/world/AI/quest/combat replication still absent. Report docs/BLACK_WORLD_POINTER_XREF_AUDIT_AR.md, Session42.

---

## Session42 residual engine and diagnostics blockers — 2026-11-22
- RESOLVED for synthetic host/guest TLS: synchronous guest READY write on future BLACK game thread replaced with capacity-one async writer and stale ticket checks. JVM44960452007/Android29 44960451936 SUCCESS. No actual BLACK scene callback yet.
- CI issue: Android29 44960759104 FAILED because instrumentation asserted files before asynchronous writer export barrier. Fixed 0de18eb19. Android29 44960934419 FAILED because persistent diagnostic thread named bo-room-safe-log-io collided with bo-room-* network worker leak assertion; renamed to bo-diagnostic-io 367052097. Check fresh run44961110132. Not a gameplay failure or request for phone trial.
- Private owner c5 XBE world global `0x2D45D4` xref audit: 892 read/write candidates, only 2 register-sourced potential writers `0x20BF48`, `0x20BF51`, not proven real world stage loader, epoch, actual mission ID or safe hook. Source docs BLACK_WORLD_POINTER_XREF_AUDIT_AR.
- OPEN: true host-owned campaign in one world, actual loaded mission scene in BOTH clients, fullbody remote human AllyBody not NPC AI, real movement/weapon/AI/quest/combat sync; no current v3 gameplay acceptance, no owner phone test until major milestone.

---

## Session41 CL-derived issues and current verification
- Historical CL roomnet logs: first-device `ERROR_BINDEXCEPTION=4` across 9 files; second 103 `SCAN_STARTED` after joining within 6 event files. New code prevents background discovery while active and duplicate HOST/JOIN, staged into APK. JVM+Android29 and source passed; NOT physically confirmed on current phones. Existing TLS Server also has setReuseAddress/1.5s bounded bind retries; don't mark all real device port behavior proven.
- Historical original player model `8/8 valid` **44 bones with arms only**, NOT an entire character; must render separate human-controlled ally body for remote players, with no NPC AI ownership. Remaining game scene loader and actor lifetime unknown. CL 22 Nov HOOK001 captured single world epoch per file and no nonlocal player samples; not evidence of remote absence from every engine path.
- Remaining actual gameplay blocker: c5 mission/stage selection and verified game-thread world-load callback, host world synchronization, remote human actor renderer/input, weapons/combat/AI/quest. Lobby-only tests cannot close.
- CI safe CL evidence44959212836 PASS; Android29 active-state scan gate44959047461 PASS; source44959047532 PASS; JVM44959047515 PASS; lobby-only APK44959047454 check live.
- Owner explicitly wants CI-first and NO small phone testing; a single substantial test later, safe automatic logs. See docs/CL_20261122_TWO_DEVICE_LOG_EVIDENCE_AR.md and docs/MAJOR_DEVICE_TEST_GATE_AR.md.

---

## Session40 Android29 test infrastructure restored; real game remains open
- RESOLVED for synthetic instrumentation: PackageManager cold-boot stall and accidental Bash replacement corruption; guarded by `bash -n` and bounded retries. Android29 `44956919973 SUCCESS`, then `44957104337 SUCCESS` with `TLS_PLATFORM_PASS` and `ROOM_LOG_ROTATION_PASS`.
- Verified current/rotated safe room log export and injected tampered-line rejection on Android29 without owner tests. Caveat: no real BLACK game/scene/remote body data or combined game telemetry yet.
- OPEN: actual host mission scene loaded event/epoch and game-thread bridge, independent fullbody real-human ally renderer, input/world/AI/combat/story. 2/4 LAN lobby and safe event transport are not playable co-op.
- User prefers CI testing; do NOT ask for phone trials until meaningful shared story/remote player milestone.

---

## CI Android29 script corruption diagnosed and guarded — Session40
Old Android29 run44956473707 failed during cold synthetic emulator PackageManager install timeout, before instrumentation. First retry44956739205 failed early because an automated JS replacement accidentally expanded a shell `$'` replacement sequence, corrupting Bash around `grep '^package:android$'`; this was a SCRIPT syntax failure, NOT Android app/TLS or game failure. Clean script commit5dd84a4a reconstructs tail explicitly; workflow commitc8c734bc runs `bash -n` BEFORE downloading SDK or emulator boot. New CI44956919973 check live. No owner phone test for CI errors.

---

## Session40 residual issues after offline owner static/XBE and diagnostics (2026-11-22)
- XBE executable entry mapping verified on actual owner c5 in CI44956138064 but a 160B disassembly window is insufficient for scene loader semantics, ABI and epoch; do not invoke as game Start or mark game-thread hook solved. Mission index0 remains valid.
- TLS v3 story diagnostic codes are automatically logged by Android room session even when overlay is dismissed; JVM test44956473684 PASS. Combined previous/current sanitized log segments are source-patched, Android29 44956473707 / lobby-only APK 44956473687 require live confirmation. No true mission scene callback means no real story event emitted yet.
- STILL UNIMPLEMENTED: remote human fullbody AllyBody and independent renderer life, shared host world/AI/combat/objectives/weapon sync, actual two-phone shared mission.
- Device policy: do not ask owner for tiny tests, new APK lobby-only or repeated model ISO. One meaningful major trial only after all acceptance gates, collecting all small details automatically. docs/MAJOR_DEVICE_TEST_GATE_AR.md.

---

## Owner test burden policy — 2026-11-22
Owner does not want small/fragmented phone tests. CI and synthetic/emulator tests are preferred. A physical two-phone test remains BLOCKED until meaningful real shared-story engine/remote-human integration exists. Automatic consolidated privacy-safe diagnostics must capture minor uncertainties during that one major test. Do not ask owner to try the successful lobby-only APK; no gameplay exists. Follow docs/MAJOR_DEVICE_TEST_GATE_AR.md and AGENTS.md.

---

## Session39 — new bridge verification and residual issues (2026-11-22)
- **PARTIAL RESOLUTION**: prior TLS v3 story transport was accessible only through RoomTls. Source commits 86faa014/5370dcae now expose a bound-thread RoomSession facade with generation reset and cover it by actual pinned-TLS JVM SessionTest. CI44954581676 SUCCESS. A caller can *claim* its thread with `bindStoryGameThread`, so actual BLACK hook/scene-loaded evidence is still essential before wiring production gameplay; this is a guard, not attestation.
- **OPEN**: exact mission start and scene-ready authoritative events inside c5 BLACK, JNI game thread contract, guest scene load. Current Android app never calls the facade. No fake READY or shared Start claims.
- **OPEN**: original owner CL shows rapid rehost BindException; source fixed-port 1.5s retry had synthetic tests, not tested again on two actual owner phones. 2/4 proves lobby only.
- **OPEN**: fullbody human-controlled remote AllyBody renderer independent from NPC AI, synchronization of input/animation/weapon/world/AI/combat/quest/saves. No player actor visible from session's owner-provided lobby screenshots.
- **CHECK**: newest Android29 44954581718 and Android lobby APK 44954581624 outcome, then prove v3 phone compatibility only with matching APKs.

---

## Session38 current residuals after real story TLS tests
- FIXED in source/CI: strict STORY PREPARE/READY/COMMIT/ABORT control carried by actual TLS; JVM44952822531 and Android29 KeyStore44952822499 PASS. NOT coupled to BLACK mission selection/loaded scene, so no playable co-op Start.
- MITIGATED in source and isolated test: CL ERROR_BINDEXCEPTION after quick Host/Leave/Host; bound 1.5s TCP retry + SO_REUSEADDR while disallowing simultaneous host, JVM44953176046/Android29_44953176054 PASS. Not yet proven on owner Xiaomi/Huawei; original event code cannot determine all contributing router/OS causes.
- INCOMPATIBILITY INTENTIONAL: RoomWire VERSION/PROFILE now3 to reject old room-only v2 clients; both real phones must run matching future build, do not mix with previous owner accepted APK.
- OPEN: actual host mission-select hook and game-thread scene load/ready, engine world snapshot, independent human-controlled FULLBODY ALLY render on other phones, AI/quest/combat/weapon animation and multi-phone same-world. 2/4 room and source/JVM/Android socket tests are not gameplay.
- OPEN: original JPG assets in CL indexed but GitHub connector refuses binary image content; don't pretend pixels verified.

---

# ACTIVE Session38 — Open and resolved distinctions (historical issues below)
- RESOLVED: Latest original-player primary-model phone test was received: 8 complete arms-only primary model samples, not absent. It is NOT proof all game model resources lack a fullbody.
- OVERRIDDEN: Session36 prohibited ally substitutes; owner now approves independent ALLY VISUAL mesh for remote real-human player ONLY, while everyone's ORIGINAL local first-person experience remains unchanged. Do not reuse AI-controlled NPC brains/actors.
- OPEN: Actual 3D ally rig skinning/material/animation, engine-owned renderer instance, cleanup and human-input ownership not validated on phones. The new RemoteAllyRoster only tracks value-only selection for both host and guest.
- OPEN: HostMissionBarrier only source/CI; need real TLS mission-control messages and actual shared BLACK stage loading/ready/host authority, then network motion/world sync; don't claim playable co-op.
- OPEN: CL host event log includes ERROR_BINDEXCEPTION during quick Host/Leave/Host; isolate TCP vs presence UDP bind/race in future.
- OPEN: New JPG metadata indexed but binaries could not be displayed via GitHub UTF-8-only tools; no fabricated visual screenshot findings.
- ACCEPTED: older two-phone room lobby 2/4 and real automatic scan JOIN_CONNECTED/ROOM_JOINED logs, not mission/actor proof. No ISO reupload needed.

---

## تسليم جلسة37 — 2026-11-22
المالك سينتقل مؤقتاً لجلسة أخرى. اقرأ memory/sessions/2026-11-22_bo-coop-37-cloud-handoff.md وdocs/CLOUD_HANDOFF_AR.md. جميع أدوات المصدر والخطط والمراجع مرفوعة؛لا تعتمد علىالجهازالمحلي. آخرAPK44946597973/artifact14724535514مفحوصة،ولا سجلاتهاتفجديدةبعدطلبها. الشخصيةالأصليةفقط،لاNPCبديل،ولاsharedStartأوgameplayبعد. افحصCLالحيةأولاًلأيcaptureجديد.


## نتيجة جلسة36 — حزمة جاهزة والتحقق مكتمل
run44946597973 على8bc7f92 SUCCESS؛قرئتjob136975283608:BUILD SUCCESSFUL وoriginalLOD/resetguardsPASS وصورةc5مطابقة. artifact14724535514 نُزل،CRCوالـAPKsha25694400c2d650d772293ab5f3a1d0c72e91d1974dea64a3004d18404e90ccb02d0 تحققااستقلالاً،الحجم21860751B. الحزمةorg.bocoop.black.defaultplayer،originalprotagonist-onlymetadata،alternate_actor_models=false،لاguestwrites/newactor/sharedStart. التقريرالآمنmemory/evidence/DEFAULT-PLAYER-APK-44946597973.json. مطلوبالآنفقطcaptureحيجديدللشخصيةالأصليةوفقdocs/DEFAULT_PLAYER_DEVICE_TEST_AR.md،لاNPCfactoryولاإعادةbaseline. لاphoneproofلهذهالحزمةبعد.
## جلسة36 — أحدث توجيه واجب
المالك يريد شخصيةاللعبةالأصليةعلىكلهاتف،ولايريدbodyبوتأوحليف. اقرَأmemory/sessions/2026-11-21_bo-coop-36-original-player-steering.md والخطةdefault-player-link. تجربةالهاتفالتاليةتتحققمنMODELالمحمّلفيlocalplayerالأصلي،لاتطلبNPCfactorycaptureبعدهذاالتوجيه. لاsharedStartأوبعيدمرئيبعد؛المحليوالـreviewللقارئالجديدPASS،APKالتاليقيدالإطلاق. لاادعاءأنمجردlocalplayerعلىكلجهازينسخالجسموالعالمعبرالشبكة.

## جلسة35 — الحالة الحية
المالك أثبتLANعلىهاتفينبالـAPKالقديمةمعPIN:HOSTING/JOINEDو2/4فيصوروسجلاتCL22:47. النسخةالتلقائيةالجديدةبُنيتوفُحصت:44942892701/artifact14722613067/SHA256b149bc2c...،ولمتُختبرعلىالهاتفبعد. لاgameplayبعد. نتابعإلىfactoryroomرصدإنشاءالأجسامالأصليopt-inمحددبـ32scalarrecord،لاspawnولاStartوهمي. اقرأmemory/sessions/2026-11-21_bo-coop-35-owner-lan-pass-factory-contract.mdوالخطةactor-factory-observation. المحليJVM/Android29/18preparer/2guardوnativeASAN/UBSANPASS؛بناءfactoryroomالتاليقيدالإطلاق.


تحقق Actions على4c9f8bb: source preflight44942174338 وJVM44942174361 وAndroid29 instrumentation44942174364 كلهاSUCCESS. سجلjob136961573450 قرئ ويؤكدTLS_PLATFORM_PASS: زرHost الفعلي بالأسماء الافتراضية فتحTCP24872، joinAutomaticنجح، وBackلميغلقActivity. APK44942174355مازالقيدالبناء؛لااختبارهاتفينبعد.
جلسة34: ثبت أن آخر محاولات الهاتفين توقفت عند تحققPIN الفارغ قبل الشبكة. أزيل الشرط وأضيف اكتشاف الأجهزة الخاملة. ما زال تحققAndroid/هاتفين مطلوباً؛ عدم اكتشاف أقرانVPN خارج/24 أو مع عزل المرور يحتاجIPيدوي. لا gameplayبعد.

## SESSION33 — حالات مفتوحة من تجربة هاتفين
- OWNER-LAN-JOIN: الجهازان على Wi-Fi نفسه لكن لا دليل authenticated JOIN/2/4؛ القديم يحتاج manual IPv4/fingerprint/PIN، ولا سجل network. الجديدة تضيف discovery+safe error codes؛ real device acceptance PENDING.
- OWNER-ROOM-UI: شاشة Activity منفصلة لا تشبه BLACK وتتسبب بمسار خروج/Back؛ source fix Dialog in BlackActivity and concise Flash buttons. لا ادعاء نجاح الجهاز قبل الاختبار. الواجهة styled وليس 100% Flash.
- OWNER-ROOM-LOGS: سجل الهاتف الثاني BLACK-Probe-20261121-194623-826/black_log.txt صفر بايت؛ تم إضافة scrubbed persistent coop_room_events.log لكل هاتف + copy; تحقق من الشمولية على الأجهزة.
- REAL-COOP: لا يوجد START قصة، remote actor+324 مستقل أو game state/mission/combat sync. عدم إظهار زر بدء خادع.
- NETWORK-TRUST: discovery UDP لا يثبت المضيف. أول اتصال يؤكد SHA256 كاملة وPIN؛ LAN broadcast قد يفشل مع AP isolation/VPN.

# المشكلات المفتوحة
- SRC-001: إصدار APK العربي والـXBE وإصدار libmain غير مثبت بالمصدر؛ استخرج hashes أولاً ولا تستعمل offsets.
- SRC-002: Windows recompiler يتطلب default.xbe صالحاً وSDK. لا نملك source الخاص بتعديلات العربية، قد تحتاج لاحقاً من المالك.
- ACTOR: singleton kWorldPlayer؛ لا دليل على قدرة إنشاء جندي متحكم به أو لاعب ثانٍ. تحديد factory/lifecycle/collision/AI/camera.
- NET: UDP probe غير مشفر وغير مصادق، لا يجرب فقد حزم أو NAT أو هاتفين؛ ليس production.
- SAVES: فصل حفظ co-op عن single player، وعدم استبدال توقيع/حزمة المالك.
- LICENSE: لا نشر game.iso/default.xbe/generated recompiled game assets؛ حقوق مستقلة عن MIT.

## نتيجة SRC-001 الفعلية — 44813529439
CI SUCCESS وartifact14666839620، قراءة job136555949209 مباشرة. اجتاز SHA256 للأصل ISO 1793179648 B وlibmain 72073216 B، XDVDFS partition 0، XBE 2813952 B، AArch64. Disc SHA1 c5bfed25449c8212d7a976f15246e87ead57713f مقابل kImageSha1 من ELF 82d0588124e70102ee650f8969c0a56374476234: **غير متطابقين**؛ disc ليس Europe المعروف. لا يثبت السبب (تعديل عربي/إصدار/غيره) ولا source commit أو أن APK الحالي لا يعمل. لا تعطيل gate، لا استخدام offsets غير مثبتة. SRC-001 فحص نجح؛ **قبول هوية المصدر BLOCKED**. التالي SRC-002a: تحديد US hash/commit من مصدر upstream وmetadata الإصدار العربي، ومقارنة XBE header/sections مع الصورة المطابقة المرخصة إن توفرت. إن تعذر، اطلب من المالك فقط default.xbe الأصلي المطابق أو مصدر تعديلات Arabic أو log تشغيل كامل؛ لا تفترض الحاجة للثلاثة. بناء baseline ثم HOOK-001. تقرير: memory/evidence/SRC-001-identity-44813529439.json، https://github.com/alwansan/BO/actions/runs/44813529439. لا APK جديدة ولا gameplay أو اختبار هاتف.

## جلسة 03 — SRC-002a: تثبيت v1.5.0 (2026-11-21)
- APKTool 1.5.0-ar1.0/code35، META-INF revision f3754d6985a8c35204e7df071546e586b10f1806 مطابق tag v1.5.0 upstream (ليس main 8e9c6fc). المصدر المرجعي للعمل على baseline أصبح tag v1.5.0؛ المصدر العربي الكامل غير مثبت. esp.cpp غير موجود في tag؛ layout/cheats/hooks موجودة. راجع docs/V150_PROVENANCE_AR.md.
- CI [44817699656](https://github.com/alwansan/BO/actions/runs/44817699656) completed/success؛ تحقق SHA256 الرسمي، ومقارنة ELF حقيقية، job136570202480، artifact14669549225. الرسمي libmain.so 17957208 B، العربي 72073216 B؛ كلاهما AArch64 وkImageSha1=82d0588124e70102ee650f8969c0a56374476234. .dynstr/.dynsym/.rodata SHA identical؛ .text مختلف، file SHA مختلف. جميع 6 hook-symbol candidates موجودة في الاثنين (بحث substring فقط؛ لا إثبات function semantics). disc الموجود c5bfed254... لا يطابق compile image، لا نُضعف الفحص. النتيجة تثبت release provenance وcompile image match الرسمية/العربية، لا source العربي المطابق ولا compatibility للـISO. دليل memory/evidence/SRC-002a-release-comparison-44817699656.json. NET regression 44817699458 SUCCESS.
- التالي: تحقق من نسخة قانونية لـdefault.xbe SHA1=82d0588124e70102ee650f8969c0a56374476234؛ المالك قد يملكها من نسخته الأصلية. لا تستعمل encrypted upstream XBE أو تدّع تحويل XBE العربي. إن لم تتوفر، واصل شبكات C++ مستقلة/اختبارات Android دون game assets. لا APK ولا gameplay. يمكن فحص hash الدوال المرشحة في ELF كدليل إضافي فقط؛ لا كتابة ذاكرة. لا نحتاج نشر أصول جديدة في public repo. إذا رفع المالك نسخة XBE مطابقة فليكن في مسار خاص مع صلاحية مشروعة، لا تُرفع للفرع العام.

## NET-003/004
- NET-003 Linux and Android cross compile PASS 44834027711. **Android phone execution untested**. CLI no game integration, no scene actors, no 4 device test. See `memory/evidence/NET-003-cpp-android-44834027711.json`.
- Packet session token is predictable/not cryptographically authenticated; no packet encryption, replay protection on join, IP roaming, NAT traversal, congestion/reliability. Use laboratory only, private networks. Binding 0.0.0.0 on trusted LAN is opt-in.
- NET-004 needs two Android phones with matching permissions/ZeroTier, device logs/performance; no public forwarding of port 24872.

## جلسة 05 — تحقق محلي ومسار بديل للتحليل فقط
- بداية الفرع الحية `fcf4b85b0b5a5ae3057a0cec57b1c2b342706809`؛ أحدث Python CI `44834294477` completed/success.
- فحص الملفات المحلية أعاد إثبات disc XBE SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f` وcompile image `82d0588124e70102ee650f8969c0a56374476234`؛ لا تطابق.
- المالك أكد أنه لا يملك XBE الأصلي المطابق أو مصدر التعديلات العربية. لا تكرر طلبهما كأنهما متوفران.
- فحص ساكن محلي: title `45410083`، retail entry `25A3F` يطابقان config v1.5.0؛ 13 قسماً اجتازت internal SHA1 section digests. هذا لا يثبت أن الكود يطابق الصورة الأصلية أو صحة offsets.
- `weapons.cpp` مثل `esp.cpp` غير موجود في tag v1.5.0؛ المراجع السابقة من main لا تستخدم لإثبات baseline.
- أضيف workflow candidate-audit لتحليل وإعادة ترجمة ساكنة في Windows بواسطة المصدر وSDK المثبتين؛ لا يبني المحرك/Gradle/APK، ولا ينشر مشتقات اللعبة. انتظار نتيجة CI قبل أي قبول.
- محلياً: unittest المصدر 2/2 PASS وPython two-client UDP selftest PASS؛ لا هاتف/Android runtime.

### أول CI للمرشح — تصحيح toolchain
Run 44840599544 على code e0e1378843e7a727feb1cfe35d7404ebaafb0e2c فشل في بناء أدوات التحليل قبل تشغيل recompiler. سبب مثبت من log job136647712886: global LLVM Clang20 اختير عبر PATH مع MSVC14.52 الذي يطلب Clang22 (`STL1000`). التعديل يختار `$VCINSTALLDIR/Tools/Llvm/x64/bin/clang-cl.exe` صراحة، كما يفعل build.bat المرجعي، ويسجل الإصدار؛ لا تعطيل لشرط STL ولا تغيير SDK/ملف اللعبة. لا نتيجة توافق للمرشح من التشغيل الفاشل.

### SRC-002b static probe — نتيجة حقيقية وخطوة فحص أعمق
Run 44840960136، job136648896882 على code dd5261a28db3443a6bacbccc7e8fdd1114e818a2 SUCCESS؛ bundled Clang22.1.3. تحليل المرشح c5bfed... نجح بنيوياً مع config/SDK المثبتين: 15042 functions و798292 instructions، لكن statistics تحتوي تعليمات غير منفذة؛ لا baseline/توافق gameplay. artifact14680518020 metadata فقط. أضيف فحص مستقل xbe_symbols لمقارنة symbols/data/ABI، وتقرير عن دوال host الغائبة والتعليمات غير المنفذة فيها وعند hooks؛ انتظار CI الجديد. لا تحويل الصورة القديمة أو تعطيل تحقق hash.

### إغلاق فحص الرموز — 44841534370
Run 44841534370 على code 08fd55b4cb1640b4b56445d43e1801dbc635b1a3 completed/success؛ job136650848324 وartifact14680348280. استقلالاً: symbols108/154 وdata6/11 مطابقة؛ البقية46/5 غير مكتشفة أو غير محسومة. ABI11/11 غير محسومة (`?` أو غياب)، لا تعارض موجب مثبت. جميع host-targets28/28 موجودة؛ لا trap لتعليمات غير مدعومة داخل هذه الدوال ولا عند مواضع hooks، لكن بقية الناتج يحوي تعليمات غير مدعومة. **لا تعادل هذه الفحوص إثبات semantics أو layout أو spawn**. الدليل memory/evidence/SRC-002b-deep-44841534370.json.
- تقرير Android upstream #33 يسجل نفس c5bfed... مع v1.5.0 وتحميل Vulkan: https://github.com/RanggaCasper/Black-Android/issues/33 ؛ ليس إثبات تشغيل نسخة المالك أو توافق co-op، ولا دليل أن اختلاف البصمة سببه التعريب.
- سُئل المالك إن كانت نسخته تدخل مهمة؛ الإجابة/سجل الهاتف لم يصلا بعد. دليل جمع سجل التشغيل: docs/DEVICE_BASELINE_AR.md. هذا طلب تشخيص للنسخة الأصلية، لا تجربة APK جديدة أو تعديل ملفات الحفظ.
- مسار الأصل82d058... ما زال غير متاح؛ مسار candidate c5bfed... لم يغلق، يحتاج حسم static ABI/offsets ودليل runtime. لا تعيد بدء مخابر الشبكة أو طلب XBE المفقود دون دليل جديد.

## جلسة 06 — حسم معنى ABI غير المحسوم
فُحص HEAD08c545a8389f509058ff49cfd62b187dae6f6fca وActions مباشرة؛ آخر run44841534370 completed/success ولا run حي آخر. فُك ترميز x86 محلياً في مجلد مؤقت لجميع مواقع ABI11. الاستخدامات المشاهدة للسجلات/stack تدعم إعداد المصدر (edx/eax/esi/ecx/ebx وret4/16 حيث شوهدت)، لا تثبت ABI/semantics كاملة. الدليل memory/evidence/SRC-002b-manual-abi-inputs.json؛ لا game bytes/assembly في المستودع، وحذفت المشتقات المؤقتة تلقائياً. لا تعتبر46 نتيجة scanner فارغة تعارضات موجبة.

الحد الحالي: نحتاج تحديد الصورة التي تحملها النسخة المثبتة فعلاً. AssetFolders يبحث في Black المشترك وفي ملفات التطبيق؛ لذلك قد يكون ISO المرفق مختلفاً عما يقرأه الهاتف. نتيجة دخول المهمة والسجل ما زالا غير واردين. لا نطلب XBE المفقود ثانية. لا نبني gameplay فوق افتراض أن القرص المرفق هو المستخدم فعلاً، ولا نقول إن APK جماعية جاهزة.

## جلسة07 — وصول تجربة الهاتف الفعلية وبناء candidate منفصل
المالك أكد أن النسخة العربية تدخل القصة وتعمل دون مشاكل، ونسخ بيانات التطبيق محلياً. قرئ السجل الخاص دون نشره: build f3754d6، API35، Adreno730، loaded XBE c5bfed... مع another release، Exit0 بعد146s. تأكد SHA256 لنسخة ISO من الهاتف=f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62 مطابق للـmanifest. هذا يحسم أن الصورة المرفقة هي المستخدمة فعلاً؛ لا تعاود طلب تجربة الأصل أو XBE المفقود. نجاح المهمة من إفادة المالك، لا من instrumentation داخل log.
السجل يحوي `quit: ConfirmWarning, FEAudio failed` في مسار exit_menu.cpp؛ سُجل ولم يخفَ، ولا نفترض منه عطل قتال/صوت أثناء المهمة. دليل metadata فقط DEVICE-existing-owner-runtime.json؛ لا raw log أو حفظ في GitHub.

بدأ مسار بناء أساس مرشح مستقل من نفس c5bfed image الفعلي بالمصدر/SDK المثبتين. ليس إعادة إنتاج ثنائية للمصدر العربي ولا قبول ABI كامل؛ compile image سيتولد تلقائياً من المدخل الحقيقي دون تعطيل match. الحزمة org.bocoop.black.baseline، Java namespace/JNI الأصلي محفوظ، native/src/config لم تُعدل، حفظ منفصل، label NO CO-OP، updater محجوب في هذه الحزمة. لا أصول لعبة في APK. هذا اختبار baseline يلزم قبل HOOK/ACTOR، لا استبدال لهدف co-op.
خطة التنفيذ docs/superpowers/plans/2026-11-21-candidate-baseline.md؛ الاختبارات المحلية4/4 PASS (بدأ test preparer قبل وجوده ثم نجح بعد التنفيذ)، YAML/Python syntax وdiff check PASS. CI baseline مجهز ولم تثبت نتيجته بعد.

Baseline CI بدأ فعلياً: run44845056293، job136662594366، code6aa718839d912d8924af62f11c4c55999847c24e؛ تابع نفس run ولا تعيد تشغيله بسبب مهلة رصد. قبول البناء/الفحص/الهاتف لم يثبت بعد. دليل تجربة الناتج عند نجاحه docs/CANDIDATE_BASELINE_DEVICE_AR.md. Python regression44845056127 SUCCESS؛ source audit44845056215 قيد الفحص.

### Baseline أول محاولة — تشخيص Windows CLI
Run44845056293 على code6aa718839d912d8924af62f11c4c55999847c24e فشل job136662594366 قبل أي native compile: Gradle رأى Task `.5.0-baseline-candidate1` بسبب tokenization لوسيط PowerShell غير المقتبس. اقتبست وسيطي -PversionName و-PversionCode كـstrings كاملين؛ لم يتغير source/SDK أو إعداد اللعبة. الاختبارات وتجهيز الحزمة وإعادة الترجمة نجحت قبله، وAPK/metadata uploads تخطيت كما يجب. source audit44845056215 SUCCESS وUDP44845056127 SUCCESS. لا APK أو runtime acceptance من هذه المحاولة.

إعادة baseline بعد إصلاح quoting تعمل فعلياً: run44845637359/job136664530136/code5212673e5bc92bbe30a7e001cc8e26924a14f0af. اجتازت تجهيز الأدوات والاختبارات/recompiler/Android SDK ووصلت إلى Gradle Android compile عند آخر رصد. هذا handle حي يتابع، لا يعاد تشغيله بسبب timeout ملاحظة. لا APK مثبتة بعد.

### بناء candidate فعلي — 44845637359 SUCCESS
Run44845637359/job136664530136 على code5212673e5bc92bbe30a7e001cc8e26924a14f0af completed/success. Gradle BUILD SUCCESSFUL8m17s. Artifact14682519899 `black-baseline-candidate-android-arm64-NO-COOP`، APK21727055B SHA2564ead325d67772a1c877b9f073aebc0e9eaaab45e9a0028d2380645c4bc2f69f7.
فحوص APK الفعلية: packageorg.bocoop.black.baseline، activitycom.racas.black.BlackActivity، ARM64 فقط، debug signature، لا صلاحيات تخزين واسعة أو ISO/XBE/generated source في APK؛ native compile-image SHA1=c5bfed25449c8212d7a976f15246e87ead57713f يطابق XBE الحقيقي. تنزيل artifact وإعادة فحص ZIP/APK SHA وELF identity محلياً نجحا مستقلاً. metadataArtifact14683233366. لا Android runtime جديد قبل تجربة المالك، ولا co-op أو read-only gameplay hook أو spawn منفذ.
الدليل memory/evidence/BASELINE-candidate-build-44845637359.json؛ رابط artifact https://github.com/alwansan/BO/actions/runs/44845637359/artifacts/14682519899 . الملف المحلي outputs/baseline-44845637359 خارج repo. دليل التجربة docs/CANDIDATE_BASELINE_DEVICE_AR.md. لا تعيد طلب تشغيل APK الأصلية؛ ننتظر تجربة الحزمة الجديدة فقط.

## جلسة08 — تسليم عاجل لجلسة أخرى
المالك جرّب APK baseline الجديدة وأكد عدم وجود تعطل/مشاكل، وقدم بيانات org.bocoop.black.baseline محلياً. log يؤكد image c5bfed... دون another release وExit0 عند162s. missing image عند1s سبق إكمال الاستيراد عند24s وليس فشل baseline. FEAudio عندالخروج ما زال معروفاً. دليل metadata DEVICE-rebuilt-baseline-owner-runtime.json؛ raw log وISO/hdd لا تنشر. لا تطلب إعادة تجربة baseline أو XBE المفقود.

بدأ HOOK001 اختيارياً للقراءة فقط، ثم طلب المستخدم إيقاف التنفيذ وتسليم كل شيء سريعاً قبل انتهاء حد الاستخدام. كود WIP محفوظ في coop/probe/{probe_core.hpp,probe.h,probe.cpp,test_probe.cpp} وخطة docs/superpowers/plans/2026-11-21-readonly-probe.md. **ليس متصلاً بالبناء أو المصدر بعد، لا APK probe ولا CI له ولا تجربة هاتف.** core test نجح محلياً بالأمر clang++ -std=c++20 -O1 -Wall -Wextra -Werror -pedantic coop/probe/test_probe.cpp -o /tmp/black-probe-test && /tmp/black-probe-test. هذا اختبار host فقط، sanitizer/Android لم يجريا لهذا الكود.

التصميم: package جديدة org.bocoop.black.probe1 لتجنب اختلاف debug signing مع baseline والحفاظ على الأصل. opt-in زر Start capture عند التشغيل، native JNI nativeCoopProbeEnable غير مضاف إلى Java بعد؛ لذلك لا تنشط النسخة الحالية من sampler وحدها. لا شبكة أو writes إلى guest أو spawn أو استدعاء guest مجهول. process_vm_readv(getpid()) للقراءات من العملية نفسها، bounds32/host overflow/partial reads/NaN guards، لا memcpy مباشر من guest pointer. سجل محلي logs/coop_probe.jsonl. حدود:64 pointer/vtable pairs لكل world epoch،256 attempts/s،32 records/s،2048 records،180s من world صالح،1MiB؛ انتظار world محدود300s في runtime. IDs مجرد correlations؛ إعادة استعمال نفس address/vtable قد لا تكتشف. Animation owners ليست character تلقائياً، لا تقرأ pos/health منها. player health فقط عند local=world+0x30، LOD يقرأ pos +A0، resource roots +320/+35C مؤقتة الدلالة. errno وfloating environment تحفظ عبر HostState.

## ما لم يُنجز قبل التسليم
1. مراجعة runtime WIP كوحدة كاملة؛ يحتاج #include <algorithm> صريح لاستخدام std::copy. catch-all يعطل capture لكنه لا يغلق fd أو يسجل السبب؛ راجع مسار الخطأ. إعادة Enable بعد End ليست مصممة لإعادة بدء التسجيل؛ اجعل زر مرة واحدة/حالة واضحة. راجع انصاف/rate rotating-ID sampling وثبات IDs وepoch/reset عبر reload؛ لا تعتبرها lifecycle مثبتة.
2. أضف prepare_probe.py مع preflight anchors قبل أي كتابة، يرث private import/no broad storage/updater isolation من prepare_baseline.py، يغير package/label إلى probe1 وREAD ONLY، ويضيف Java opt-in/native declaration. حافظ على JNI class com.racas.black.BlackActivity. لا توقع بمفاتيح الأصل ولا تستبدل الأصل أو baseline.
3. اربط source cheats.cpp entry sub_001E5FA0 بنداء ObservePlayer(ctx.ecx,base)، وhooks.cpp entry sub_0017B130 بنداء ObserveAnimation(ctx.esp,base). لا تغير __imp calls أو guest context. انسخ probe sources إلى reference/src/coop/؛ glob الموجود src/*/*.cpp يشملها بلا تعديل CMake.
4. new wrapper sub_001C55A0 في probe.cpp يفترض generated __imp__sub_001C55A0. أكد تعريفه الحقيقي في generated code/mapping داخل CI fail-closed قبل link، وليس مجرد اسم في source. لا تبدل offsets أو تعطّل image hash check.
5. أضف workflow probe منفصل مبني على black-coop-baseline.yml: Linux host ASAN/UBSAN core tests، Python preparer tests، Android APK build/identity/package/signature/assets verification. baseline workflow لا يحتاج إعادة تشغيل لمجرد WIP غير موصول. راجع مستقل ثم Android device capture.
6. لا ترفع raw phone logs، saves، ISO/XBE، generated game source، أو signing credentials. المدخل ISO المرفوع والمقسم كاف في CI؛ راجع manifestrestore/source_audit.

## أدلة مستقلة للمواقع الجديدة
مراجعة agent read-only ثانية للـXBE ذاته:0x1C55A0 prologue/preceding padding، caller0x1C61F4 يسبقه pushEDI عند1C61F3، callee [EBP+8] عند1C55A9 ثم ret4؛ إذن entry[ESP+4] object. عند1C561E load16B vector object+A0 ثم camera distance. object+320 pointer محمل1C55AC ومفكوك1C55B9؛ +35C pointer مفكوك1C5491/1C54C0. تعريف generated ما زال يجب فحصه في CI. player ECX يؤكده1E5FAD ومصدر cheats؛ animation stack/controller+0 يؤكده1_7B130 disassembly وhooks.cpp. لا full-body model أو factory أو playable second actor مثبت. مشتقات التحليل المؤقتة حُذفت.

## مصادر reader
AOSP Android15 SYSCALLS.TXT يحتوي process_vm_readv all، app/common blocklists لا تحتويه. قرئت المصادر الأساسية https://android.googlesource.com/platform/bionic/+/refs/tags/android-platform-15.0.0_r9/libc/SYSCALLS.TXT وSECCOMP_BLOCKLIST_APP.TXT وSECCOMP_BLOCKLIST_COMMON.TXT. ليس هذا إثبات runtime على هاتف المالك أو كل OEM؛ reader يجب أن يفشل مغلقاً إذا رجع EPERM/ENOSYS. لا root أو تجاوز صلاحيات أو معالجة SIGSEGV للحيلولة دون حماية الذاكرة.

## جلسة09 HOOK001 active risks
- Fresh Windows probe1 build pending run 44852419929; confirm generated __imp__sub_001C55A0 and JNI symbol; fail closed rather than bypass. No device capture yet.
- Bounded read-only observational IDs are not actor lifetimes, no proven full-body factory. process_vm_readv on Android must be device tested later, no root/bypass. Java one-shot optional prompt.
- Uploaded ZIP parts present only in this session; repeated ISO content already exists in BO manifest; see docs/OWNER_ISO_UPLOAD_AR.md. No raw asset upload to GitHub.

## Windows Probe APK — first result PASSED (not phone acceptance)
- [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) completed SUCCESS, job136686638837: real Windows/NDK Android ARM64 build with newly injected read-only observer hooks, generated LOD symbol preflight, JNI export, source change allowlist. Debug APK package `org.bocoop.black.probe1` with separate storage and label BLACK Probe 1 - READ ONLY, no internet/network/broad storage permission, no game ISO/XBE in artifact. Built compile image c5bfed25449c8212d7a976f15246e87ead57713f matches owner ISO. APK 21,738,227 B, SHA256 `531f6cf29fdcc29db1cb5ab39971fab2670672a389e33b0b2e50597ecfaae0fb`; artifact14686665295, metadata14685973542. Exact CI evidence `memory/evidence/HOOK001-probe-candidate-44852419929.json`.
- This first build belongs to source `f911ef9e5b6b83ff19aac39406af8c70d3814f74`. A small kernel namespace convention patch was committed after that run; newer [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) still pending/in-progress at writing. Do not claim updated head APK verified until its result. **No probe phone test or gameplay/co-op acceptance.**

## HOOK001 session10 risk & correction
- Local player could starve under 64 observations. Reserved first slot; source and host tests pass 44854667613, Android new rebuild not yet confirmed. ID1 may be reused across local respawn in same world; identifiers remain correlations only, not actors.
- Actual phone support for process_vm_readv/getpid under OEM Android and LOD/vtable filtering, log directory/access and clock limits is **not demonstrated by CI**. Do not infer from build alone.

## تحديث جلسة10 — تحليل الخصوصية والنتائج الحية
- [44854855170](https://github.com/alwansan/BO/actions/runs/44854855170) completed SUCCESS for privacy-safe probe log analyzer tests (synthetic only), on code bcbf1964b7fb378ad17fed2724aaeefbab91de46. Added bounded diagnostic reason-code flags: capture_end_not_seen, no_samples, no_player_samples, no_lod_samples, no_animation_samples, reader_unavailable, local_output_error. No raw pointer, position, names, thread/epoch IDs or raw logs in summaries. Tests include empty/permission and truncated capture. These flags DO NOT prove model absence or factory success.
- [44854855083](https://github.com/alwansan/BO/actions/runs/44854855083) Python UDP regression SUCCESS. Android candidate run44854667587 was in progress (Gradle native compile) at this documentation update. Do not infer Android runtime from host CI.

## Session11 remaining risks
- Old reserved-slot registry fails to promote previously NPC-classified local object; fixed with slot serial in f7abf7de, host tests pass44855984868. Android current run44855984997 in_progress; runtime unproven until phone.
- Serial prevents stale per-slot cooldown but **does not establish actor lifetime**. Registry matches address/vtable and resets on changed world pointer; same-pointer reload reuse may go unnoticed.
- New NET-004 inbox is isolated and not connected to guest/thread hooks. No secure session identity, delivery reliability, overflow recovery, or real-world scene epoch yet. Must not be treated as production co-op.

HOOK001 starvation under LOD: reserved per-second/lifetime player opportunity without expanding global limits in e9ad056; host CI44856368591 PASS. Android run44856368592 pending. User phone runtime still unverified; sample absence may be caused by no hook, reader restriction, vtable check or timing.

## NET004-PREP composition test (2026-11-21)
- Code commit a5d164ca6f5f19bb807851429c7ba294a5d572b4 wires two previously isolated *test components* under coop/bridge/test_transport_pipeline.cpp. A deterministic host validates Join/Accept and Input token/endpoint/sequence; a transport worker stages value-only commands into SceneInbox; the owning simulated game thread drains them into a mock scene. Negative tests cover forged endpoint/token, duplicate replay, wrong drain thread, and scene reload dropping stale input. No actual network socket is opened by this composition test (separate Linux UDP socket tests exist), no BLACK hooks/actor memory access/gameplay, no authentication or phone test.
- [44856542339](https://github.com/alwansan/BO/actions/runs/44856542339) completed SUCCESS with ASAN/UBSAN and strict C++20; input path tested synthetically only. Keep wording «mock host→scene pipeline», not co-op, and do not treat a pseudorandom lab token as secure authentication.

## جلسة12 — remaining runtime blockers
- Newest Probe1 fairness APK [44856368592](https://github.com/alwansan/BO/actions/runs/44856368592) CI SUCCESS, but never exercised on phone. Android security restrictions may cause process_vm_readv to fail closed; malformed logs or missing LOD are not evidence of no actor.
- `correlation_overlap_counts` compares per-(world epoch, id) **temporary** correlations; same-address reuse and callback timing may create false overlaps/absence. Synthetic test success alone does not establish a full-body model, an allocator/factory or correct game-thread ownership.
- Production co-op still lacks actor spawning/ownership, combat & mission sync, secure multiplayer transport and two-phone runtime tests. Do not describe mock scene inbox as game integration.

## Final APK extraction verified, session12 (2026-11-21)
- Downloaded GitHub Actions **artifact14687833595** from run44856368592 through GitHub connector into this session's isolated container. ZIP extraction yielded actual `black-probe1-candidate-READ-ONLY-NO-COOP.apk` at **21,738,563 bytes** SHA256 **371de65dd7972d1505713d31d2a41080aa99877f848ac35a24fefe8b825ca2d7**, exact CI metadata match. Checked APK ZIP has16 entries and three ARM64 native libraries: libSDL3.so, libc++_shared.so, libmain.so; no files ending .iso/.xbe/.enc/.cpp. Original JNI/signature/package checks were verified in Windows job, not re-run by the container.
- Delivered temporary sandbox APK link in chat `/mnt/data/BLACK-Coop-Probe1-ReadOnly-20261121.apk` after verifying exact path; local container files **are not guaranteed durable across sessions**. Durable download remains GitHub Actions run44856368592 (retention 14 days). Do not commit APK or ISO to BO. No phone runtime report yet.

## Session13 issues
- Current CL copy of `coop_probe.jsonl` size **0**, although owner's app file exists: Shizuku shell uid2000 cannot read app-private mode0600. No full record analysis possible. A `run-as` extraction command was provided as optional attempt; success unknown.
- Android15 storage: exporting through MediaStore.Downloads by the owning app needs no all-files permission; Android OEM runtime still requires phone verification. Shared copies accessible to other apps, user opt-in required and text disclosed. If copy fails, preserve original and log minimal generic failure.
- New standalone Probe2 package avoids replacing Probe1 with a potentially different debug certificate, but requires importing ISO into Probe2's own app-specific assets via existing picker. Don't request missing XBE.
- Room menu contains read-only planned host/join status; no actual multiplayer, room creation, reliable network transport or actor cloning; prove Android build then UI runtime.

## جلسة14 — unresolved despite CI success
- Despite APK successful compile and binary presence, **MediaStore Downloads export has not been exercised on Android 15**; provider handling/permissions, file-manager discoverability and exact path may vary. Private log still mode0600 and CL copy 0 bytes. No assertion of export success or actor samples.
- CO-OP / ROOM is currently a sixth internal OPTIONS page, **not** a top-level Flash pause entry immediately below SETTINGS. Verify on owner's UI, then consider safe separate menu hook; don't misrepresent hierarchy.
- Probe2 app has separate app-private assets; import via SAF needed once on that package; baseline saves/ISO remain untouched. No secure production sessions, guest actor creation or gameplay co-op.

## جلسة15 — مخاطر بعد إثبات الرصد الحقيقي
- Raw public CL `coop_probe.txt` exposes guest pointers, positions and animation labels; **do not copy raw lines into BO**. Next user logs should be private or anonymized with `summarize_probe.py` if possible.
- LOD sample's read success for resource320/render35c doesn't prove ownership; observed render35c has only1 distinct nonzero value across13 LOD groups (possibly shared). No body rig ownership guaranteed.
- clip_hash field in source actually reads `clip+0x18`, not proven hashed clip ID; a changing value is **not** 12 animation clip transitions; safe analyzer corrects the name.
- Epoch count only1 and no end event, so object disappearance/reappearance and safe allocator/factory remain unresolved. One observed callback thread is not proof of single-threaded game.
- Current screenshots show only main menu and Probe2 opt-in, not the injected in-OPTIONS page; requested direct menu option below Settings still open.

## جلسة15 — تصحيح CI ومرشح القائمة العليا
- After privacy analyzer field renamed to explicitly `nonzero_unverified_clip_field_0x18_count`, synthetic test used its old key, causing [44863951613](https://github.com/alwansan/BO/actions/runs/44863951613) FAILURE (KeyError only). Fixed test expectation in code `77e5c031ecc1d55cbefdc73a7a1139a714150b82`; [44864020398](https://github.com/alwansan/BO/actions/runs/44864020398) privacy analyzer SUCCESS and [44864020404](https://github.com/alwansan/BO/actions/runs/44864020404) isolated UDP SUCCESS. No Android build needed (only offline analysis/test).
- Pinned v1.5.0 upstream `src/game/exit_menu.cpp` was inspected read-only: title menu already duplicates Options text clip into QuitButton below settings using Flash `CopyButton/Follow/JoinRing`. Strong candidate for independent CO-OP top-level button **between OPTIONS and QUIT**, with no claim implementation yet. Plan and hazards in `docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md`; keep current Probe2 APK intact until isolated Probe3 experiment and device UI proof.

## جلسة16 — source availability and identity caveats
- `alwansan/biso` is **source of extracted game assets**, branch `main` read-only, pinned commit `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`. It is not equivalent to generated C++ source and not proof of actor factory.
- 574 names/byte sums and 5 independent file SHA256 match owner ISO, but a full 574/574 SHA256 comparison was not yet completed. `coop/biso_manifest_audit.py --iso` requires both owner ISO and manifest locally; public CI must remain metadata-only.
- Repacked XISO from restored file tree may differ bytewise from original even when playable; don't bypass original-image identity gates or mutate default.xbe protection without explicit separate reason.
- /mnt/data parts and extracted ISO are **currently** available only; not guaranteed durable across conversations. Check GitHub BO manifest and pinned biso before asking owner to re-upload anything.

## Clarification of source identity gate
- 574 file SHA256 values from the actual ISO match pinned biso **manifest** in an order-sensitive 256-bit digest. The posted `part-0000*` data blobs themselves have not been downloaded and hashed. Source consistency is strong; do not state repository blob corruption was excluded by independent hash verification.

## جلسة17 — ACTOR-001A observation coverage + remote proxy staging (2026-11-21)
- Returned to owner priority after biso validation. Read actual source (`coop/probe/probe.cpp`, `coop/probe/probe_core.hpp`, `coop/bridge/scene_inbox.hpp`, `coop/native/net_core.hpp`), Probe2 evidence and latest plan. Current `Registry.Id` is an epoch, pointer/vtable **temporary correlation**, with serial internal to slot but not a guaranteed object generation. Round-robin non-player sampling may produce silence even if NPC remains active.
- Derived aggregate time-window analysis from owner CL Probe2 capture **without copying raw file into BO**: 13 LOD correlations, 13 animation-related; 9 first observed >=10 sec after first captured sample, 2 with >=10 sec internal observation gaps, zero with last sample >=10 sec before final observed event, 1 epoch, no end. **No proof of NPC birth/death, disappearance, reuse, or resource ownership.** Sanitized evidence `memory/evidence/ACTOR001A-coverage-owner-Probe2-20261121.json`.
- Added `coop/analysis/actor_lifecycle_triage.py` (commit `31240e47f64ca890c3844b1ad9a66aaecba3bfc8`): bounded PRIVATE JSONL reader, per-(epoch,id) gap windows, multi-epoch observations and explicitly false lifecycle/factory claims, outputs anonymous counts only. Tests for partial capture, stale samples, scene epoch split, malformed private input, privacy; privacy workflow [44867782174](https://github.com/alwansan/BO/actions/runs/44867782174) **SUCCESS**. No owner raw log in CI, no APK modifications.
- Added `coop/bridge/remote_proxy_roster.hpp` + `test_remote_proxy_roster.cpp` (commit `33f08e654ba624d4fb44cbdb562128f65fb44894`) and extended `test_transport_pipeline.cpp` (commit `15092111638cd68ba38715712ebe0b0399b8630f`). C++20 **value-only** authoritative toy host roster for player IDs2–4, owner-game-thread guard, scene epoch, monotonic tick, timeout, full-roster absence, increasing generation on return, strict bounded positions/IDs and rollback rejection. Zero guest pointers, no NPC spawn, no render, no combat and **no secure session authentication**. First isolated sanitizer bridge CI [44867858529](https://github.com/alwansan/BO/actions/runs/44867858529) **SUCCESS**; integrated toy host→proxy+inbox rerun [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) **pending at documentation time**. Do not mark final integration accepted until checked.
- Latest BO app still owner-accepted Probe2 for game import/export, top-level CO-OP/ROOM button below settings not yet implemented; current page is within OPTIONS. No Android build warranted for offline-only code. biso pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, original 574 files already matched ISO manifest; no reupload needed.

### Session17 caveats
- Sample first-appearance and gaps do not identify actor spawn/death; limited round-robin samples and one incomplete epoch are confounders.
- RemoteProxyRoster is a bounded **toy value store** (slots2-4), not a real actor handle or render proxy; existing toy transport lacks secure authentication, no host/client game integration. Only ApplyTrustedSnapshot after eventual verified host/transport validation.
- Room menu top-level placement and co-op mission synchronization remain open.

## ACTOR-001A integrated synthetic pipeline CI closure
- [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) completed SUCCESS for source `15092111638cd68ba38715712ebe0b0399b8630f`, job136735683825. Verified job log explicitly prints PASS for bounded SceneInbox, toy UDP Host→ProxyRoster+SceneInbox→mock scene, and value-only remote roster scene/owner/generation/stale tests. C++20 strict `-Wall -Wextra -Werror -pedantic`, ASAN/UBSAN on Ubuntu. **No Android build, no BLACK runtime, no real-player actor, no authenticated transport, no host+phone session**. New tests do not justify declaring ACTOR-001 complete.

## Pinned source review for ACTOR001A death/ownership hooks
- Reviewed real upstream v1.5.0 `src/game/cheats.cpp`, `src/game/hooks.cpp`, `src/game/layout.h` by pinned GitHub commit, and BO's probe observer/Registry. Candidate `sub_001E74B0` handles soldier **damage**, not necessarily final death or destructor; `kCharDying=0x384` is an observed dying flag but cannot prove deallocation. `sub_0017B130` references animation owner/clip, not actor factory. LOD observer is a render-distance hook, absent samples are not actor death. `Registry.World` only increments epoch on changed **world address**, not necessarily new mission if address reused. Precise review `docs/ACTOR001A_SOURCE_HOOK_DECISIONS_AR.md` is **design/reading only**, no changed APK or hook instrumentation.

## جلسة18 — Peterson047 BLACK-XBEMultiplayer reference review (2026-11-21)
- User-supplied repo `https://github.com/Peterson047/BLACK-XBEMultiplayer`; read README, third-party notices, bundled tool scripts `extract_mp_map.py`, `prepare_pvp_audio.py`, `prepare_pvp_weapons.py`, all Windows bat scripts and inspected full91-entry tree. Pin `0ee3ae6411c7f0989c62cbe36b224d3f55043923`. **Windows binary-only TDM alpha** for up to8 users, not Android or story coop. External claimed source `Peterson047/BLACK-Coop:feat/windows-multiplayer@095488c` returns **404** via connected GitHub; don't claim we accessed original native C++ or copy it. No top-level LICENSE; body.bca animation derived from Kevin Iglesias explicitly restricted to that mod, no re-distribution.
- Useful design: remote-body animation concept, standalone net worker, LAN discovery port1510 separate from game port1508, direct IP, host/dedicated topology, strict version rejection, owner ISO-derived isolated map and weapon packs with speech.slb tables; some audio offsets unverified for BO. Keep owner phone HOST, campaign-specific NPC/mission/quest/save separately. Detailed architecture/risks `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`; evidence `memory/evidence/REFERENCE-BLACK-XBEMultiplayer-20261121.json`.
- Cross-check **actual owner biso manifest**: all 13 required `level_00` core/stage files exist, their sizes match Peterson's published map profile, plus 16 DB/WDD weapon pairs. Independent **original** Python `coop/analysis/tdm_reference_profile.py` tests this metadata with negative synthetic tests; [44868801462](https://github.com/alwansan/BO/actions/runs/44868801462) SUCCESS, no source scripts, ISO, XBE or TDM assets copied; code `49d987bc5b8afcff0b796dfda087a80e4422d9cf`. Existing privacy CI [44868801450](https://github.com/alwansan/BO/actions/runs/44868801450) SUCCESS.
- Priority remains ACTOR-001A actual NPC/remote-avatar lifetime and safe body ownership. Next independent LAN discovery codec/rate-limit/ports on native host test, separate top-level menu and game integration later. No playable co-op claimed.

### Third-party TDM reference risks
- Peterson047/BLACK-XBEMultiplayer has **no top-level license** and binary-only native TDM; referenced full source Peterson047/BLACK-Coop unavailable 404. THIRD-PARTY-NOTICES licenses components, not blanket permission to copy mod code/art. `assets/anim/body.bca` explicitly no standalone redistribution. No direct code/binary/media copies.
- Windows TDM design for 8 players and optional dedicated server does **not** establish story co-op NPC, quests, checkpoint/save behavior or Android ABI. Do not adopt offsets or hook semantics blindly. LAN discovery has **UDP port collision** hazard (1508 game/1510 discovery). Discovery untrusted and NOT WAN authentication.

## جلسة18 — LAN discovery codec مستقل نجح في CI (2026-11-21)
- Developed original `coop/native/lan_discovery.hpp` and `test_lan_discovery.cpp`, source code `33bff07535a85f256c50a6a2842c19714148d5fe`. Inspired by Peterson047's *documented architecture only* (game and discovery ports distinct, LAN browser concept), **no source/assets copied**.
- Wire format `BCLN` version1: short query/reply with 32-bit challenge (correlation, **not auth**), game port, occupied/capacity (1..4), printable room name max32, length/byte/version/duplicate port guards and outgoing 1Hz RateGate. Only **codec/state validation**, **no UDP broadcast socket, actual browser, phone JNI integration, encrypted/authenticated session or playable game network**. Suggested game1508/discovery1510 are defaults referenced by Peterson project, configurable later. Android might require system multicast/broadcast considerations; not yet proven on phone.
- [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957) **SUCCESS BOTH JOBS**: native-linux job136739329989 printed passing strict packet parsing, port/name/rate guard,5000 deterministic malformed packets; legacy UDP loopback 2 clients passes. Android ARM64 job136739329666 cross-compiled LAN test binary and `udp_cli` with NDK28 and confirmed AArch64 ELF; **not executed on Android phone**. Existing unrelated UDP mock [44869092971](https://github.com/alwansan/BO/actions/runs/44869092971) SUCCESS.
- Remote author source `Peterson047/BLACK-Coop` remained 404; do not copy its restricted `body.bca`, binary, badges or unlicensed scripts. Actor lifetime/rig/weapon/quest sync remains primary main blocker. Safe discovery design `docs/NET_LAN_DISCOVERY_AR.md`.

### NET-LAN security status
- `BCLN-v1` challenge is correlation only, NOT authentication. No session crypto, trusted peer, WAN safety, real LAN socket or Android broadcast validation. Outgoing rate limiter is not an inbound DDoS limiter; add per-IP limit and source validation before network listener.
- Keep `host_port` distinct from `discovery_port`, test SDK Android/permissions when binding sockets on user device; don't confuse cross-compile success with execution.

## جلسة18 — POSIX UDP LAN discovery loopback فعلي (2026-11-21)
- Added original `coop/native/lan_socket.hpp` and `test_lan_socket.cpp`, source commit `d97c3ffe3bb19c2950acca7db7db3943fee16d2d`. Portable Linux/Android-NDK POSIX socket RAII, default loopback bind, no SO_REUSEADDR to prevent duplicate port consumption, bounded MSG_TRUNC recv, source IP+port checks, 16-slot inbound per-endpoint 500ms PeerRateGate/5s slot expiry, outgoing1Hz budget from BCLN-v1, strict version/nonce/room/port validation. **No broadcast, app/JNI integration, WAN auth/encryption or actual BLACK player**.
- [44869388876](https://github.com/alwansan/BO/actions/runs/44869388876) **SUCCESS** BOTH JOBS on source `d97c3ffe`: Linux job136740257311 used ASAN/UBSAN and validated **actual UDP IPv4 loopback** Query/Reply, source+challenge, duplicate binding rejected, bounded timeout, incoming/outgoing rate caps and synthetic 5000 malformed datagram decodes. Android ARM64 job136740257470 successfully NDK28 cross-compiled test and checked ELF AArch64; **not executed on actual phone**. Separate toy UDP CI [44869388854](https://github.com/alwansan/BO/actions/runs/44869388854) SUCCESS.
- Peterson047 reference remains read-only and unlicensed external binaries/media unavailable for copying. Their Windows Team Deathmatch source `Peterson047/BLACK-Coop` 404; the independent BO room discovery now has a bounded real UDP loopback transport, but it **cannot be marketed as LAN game hosting** until Android sockets interface, LAN network discovery, authenticated join and a real NPC-player render hook exist.

### LAN socket implementation boundaries
- Now real UDP **loopback-only** tests, but still no real subnet broadcast or Android runtime integration. Host NDK compile isn't device test. Fixed per-source table not spoof-resistant; challenge is not auth. Require authenticated session and visibility controls before enabling gameplay.


## ACTOR-001A — تحديث جلسة19
pool32 ثابت وserial مرشح لا يثبتان active lifetime أو remote body؛ دوال acquisition/retire/list ما زالت فرضيات. تفصيل العناوين في memory/sessions/2026-11-21_bo-coop-19-cloud-handoff.md.


### تصحيح عقود pool — جلسة19
اقرأ docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md: +4 capacity/+8 occupied؛ sparse lists32/16/8/8،actor+250 membership،acquisition1DF440→init1DA4F0. إثبات ثابت فقط؛reset/overflow/runtime/model ما زالت مفتوحة.


### متابعة19 — reset مثبت ونطاق المنتج
دالة1D5E00 على مسارAL1 تعيد32 خانة إلى79A0 وتكتبcounter8524=1؛لا هوية دائمة بالserial وحده، ولاepoch بمجرد دخولreset الذي قد يفشل. التفاصيل docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md. نص هدف المالك الكامل محفوظ docs/OWNER_PROJECT_OBJECTIVE_AR.md. تحليل ثابت فقط،لاAPK/CI جديد.


## جلسة20 — pool reader
راجع memory/sessions/2026-11-21_bo-coop-20-pool-reader.md والخطةProbe3. core مستقلbounded sparse snapshot محليPASS،review fix rawarrayreorder مطبق،CIpending؛لاgameintegration/APK بعد. أداءsyscalls وepoch/reset وpackage/export ضمنTask2.


## جلسة21 — Probe3 Android integration
راجع memory/sessions/2026-11-21_bo-coop-21-probe3-device-build.md؛coreCI44871689814SUCCESS،دمجProbe3واختباراتمحليةومراجعةPASS،APKworkflowجديدقيدالإطلاق. لاdeviceproofولاcoop. تابعبناءProbe3حتىالنهايةثمطلباختبارNPCdeath/reload/logمحدد.


### Probe3 core CI مغلق
44872596855 SUCCESS لكلاjob،logs مقروءة؛الدليل memory/evidence/PROBE3-core-44872596855.json. APK44872596889 لايزالجارياً؛تابعهقبلأيطلبهاتف،ثم docs/PROBE3_DEVICE_TEST_AR.md.


## جلسة22 — generatedguardfalsepositive
APK44872596889FAILقبلGradle؛توقعاتصيغةea/castsفيالفاحصصححتوفقSDK،Python12/preparers6PASSومراجعةPASS. اقرأ memory/sessions/2026-11-21_bo-coop-22-reset-guard-format.md وتابعالبناءالجديد. لاAPKجديدةجاهزةبعد.


## جلسة23 — Probe3 جاهز للهاتف
APK44872995034SUCCESS/core44872995111SUCCESS،artifact14695275729 نُزلوفُحصsha256352d78ed1811b918b91f344e0ba613eb7a0a9f1cc43ba93ae952fccb83ffec44. اقرأ memory/sessions/2026-11-21_bo-coop-23-probe3-apk-ready.md؛التالياختبارمالكوفقdocs/PROBE3_DEVICE_TEST_AR.md ثمlifetime/modelanalysis. لاco-op/ISOجديدبعد.


### بوابة الجهاز الحالية
Probe3جاهز؛بعد3goalturnsلايزالcoop_probe.txtالجديدغيرمتاحفيCL/attachments. التنفيذblockedانتظاراختبارمالكNPCdeath/reloadوفقdocs/PROBE3_DEVICE_TEST_AR.md،وليسبناءنسخةمطابقةأوتوسيعmocknet. الهدفالكاملغيرمكتمل.


## جلسة24 — حل انتظار Probe3
وصلcapture090753،56/56poolvalid و2serialreplacements؛الملخصالآمنوالخطةفيmemory/sessions/2026-11-21_bo-coop-24-probe3-owner-analysis.md. actor328controllerbacklinkودخولهdrawمثبتانثابتاً؛التاليinstancegraph. لاتكررطلبProbe3أوbaseline؛لاspawn/render/coopمقبولبعد.


## جلسة25 — رصد instance graph
محلل Probe3 CI44875336266SUCCESS. إضافة Probe4 منفصلة لروابط controller328/owner/runtime/scratch؛محلياً native/JSON/preparer9tests ومراجعة PASS،CI/APK pending. راجع memory/sessions/2026-11-21_bo-coop-25-instance-graph-build.md؛لا duplicateProbe3 ولا co-op claim.


## جلسة26 — Probe4 ready
APK44876183662SUCCESS؛artifact14696342069 نُزلوفُحصhash. راجع memory/sessions/2026-11-21_bo-coop-26-probe4-ready.md؛التالياختبارownerinstancegraph وفقdocs/PROBE4_DEVICE_TEST_AR.md،بلاrepeatProbe3/baseline. analyzergraphextensionsمحلياً19+11testsPASSومراجعةPASS؛CIتحليلهذهالدفعةقيدالإطلاق. لاco-opجاهز.


### محلل instance graph — CI مثبت
44908343745SUCCESS على399cf730؛لاrunجارٍلهذهالدفعة. Probe4APK44876183662/artifact14696342069جاهز،التاليphonegraphcaptureفقطقبلbody/proxyتجريبيحقيقي.


## تحديث جلسة27 — Probe4
قُبل سجل المالك الجديد ضمن الرصد:111/111 pool و548/548 links صالحة،لا مشاكل حسب المالك،لا reload proof أوco-op. لا إعادة Probe4. الخطوة الحالية تدقيق تسجيل/إزالة مجسم إضافي ومسارات إصابته؛full factory مع null controller غير آمن. التفاصيل في `memory/sessions/2026-11-21_bo-coop-27-probe4-owner-graph.md`.

قرار جلسة27 بعد المراجعة: رُفض مجسم null-brain المسجل؛1E7753يقرأ324+90دونguard. التاليacquisitionأصليمن1AF880مع324مستقلوفحصdescriptor/questIDsوالسعات. لاAPKجديد.


## جلسة28 — طلب المالك وتنفيذ title UI
اختبارات الهاتف السابقة فردية فقط؛الآخرونNPC. يريد المالك أجسام بشر مستقلين وانتقالهم مع المضيف بينالمراحل. إذن جديد بفرع تجريبيbiso وتعديلISOعندالحاجة،دونmain. CO-OP تحتOptionsنُفذفيprepare_room_ui.py،مباشرةإلىالغرفةمعBackللـtitle؛12preparer+12rootPython+native routePASSومراجعةمستقلةواضحة،NDKوالهاتفلميثبتابعد. HOST/JOINغيرمتاحينولاجسملاعبثانبعد. راجع`memory/sessions/2026-11-21_bo-coop-28-title-room-ui.md`.

جلسة28/إصلاحCI:run44913282838فشلقبلcompileبسببCRLFفيexit_menu.cpp. اختبارreproducerREDثمLFnormalizationGREEN،13/13preparerPASS؛إعادةCIمطلوبة. المصدرالأولbf9a5c8ليسAPKجاهزاً.

جلسة28/النتيجة: UI APK جاهز،run44913654405 SUCCESS على96f69b6،artifact14710883570،APK SHA256 af133c9b2a74a9714d62ca943bdd209ffc552267a053951085980ad7addfebfe وحجمه21753039B؛نُزل وفُحص مستقلاً. المطلوب أول فحص لموضعCO-OP والدخول والعودة على الهاتف حسبdocs/ROOM_UI_DEVICE_TEST_AR.md. لا Host/Join فعليين أو أجسام بشر متصلين،ولا ISO جديدة. خريطةالربطdocs/COOP_INTEGRATION_ROADMAP_AR.md.


## جلسة29 — قبول UI وTLS فعلي
المالك قبل القوائم وعمل اللعبة؛قُرئت5صوروجميع140pool/692linksمنسجل173522-325صالحة. لا إعادةUI/Probe4. خطأFEAudioواحدبلاcrashمبلغ،السببغيرمثبت. RoomWire/RoomTlsالجديدانينفذانغرفةTLSفعليةعلىJVMمضيف+3عملاءمعpin/PINوheartbeatورفضنسخة/امتلاءوإغلاق. مراجعةمستقلةوجدتpublication-after-closeوmissingPONG؛reproducersREDثمfixesGREENوsuitePASS. Android/discovery/JNIوالأجسام/القصةغيرمكتملة. التفاصيل`memory/sessions/2026-11-21_bo-coop-29-real-tls-core.md`؛الخطةreal-lobby.md.

جلسة29/CI:run44917272041/job136885459965 SUCCESS على591372ba؛logsreadوالـTLSsockettestsPASS. التاليRoomIdentityAndroidKeyStoreواختبارAndroid29emulatorبـAPKsyntheticبلاgameassets؛لميثبتAndroidبعد. لاheavyphonetestقبلحزمةاتصالغرفةفعلية.

## جلسة30 — استعادة تقدم الجلسة الأخرى وDirect TLS Android (2026-11-21)
- Owner supplied shared-chat URL `https://chatgpt.com/s/cx_6b01ba1d7a048191b3880e16288211c7`, but public URL fetch was disabled. **Do not claim transcript was read.** Instead read newest live `alwansan/BO:experiment/black-coop` HEAD starting `c9e4db52ac802371dc6e8ac4fd6b97343f797c4d`, latest `memory/sessions/2026-11-21_bo-coop-29-real-tls-core.md`, STATE, RESTORE, verified CI and docs. State29 supersedes previous session18: owner UI+Probe4 device accepted, code now real JVM TLS room/3 peers+UDP discovery, native body/quest/combat not yet.
- Implemented `coop/lobby/java/com/racas/black/coop/RoomDirect.java` and `coop/lobby/test/DirectTest.java`: strict DNS-free IPv4:port numeric parser, rejects ambiguous zeros/broadcast/multicast/out-of-range/hostname/Unicode control; full 32-byte SHA256 fingerprint in canonical hex or colon notation, then passes into existing `RoomTls.pinnedContext`/real join. Wrong PIN/full pin tested against **real TLS socket**, no game assets/JNI/Flash or gameplay actor. Initial [44920652328](https://github.com/alwansan/BO/actions/runs/44920652328) failed strict `javac -Werror` test-only unused resource warning; corrected on `8df7a68346aed269bbd762449db7b7af5af6ea3f`. [44920763536](https://github.com/alwansan/BO/actions/runs/44920763536) **SUCCESS**, [44921276865](https://github.com/alwansan/BO/actions/runs/44921276865) and [44921892219](https://github.com/alwansan/BO/actions/runs/44921892219) subsequent core regressions SUCCESS.
- Identified Android emulator platform sequence from real logs: 44919439734 cancelled after adb wait hung 18min; bounded every ADB get-state/boot/installer/instrumentation call with `timeout -k` and diagnostic tails in `coop/lobby/run_android_platform_test.sh` (source `a954306c2b4afdde2b3cd5f7ffc85c9401f07226`). Android29 emulator then **BOOTED and installed test APK successfully**, but real Conscrypt TLS handshake failed `SSLHandshakeException: Connection closed by peer` [44920763565](https://github.com/alwansan/BO/actions/runs/44920763565) and [44921276909](https://github.com/alwansan/BO/actions/runs/44921276909). This is **not** a simulated Android pass.
- Added AndroidKeyStore DIGEST_NONE for prehashed TLS signatures per Android KeyGenParameterSpec guidance (source `650d50eb1eae630ea879fa0bb28747601e74630b`), but Android platform [44921495502](https://github.com/alwansan/BO/actions/runs/44921495502) still failed. Added sanitized pre-JOIN server error/key-type diagnostics `903bb225ab1a923b30deffb9be0b0a91c512591b`: [44921892101](https://github.com/alwansan/BO/actions/runs/44921892101) failure revealed `SSLHandshakeException/SSLProtocolException Handshake failed` and **Android Conscrypt requested key type `EC`**, while server identity/keymanager was RSA-only. No secret/PIN/private key logged.
- Implemented separate opaque `AndroidKeyStore` EC P-256 identity alias `BLACK_COOP_TLS_EC_V1` and EC-only X509KeyManager (source `af67599d8cbe7db7f99bf36f1ebc139f0ec4e0c0`) instead of falsely returning RSA key for EC. New platform run [44922289967](https://github.com/alwansan/BO/actions/runs/44922289967) **PENDING at handoff**. Do not mark Android TLS success until CI instrumentation emits `TLS_PLATFORM_PASS`. A new alias avoids silent reuse of old certificate pin; avoid silent identity rotations in any production-installed package.
- Existing owner acceptance remains: Room UI APK package `org.bocoop.black.roomui`, owner photographs show CO-OP below Options in title/pause menu and room page; Probe4 reports 140 pool/692 links on latest owner capture, NO player2/actor factory. No baseline/Probe4/UI retest requested. ISO in BO and `biso` already verified; no reupload needed.

## Session30 Android KeyStore platform blocker
- Android29 emulator boots and installs synthetic APK, but RSA-only KeyManager handshake fails even with DIGEST_NONE. Observed server SSLHandshakeException/SSLProtocolException and requested EC keyType in 44921892101; not a certificate pin bypass issue. EC P-256 identity is being tested, no PASS yet.
- Avoid treating test-only self-signed certificate and full SHA256 pin as production PKI; user must verify pin out-of-band. Do not silently replace already-pinned identities on devices.
- Shared-chat URL inaccessible directly; GitHub session29 live source used. No claim of exact transcription from link.

## Session31 remaining issues
- Android TLS core passed emulator 44922935535. Separate run44923264409 failed at `adb shell pm install` (timeout 124 after staged APK push); runner/emulator install flake, not proven TLS defect. New UI/platform test run44923426931 pending.
- Android Host/Join/Leave forms added as isolated synthetic Activity. No integration with BlackActivity/Flash menu yet; no two-phone LAN test; do not release this as co-op gameplay.
- Real actor creation still blocked by independent +324 ownership, 16-element pool, unguarded damage access and full-body/animation resource verification. Preserve story saves, NPCs, ISO, and main.
## Session32 unresolved gates
- The game Flash/Android bridge is a **source-verified candidate**, [44924220185](https://github.com/alwansan/BO/actions/runs/44924220185) success; Windows/NDK APK [44924346601](https://github.com/alwansan/BO/actions/runs/44924346601) running. No claim that in-game HOST/JOIN has booted on device.
- Network room remains session-only (real authenticated humans, not playable gameplay actors), with physical LAN/firewall and Android lifecycle yet to verify. Flash room UI text is read-only roster, not an actor roster. PIN transmitted inside TLS after full fingerprint verification; it is not public WAN-ready hosting.
- The process-owned `RoomAppRooms` persists when form closes and shuts down with game Activity destroy; lifecycle, pause/resume and player disconnect need device testing. Original `RoomLobbyActivity` test-only stops onStop; the game variant intentionally changes this in staged source.
- Original `roomui` remains signed separately with no network permissions; game branch isolated as `roomnet`. Do not overwrite game saves/ISO.

APK44942174355فشلفيWindowsقبلالبناء: اختبارroom_network_prepare_checksقرأRoomOverlayالعربيباستخدامcp1252الافتراضي. أُصلحread_textبـUTF-8صريح؛ليسفشلاًفيAndroidأوالشبكة. مراجعةالإلغاءالمستقلةأُعيدتولمتجدمانعاًمتبقياً.

## مرجع وتقدم أصول جديد بعد رفع جلسة35
اكتشفنا portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628،LICENSE MIT قرئ. راجع docs/REFERENCE_BLACK_PC_COOP_AR.md؛PC split-screen لديه player-context وpuppets لكنAIلايستهدفP2 بعد،وليسAndroid/network coop. لا نقل أعمىلـslotcopy؛+250عضويةالمجموعةيحتاجحماية. أداةBOالأصليةmodel_inventory.pyقرأَت271ملفاًمحلياًو4286MODEL،76مرشحbodyبـ10keysفريدة؛protagonistغيرمثبت.3اختباراتحدودوتصنيفPASS،لاgeometry/أصولمصدرة. نتائجآمنةفيOWNER-MODEL-CANDIDATES-20261121.json. تبقىخطةactualactor/control/AI/story،ولاStartوهمي.

FactoryAPK run44944691802 على96413485bdde48981c7000ff9f9ff6019740fa7b:coreنجحوالـgeneratedABIguardنجحوبناءARM64قيدالتجميعوقتكتابةهذاالتحديث. لا تطلبهاتفاًحتىنجاحartifactوفحصه.
