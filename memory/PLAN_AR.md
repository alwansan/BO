## OWNER TEST GATE — 2026-11-22
- **CI-FIRST:** compile/sanitize Java/C++/Python, run JVM TLS, Android29 KeyStore emulator, staged Android APK integrity, private XBE static entry audit, synthetic disconnection/rehost/scene/proxy tests until robust.
- **NO PHONE TEST NOW:** v3 lobby transport/Java story facade are NOT real BLACK shared story and cannot justify a physical owner trial. A new APK alone is not a reason to request testing.
- **ONLY MAJOR MILESTONE:** real verified host mission choice+scene loader with consistent guest scene epoch, remote human-controlled ALLY visuals and movement at minimum, then single bundled owner two-phone test. Ideally include world/combat/quest acceptance. One app build on BOTH phones and automatically exported safe diagnostics for all small questions.
- **AUTO CAPTURE:** bounded event codes, per-stage and room state lifecycle, remote spawn/render state, scene epoch match, AI/world sync, performance crashes. Never expose owner assets/addresses/keys.
- Owner instruction committed to AGENTS/MEMORY/RESTORE and docs/MAJOR_DEVICE_TEST_GATE_AR.md. No repeated ISO/model capture requests.

---

## ACTIVE Session39 — verified TLS handoff to RoomSession, engine remains next gate (2026-11-22)
- ENGINE BRIDGE STAGE 1 SOURCE DONE: commits `86faa014` / `5370dcae` provide `RoomSession.bindStoryGameThread`, read-only story status, host-only PREPARE, game-thread scene-loaded READY, host cancel and thread binding generation reset. This still depends on **an unimplemented, independently verified BLACK game-thread callback**; calling these from synthetic JVM test does not prove game loading.
- JVM real TLS test `44954581676` SUCCESS confirmed log strings `SESSION_STORY_BRIDGE_PASS`, `STORY_TLS_PASS`, `ROOM_REHOST_PASS`; source staging `44954581651` SUCCESS; Android29 `44954581718`, Android lobby-only APK `44954581624` verify live before acceptance.
- ENGINE BRIDGE STAGE 2 OPEN: verify c5 image mission choice `sub_00124CF0` and actual game-thread loaded scene/epoch and JNI. Do not launch a second solo campaign on guest. Avoid writing unlock table, guest actors, NPC AI brain or saves from TLS worker.
- BODY, INPUT, WORLD, AI/QUEST, two-phone same-world acceptance OPEN; only original local first-person and independently owned remote human ally fullbody appearance is acceptable.
- Evidence: user nine screenshots 2/4 host+guest; CL last room logs show old rapid Host/Leave/Host bind races; original primary arms-model capture already present. No repeat ISO/probe request. See Session39 docs and RESTORE.

---

## ACTIVE Session38 progress — actual real room TLS mission control
- VIEW-001 value-only local POV vs remote human AllyBody: PASS C++ CI44951984399, REAL engine render OPEN.
- MISSION-001 source gate: PASS CI44951984399, corrected mission index0 and stage0 accepted.
- WIRE-001 encrypted actual host/guest story frames: **PASS TRANSPORT ONLY** CI44952822531 JVM + CI44952822499 Android29; PREPARE/READY/COMMIT/ABORT over TLS, exact room/session/roster and compact slots. Story protocol VERSION/PROFILE=3; old v2 owner app incompatible with new app, so do not compare the two mixed builds.
- TCP quick host restart: CI44953176046 PASS ROOM_REHOST_PASS 4 same-port real TLS sessions, second simultaneous listener denied. Android run44953176054 needs live check before marking PASS on Android.
- ENGINE-001 real BLACK stage selection/load/ready connection: OPEN. Candidate hook sub_00124CF0 must be verified on owner c5 image and proper game thread; do not write mission unlock table or autonomously start two solo missions.
- BODY-001 actual independently owned ALLY visual for OTHER human participants: OPEN. Original player hands/camera remain local. INPUT-001, WORLD-001, phone shared story ACCEPT OPEN.
- Latest owner-tested APK remains run44946597973/artifact14724535514. New story tests do not produce a game-co-op-ready APK. See docs/ROOM_STORY_TLS_PROTOCOL_AR.md and owner Session38 plan.

---

# ACTIVE Session38 — Corrected implementation gates (older checklists below are historical)
Owner direction now: ORIGINAL first-person local on EVERY phone; use ALLY full-body model/rig ONLY for OTHER human network players; never NPC AI. One host-authoritative mission/world, not separately started solo games. New CL default player test exists with 8/8 primary arms-only samples. No repeat phone test for that diagnostic required.

| Gate | Status |
|---|---|
| Owner original primary model evidence | READ/CONFIRMED 8/8 complete, 44 bones, 2 buffers, mask32, arms only; source CL BLACK-Probe-20261121-235407-267 |
| Real LAN lobby | Older 2/4 on two phones ACCEPTED; still lobby only; automatic no-PIN source ready |
| VIEW-001 host+guest first-person/ally state | Source-only CI PASS 44951425780; no real render or game actor |
| MISSION-001 host Prepare/Ready/Commit barrier | Source-only CI PASS 44951425780; no TLS mission wire/game-stage loading |
| WIRE-001 authenticated story control | OPEN |
| ENGINE-001 host-selected real shared stage and client loaded/ready | OPEN |
| BODY-001 human-controlled remote ALLY rig with lifetime/animation | OPEN |
| INPUT-001 local movement/weapon snapshots to remote display | OPEN |
| WORLD-001 AI/combat/quest/checkpoint/stage/save sync | OPEN |
| COOP-ACCEPT 2-4 phones SAME mission and bodies | OPEN |

