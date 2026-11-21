# BO-COOP — رسالة استعادة جلسة14 (2026-11-21)
المستودع https://github.com/alwansan/BO؛ فرع الكتابة **experiment/black-coop** فقط. **اقرأ أولاً AGENTS.md، ثم memory/STATE.json وmemory/MEMORY.md وmemory/PLAN_AR.md وmemory/ISSUES_AR.md وprompt/SESSION_PROTOCOL_AR.md وأحدث جلسة memory/sessions/2026-11-21_bo-coop-14-probe2-apk-verified.md**، ثم docs/ROOM_AND_LOG_EXPORT_AR.md وdocs/PROBE_DEVICE_GUIDE_AR.md وdocs/ACTOR_FEASIBILITY_AR.md. تحقق من HEAD وActions مباشرة قبل كل تغيير.

## اختبارات فعلية
- المالك جرّب Probe1 على هاتف Android15، دخل اللعبة واستورد ISO وبدأ Start capture دون مشكلة. `CL/black log/من المسار/1/org.bocoop.black.probe1/files/logs/black.log` يؤكد image c5bfed... وStart capture عند127.947s وExit0؛ لكن `coop_probe.jsonl` في CL **0B** وShell Shizuku لم يستطع قراءته. لا عينات player/LOD/animation حتى الآن؛ لا ACTOR/COMBAT proof. Evidence memory/evidence/DEVICE-probe1-owner-20261121.json.
- الـBaseline `org.bocoop.black.baseline` مقبولة من المالك، لا تعِد اختبارها. Probe1 القديمة منفصلة ولا تُحذف. ISO خمسة أجزاء سبق التحقق منها، SHA256 f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62 ويمكن استعادتها من BO manifest؛ لا تطلب XBE المفقود ولا ترفع أصل اللعبة للـartifact.

## أحدث APK مثبتة بالبناء
- **Probe2** `org.bocoop.black.probe2` build [44860874976](https://github.com/alwansan/BO/actions/runs/44860874976) **SUCCESS** على source `d8dba8d8da2da69c62718a87f3e6f03f16be0d2f`؛ Windows job136715739613، artifact **14690036713**، metadata14690226530، APK 21,739,739B SHA256 `861fd501497f2251feeb544c56430e084c3b48d3afa87e0d3a65c12e8a1115c0`.
- نجاح package/JNI/AArch64/debug signature/compile image c5bfed.../no INTERNET or broad-storage permissions/no ISO-XBE in APK. تم تنزيل Archive وفحص APK وبصمتها بشكل مستقل في container الجلسة، والتحقق من DEX وفيه `ProbeLogExporter` وMediaStore path ومن `libmain.so` وفيها نصوص CO-OP / ROOM. Evidence `memory/evidence/PROBE2-android-44860874976.json`.
- ملف مرفق مباشر أنشئ في المحادثة `/mnt/data/BLACK-Coop-Probe2-ROOM-LOGS-20261121.apk` بعد التحقق منه. **ملفات container مؤقتة بين الجلسات**؛ رابط GitHub Actions يدوم حسب احتفاظ artifacts؛ لا تدّع امتلاك APK محلياً في جلسة جديدة بلا تحقق.
- Probe2 ينقل `coop_probe.txt` و`black_log.txt` بعد opt-in كل10s via `MediaStore.Downloads` إلى `/storage/emulated/0/Download/Games/cl/black log/logs/BLACK-Probe-[timestamp]/`، مع حفظ الأصول الخاصة. **هذا لم يختبر على الهاتف بعد**.
- صفحة جديدة داخل OPTIONS الأصلية `CO-OP / ROOM` تعرض HOST/JOIN NOT AVAILABLE، 2–4 PLANNED، LAN/VPN PLANNED. **لم تُختبر على هاتف Android**. التنفيذ الحالي صفحة داخل SETTINGS وليس زر مستقل تحت الإعدادات كما طلب المالك حرفياً؛ لا تدّع الانتهاء من تخطيط القوائم تماماً.
- CI host preparer/sanitizer [44860875040](https://github.com/alwansan/BO/actions/runs/44860875040) SUCCESS؛ build قديم44860718925 SUCCESS لكنه superseded؛ 44860822227 cancelled.

## القادم
1. المستخدم يثبت **Probe2 فقط** ويستورد ISO الموجودة على الهاتف داخل مساحة الحزمة الجديدة؛ يختار `Start capture`، يدخل مهمة30–90s، يبحث عن صفحة `CO-OP / ROOM` داخل خيارات اللعبة، ويتحقق من نسخ `coop_probe.txt` و`black_log.txt` في مجلد Downloads المؤرخ. لا تطلب إعادة Baseline/Probe1. إذا لم تظهر السجلات، تحقق من MediaStore provider وopenOutputStream على الجهاز مع التعامل fail-closed.
2. عند وصول ملفات الرصد الحقيقية، حلّل player/lod/animation/epoch/threads؛ لا تعتبر الأرقام actor lifetimes؛ لا تنشر raw log على public GitHub. `coop/analysis/summarize_probe.py` يخرج ملخصاً محايداً (synthetic privacy CI succeeded).
3. صحح موضع القائمة إن احتاج المستخدم زرّاً مباشراً تحت الإعدادات، بعد تجربة الصفحة الحالية؛ لا تفعّل HOST/JOIN حتى ACTOR/COMBAT/NET مثبتة فعلاً. حافظ على main والأصل والحفظ وBaseline، وجميع التعديلات على experiment/black-coop فقط.
4. استمر بتحديث STATE/MEMORY/PLAN/ISSUES والجلسة وRESTORE في كل دفعة، وتأكد من HEAD.
