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

## جلسة 03 — SRC-002a: تثبيت v1.5.0 (2026-11-21)
- APKTool 1.5.0-ar1.0/code35، META-INF revision f3754d6985a8c35204e7df071546e586b10f1806 مطابق tag v1.5.0 upstream (ليس main 8e9c6fc). المصدر المرجعي للعمل على baseline أصبح tag v1.5.0؛ المصدر العربي الكامل غير مثبت. esp.cpp غير موجود في tag؛ layout/cheats/hooks موجودة. راجع docs/V150_PROVENANCE_AR.md.
- CI [44817699656](https://github.com/alwansan/BO/actions/runs/44817699656) completed/success؛ تحقق SHA256 الرسمي، ومقارنة ELF حقيقية، job136570202480، artifact14669549225. الرسمي libmain.so 17957208 B، العربي 72073216 B؛ كلاهما AArch64 وkImageSha1=82d0588124e70102ee650f8969c0a56374476234. .dynstr/.dynsym/.rodata SHA identical؛ .text مختلف، file SHA مختلف. جميع 6 hook-symbol candidates موجودة في الاثنين (بحث substring فقط؛ لا إثبات function semantics). disc الموجود c5bfed254... لا يطابق compile image، لا نُضعف الفحص. النتيجة تثبت release provenance وcompile image match الرسمية/العربية، لا source العربي المطابق ولا compatibility للـISO. دليل memory/evidence/SRC-002a-release-comparison-44817699656.json. NET regression 44817699458 SUCCESS.
- التالي: تحقق من نسخة قانونية لـdefault.xbe SHA1=82d0588124e70102ee650f8969c0a56374476234؛ المالك قد يملكها من نسخته الأصلية. لا تستعمل encrypted upstream XBE أو تدّع تحويل XBE العربي. إن لم تتوفر، واصل شبكات C++ مستقلة/اختبارات Android دون game assets. لا APK ولا gameplay. يمكن فحص hash الدوال المرشحة في ELF كدليل إضافي فقط؛ لا كتابة ذاكرة. لا نحتاج نشر أصول جديدة في public repo. إذا رفع المالك نسخة XBE مطابقة فليكن في مسار خاص مع صلاحية مشروعة، لا تُرفع للفرع العام.

## جلسة 05 — تحقق محلي ومسار بديل للتحليل فقط
- بداية الفرع الحية `fcf4b85b0b5a5ae3057a0cec57b1c2b342706809`؛ أحدث Python CI `44834294477` completed/success.
- فحص الملفات المحلية أعاد إثبات disc XBE SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f` وcompile image `82d0588124e70102ee650f8969c0a56374476234`؛ لا تطابق.
- المالك أكد أنه لا يملك XBE الأصلي المطابق أو مصدر التعديلات العربية. لا تكرر طلبهما كأنهما متوفران.
- فحص ساكن محلي: title `45410083`، retail entry `25A3F` يطابقان config v1.5.0؛ 13 قسماً اجتازت internal SHA1 section digests. هذا لا يثبت أن الكود يطابق الصورة الأصلية أو صحة offsets.
- `weapons.cpp` مثل `esp.cpp` غير موجود في tag v1.5.0؛ المراجع السابقة من main لا تستخدم لإثبات baseline.
- أضيف workflow candidate-audit لتحليل وإعادة ترجمة ساكنة في Windows بواسطة المصدر وSDK المثبتين؛ لا يبني المحرك/Gradle/APK، ولا ينشر مشتقات اللعبة. انتظار نتيجة CI قبل أي قبول.
- محلياً: unittest المصدر 2/2 PASS وPython two-client UDP selftest PASS؛ لا هاتف/Android runtime.

### إغلاق فحص الرموز — 44841534370
Run 44841534370 على code 08fd55b4cb1640b4b56445d43e1801dbc635b1a3 completed/success؛ job136650848324 وartifact14680348280. استقلالاً: symbols108/154 وdata6/11 مطابقة؛ البقية46/5 غير مكتشفة أو غير محسومة. ABI11/11 غير محسومة (`?` أو غياب)، لا تعارض موجب مثبت. جميع host-targets28/28 موجودة؛ لا trap لتعليمات غير مدعومة داخل هذه الدوال ولا عند مواضع hooks، لكن بقية الناتج يحوي تعليمات غير مدعومة. **لا تعادل هذه الفحوص إثبات semantics أو layout أو spawn**. الدليل memory/evidence/SRC-002b-deep-44841534370.json.
- تقرير Android upstream #33 يسجل نفس c5bfed... مع v1.5.0 وتحميل Vulkan: https://github.com/RanggaCasper/Black-Android/issues/33 ؛ ليس إثبات تشغيل نسخة المالك أو توافق co-op، ولا دليل أن اختلاف البصمة سببه التعريب.
- سُئل المالك إن كانت نسخته تدخل مهمة؛ الإجابة/سجل الهاتف لم يصلا بعد. دليل جمع سجل التشغيل: docs/DEVICE_BASELINE_AR.md. هذا طلب تشخيص للنسخة الأصلية، لا تجربة APK جديدة أو تعديل ملفات الحفظ.
- مسار الأصل82d058... ما زال غير متاح؛ مسار candidate c5bfed... لم يغلق، يحتاج حسم static ABI/offsets ودليل runtime. لا تعيد بدء مخابر الشبكة أو طلب XBE المفقود دون دليل جديد.
