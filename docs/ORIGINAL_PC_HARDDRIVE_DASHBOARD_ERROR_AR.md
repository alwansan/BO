### Session109 verified root — Windows source passes relative save to absolute-only directory API

Evidence chain: original PC [45210679103](https://github.com/alwansan/BO/actions/runs/45210679103) private engine built from owner's XBE/574 files but exited Xbox Dashboard Invalid Harddrive at 390ms; its new sanitized probe recorded **`hdd_table_create_failed=true`**, before Partition0 open/sector4 read. Pinned source `portingpete/Black-2006-Recomp@b2512fa/src/main.c` passes `xbox_path_init(YOUR_GAME_DIR,"save")`, while `portingpete/xboxrecomp@e4b6217/src/kernel/kernel_path.c` calls `SHCreateDirectoryExW(NULL,s_save_dir,NULL)` directly on that relative pathname. [Microsoft documentation](https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shcreatedirectoryexw) explicitly requires a fully qualified pathname, confirming a **source-level mismatch**. No owner XBE/ISO or Android storage fault implied.

New temporary pinned **PC emulator** code [737bc70f](https://github.com/alwansan/BO/commit/737bc70f7eeef0185eb6047284b2a5b790399fdf) normalizes the save directory via `GetFullPathNameW` before `SHCreateDirectoryExW`; emits only bounded path-normalized/dir-created/failed boolean markers; never prints private paths. [cfff509c](https://github.com/alwansan/BO/commit/cfff509c10fa8efab9c81131e2c964839c548a7e) adds regression for correct call order, invalid source refusal and private fixed flags. Real private engine test [45213250944](https://github.com/alwansan/BO/actions/runs/45213250944) was **PENDING** on initial review. This is a candidate fix, not yet a demonstrated original game boot. Earlier source-only harness [45210679103](https://github.com/alwansan/BO/actions/runs/45210679103) SUCCESS for earlier raw sector probe.

Android [45206563984](https://github.com/alwansan/BO/actions/runs/45206563984) remains a successful compile-only APK with original player-storage/proxy C++ headers, **NO GAMEPLAY CO-OP**. Need native actor P2–4 initialization/safe P1 restore, independent weapon/audio/animation/physics/AI, host canonical enemy/doors/objectives, guest original state application. One large owner multi-phone trial only after shared mission integration.


### Session108 verified Android-native outcome

[45206563984](https://github.com/alwansan/BO/actions/runs/45206563984) **SUCCESS** after [242fa976](https://github.com/alwansan/BO/commit/242fa976716961f2c2eacb459de64ca9bba0229c) strict exact header allowlist repair. Actual Windows runner `candidate-probe` built Android original `libmain.so` and isolated APK; the workflow's `Compile isolated Android baseline with bounded parallelism`, `Verify ... JNI, signature, ABI and compile image` and artifact-upload steps each SUCCESS. Therefore the original player storage/proxy headers are now **compiled into the pinned Android game source without breaking the build**. This is NOT actor initialization or playable shared-world co-op: no extra game-engine actor, separate host player weapons, synced enemy AI/doors/objectives or guest original world has been proven. Avoid unnecessary user APK test.

The original Windows private engine [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) has confirmed `dashboard_error_harddrive`; this is virtual-HDD compatibility of the **different PC emulator**, not phone storage. Track work separately. See `docs/ORIGINAL_PC_HARDDRIVE_DASHBOARD_ERROR_AR.md`.

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
