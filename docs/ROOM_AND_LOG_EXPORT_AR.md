# Probe2: تصدير السجلات وصفحة الغرفة — 2026-11-21
## نتيجة جهاز المالك
المالك جرّب Probe1 بنفسه: استيراد game.iso، القصة تعمل، حوار Start capture ظهر. `black.log` في CL يقول بدأ الرصد عند127.947s وانتهت العملية Exit0. ملف `coop_probe.jsonl` في CL حجمه0B، والأصل تحت Android/data قراءته ممنوعة على Shizuku shell. هذه ليست شهادة اكتمال عينات الشخصيات.
## تصدير القراءة دون صلاحيات شاملة
نسخة **Probe2 المستقلة** `org.bocoop.black.probe2` عند اختيار Start capture تستدعي `ProbeLogExporter` في Java. ينسخ بواسطـة MediaStore.Downloads كل10 ثوانٍ على خيط I/O مستقل:
- `/storage/emulated/0/Download/Games/cl/black log/logs/BLACK-Probe-YYYYMMDD-HHMMSS-SSS/coop_probe.txt`
- `.../black_log.txt`
الأول نسخة نصية كاملة من JSONL الأصلي إن توفر؛ الثاني نسخة من `black.log`. الحد2MiB/4MiB، التصدير أقصاه8min، يحتفظ بالأصل في مجلد التطبيق، لا ISO ولا XBE ولا hdd ولا saves ولا shader caches. شاشة Start capture توضّح أن نسخ Downloads عامة على الجهاز ولا ينبغي نشرها عامة على الإنترنت. لا أذونات شبكة أو MANAGE_EXTERNAL_STORAGE جديدة. Android 10+ يشترط أن تكتب عبر ContentResolver/MediaStore؛ لم نغيّر chmod للأصل.
## قائمة CO-OP / ROOM
المصدر `src/game/options_menu.cpp` في المرجع v1.5.0 يتولى صفحات SETTINGS في واجهة Flash داخل اللعبة، بما فيها pause. `prepare_probe.py` يضيف صفحة سادسة `CO-OP / ROOM` فيها:
- ROOM STATUS = OFFLINE / PREVIEW
- HOST ROOM = NOT AVAILABLE
- JOIN ROOM = NOT AVAILABLE
- PLAYERS = 2-4 PLANNED
- NETWORK = LAN / VPN PLANNED

تجربة الصفحة تتم عبر إعدادات اللعبة (في pause OPTIONS اختر page Co-op/Room، أو من قائمة الخيارات في الواجهة الرئيسية). لا شبكة أو player2 حتى الآن؛ جميع البنود معلومات فقط. Fail-closed على هوية المصدر، لا writes للشخصيات ولا مساس بحفظ الطور الفردي.
## القبول
- `44860875040`: Host/pinned source PASS. Android `44860874976` **pending** وقت الكتابة؛ ليس APK مثبتاً. جهز فقط عند PASS للتحقق من package وABI وJNI والـsignature وأصول اللعبة وحجم وبصمة APK.
- المستخدم يحتاج اختبار **Probe2 فقط** بعد جاهزيته، وليس baseline القديمة أو XBE. الحزمة الجديدة لا تمس Probe1 القديمة، لكن يجب استيراد ISO من الملفات الموجودة على الهاتف مرة ثانية داخل مساحة Probe2 عبر المنتقي.
