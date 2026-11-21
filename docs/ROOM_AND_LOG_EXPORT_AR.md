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
## حالة البناء المُثبتة
- Host/pinned source [44860875040](https://github.com/alwansan/BO/actions/runs/44860875040) SUCCESS. Android [44860874976](https://github.com/alwansan/BO/actions/runs/44860874976) SUCCESS on `d8dba8d8da2da69c62718a87f3e6f03f16be0d2f`, artifact `14690036713`, size **21,739,739B**, SHA256 `861fd501497f2251feeb544c56430e084c3b48d3afa87e0d3a65c12e8a1115c0`.
- APK الحقيقي استُخرج وفُحص بشكل مستقل: ثلاث مكتبات ARM64، وفي DEX كود MediaStore، وفي libmain.so نصوص CO-OP / ROOM. لا أصول ISO/XBE في الحزمة.
- **غير مختبر بعد على الهاتف** تصدير Downloads وظهور صفحة الغرفة. الحزمة لا تمس Probe1 القديمة ولا الـBaseline المقبولة.
- الطلب الأصلي يشير إلى خيار تحت `الإعدادات`؛ التنفيذ الحالي صفحة سادسة **داخل** OPTIONS (وليست زر أعلى مستوى أسفل الإعدادات). يجب قياس ملاءمة موضعها على هاتف المالك قبل تطوير injection إضافي.
## تجربة الهاتف القادمة
1. نزّل APK Probe2 من Artifact 14690036713 أو ملف APK المباشر في المحادثة وثبّته دون حذف Probe1 أو Baseline.
2. عند أول فتح، اختَر `game.iso` الموجود على هاتفك من منتقي Android (من دون إعادة رفعه للمشروع). عند ظهور الاختيار اختر `Start capture`. سيصبح نسخ السجلات **مشتركاً في Downloads ويمكن لتطبيقات أخرى قراءته**؛ لا تنشره في GitHub عام.
3. ادخل مهمة وتحرّك قرب الجنود، وانتظر 30–90 ثانية، ثم افتح OPTIONS وابحث عن `CO-OP / ROOM` (قد يكون آخر صفحة) والتقط صورة إذا ظهرت. HOST/JOIN يجب أن تكون NOT AVAILABLE.
4. افحص `/storage/emulated/0/Download/Games/cl/black log/logs/` وابحث عن آخر مجلد `BLACK-Probe-YYYYMMDD-HHMMSS-SSS` وداخلَه `coop_probe.txt` و`black_log.txt`. أرسل السجل/الملخص **خاصاً في المحادثة** لتقييم ACTOR-001، وليس إلى مستودع CL العام. إذا لم يظهر، صوّر محتويات Download وتعامل معه كـruntime bug لا تستنتج منه عدم وجود actor.
5. لا تعِد اختبار Baseline أو Probe1 القديمة.