Prior plans requiring PIN, mandatory fingerprint confirmation, manual port, no ally-body ever, new defaultplayer capture still missing, Windows APK44924346601 pending are obsolete. Use docs/superpowers/plans/2026-11-22-host-story-remote-ally.md and Session38. Latest verified Android APK run44946597973/artifact14724535514, owner test captured, no new APK created by source-only contracts.

---

## تسليم جلسة37 — 2026-11-22
المالك سينتقل مؤقتاً لجلسة أخرى. اقرأ memory/sessions/2026-11-22_bo-coop-37-cloud-handoff.md وdocs/CLOUD_HANDOFF_AR.md. جميع أدوات المصدر والخطط والمراجع مرفوعة؛لا تعتمد علىالجهازالمحلي. آخرAPK44946597973/artifact14724535514مفحوصة،ولا سجلاتهاتفجديدةبعدطلبها. الشخصيةالأصليةفقط،لاNPCبديل،ولاsharedStartأوgameplayبعد. افحصCLالحيةأولاًلأيcaptureجديد.


## نتيجة جلسة36 — حزمة جاهزة والتحقق مكتمل
run44946597973 على8bc7f92 SUCCESS؛قرئتjob136975283608:BUILD SUCCESSFUL وoriginalLOD/resetguardsPASS وصورةc5مطابقة. artifact14724535514 نُزل،CRCوالـAPKsha25694400c2d650d772293ab5f3a1d0c72e91d1974dea64a3004d18404e90ccb02d0 تحققااستقلالاً،الحجم21860751B. الحزمةorg.bocoop.black.defaultplayer،originalprotagonist-onlymetadata،alternate_actor_models=false،لاguestwrites/newactor/sharedStart. التقريرالآمنmemory/evidence/DEFAULT-PLAYER-APK-44946597973.json. مطلوبالآنفقطcaptureحيجديدللشخصيةالأصليةوفقdocs/DEFAULT_PLAYER_DEVICE_TEST_AR.md،لاNPCfactoryولاإعادةbaseline. لاphoneproofلهذهالحزمةبعد.
## جلسة36 — أحدث توجيه واجب
المالك يريد شخصيةاللعبةالأصليةعلىكلهاتف،ولايريدbodyبوتأوحليف. اقرَأmemory/sessions/2026-11-21_bo-coop-36-original-player-steering.md والخطةdefault-player-link. تجربةالهاتفالتاليةتتحققمنMODELالمحمّلفيlocalplayerالأصلي،لاتطلبNPCfactorycaptureبعدهذاالتوجيه. لاsharedStartأوبعيدمرئيبعد؛المحليوالـreviewللقارئالجديدPASS،APKالتاليقيدالإطلاق. لاادعاءأنمجردlocalplayerعلىكلجهازينسخالجسموالعالمعبرالشبكة.

## جلسة35 — الحالة الحية
المالك أثبتLANعلىهاتفينبالـAPKالقديمةمعPIN:HOSTING/JOINEDو2/4فيصوروسجلاتCL22:47. النسخةالتلقائيةالجديدةبُنيتوفُحصت:44942892701/artifact14722613067/SHA256b149bc2c...،ولمتُختبرعلىالهاتفبعد. لاgameplayبعد. نتابعإلىfactoryroomرصدإنشاءالأجسامالأصليopt-inمحددبـ32scalarrecord،لاspawnولاStartوهمي. اقرأmemory/sessions/2026-11-21_bo-coop-35-owner-lan-pass-factory-contract.mdوالخطةactor-factory-observation. المحليJVM/Android29/18preparer/2guardوnativeASAN/UBSANPASS؛بناءfactoryroomالتاليقيدالإطلاق.

APK44942174355 توقف قبلالبناء بسببترميزcp1252فيقراءةاختبارالواجهةالعربية؛أُصلحUTF-8،إعادةالبناءمطلوبة.


تحقق Actions على4c9f8bb: source preflight44942174338 وJVM44942174361 وAndroid29 instrumentation44942174364 كلهاSUCCESS. سجلjob136961573450 قرئ ويؤكدTLS_PLATFORM_PASS: زرHost الفعلي بالأسماء الافتراضية فتحTCP24872، joinAutomaticنجح، وBackلميغلقActivity. APK44942174355مازالقيدالبناء؛لااختبارهاتفينبعد.
الأولوية الحالية جلسة34: تثبيت استضافة بالأسماء فقط وانضمام/اكتشاف أجهزة تلقائي، تحققAndroid وAPK ثم اختبار هاتفين؛ بعدها actor/control/AI/story. PIN والبصمة اليدوية ملغيان بطلب المالك.

## SESSION33 — إصلاح تجربة هاتفين وغرفة LAN
| البند | الحالة |
|---|---|
| فتح UI على هاتفين وتحميل نفس XBE | جهازان CONFIRMED؛ لا JOIN مثبت |
| شاشة Android المنفصلة وBack/Leave | Source fixed via same-Activity overlay؛ هاتفان يعيدان التحقق |
| UDP discovery host announce وJOIN scan | Source/JVM PASS؛ Android LAN الحقيقي NOT YET VERIFIED |
| full TLS fingerprint from untrusted discovery + first-trust confirmation + PIN | Source/JVM PASS؛ phone pending |
| second-device diagnostics | New scrubbed coop_room_events.log + clipboard; phone pending |
| room source preflight | PASS run44930785583 |
| TLS JVM core | PASS run44930785371 |
| latest full APK Windows/NDK | CHECK LIVE Actions for newest run; do not claim APK |
| actual 2/4 phone roster, game START, independent players | OPEN |
Remember: owner tested older room-net APK run44924346601 SUCCESS/artifact14715302629. Do not confuse build success with real connection.

