### Session108 evidence update — exact Dashboard harddrive error and Android builder gating

- Real Windows original-game lab [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) **FAILED gameplay** but established `dashboard_error_harddrive=true`, `firmware_kind_2`, Dashboard Type1 Reason1, 234ms; Xbox launch parameter[0]=2 is Invalid Harddrive. This is PC virtual-Xbox disk emulation, **NOT evidence the user phone storage is bad**. Distinct Android SDK originally runs through `TGP482/XboxXBERecomp`, PC lab through `portingpete/xboxrecomp`. Investigate virtual partition/utility mount semantics and specific API only with privacy-safe evidence, no fake firmware success. Detailed source comparison `docs/ORIGINAL_PC_HARDDRIVE_DASHBOARD_ERROR_AR.md`.
- Native Android first full build of new headers [45205630319](https://github.com/alwansan/BO/actions/runs/45205630319) failed BEFORE C++ at strict candidate file allowlist: exactly four new reviewed headers needed adding. Commit [242fa976](https://github.com/alwansan/BO/commit/242fa976716961f2c2eacb459de64ca9bba0229c) updated that allowlist and retained protections. Retest [45206563984](https://github.com/alwansan/BO/actions/runs/45206563984) was IN_PROGRESS at last check.
- Dedicated Android SDK pinned-header C++ compile smoke in [bd0cafe1](https://github.com/alwansan/BO/commit/bd0cafe15126d53bddca6f24e3cdefa6285dda3c) has [CI 45207066206](https://github.com/alwansan/BO/actions/runs/45207066206) **SUCCESS**. This validates header compatibility, not original actor2 game initialization.
- Remains: actual original Android actor2–4 creation with independent camera/animation/weapon/audio/physics/AI and stable scene lifetime, host original enemy/door/objective and guest original world apply. Android room-only APK is NOT co-op yet. One major owner trial only when genuine shared mission works.


# تحليل Quick Reboot الأصلي قبل أول مهمة BLACK

Session105 — 2026-11-23. الهدف ليس تجاوز HAL، بل معرفة سبب طلب المحرك الأصلي إعادة تشغيله قبل أول قراءة USB أو تحديث قائمة.

## Session107 — 2026-11-23 — ثبت سبب الخروج العام: خطأ يعيد المستخدم إلى Dashboard

الدليل الحي [45197279738](https://github.com/alwansan/BO/actions/runs/45197279738): نجح بناء محرك BLACK الأصلي، لكنه بعد **484ms** طلب `HalQuickRebootRoutine`، وكانت البيانات `launch_type_1=true` و`launch_word0_1=true` و`launch_path_empty=true`. لم يصل إلى القائمة أو USB أو المهمة، لذلك فشل قبول اللعب الأصلي.

وفق [توثيق Xbox الأصلي لصفحة LaunchData](https://xboxdevwiki.net/Kernel/LaunchDataPage)، النوع1 يعني **Switch to Dashboard**، والسبب1 يعني **Error**. إذن هذا الخروج خطأ مبكر موجّه إلى Dashboard، وليس طلباً موثقاً لتشغيل ملف XBE جديد. لم نحدد بعد نوع الخطأ: أول بارامتر ضمن حمولة النوع1 (الكلمة الثالثة) يميز 1=Invalid XBE، 2=Invalid Harddrive، 3=Region، 4=Parental control، 5=Bad media type. لا يجوز استنتاج النوع من reason1 وحده.

التنفيذ الجديد:
- [95c01225](https://github.com/alwansan/BO/commit/95c01225497bcbdcbbb8bc756157582a94f92078) أضاف مؤشرات ثابتة لمسار فتح جهاز DVD وفحص MODE SENSE وبعض إخفاقات فتح الملفات، من دون مسار أو رمز خطأ أصلي في النتائج. اختبار الأداة نجح [45201724219](https://github.com/alwansan/BO/actions/runs/45201724219).
- [83d7d566](https://github.com/alwansan/BO/commit/83d7d566e4520ada649b4eeb3114ebcb70e76b2c) أضاف تصنيف السبب الدقيق Dashboard type1/reason1/parameter0 إلى فئات Boolean محدودة، مع اختبارات تمنع إخراج titleid ومسارات الملفات وكلمات اللعبة الأصلية. المختبر الجديد [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) لم تكتمل نتيجته عند أول فحص.
- وسيطان سابقان [dcc9fc76](https://github.com/alwansan/BO/commit/dcc9fc760e3b6be0b6ceffca20310abca39490f9) و[926fc93a](https://github.com/alwansan/BO/commit/926fc93a8c8436ada7026ad024a4d73527b8d839) احتويا خطأ Python syntax أثناء إضافة المؤشرات، كشفه CI، وصُحح في 95c01225؛ ليسا مرجعاً للاستخدام.

**الإجراء الصحيح القادم:** انتظر نتيجة كود الخطأ المقيّد من المختبر ثم أصلح سبب الإقلاع المثبت، مع الحفاظ على إعادة التشغيل الطبيعية، دون إنشاء نجاح مزيف. لا يدّعي هذا التشخيص وجود لاعبين داخل عالم BLACK الأصلي، ونسخة Android الحالية غرفة فقط.


## أدلة حية، لا تخمين

- مختبر PC الأصلي الخاص [45193373405](https://github.com/alwansan/BO/actions/runs/45193373405) بنى اللعبة من XBE المالك المثبت و574 ملفاً، وخرج خلال 202ms مع `firmware_kind_2=true`، `firmware_return=true`، بلا mission أو frontend tick أو USB pad. `exit_code=0` ليس قبولاً. الاختبار السابق [45186010290](https://github.com/alwansan/BO/actions/runs/45186010290) خرج خلال 219ms ولم يكن يميز routine.
- في SDK Xbox الأصلي enum `FIRMWARE_REENTRY`: `HalHaltRoutine=0`، `HalRebootRoutine=1`، `HalQuickRebootRoutine=2`. إذن الملاحظة هي **Quick Reboot**، لا hard crash ولا إثبات انتقال ناجح.
- المصدر المسمار `portingpete/xboxrecomp@e4b621706a3b5e58c084838d1f27547548cf27a8/src/kernel/kernel_bridge.c` في `bridge_HalReturnToFirmware` يشرح أن Quick Reboot يمكن أن يتبع `XLaunchNewImage` ومعه launch data page. كما يطبع في سجل خاص نوع الصفحة وpath وكلمات launch data ثم stack عناوين. **لا يحق نشر هذه السجلات أو محتوى الصفحة**.

## إضافة تحليل session105

Commit [476410fb](https://github.com/alwansan/BO/commit/476410fb1c025075f32c98171305f0f9b4e8b136) أضاف `collect_launch_hints` في `coop/engine_lab/lab.py`. يقرأ log داخلياً ويصدر فقط مؤشرات Boolean محدودة:

- `launch_page_present` / `launch_page_absent` وفق تطابق السطر الأصلي تماماً.
- `launch_type_0..3/other`، و`launch_path_present` / `launch_path_empty`، دون نشر path أو titleid.
- `launch_word0_0..3/other` وهو تصنيف أول word من بيانات launch، **ليس دائماً سبب BootToDash**، فلا تفسره تفسيراً حاسماً دون ربطه بمسار المصدر.
- ثلاثة اختبارات تضمن رفض النصوص والأرقام غير المطابقة والاحتفاظ بالمؤشرات فقط، مع اختبار المرور عبر reader الفعلي.

## الفرضيات المفتوحة والمعالجة الصحيحة

1. لو كانت launch page موجودة ومعها path، افحص عقد `XLaunchNewImage` من كود المصدر وافهم مسار العودة المطلوبة. لا تشغل ملفاً مجهولاً، ولا تسرّب path أو تحمل نسخة أصلية خارج sandbox المالك.
2. لو لا توجد page، فطلب Quick Reboot قد يكون استجابة لفشل إقلاع/وسائط/موارد. ميّز أخطاء DVD والفيديو والصوت والأقسام وفق أدلة المصدر والفحص الخاص، ولا تفترض السبب من رقم routine.
3. لا تلغِ `ExitProcess(0)` أو تستبدل `QuickReboot` بالعودة إلى original game tick؛ هذا يكسر control flow ويعطي نجاحاً كاذباً.
4. حتى إصلاح boot في مختبر Windows لا يعني Android true co-op. بعد ذلك يلزم إنشاء original actor2 مستقل ومتكامل، physics/weapon/AI، authoritative host world وguest engine apply، ثم تجربة هاتف مشتركة كبيرة.

كل تعديل على `experiment/black-coop` فقط. لا تغيّر ألعاب المالك أو تعيد توزيع XBE/ISO/سجلات خاصة.
