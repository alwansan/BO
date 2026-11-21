## Verified cross-directory automated CL analysis (GitHub Actions run 44959212836)
- Branch CL main pinned `82b2c46f20aa2c911c6dfaec053be6d7d73962d4`; `.github/workflows/black-coop-cl-room-evidence.yml` checkout publicly readable CL subfolders without game binaries and uploads **safe-only aggregate counts** artifact `14728782486`. Both synthetic and owner-evidence jobs SUCCESS, markers `BO_CL_ROOM_TRIAGE_SYNTHETIC_PASS`, `BO_CL_ROOM_OWNER_AGGREGATE_PASS`, `DISCOVERY_AFTER_JOIN_DETECTED=true`, `HOST_BIND_ERRORS_DETECTED=true`.
- Main first device: 9 safe exported room log files, `ROOM_HOSTING=18`, `ERROR_BINDEXCEPTION=4`, `SCAN_STARTED=45`, `ROOM_JOINED=1`, `scans_after_join_within_same_log=2`.
- Second device: 6 safe exported room files, `ROOM_JOINED=19`, `JOIN_CONNECTED=18`, `SCAN_STARTED=187`, `scan_after_join_within_same_log=103`, `largest_single_log_scans_after_join=52`, `files_with_scans_after_join=4`.
- These are aggregate event counts across captures (some overlap and retries); **never claim 19 independent sessions or synchronized campaigns**. Intra-file POST-JOIN counts are based on order until STOP/LEFT/ERROR, not timing/performance. Unknown event labels are omitted from the artifact (only counted).
- CI JVM scan gate `44959047515 SUCCESS` marker `ROOM_SCAN_GATE_PASS`; Android29 emulator `44959047461 SUCCESS` marker `ROOM_ACTIVE_SCAN_GATE_PASS`; Android source staging `44959047532 SUCCESS`. No physical-device confirmation of current branch required or requested. The run `44959047454` produces room-only APK, verify final status separately.
- The more important real gameplay requirement remains unproven: actual host campaign scene loader, full-body remote human and world/synchronized AI/combat/quest.

---

# BO-COOP — مراجعة أدلة CL من الهاتفين، 22 نوفمبر 2026

## المصدر والحدود
المصدر المقروء مباشرة: `alwansan/CL` فرع `main` عند `82b2c46f20aa2c911c6dfaec053be6d7d73962d4` في `black log/`، بما فيه `logs/` و`من الهاتف الثاني/`. هذه تسجيلات نسخ **roomnet / defaultplayer القديمة**، لا اختبار على الإصدار الحالي TLS v3 ولا إثبات تشغيل القصة المشتركة. لم تُنسخ أصول اللعبة أو عنوان IP أو اسم لاعب أو موقعه/إحداثياته إلى BO. لا حاجة لأي تجربة إضافية من المالك.

## المشاهدات الفعلية، لا التخمينات
- سجل المضيف `logs/BLACK-Room-1795294900585-events.txt`: 36 حدثاً، منها `ROOM_HOSTING` ثلاث مرات و`ERROR_BINDEXCEPTION` مرتين بعد طلب HOST مجدداً. سجل `1795294672034`: 31 حدثاً، اثنان `ERROR_BINDEXCEPTION`. يتوافق مع فشل بعض محاولات إعادة استخدام منفذ الغرفة، **ولا يثبت فشل النسخة الجديدة**.
- سجل الهاتف الثاني `من الهاتف الثاني/BLACK-Room-1795294904769-events.txt`: 187 حدثاً، منها `SCAN_STARTED=70` و`SCAN_RESULTS_1=67` و`JOIN_CONNECTED=7` و`ROOM_JOINED=7`. تظهر عمليات بحث كثيرة بعد الانضمام؛ قد تُهدر عمل الشبكة/المعالج وتتسبب بتضخم سجل الأحداث، لكن مقدار تأثيرها على الأداء لم يُقَس. سجل ثانٍ `...1795293049908...` يسجل `SCAN_STARTED=41` مع انضمامين ناجحين.
- سجلات التشغيل الحديثة من المضيف `BLACK-Probe-20261122-010310-710`، `011823-703` و`014458-622`: 174 و482 و365 سجلاً من HOOK001، بقراءات pool/actor/LOD/animation. كل ملف فيه قيمة world_epoch واحدة وقيمة reset_sequence واحدة، وظهرت فقط عينات player محلية (50، 116، 93) دون player غير محلي في تلك العينات. **هذا لا يعني إثبات غياب لاعب بعيد بكل مسارات المحرك**؛ ولا يثبت scene-loaded callback أو إعادة إحياء NPC أو ملكية شخصية شبكية.
- ملف `logs/BLACK-Probe-20261121-235407-267/coop_probe.txt`: ثمانية سجلات `default_player_model` متطابقة: `valid=true`, `complete=true`, `bone_count=44`, `buffer_count=2`, `arms=true`, `head=false`, `torso=false`, `legs=false`, `body_candidate=false`. يعضد فصل المنظور الأول الأصلي (الذراعان) عن مجسم ALLY كامل مستقل للاعبين البعيدين، وليس إعادة استخدام النموذج المحلي للأجسام البعيدة.
- على الأجهزة المدونة: Android 10/API 29 وAndroid 14/API 34 وAndroid 15/API 35، Vulkan 1.1 وMali/Adreno. الشريط `no game files ... waiting` في بداية بعض ملفات black_log عَقِبه اكتشاف ISO والتحقق من hash c5 بعد ثوانٍ، فلا تُصنَّف تلك الرسالة وحدها على أنها فقدان دائم للأصول. تظهر `ConfirmWarning, FEAudio failed` في تسجيلات خروج، وهي مشكلة منفصلة لا تفسر عدم اكتمال co-op.