# خطة BO-COOP
| Gate | القبول | الحالة |
|---|---|---|
| NET-001 | عميلان loopback وحالة authoritative مستقلة | PASS 44808411128 |
| SRC-001 | تحقق SHA256/XBE/ELF؛ مطابقة هوية compile image | audit PASS 44813529439، identity BLOCKED (SHA1 mismatch) |
| SRC-002 | إعادة بناء upstream pinned على Windows دون أصول منشورة أو تغييرات gameplay | مفتوح |
| HOOK-001 | تسجيل محلي read-only للحالة دون crash أو تعديل الحفظ | مفتوح |
| ACTOR-001 | spawn + render + move شخصية ثانية دون singleton conflict | مفتوح |
| ACTOR-002 | لاعبان مستقلان وcamera/weapon/collision + cleanup | مفتوح |
| NET-002 | اتصال هاتفين LAN/ZeroTier؛ خسارة/ترتيب/timeout وإعادة اتصال واتصال مصادق | مفتوح |
| COMBAT-001 | authoritative AI/damage/ammo/projectile/props/objectives | مفتوح |
| COOP-001 | مهمة أولى كاملة على هاتفين مع checkpoint | مفتوح |
| COOP-002 | 4 لاعبين وبقية المهام وواجهة غرف وWAN | مفتوح |
قبل كل gate: اختبارات سالبة، نتائج صريحة، لا نسبة اكتمال وهمية. لا بناء APK منشور قبل إثبات الهوية وbaseline.

SRC-002a: provenance/image match قبل SRC-002 baseline. راجع evidence SRC-001.

SRC-002a PASS: v1.5.0 metadata and compile image identity matched official ELF. SRC-002 baseline BLOCKED on exact XBE. See memory/evidence/SRC-002a-release-comparison-44817699656.json. NET-002 isolated native lab may proceed independently.

