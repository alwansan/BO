# BO-COOP — Session41: مراجعة CL الفعلية وإيقاف البحث الزائد وتجميع الأدلة تلقائياً
Date: 2026-11-22

## مهمة المالك وتعليمات ثابتة
المستخدم أرسل `https://github.com/alwansan/CL/tree/main/black%20log` وطلب Continue، بالاعتماد على GitHub Actions والملفات السابقة. **لا تطلب منه تجارب بسيطة على الهاتف**. لا تجربة الآن: APK الغرف وحدها لا توفر عالم BLACK مشتركاً أو remote AllyBody. تجربة واحدة كبيرة فقط مع تلقائية التقاط كل التفاصيل الصغيرة في logs آمنة بعد اكتمال مرحلة فعالة. لا تعدل main ولا مصادر/أصول ISO أو CL؛ الكتابة حصراً `alwansan/BO:experiment/black-coop`.

## أدلة فعلية من CL
- فرع CL main pin: `82b2c46f20aa2c911c6dfaec053be6d7d73962d4`. فحص `black log/logs` و`black log/من الهاتف الثاني` بما فيها أحدث captures بتاريخ 2026-11-22.
- نتيجة تحليل CI آلي بالكامل `44959212836 SUCCESS`، artifact **14728782486** `bo-cl-room-safe-summary-only` (فقط counts آمنة؛ لا ترفع raw logs أو عنوان/IP/أسماء/أوقات). 9 ملفات غُرف من الجهاز الأول و6 من الثاني؛ `ERROR_BINDEXCEPTION=4` عند الأول، `ROOM_JOINED=19` و`SCAN_STARTED=187` بالثاني، `scans_after_join_within_same_log=103`، largest 52 ضمن ملف واحد. هذه **أحداث متراكمة وليست 19 جلسة لعب**؛ تنتمي لنسخ roomnet قديمة.
- ملف `BLACK-Probe-20261121-235407-267/coop_probe.txt`: ثمانية قياسات `default_player_model`، جميعها valid/complete، 44 bone, 2 buffer, arms=true, head/torso/legs=false, body_candidate=false. يدعم الاحتفاظ بمنظور اللاعب الأصلي محلياً واستخدام جسم ally كامل مستقل للبشر الآخرين؛ لا يثبت mesh/renderer صالحاً بعد.
- عينات pool/animation/LOD في ثلاثة ملفات HOOK001 بتاريخ 22 نوفمبر (174،482،365 records) مع epoch واحد وreset واحد في كل capture وعينات اللاعب المحلي فقط. لا إثبات world transition أو actual scene loaded أو NPC actor lifetime أو remote human actor.
- سجل `no game files waiting` في البداية يتبعه العثور على ISO بعد ثوانٍ؛ `ConfirmWarning FEAudio failed` يظهر عند الخروج ولا يدل على فشل co-op.
- المصدر التفصيلي: `docs/CL_20261122_TWO_DEVICE_LOG_EVIDENCE_AR.md`.

## التغييرات المنفذة
1. `coop/lobby/java/com/racas/black/coop/RoomScanGate.java` سياسة scan only IDLE/ERROR، توقف أثناء CONNECTING, JOINED, HOSTING (لا network scan storm). `RoomScanGateTest.java` يغطي الحالات/التوقيت JVM، workflow core `ROOM_SCAN_GATE_PASS` ضمن `44959047515 SUCCESS`.
2. `RoomOverlay.java`: إلغاء scanning الموجود عند الاتصال النشط، تجاهل نتيجة اكتشاف قديمة، حظر HOST/JOIN المكرر، وإصدار أكواد آمنة `SCAN_PAUSED_ACTIVE_ROOM` / `HOST_BLOCKED_ALREADY_ACTIVE` / `JOIN_BLOCKED_ALREADY_ACTIVE`. `RoomAppRooms.attach` يهيئ RoomDiagnostics من بداية العملية، لا ينتظر ظهور Dialog. لا تعديل حزمة اللعبة الأصلية أو تسرب بيانات.
3. `coop/prepare_room_network.py` أدرج RoomScanGate في حزمة Android المصدرية وتأكد `room_network_prepare_checks.py` من ذلك. أول فشل `44958830153` كان Java class missing من staged APK (عولج `6689896c`)، ثم فشل `44958907438` من test assertion يتوقع وجود نص JOINED حرفياً بدلاً من IDLE/ERROR allowlist (عولج `7fc0c0fc`). **CI `44959047532 SUCCESS`** للمصدر staging، فلا تبقِ المشاكل القديمة مفتوحة.
4. `PlatformInstrumentation.java` Android29 يفحص HOST double-tap بدون listener ثاني، وفتح تبويب الأجهزة أثناء HOSTING مدة 6.1 ثانية دون اكتشاف دوري؛ **run `44959047461 SUCCESS` marker `ROOM_ACTIVE_SCAN_GATE_PASS`** و`ROOM_LOG_ROTATION_PASS`.
5. محلل `coop/analysis/room_event_triage.py` يقرأ ملفات CL الحالية من الهاتفين مع whitelist للأحداث وتحديد 4 BindException و103 post-JOIN scans وإخراج counts only، مع اختبارات leak/tamper/rejected input `test_room_event_triage.py`. workflow `.github/workflows/black-coop-cl-room-evidence.yml` ينفذ unit tests ويقرأ sparse-checkout من CL العام ويصدر **sanitized counts only**. `44959212836 SUCCESS` markers `BO_CL_ROOM_TRIAGE_SYNTHETIC_PASS`, `BO_CL_ROOM_OWNER_AGGREGATE_PASS`; NO gameplay.
6. بناء Android room-only المصدر `44959047454` كان ما يزال in_progress عند آخر فحص؛ تحقق live قبل قبول APK. لا تنصح المالك بتجربته.

## المرحلة الهندسية المتبقية وأولوية العمل
- تحديد actual mission select + real stage loaded scene epoch + game thread in owner c5 XBE (المرشح 00124CF0 نافذة صغيرة فقط؛ لا ABI أو hook آمن بعد). لا تستخدم مشهد قوائم Flash كدليل. رسائل RoomSession STORY_PREPARE/READY/COMMIT كانت synthetic/TLS فقط.
- بناء/ربط مجسم الحليف الكامل للإنسان البعيد على كل هاتف، من دون امتلاك AI brain أو تغيير اللاعب المحلي الأول أو ربط NPC حقيقي، ثم مزامنة الحركة والسلاح والأعداء والقتال والأهداف وقصة المضيف في عالم واحد.
- كل اختبارات CI/Emulator أولاً. لا تطلب phone اختبار قبل shared-world meaningful milestone؛ لاحقاً تجربة واحدة كبيرة تجمع آلياً الشبكة والمهمة/المشهد والمجسم والقتال والأخطاء/الأداء مع سجل واحد من كل هاتف. See `docs/MAJOR_DEVICE_TEST_GATE_AR.md`.

## استعادة الجلسة
اقرأ AGENTS.md، `prompt/RESTORE_SESSION_AR.md`, `docs/CLOUD_HANDOFF_AR.md`, `memory/STATE.json`, Session41، Session40، `docs/CL_20261122_TWO_DEVICE_LOG_EVIDENCE_AR.md`, `docs/BLACK_STORY_ENTRY_AUDIT_AR.md`, `docs/MAJOR_DEVICE_TEST_GATE_AR.md` وخطة المحرك. افحص BO HEAD وActions الحية مباشرة؛ ثم استمر على `experiment/black-coop` وحده. حافظ على فحوص privacy وسياسة تخفيف تجارب المالك.