## الإجراءات التي أُدخلت على فرع BO فقط
1. `RoomScanGate.java`: سياسة whitelist للبحث: يسمح باكتشاف الغرف فقط في `IDLE` أو `ERROR` وعلى تبويب الغرف، وبحد أدنى 5500ms بين عمليات البحث التلقائي. لا تبحث عند `CONNECTING`, `JOINED`, `HOSTING`. منع طلب HOST/JOIN المكرر ما لم تغادر الغرفة، وليس إعادة تشغيل الشبكة خلفياً.
2. `RoomOverlay.java`: إيقاف المهمة غير الموثوقة عند تغير الحالة إلى اتصال نشط/جاري، وتجاهل أي نتائج اكتشاف متأخرة. منع الطلب المزدوج وإصدار event code آمن `SCAN_PAUSED_ACTIVE_ROOM` أو `HOST_BLOCKED_ALREADY_ACTIVE` أو `JOIN_BLOCKED_ALREADY_ACTIVE`، من دون عناوين/أسماء.
3. `prepare_room_network.py` يضم `RoomScanGate.java` في APK المجزّأ (كان مفقوداً في أول preflight وتم إصلاحه). الاختبارات تتأكد من الإدخال والربط، و`RoomScanGateTest.java` يختبر كل الحالات على JVM.
4. اختبار Android29 داخل `PlatformInstrumentation.java` يؤكد أن النقر المكرر على HOST لا يعيد فتح منفذ، ولا تحدث عمليات SCAN جديدة خلال فترة أطول من 5.5 ثانية رغم فتح تبويب الأجهزة عند HOSTING. لا يحاكي محرك القصة الحقيقية. `RoomAppRooms.attach` يهيئ RoomDiagnostics قبل فتح الواجهة، حتى تُحفظ الأحداث المهمة المبكرة.
5. إعادة الاستضافة على TLS v3 تملك أصلاً `setReuseAddress(true)` ومحاولات bind قصيرة ومحدودة؛ اختبارات JVM/Android29 سابقة نجحت. لا يعني هذا نجاحها على كل جهاز دون اختبار المرحلة الكبيرة في المستقبل.

## البوابة القادمة
- أولوية برمجية: إثبات مصدر mission/stage/scene_epoch الحقيقي على game thread بمحرك BLACK وصورة c5، ثم وصل `RoomSession` و`MissionBarrier` عند التحميل الحقيقي فقط.
- بعدها بناء رندر جسد حليف كامل **للبشر البعيدين حصراً**، وربطه بملكية بشرية ومزامنة الحركة والسلاح ثم AI/الأعداء/الأهداف والقصة في عالم المضيف.
- المالك لا يُطلب منه أي اختبار لهذه الإصلاحات الصغيرة. سننتظر بوابة تجربة واحدة كبيرة فقط بعد جاهزية gameplay حقيقي، مع logs آمنة مجمّعة تلقائياً على الهاتفين، وفق `docs/MAJOR_DEVICE_TEST_GATE_AR.md`.

## روابط الأدلة
- https://github.com/alwansan/CL/tree/main/black%20log/logs
- https://github.com/alwansan/CL/tree/main/black%20log/%D9%85%D9%86%20%D8%A7%D9%84%D9%87%D8%A7%D8%AA%D9%81%20%D8%A7%D9%84%D8%AB%D8%A7%D9%86%D9%8A
- https://github.com/alwansan/BO/blob/experiment/black-coop/docs/MAJOR_DEVICE_TEST_GATE_AR.md
