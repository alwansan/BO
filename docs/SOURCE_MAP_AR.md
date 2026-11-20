# خريطة المصدر
مرجع مثبت: https://github.com/RanggaCasper/Black-Android/tree/8e9c6fc7d570503ea68d039cff16c09339ee6d30
| الملف | الدليل |
|---|---|
| README.md / CMakeLists.txt | AOT Xbox x86 → C++ → ARM64، SDL3/Vulkan، kImageSha1 مولد |
| build.bat / .github/workflows/build.yml | Windows recompiler + Android build؛ لا نستعمل encrypted upstream XBE أو مفتاحه |
| native/game_files.cpp | التحقق SHA1 وصلاحية Europe patch؛ لا تعطيل gate |
| src/game/layout.h:34-35 | kWorld=0x2D45D4, kWorldPlayer=0x30، افتراض لاعب واحد |
| src/game/esp.cpp:20-29 | kCharKind=0xC4، kCharPosition=0xA0، yaw/pitch (controller) |
| src/game/cheats.cpp:17-28 | health=0x2F0، kind player=2، local player world+0x30 |
| src/game/hooks.cpp:162-173 | player forward/eye/weapon/fire request؛ لا دليل multi actor |
| src/game/weapons.cpp | inventory, weapon manager, create/free weapon، world models؛ يحتاج lifetime tracing |
هذه offsets مستخرجة من upstream فقط وليست تكاملاً أو إثبات إصدار عربي. قبل telemetry: طابق XBE ونسخة المصدر، تحقق من mission lifecycle، bounds، thread ownership. لا كتابة ذاكرة في HOOK-001.
مسار BO: BLACK-Arabic_src/assets/bundle/game.iso 1793179648 bytes SHA256 f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62، 39 أجزاء؛ libmain.so 72073216 bytes SHA256 fe3a80fbbffd57d4fa10025d1608ab31cc98c941b8be7450b879552f418e5c38، جزآن. لا تعيد جمع النسختين بلا حاجة.

## نقاط دخول أكثر تحديداً — مرجع فقط
- cheats.cpp:61-78: X86_FUNC(sub_001E5FA0)، ecx هو اللاعب، يطابق Player() وkind=2. مرشح read-only بعد __imp__ عند التأكد من حالة world وعمر actor؛ لا تُستبدل وظيفة الأصل ولا تكتب god-mode.
- cheats.cpp:80-87: X86_FUNC(sub_001E74B0)، ecx الضحية وstack+0x18 المهاجم؛ مسار ضرر الجندي (اللاعب له مسار آخر). مرشح رصد أحداث القتال، ليس authoritative replication.
- cheats.cpp:89-100: animation event sub_001E8830 يحوي pointer للشخصية؛ مرشح رصد أحداث لا إرسال pointer عبر الشبكة.
- config/game.toml: hook MouseLook عند 0x1BD6A8؛ لا تعتمد عليه كمصدر لكل مدخلات اللاعب قبل تتبع touch/controller أيضاً.
لا نعتبر أي نقطة مقبولة في APK العربية حتى إثبات الهوية وإعادة إنتاج baseline.

## نتيجة SRC-001 الفعلية — 44813529439
CI SUCCESS وartifact14666839620، قراءة job136555949209 مباشرة. اجتاز SHA256 للأصل ISO 1793179648 B وlibmain 72073216 B، XDVDFS partition 0، XBE 2813952 B، AArch64. Disc SHA1 c5bfed25449c8212d7a976f15246e87ead57713f مقابل kImageSha1 من ELF 82d0588124e70102ee650f8969c0a56374476234: **غير متطابقين**؛ disc ليس Europe المعروف. لا يثبت السبب (تعديل عربي/إصدار/غيره) ولا source commit أو أن APK الحالي لا يعمل. لا تعطيل gate، لا استخدام offsets غير مثبتة. SRC-001 فحص نجح؛ **قبول هوية المصدر BLOCKED**. التالي SRC-002a: تحديد US hash/commit من مصدر upstream وmetadata الإصدار العربي، ومقارنة XBE header/sections مع الصورة المطابقة المرخصة إن توفرت. إن تعذر، اطلب من المالك فقط default.xbe الأصلي المطابق أو مصدر تعديلات Arabic أو log تشغيل كامل؛ لا تفترض الحاجة للثلاثة. بناء baseline ثم HOOK-001. تقرير: memory/evidence/SRC-001-identity-44813529439.json، https://github.com/alwansan/BO/actions/runs/44813529439. لا APK جديدة ولا gameplay أو اختبار هاتف.
