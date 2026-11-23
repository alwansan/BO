# مرحلة تجهيز جسم اللاعب الثاني الأصلي في BLACK — Session110

23 نوفمبر 2026. الفرع الوحيد للكتابة: `experiment/black-coop`.

## المنفذ فعلاً

- [edd78276](https://github.com/alwansan/BO/commit/edd782765b5774cdb0c140b4f0b4db60beaa88bf) أضاف `coop/room_network/room_original_host_player_shadow_plan.hpp`: يعمل على تجهيز لقطة *داخل ذاكرة C++ المنفصلة* من original BLACK actor2 اعتماداً على actor1 الحقيقي ومالك الكاميرا، مستنداً إلى المصدر المثبت `portingpete/Black-2006-Recomp@b2512fa` و`black_p2_start` (الأوفستات والمقاسات).
- [181cba15](https://github.com/alwansan/BO/commit/181cba15b9f17f6340547b21e8f97daf0324f5ee) أضاف اختبار C++ يتأكد من عدم تغيير **أي بايت** بذاكرة P1 أثناء `Prepare`، عزل `0xB4` (مؤشر physics)، و`0x298` (مخزن السلاح الجديد)، و`0x34C/0x350` (أجزاء مجسم منفصلة)، وروابط `0x34/0x38` الذاتية ونسخ روابط camera owner المنقولة. يرفض تداخل ذاكرة اللاعب مع P1/NPC/physics أو مشهداً خاطئاً أو thread غريباً.
- [816803a0](https://github.com/alwansan/BO/commit/816803a0a8057a19fa5ad681c7918f5dfbe193b6) ربط الملف بمصدر Android `src/game/cheats.cpp` **للتجميع فقط**، وأضاف `OriginalPlayerStorageKernelIO::CopyGuest` قراءة محدودة لصفحات أصلية مخصصة، وحدث whitelist وشروط التحقق واختبار ASan/UBSan الخاص بالمرحلة.
- [source preflight 45216969474](https://github.com/alwansan/BO/actions/runs/45216969474) **SUCCESS**: خطوة `Original player shadow source planning without original guest writes` وخطوة `Compile original host proxy and player storage headers against pinned Android Xbox SDK` كلتاهما ناجحتان.
- بناء Android الكامل من المصدر الجديد [45216969800](https://github.com/alwansan/BO/actions/runs/45216969800) ظل قيد التنفيذ عند أول تدوين؛ لا تعلن نجاح APK هذا البناء قبل تحقق نتيجته.

## قيود أمان مهمة

`OriginalHostPlayerShadowPlan::Prepare` لا يكتب إلى original guest RAM، لا ينشئ P2، لا يبدل original core، لا ينفذ `CallGuest`، لا يحمل موديل/أسلحة/مؤثرات/أنيميشن حقيقية، ولا يربط collider أو original AI. البيانات تبقى داخل snapshot مؤقت مع `Reset` عند الفشل. الـPreview الداخلي للتجارب **لا يجوز تسجيله** في لوج المستخدم أو إرساله إلى الشبكة/JNI. ملف اللاعب الجديد ليس شهادة العالم المشترك؛ نجاح source fixture لا يعني ظهور جسم.

المراحل القادمة حتى يصبح لاعباً فعلياً: إجراء `OriginalHostPlayerStorage::Allocate` على game thread بعد شاهد scene+owner+roster أصلي ثابت، تهيئة original camera constructor، نقل snapshot إلى مساحات **مملوكة** بعد التأكد من حياة original cache والسلاح والبنوك الصوتية، التشغيل الحقيقي التدريجي `0x1E0E40` مع تبديل atomic P1/P2 وكامل سياق الأنيميشن والصوت/الأسلحة، ثم proxy `0xDF400` عبر `OriginalProxySceneLedger` وتحرير مثبت عند scene teardown. دون ذلك تبقى هذه مرحلة تجهيز فقط.

## أصل مشكلة إقلاع مختبر PC المنفصلة

مختبر Windows [45210679103](https://github.com/alwansan/BO/actions/runs/45210679103) حدد `hdd_table_create_failed`. تم تصحيح تعارض save-relative و`SHCreateDirectoryExW` في [737bc70f](https://github.com/alwansan/BO/commit/737bc70f7eeef0185eb6047284b2a5b790399fdf) مع اختبار [cfff509c](https://github.com/alwansan/BO/commit/cfff509c10fa8efab9c81131e2c964839c548a7e). إعادة اختبار المحرك [45213250944](https://github.com/alwansan/BO/actions/runs/45213250944) كانت قيد التنفيذ عند التدوين، ولا نعلن أن original game boot أصبح سليماً حتى نرى مشهد القصة نفسه.

لا تعديل main/CL/biso أو أصول XBE/ISO/saves، ولا حاجة لتجربة هاتف المستخدم قبل اختبار كبير لمرحلة مشتركة حقيقية.
