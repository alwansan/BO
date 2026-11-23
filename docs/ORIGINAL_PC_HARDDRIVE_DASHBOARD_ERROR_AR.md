# سبب الخروج من محرك BLACK الأصلي في مختبر Windows: خطأ القرص الافتراضي

Session108 — 2026-11-23.

## نتيجة مؤكدة من المحرك الحقيقي

- في [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) نجح توليد وتجميع محرك PC من ملف المالك original XBE المثبت ومجموعة 574 ملفاً. بعد 234ms أرسل المحرك `HalQuickRebootRoutine`، وكانت المؤشرات `launch_page_present=true`, `launch_type_1=true`, `launch_path_empty=true`, `launch_word0_1=true`, `dashboard_error_harddrive=true`. لم يحدث front-end tick أو mission أو إدخال USB أو تشغيل لاعب2؛ قبول runtime **FAILED**.
- حسب [Xbox LaunchDataPage](https://xboxdevwiki.net/Kernel/LaunchDataPage)، Type1 يعني Switch to Dashboard، reason1 يعني Error، و`parameters[0]=2` يعني Invalid Harddrive. هذا **تصنيف الخطأ الذي أبلغ عنه عنوان XBE**، ولا يثبت أن قرص المستخدم أو جهاز Android مكسور.

## مقارنة المحاكيات الأصلية المثبتة — مهم جداً

- مختبر Windows يستخدم **غير** SDK الخاص بتطبيق الهاتف: `portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628` مع `portingpete/xboxrecomp@e4b621706a3b5e58c084838d1f27547548cf27a8`. المصدر العام `src/kernel/kernel_path.c` ينشئ `Partition0.img` ويكتب جدول تقسيم افتراضي عند offset `0x800`؛ `kernel_file.c` يقدم Windows NT file/volume adapter ويستجيب لبعض استعلامات FATX.
- تطبيق Android الأصلي المرجعي يستخدم `RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806` مع `TGP482/XboxXBERecomp@99078b119b5d5e0d1d6afb1390e29437c0c984c0`. SDK الهاتف يحتوي `runtime/xdk/xapi.cpp:XMountUtilityDrive` الذي يعيد 1 ويصل `Z:` بالـPartition5، و`runtime/kernel/files.cpp` الذي يربط partitions 1..5 بمجلدات `E,C,X,Y,Z`.
- لا يجوز تطبيق إصلاح PC kernel على Android عشوائياً، ولا اعتبار خطأ PC دليلاً على أن لعبة الهاتف نفسها لا تُقلع. وجود نفس أسماء `CallGuest` و`mem::AllocSystem` لا يعني تكافؤ محاكاة القرص أو السيناريو.

## خطوات إصلاح جذر الخطأ

1. افحص لماذا تختار اللعبة الأصلية invalid harddrive داخل **مختبر PC**: أي استدعاء `NtCreateFile/NtOpenFile/NtReadFile/NtQueryVolumeInformationFile` أو تركيب utility drive أعطى قيمة غير صحيحة؟ التصنيف العام لا يحدد أي API.
2. استخدم في المختبر **مؤشرات ثابتة فقط** لنجاح/فشل استعلامات الأقسام والقرص؛ لا تنشر مسارات المالك، title ID، رموز XBE الخاصة أو كلمات stack. لا تستخدم إصلاحاً بإلغاء `HalReturnToFirmware`.
3. أصلح تمثيل أقسام الـHDD في محاكي PC فقط بعد إثبات الاستدعاء والفشل؛ أعد تشغيل اختبار قبول المحرك الأصلي، وليس مجرد انتظار exit_code=0.
4. بالتوازي تقدّم على Android عبر original game-thread hooks، actor storage/physics ownership، وHOST original world; لا تشترط نجاح emulator Windows مختلف حتى تجري source/native Android integration، لكن لا تدّعِ تشغيل مهمّة مشتركة قبل تحققها.
5. تجربة المالك على هاتفين تكون واحدة كبيرة عند تحقق original host + guest runtime، لا لفحص تفاصيل صغير.

## دليل تقدم Android

- [2910ab90](https://github.com/alwansan/BO/commit/2910ab90004b40749008ca8d82daa7787d3935e4): رفض actor غير مخصص من ledger أصلي.
- [10e546bf](https://github.com/alwansan/BO/commit/10e546bf2b978b00d086ed7554c71081624dc023): إدراج headers الأصلية في مصدر Android مع include للتجميع فقط، **لا إنشاء player2**.
- [242fa976](https://github.com/alwansan/BO/commit/242fa976716961f2c2eacb459de64ca9bba0229c): إبقاء الـstrict build allowlist مع إضافة أربعة headers فقط.
- [bd0cafe1](https://github.com/alwansan/BO/commit/bd0cafe15126d53bddca6f24e3cdefa6285dda3c): اختبار مستقل لتجميع headers مع SDK الخاص بـAndroid؛ [45207066206](https://github.com/alwansan/BO/actions/runs/45207066206) **SUCCESS**. بناء APK الكامل [45206563984](https://github.com/alwansan/BO/actions/runs/45206563984) ظل قيد التنفيذ عند التدوين.

الفرع المسموح `experiment/black-coop` فقط، بدون تعديل `main` أو `CL` أو `biso` أو ISO/XBE/saves/سجلات خاصة.
