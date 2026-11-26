# Work17 — إصلاح دمج الرسم وتجميع ملفات Android كاملة بلا أصول (2026-11-26)

كشف تجميع hooks.cpp الكامل على Android ARM64 خطأ حقيقيًا في Work16: SDL_GetTicks غير معلن لأن إزالة ترويسة مسار NPC القديم أزالت SDL.h التي كان يعتمد عليها ضمنيًا. أصلح prepare_room_network.py إضافة SDL_timer.h صراحة؛ ملفا backend وتحديث اللاعب كانا يجتمعان أصلًا. اختبارات fixtures وحدها لم تكن تغطي وحدة hooks كاملة، لذلك أضيفت أداة قابلة للاستعادة coop/android_source_units.py وworkflow black-coop-android-source-units.yml. تُجهز المصدر الحقيقي مع جميع ترويساته وتفحص الوحدات السبع المعدلة: hooks،d3d8،cheats،options_menu،exit_menu،difficulty،native/input، بمترجم Android ARM64 حقيقي.

الأداة تثبت Android f3754d6 وSDK99078b1 وSDLfa2c02b (release-3.4.16 من Gradle الأصلي). لا تقرأ XBE/ISO أو تولد أجسام guest؛ image_symbols.h metadata من config/game.toml العام فقط، بنفس sorted-map/optional-functions/data/layout/ABI algorithm في SDK tools/recomp/codegen_emit.cpp:203–227. missing functions تحصل placeholders مميزة؛ missing data تبقى صفرًا. كل التجهيز في output جديد، لا تعديل checkouts المدخلة، ولا أصل/حفظ أو executable/ربط/APK أو تشغيل لعبة. CI يستخدم NDK28.2.13676358 وAPI29 كما Gradle، ويرفع report.json وسجلات frontend فقط. الأدوات كلها هنا ويمكن تشغيلها في GitHub بلا ملفات مالك إضافية.

نجحت الوحدات السبع محليًا (clang21.1.8، Android ARM64 API24 في Termux) بعد إصلاح الترويسة، ونجح calibration metadata واختبارات التحضير الستة وdiff-check. API24 المحلي ليس إثبات build baseline API29؛ CI الجديد ينتظر نتيجته. دليل محلي memory/evidence/WORK_BATCH17_LOCAL_SOURCE_UNITS_2026-11-26.json. النجاح هنا frontend syntax compile فقط، لا ربط libmain أو runtime أو صورة هاتف أو co-op. دليل Work16 السابق يثبت SDK WARP المعزول والجسر الاصطناعي فقط.

التالي: افحص CI NDK/API29 وأصلح أي فشل حقيقي، ثم عُد إلى provider الأجسام الأصلية المستقلة/الفيزياء/الأسلحة وFPV/audio الدائمة (لا byte-swap لكائنات صوت منشورة)، وفعل الضيف في عالم الهوست الأصلي وتطبيقه لدى الضيف وانتقال القصة. مسار العرض الجديد مجرد مجسمات للمود؛ لا تفتح بوابات physical readiness بسبب compile أو renderer أخضر. لا تغيير AI الأعداء/الحلفاء أو assets/save/checkpoint، لا manual ISO/APK workflows أو phone microtests. المشروع لم يكتمل؛ استمر على experiment/black-coop فقط، وحدث الخطة والاستعادة والأدوات باستمرار.

---

