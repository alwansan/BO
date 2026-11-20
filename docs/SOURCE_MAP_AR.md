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