## مرحلة NET-003 (21 نوفمبر)
| Gate | النتيجة | الدليل |
|---|---|---|
| NET-003a codec/game-neutral Host | PASS Linux ASAN/UBSAN UDP loopback | [44833884316](https://github.com/alwansan/BO/actions/runs/44833884316) |
| NET-003b CLI two processes | PASS Linux loopback | [44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) |
| NET-003c Android ARM64 compile | PASS cross compile, **device untested** | artifact 14677732935 |
| NET-004 device LAN/ZeroTier | OPEN | no Android runtime test |
| NET-005 authenticated reliable transport | OPEN | no production-grade crypto/reliability |
| HOOK-001/ACTOR-001 | BLOCKED by exact game baseline | original XBE missing |

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

## جلسة09 — HOOK001 (read-only)
| Gate | State |
|---|---|
| source preflight + C++ guarded reads ASAN/UBSAN | PASS [44852312025](https://github.com/alwansan/BO/actions/runs/44852312025) |
| Android probe1 opt-in JNI + original generated hook verify | CI [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) pending |
| probe APK verification / signature / isolated storage / no network permissions | pending Windows artifact |
| device mission capture | NOT REQUESTED, only after verified probe APK |
| full-body actor creation / co-op | not started |
Owner ISO uploaded 5 parts and fully SHA-verified. See docs/OWNER_ISO_UPLOAD_AR.md; GitHub copy equivalent.

## تحديث جلسة09 — تجميع Windows ومتغير namespace
- Hook C++ sources الأصلية المستعملة في الإصدار v1.5.0 تستخدم `using namespace kernel;` أمام X86_EXTERN_FUNC/X86_FUNC. أضيفت المطابقة إلى `coop/probe/probe.cpp` commit5c5be3972e2131d4b34bad273f41b89e54de5c74؛ إجراء وقائي وليس دليلاً أن البناء القديم فشل.
- Windows الأول [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) ما زال قيد التجميع عند هذا التوثيق، والثاني بالشفرة الأحدث [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) pending/queued (concurrency). لا تعتبر الأول build نهائياً بعد تغيير source؛ اختبر الثاني وأصلح سجلات الفشل الحقيقية. تحديث preflight [44852913135](https://github.com/alwansan/BO/actions/runs/44852913135) يحتاج تحقق نهاية.

## Windows Probe APK — first result PASSED (not phone acceptance)
- [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) completed SUCCESS, job136686638837: real Windows/NDK Android ARM64 build with newly injected read-only observer hooks, generated LOD symbol preflight, JNI export, source change allowlist. Debug APK package `org.bocoop.black.probe1` with separate storage and label BLACK Probe 1 - READ ONLY, no internet/network/broad storage permission, no game ISO/XBE in artifact. Built compile image c5bfed25449c8212d7a976f15246e87ead57713f matches owner ISO. APK 21,738,227 B, SHA256 `531f6cf29fdcc29db1cb5ab39971fab2670672a389e33b0b2e50597ecfaae0fb`; artifact14686665295, metadata14685973542. Exact CI evidence `memory/evidence/HOOK001-probe-candidate-44852419929.json`.
- This first build belongs to source `f911ef9e5b6b83ff19aac39406af8c70d3814f74`. A small kernel namespace convention patch was committed after that run; newer [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) still pending/in-progress at writing. Do not claim updated head APK verified until its result. **No probe phone test or gameplay/co-op acceptance.**

## جلسة09 — تحليل سجلات الرصد دون نشر البيانات
- `coop/analysis/summarize_probe.py` يدقق start header/حدود2MiB و8192 سطر و4096B للسطر، يحصي event/epoch/thread IDs كأعداد إجمالية فقط، يحذف من المخرجات أي raw position/vtable/animation names/actor pointer/ID/phone-specific path. `capture_analysis_checks.py` يختبر التسريب والحالات السالبة بما فيها سبب End غير قابل للتجزئة.
- [44853385306](https://github.com/alwansan/BO/actions/runs/44853385306) و[44853421222](https://github.com/alwansan/BO/actions/runs/44853421222) completed SUCCESS على synthetic inputs فقط. لا raw phone logs، ولا device runtime proof. تعليمات docs/PROBE_DEVICE_GUIDE_AR.md.

## جلسة 10 — HOOK001 coverage
- [x] Android namespace-aligned previous Probe1 APK CI [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) SUCCESS; phone execution open.
- [x] Prevent local-player sample starvation by reserving local slot; host sanitizer + pinned-source [44854667613](https://github.com/alwansan/BO/actions/runs/44854667613) SUCCESS.
- [ ] Verify Android rebuild [44854667587](https://github.com/alwansan/BO/actions/runs/44854667587) and associated signed APK metadata before recommending artifact.
- [ ] HOOK001 phone runtime: optional capture only, store logs privately, summarize without raw locations; **not** baseline retest.
- [ ] ACTOR-001 (full body proxy) remains unproven; no networked gameplay yet.

## تحديث جلسة10 — تحليل الخصوصية والنتائج الحية
- [44854855170](https://github.com/alwansan/BO/actions/runs/44854855170) completed SUCCESS for privacy-safe probe log analyzer tests (synthetic only), on code bcbf1964b7fb378ad17fed2724aaeefbab91de46. Added bounded diagnostic reason-code flags: capture_end_not_seen, no_samples, no_player_samples, no_lod_samples, no_animation_samples, reader_unavailable, local_output_error. No raw pointer, position, names, thread/epoch IDs or raw logs in summaries. Tests include empty/permission and truncated capture. These flags DO NOT prove model absence or factory success.
- [44854855083](https://github.com/alwansan/BO/actions/runs/44854855083) Python UDP regression SUCCESS. Android candidate run44854667587 was in progress (Gradle native compile) at this documentation update. Do not infer Android runtime from host CI.

## Session11 gate update
- [x] Windows verified reserved-slot Probe1 candidate [44854667587](https://github.com/alwansan/BO/actions/runs/44854667587) SUCCESS; not phone-tested. Artifact14687571586 and memory/evidence/HOOK001-player-slot-44854667587.json.
- [x] Local-promotion and slot serial negative tests [44855984868](https://github.com/alwansan/BO/actions/runs/44855984868) PASS; current Windows [44855984997](https://github.com/alwansan/BO/actions/runs/44855984997) in_progress at recording.
- [x] Game-neutral scene inbox [44856160545](https://github.com/alwansan/BO/actions/runs/44856160545) PASS; no device/BLACK integration.
- [ ] Verify latest APK and optionally first HOOK001 device capture (not baseline re-test).
- [ ] Validate actor lifetimes/full-body before ACTOR-001; no guest writes or production auth claims.

HOOK001 fairness source e9ad056: host sanitizer + pinned-source [44856368591](https://github.com/alwansan/BO/actions/runs/44856368591) PASS, Android [44856368592](https://github.com/alwansan/BO/actions/runs/44856368592) pending. Next verify APK SHA/artifact; older f7 build 44855984997 still running. No device acceptance.

## NET004-PREP composition test (2026-11-21)
- Code commit a5d164ca6f5f19bb807851429c7ba294a5d572b4 wires two previously isolated *test components* under coop/bridge/test_transport_pipeline.cpp. A deterministic host validates Join/Accept and Input token/endpoint/sequence; a transport worker stages value-only commands into SceneInbox; the owning simulated game thread drains them into a mock scene. Negative tests cover forged endpoint/token, duplicate replay, wrong drain thread, and scene reload dropping stale input. No actual network socket is opened by this composition test (separate Linux UDP socket tests exist), no BLACK hooks/actor memory access/gameplay, no authentication or phone test.
- [44856542339](https://github.com/alwansan/BO/actions/runs/44856542339) completed SUCCESS with ASAN/UBSAN and strict C++20; input path tested synthetically only. Keep wording «mock host→scene pipeline», not co-op, and do not treat a pseudorandom lab token as secure authentication.

## جلسة12 — Verified Android build and ACTOR triage
- [x] Close pending Windows [44855984997](https://github.com/alwansan/BO/actions/runs/44855984997) SUCCESS (intermediate promotion), verify APK SHA and artifact.
- [x] Close final Windows [44856368592](https://github.com/alwansan/BO/actions/runs/44856368592) SUCCESS, APK SHA256 371de65dd7972d1505713d31d2a41080aa99877f848ac35a24fefe8b825ca2d7, artifact14687833595, source e9ad056805cabacadcf3882d5ff7aeb5f45df914; report memory/evidence/HOOK001-final-fairness-build-44856368592.json.
- [x] Implement anonymous event-kind correlation analysis under `coop/analysis`; synthetic privacy CI [44857976254](https://github.com/alwansan/BO/actions/runs/44857976254) SUCCESS. No APK rebuild.
- [ ] First private read-only **Probe1** device runtime capture; never repeat accepted baseline. Use docs/PROBE_DEVICE_GUIDE_AR.md.
- [ ] Interpret LOD/animation/position & thread observations conservatively. Confirm actor identity/lifetime and safe factory before ACTOR-001 writes or networked gameplay.

## Final APK extraction verified, session12 (2026-11-21)
- Downloaded GitHub Actions **artifact14687833595** from run44856368592 through GitHub connector into this session's isolated container. ZIP extraction yielded actual `black-probe1-candidate-READ-ONLY-NO-COOP.apk` at **21,738,563 bytes** SHA256 **371de65dd7972d1505713d31d2a41080aa99877f848ac35a24fefe8b825ca2d7**, exact CI metadata match. Checked APK ZIP has16 entries and three ARM64 native libraries: libSDL3.so, libc++_shared.so, libmain.so; no files ending .iso/.xbe/.enc/.cpp. Original JNI/signature/package checks were verified in Windows job, not re-run by the container.
- Delivered temporary sandbox APK link in chat `/mnt/data/BLACK-Coop-Probe1-ReadOnly-20261121.apk` after verifying exact path; local container files **are not guaranteed durable across sessions**. Durable download remains GitHub Actions run44856368592 (retention 14 days). Do not commit APK or ISO to BO. No phone runtime report yet.

## جلسة13 — Actual device evidence + new export/UI gates
- [x] Inspect owner's Probe1 black.log in CL + screenshots: import, game running, probe capture activated, Exit0. Private JSONL not readable; CL file 0B. memory/evidence/DEVICE-probe1-owner-20261121.json.
- [x] Implement bounded MediaStore Downloads export opt-in (`coop_probe.txt`, `black_log.txt`) in Probe2 separate package preserving installed Probe1.
- [x] Add in-game CO-OP/ROOM page as read-only in OPTIONS (native Flash extension, pinned source); host/preparer CI [44860875040](https://github.com/alwansan/BO/actions/runs/44860875040) SUCCESS.
- [ ] Android package2 compile [44860874976](https://github.com/alwansan/BO/actions/runs/44860874976) pending. Verify JNI, native image, package, no game assets/network/storage permissions and exporter compiled; obtain artifact/hash.
- [ ] On phone: optional new Probe2 session and public snapshots at exact Download path + actual menu screenshot. Existing baseline remains tested, no repeat.
- [ ] Use capture samples for ACTOR-001 read-only lifecycle evidence; no guest writes/host/join feature until validated.

## جلسة14 — Probe2 CI/artifact verified and next device gate
- [x] Android final [44860874976](https://github.com/alwansan/BO/actions/runs/44860874976) SUCCESS, source `d8dba8d8da2da69c62718a87f3e6f03f16be0d2f`, artifact 14690036713, APK SHA256 `861fd501497f2251feeb544c56430e084c3b48d3afa87e0d3a65c12e8a1115c0`, verified actual archive/DEX/native strings. Evidence `memory/evidence/PROBE2-android-44860874976.json`.
- [x] Prior e2 Android build 44860718925 SUCCESS but superseded; intermediate 44860822227 cancelled. Host ASAN/UBSAN 44860875040 PASS.
- [ ] First **Probe2 only** Android15 run: check actual MediaStore directory, non-empty `coop_probe.txt`, `black_log.txt`, assess if sampler ends/has records; avoid pushing raw logs to public CL.
- [ ] Verify native in-game SETTINGS -> CO-OP / ROOM sixth page, scroll and back; design change later if owner wants directly under main/pause SETTINGS button.
- [ ] Only after real character trace: pursue ACTOR-001 lifecycle proof. No gameplay write/network integration until then.

## جلسة15 — الرصد الحقيقي للـACTOR على جهاز المالك
- [x] Read CL Probe2 exported JSONL: 400 valid observations (131 player,172 lod,97 animation), one epoch, one observed thread,14 correlations; end absent => partial. Evidence `memory/evidence/DEVICE-probe2-owner-20261121-064812-safe-summary.json`.
- [x] Verify MediaStore export to `Download/Games/cl/black log/logs` **works on device**; Probe2 capture actually recorded events.
- [x] Enrich privacy-safe analyzer: player/LOD vs NPC-like LOD/animation groups, resource/render coverage, in-memory anonymous position variance, animation label changes; field at clip+0x18 semantics **unproven**. Privacy [44863742397](https://github.com/alwansan/BO/actions/runs/44863742397) SUCCESS; UDP [44863742412](https://github.com/alwansan/BO/actions/runs/44863742412) SUCCESS. No APK rebuild needed.
- [ ] ACTOR-001A: prove appearance/disappearance/respawn and model ownership across scene/epoch before factory/clone.
- [ ] In-game CO-OP/ROOM page still not pictured/confirmed; top-level option immediately below `الإعدادات` not yet built. Review safe UI hook points separately.
- [ ] ACTOR-001/COMBAT-001/gameplay network remain unimplemented; no guesswork about remote render proxy.

## جلسة15 — تصحيح CI ومرشح القائمة العليا
- After privacy analyzer field renamed to explicitly `nonzero_unverified_clip_field_0x18_count`, synthetic test used its old key, causing [44863951613](https://github.com/alwansan/BO/actions/runs/44863951613) FAILURE (KeyError only). Fixed test expectation in code `77e5c031ecc1d55cbefdc73a7a1139a714150b82`; [44864020398](https://github.com/alwansan/BO/actions/runs/44864020398) privacy analyzer SUCCESS and [44864020404](https://github.com/alwansan/BO/actions/runs/44864020404) isolated UDP SUCCESS. No Android build needed (only offline analysis/test).
- Pinned v1.5.0 upstream `src/game/exit_menu.cpp` was inspected read-only: title menu already duplicates Options text clip into QuitButton below settings using Flash `CopyButton/Follow/JoinRing`. Strong candidate for independent CO-OP top-level button **between OPTIONS and QUIT**, with no claim implementation yet. Plan and hazards in `docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md`; keep current Probe2 APK intact until isolated Probe3 experiment and device UI proof.

## جلسة16 — مصادر المالك المفككة والتطابق
- [x] Pin `biso@9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, review manifest and restore script; metadata574 original files/578 stored parts, 3 large split files,0 missing,0 size mismatches.
- [x] Independently read owner's **actual ISO** currently mounted; SHA256 `f6d15ff...3ec246c62` passes, XDVDFS574 files/40 dirs, exact byte-sum match, XBE+four additional file SHA256 samples match `biso`.
- [x] Add pinned metadata-only + synthetic XDVDFS CI [44867209688](https://github.com/alwansan/BO/actions/runs/44867209688) SUCCESS; `coop/biso_manifest_audit.py` supports `--checkout` and `--iso` local full verification when permitted.
- [x] Save pinned sources to `memory/STATE.json`, `docs/OWNER_BISO_SOURCE_AR.md`, `docs/ACTOR_BISO_ASSET_INDEX_AR.md`, and restore. Do not rely on sandbox persistence.
- [ ] If needed for ACTOR-001A, perform full per-file SHA comparison of owner ISO and restored biso locally; check format of selected model/animation candidate files without asserting unproven meaning.
- [ ] Continue ACTOR ownership/lifetime and isolated CO-OP top-level UX without modifying successful Baseline/Probe2 or game saves. No playable multiplayer yet.

## Full ISO manifest identity improvement
- [x] SHA256 every one of 574 ISO files and confirm canonical ordered 256-bit digest `b3e56e650e8a2b91af1380204599e9faf8722e98ef9d5d22790b69e798302d6f` equals pinned biso manifest.
- [x] Separate path/size canonical FNV64 digest `dad4e7e9643c8bfb` identical.
- [ ] Independently validate **remote biso blob bytes**, rather than just its manifest entries and tree byte lengths, only when needed through a local checkout/restore to avoid copying 1.7GB publicly.

## جلسة17 — ACTOR-001A observation coverage + remote proxy staging (2026-11-21)
- Returned to owner priority after biso validation. Read actual source (`coop/probe/probe.cpp`, `coop/probe/probe_core.hpp`, `coop/bridge/scene_inbox.hpp`, `coop/native/net_core.hpp`), Probe2 evidence and latest plan. Current `Registry.Id` is an epoch, pointer/vtable **temporary correlation**, with serial internal to slot but not a guaranteed object generation. Round-robin non-player sampling may produce silence even if NPC remains active.
- Derived aggregate time-window analysis from owner CL Probe2 capture **without copying raw file into BO**: 13 LOD correlations, 13 animation-related; 9 first observed >=10 sec after first captured sample, 2 with >=10 sec internal observation gaps, zero with last sample >=10 sec before final observed event, 1 epoch, no end. **No proof of NPC birth/death, disappearance, reuse, or resource ownership.** Sanitized evidence `memory/evidence/ACTOR001A-coverage-owner-Probe2-20261121.json`.
- Added `coop/analysis/actor_lifecycle_triage.py` (commit `31240e47f64ca890c3844b1ad9a66aaecba3bfc8`): bounded PRIVATE JSONL reader, per-(epoch,id) gap windows, multi-epoch observations and explicitly false lifecycle/factory claims, outputs anonymous counts only. Tests for partial capture, stale samples, scene epoch split, malformed private input, privacy; privacy workflow [44867782174](https://github.com/alwansan/BO/actions/runs/44867782174) **SUCCESS**. No owner raw log in CI, no APK modifications.
- Added `coop/bridge/remote_proxy_roster.hpp` + `test_remote_proxy_roster.cpp` (commit `33f08e654ba624d4fb44cbdb562128f65fb44894`) and extended `test_transport_pipeline.cpp` (commit `15092111638cd68ba38715712ebe0b0399b8630f`). C++20 **value-only** authoritative toy host roster for player IDs2–4, owner-game-thread guard, scene epoch, monotonic tick, timeout, full-roster absence, increasing generation on return, strict bounded positions/IDs and rollback rejection. Zero guest pointers, no NPC spawn, no render, no combat and **no secure session authentication**. First isolated sanitizer bridge CI [44867858529](https://github.com/alwansan/BO/actions/runs/44867858529) **SUCCESS**; integrated toy host→proxy+inbox rerun [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) **pending at documentation time**. Do not mark final integration accepted until checked.
- Latest BO app still owner-accepted Probe2 for game import/export, top-level CO-OP/ROOM button below settings not yet implemented; current page is within OPTIONS. No Android build warranted for offline-only code. biso pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, original 574 files already matched ISO manifest; no reupload needed.

### Next gates — ACTOR-001A (session17)
- [x] Synthetic coverage privacy tool and actual sanitized Probe2 gap aggregate (9 late,2 long), run44867782174 PASS; no raw owner data in BO.
- [x] Value-only actor proxy roster scene/thread/generation/stale checks, host ASAN/UBSAN run44867858529 SUCCESS.
- [ ] Integrated toy network→proxy+SceneInbox CI44867924228 pending; inspect logs and fix on branch if failure.
- [ ] Prove actual NPC lifetime/ownership before applying any proxy into BLACK, design minimally invasive opt-in hook for death/reload proof.
- [ ] Implement main-menu CO-OP entry under Settings via separate isolated source-gated experiment; don't touch accepted Probe2.

## ACTOR-001A integrated synthetic pipeline CI closure
- [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) completed SUCCESS for source `15092111638cd68ba38715712ebe0b0399b8630f`, job136735683825. Verified job log explicitly prints PASS for bounded SceneInbox, toy UDP Host→ProxyRoster+SceneInbox→mock scene, and value-only remote roster scene/owner/generation/stale tests. C++20 strict `-Wall -Wextra -Werror -pedantic`, ASAN/UBSAN on Ubuntu. **No Android build, no BLACK runtime, no real-player actor, no authenticated transport, no host+phone session**. New tests do not justify declaring ACTOR-001 complete.

## Pinned source review for ACTOR001A death/ownership hooks
- Reviewed real upstream v1.5.0 `src/game/cheats.cpp`, `src/game/hooks.cpp`, `src/game/layout.h` by pinned GitHub commit, and BO's probe observer/Registry. Candidate `sub_001E74B0` handles soldier **damage**, not necessarily final death or destructor; `kCharDying=0x384` is an observed dying flag but cannot prove deallocation. `sub_0017B130` references animation owner/clip, not actor factory. LOD observer is a render-distance hook, absent samples are not actor death. `Registry.World` only increments epoch on changed **world address**, not necessarily new mission if address reused. Precise review `docs/ACTOR001A_SOURCE_HOOK_DECISIONS_AR.md` is **design/reading only**, no changed APK or hook instrumentation.

## جلسة18 — Peterson047 BLACK-XBEMultiplayer reference review (2026-11-21)
- User-supplied repo `https://github.com/Peterson047/BLACK-XBEMultiplayer`; read README, third-party notices, bundled tool scripts `extract_mp_map.py`, `prepare_pvp_audio.py`, `prepare_pvp_weapons.py`, all Windows bat scripts and inspected full91-entry tree. Pin `0ee3ae6411c7f0989c62cbe36b224d3f55043923`. **Windows binary-only TDM alpha** for up to8 users, not Android or story coop. External claimed source `Peterson047/BLACK-Coop:feat/windows-multiplayer@095488c` returns **404** via connected GitHub; don't claim we accessed original native C++ or copy it. No top-level LICENSE; body.bca animation derived from Kevin Iglesias explicitly restricted to that mod, no re-distribution.
- Useful design: remote-body animation concept, standalone net worker, LAN discovery port1510 separate from game port1508, direct IP, host/dedicated topology, strict version rejection, owner ISO-derived isolated map and weapon packs with speech.slb tables; some audio offsets unverified for BO. Keep owner phone HOST, campaign-specific NPC/mission/quest/save separately. Detailed architecture/risks `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`; evidence `memory/evidence/REFERENCE-BLACK-XBEMultiplayer-20261121.json`.
- Cross-check **actual owner biso manifest**: all 13 required `level_00` core/stage files exist, their sizes match Peterson's published map profile, plus 16 DB/WDD weapon pairs. Independent **original** Python `coop/analysis/tdm_reference_profile.py` tests this metadata with negative synthetic tests; [44868801462](https://github.com/alwansan/BO/actions/runs/44868801462) SUCCESS, no source scripts, ISO, XBE or TDM assets copied; code `49d987bc5b8afcff0b796dfda087a80e4422d9cf`. Existing privacy CI [44868801450](https://github.com/alwansan/BO/actions/runs/44868801450) SUCCESS.
- Priority remains ACTOR-001A actual NPC/remote-avatar lifetime and safe body ownership. Next independent LAN discovery codec/rate-limit/ports on native host test, separate top-level menu and game integration later. No playable co-op claimed.

### Reference adoption gates — Peterson047
- [x] Reviewed public repo tree, docs, scripts, notices; source ref to BLACK-Coop inaccessible (404); pinned commit `0ee3ae6411c7f0989c62cbe36b224d3f55043923`.
- [x] Resource profile metadata-only against owner biso:13 stage-file sizes and16 DB/WDD pairs; CI44868801462 SUCCESS.
- [ ] Add independent LAN discovery codec and port distinctness/rate-limit tests (do NOT expose unauthenticated WAN).
- [ ] NPC actor/render/animation ownership proof for story player2, then optionally remote-body visuals after licenses/format support.
- [ ] Top-level CO-OP room menu + Android package only after safe source integration; no asset/binary source copying.

## جلسة18 — LAN discovery codec مستقل نجح في CI (2026-11-21)
- Developed original `coop/native/lan_discovery.hpp` and `test_lan_discovery.cpp`, source code `33bff07535a85f256c50a6a2842c19714148d5fe`. Inspired by Peterson047's *documented architecture only* (game and discovery ports distinct, LAN browser concept), **no source/assets copied**.
- Wire format `BCLN` version1: short query/reply with 32-bit challenge (correlation, **not auth**), game port, occupied/capacity (1..4), printable room name max32, length/byte/version/duplicate port guards and outgoing 1Hz RateGate. Only **codec/state validation**, **no UDP broadcast socket, actual browser, phone JNI integration, encrypted/authenticated session or playable game network**. Suggested game1508/discovery1510 are defaults referenced by Peterson project, configurable later. Android might require system multicast/broadcast considerations; not yet proven on phone.
- [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957) **SUCCESS BOTH JOBS**: native-linux job136739329989 printed passing strict packet parsing, port/name/rate guard,5000 deterministic malformed packets; legacy UDP loopback 2 clients passes. Android ARM64 job136739329666 cross-compiled LAN test binary and `udp_cli` with NDK28 and confirmed AArch64 ELF; **not executed on Android phone**. Existing unrelated UDP mock [44869092971](https://github.com/alwansan/BO/actions/runs/44869092971) SUCCESS.
- Remote author source `Peterson047/BLACK-Coop` remained 404; do not copy its restricted `body.bca`, binary, badges or unlicensed scripts. Actor lifetime/rig/weapon/quest sync remains primary main blocker. Safe discovery design `docs/NET_LAN_DISCOVERY_AR.md`.

### NET-LAN-001 (Peterson idea independently implemented)
- [x] Portable discovery Query/Reply codec, room and port guards, 1 Hz outgoing rate,5000 malformed packet tests; Linux ASAN/UBSAN and Android ARM64 NDK28 compile SUCCESS [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957).
- [ ] POSIX socket LAN responder/browser isolated mock + per-peer inbound rate cap, Android broadcast viability and explicit JOIN/authentication gate.
- [ ] Separate game/room UI Host/Join; no source/animation media copied.

## جلسة18 — POSIX UDP LAN discovery loopback فعلي (2026-11-21)
- Added original `coop/native/lan_socket.hpp` and `test_lan_socket.cpp`, source commit `d97c3ffe3bb19c2950acca7db7db3943fee16d2d`. Portable Linux/Android-NDK POSIX socket RAII, default loopback bind, no SO_REUSEADDR to prevent duplicate port consumption, bounded MSG_TRUNC recv, source IP+port checks, 16-slot inbound per-endpoint 500ms PeerRateGate/5s slot expiry, outgoing1Hz budget from BCLN-v1, strict version/nonce/room/port validation. **No broadcast, app/JNI integration, WAN auth/encryption or actual BLACK player**.
- [44869388876](https://github.com/alwansan/BO/actions/runs/44869388876) **SUCCESS** BOTH JOBS on source `d97c3ffe`: Linux job136740257311 used ASAN/UBSAN and validated **actual UDP IPv4 loopback** Query/Reply, source+challenge, duplicate binding rejected, bounded timeout, incoming/outgoing rate caps and synthetic 5000 malformed datagram decodes. Android ARM64 job136740257470 successfully NDK28 cross-compiled test and checked ELF AArch64; **not executed on actual phone**. Separate toy UDP CI [44869388854](https://github.com/alwansan/BO/actions/runs/44869388854) SUCCESS.
- Peterson047 reference remains read-only and unlicensed external binaries/media unavailable for copying. Their Windows Team Deathmatch source `Peterson047/BLACK-Coop` 404; the independent BO room discovery now has a bounded real UDP loopback transport, but it **cannot be marketed as LAN game hosting** until Android sockets interface, LAN network discovery, authenticated join and a real NPC-player render hook exist.

### NET-LAN-002 host socket test result
- [x] Real POSIX UDP `127.0.0.1` Query/Reply transport between separate sockets with bounded recv, no SO_REUSEADDR, peer rate caps, explicit source/challenge verification, timeout and collision test; host ASAN/UBSAN + Android ARM64 cross compile [44869388876](https://github.com/alwansan/BO/actions/runs/44869388876) SUCCESS.
- [ ] Actual LAN broadcast discovery on Android and on two devices, in-game menu, authenticated JOIN. Do not use external binary/animation assets.
- [ ] ACTOR-001A remains blocking the real co-op player/animation/AI mission sync.


## أولوية جلسة19
افحص pool funcs وserial/scene reset وفق memory/sessions/2026-11-21_bo-coop-19-cloud-handoff.md ثم Probe3 منفصل read-only واختبارات/مراجعة/Actions ثم اختبار هاتف محدد. ACTOR قبل مزيد mock net.


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

## Session30 Real Lobby steps
- [x] Recover newer session29 live state rather than redoing session18 UI/Probe4.
- [x] Strict direct IPv4 + full certificate pin helper to real TLS, JVM CI44920763536 SUCCESS.
- [x] Bound Android emulator ADB operations and obtain actual Android29 platform handshake failure, no hangs.
- [x] Diagnose Conscrypt requesting EC; new opaque P-256 KeyStore identity source af67599d; platform CI44922289967 pending.
- [ ] Android TLS platform instrumentation PASS is essential before connecting live room to Flash Host/Join forms and actual Android LAN.
- [ ] Build standalone app form/bridge with immutable roster; keep network worker off game thread.
- [ ] Actual independent character body/controller 324 + combat AI mission checkpoint sync remain mandatory.

## Session31 — Room networking to game integration
- [x] AndroidKeyStore EC P-256 real Android29 TLS/socket gate `TLS_PLATFORM_PASS` in 44922935535; prior timeout was APK install, not proof of certificate failure.
- [x] Bounded Java RoomSession with real JVM host/join/cancel/roster tests `ROOM_SESSION_PASS` 44923264389.
- [ ] Android opt-in RoomLobbyActivity compile/runtime gate (CI44923426931 pending); inspect emulator install flakiness and logs.
- [ ] Wire accepted Flash room Host/Join to game Activity and immutable roster, separate isolated APK, then two-device test; no untrusted discovery as authenticated lobby.
- [ ] Independently solve ACTOR+324 allocation/lifecycle/body rig, network-to-game-thread snapshot apply, combat/story missions. Never equate room participants with in-game actors.
## Session32 — Flash HOST/JOIN Android TLS bridge
- [x] Android29 form+real TLS loopback platform marker `TLS_PLATFORM_PASS` 44923426931; no human tapping UI/phone pair test.
- [x] New separate game-network stage source/Java gate [44924220185](https://github.com/alwansan/BO/actions/runs/44924220185) SUCCESS. Flash confirmed HOST/JOIN row -> BlackActivity -> Android RoomLobbyActivity and immutable room count status via cached JNI, no app singleton actor writes; `roomui` accepted package unchanged.
- [ ] Build complete isolated Windows ARM64 APK via [44924346601](https://github.com/alwansan/BO/actions/runs/44924346601), verify Gradle, symbol/image SHA, permission allowlist, APK SHA, artifact availability. Not a phone-tested build yet.
- [ ] Test 2 real Android devices on trusted LAN/VPN with full verified SHA-256 host fingerprint, PIN, roster join/leave and return to Flash. No public forwarded ports. Capture sanitized logs only.
- [ ] Separately ACTOR+324 full-body/ownership, game-thread state apply, AI/combat/quest/mission sync; only then real story co-op.

## مرجع وتقدم أصول جديد بعد رفع جلسة35
اكتشفنا portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628،LICENSE MIT قرئ. راجع docs/REFERENCE_BLACK_PC_COOP_AR.md؛PC split-screen لديه player-context وpuppets لكنAIلايستهدفP2 بعد،وليسAndroid/network coop. لا نقل أعمىلـslotcopy؛+250عضويةالمجموعةيحتاجحماية. أداةBOالأصليةmodel_inventory.pyقرأَت271ملفاًمحلياًو4286MODEL،76مرشحbodyبـ10keysفريدة؛protagonistغيرمثبت.3اختباراتحدودوتصنيفPASS،لاgeometry/أصولمصدرة. نتائجآمنةفيOWNER-MODEL-CANDIDATES-20261121.json. تبقىخطةactualactor/control/AI/story،ولاStartوهمي.

FactoryAPK run44944691802 على96413485bdde48981c7000ff9f9ff6019740fa7b:coreنجحوالـgeneratedABIguardنجحوبناءARM64قيدالتجميعوقتكتابةهذاالتحديث. لا تطلبهاتفاًحتىنجاحartifactوفحصه.
