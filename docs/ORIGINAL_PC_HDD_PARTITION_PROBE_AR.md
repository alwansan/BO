# فحص أصل خطأ القرص الافتراضي في محرك BLACK الأصلي — Session109

2026-11-23. مختبر Windows الأصلي مسار مستقل عن Android: `portingpete/Black-2006-Recomp@b2512fa` مع `portingpete/xboxrecomp@e4b6217`.

## دليل السبب المعروف

[45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) بنى ملفات اللعبة الخاصة الصحيحة (574 ملفاً) ثم خرج إلى Xbox Dashboard خلال 234ms عبر `HalQuickRebootRoutine` مع launch Type1/Reason1/parameter[0]=2، ما يعني **Invalid Harddrive** داخل محاكي PC. لا يوجد دليل أن قرص الهاتف معطوب أو أن المحرك وصل إلى مهمة اللعب.

## فحص source-backed جديد

- الكود [58bc280a](https://github.com/alwansan/BO/commit/58bc280a7c6b5fa0448de09304afefc630b7dbe0) و[cb5a405e](https://github.com/alwansan/BO/commit/cb5a405e9b42582fbbde730b3ffa4b53abf9c624) و[test e440e3b3](https://github.com/alwansan/BO/commit/e440e3b3348478fe5ce992a59b4b6377a1faf903).
- `coop/engine_lab/lab.py` على checkout مؤقت pinned فقط يُضيف علامات `BO_ENGINE_HDD_*` ثابتة إلى مصدر **محاكي Xbox العام**، لا إلى XBE/ISO أو جسم اللعبة المُعاد ترجمته. `src/kernel/kernel_path.c`: إنشاء `Partition0.img` وكتابة جدول retail Xbox عند `0x800` (طول512) نجاح/فشل. `src/kernel/kernel_file.c` Windows: محاولة فتح `Partition0`، نجاح/فشل، فتحه كدليل، طلب قراءة القطاع4، فحص *توقيع المحاكي الافتراضي فقط* `****PARTINFO****`، استعلام حجم نظام الملفات.
- يُستخرج فقط **حقل Boolean ثابت** لكل علامة؛ لا يُنشر مسار القرص، title id، error code، بيانات XBE أو حتى كلمات جدول التقسيم. اختبارات اختبار النص الأصلي، رفض الانحراف، عدم إفشاء private game bytes، والحفاظ على Win32 error semantics نجحت ضمن [harness-run 45210679103](https://github.com/alwansan/BO/actions/runs/45210679103) (التحقق من المصدر فقط). لا يجوز اعتبار نجاح harness نجاح تشغيل.
- على Windows، `GetLastError` يُقرأ قبل إخراج marker إلى `stderr`، لضمان أن marker لا يغير نتيجة `STATUS` أو التشخيص. Native ReadFile يرجع STATUS وفق نفس الحالة السابق؛ لا `ExitProcess` أو تغيير مسار reboot.
- تشغيل native engine الجديد [45210679103](https://github.com/alwansan/BO/actions/runs/45210679103) ظل **IN_PROGRESS** عند كتابة أول نسخة من هذه الوثيقة. نتيجة فئات فحص الأقسام لا تُختلق. نتيجة نسخة الوسيط [45210316015](https://github.com/alwansan/BO/actions/runs/45210316015) فشلت عند `source_patch_failed` بسبب توقع تكرار خاطئ لعناوين مشتركة مع backend POSIX، وصُحح ذلك في cb5a405e.

## تفسير ما سيخرج من المختبر

- `hdd_table_created/hdd_table_written`: فقط نجاح إنشاء/كتابة صورة الأقسام الافتراضية في التشغيل الحالي.
- `hdd_partition0_open_tried/open_ok/open_failed`: فقط حالة فتح الجهاز المعين، لا دليل سلامة كل أقسام القرص.
- `hdd_partition0_sector4_read/valid/invalid`: محاولة قراءة القطاع4 وفحص التوقيع غير الخاص؛ إن لم تظهر فلا تنفي وجود API غير مغطاة.
- `hdd_fs_size_query_seen`: فحص FileFsSizeInformation عام، لا يثبت أن الاستعلام بالضرورة على HDD إلا إن تثبت علاقة الـhandle.
- حتى إن نجحت كل المؤشرات، فقد يكون خطأ الـHDD من `NtDeviceIoControlFile` أو `NtQueryVolumeInformationFile` بصنف آخر أو منطق game/XAPI مختلف. لا تصنع نتيجة ولا تخمن API.

## خطوات العمل

اقرأ أولاً نتيجة runner الحقيقية في `boot_progress` و`safe_build_diagnostics`. إن ظهر فشل فعلي في إنشاء الصورة، أصلح فقط تلك العملية على checkout محاكي PC مع اختبار إعادة التشغيل. إن نجحت الصورة لكن ظل Dashboard error2، أضف فحصاً أصغر للـAPI المشتبه وفق المصدر. بالتوازي Android **نجح تجميع original actor resource headers** في [45206563984](https://github.com/alwansan/BO/actions/runs/45206563984)، لكنه NO GAMEPLAY CO-OP. لا حاجة لتجربة هاتف قبل ربط الممثلين الحقيقيين بفيزياء وأسلحة العالم نفسه.

الملفات المحمية: `main`, `CL`, `biso`, ملفات اللعبة وصورها وسجلات الهاتف الخاصة. كل التغييرات حصراً `experiment/black-coop`.
