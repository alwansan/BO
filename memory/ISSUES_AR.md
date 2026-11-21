# المشكلات المفتوحة
- SRC-001: إصدار APK العربي والـXBE وإصدار libmain غير مثبت بالمصدر؛ استخرج hashes أولاً ولا تستعمل offsets.
- SRC-002: Windows recompiler يتطلب default.xbe صالحاً وSDK. لا نملك source الخاص بتعديلات العربية، قد تحتاج لاحقاً من المالك.
- ACTOR: singleton kWorldPlayer؛ لا دليل على قدرة إنشاء جندي متحكم به أو لاعب ثانٍ. تحديد factory/lifecycle/collision/AI/camera.
- NET: UDP probe غير مشفر وغير مصادق، لا يجرب فقد حزم أو NAT أو هاتفين؛ ليس production.
- SAVES: فصل حفظ co-op عن single player، وعدم استبدال توقيع/حزمة المالك.
- LICENSE: لا نشر game.iso/default.xbe/generated recompiled game assets؛ حقوق مستقلة عن MIT.

## نتيجة SRC-001 الفعلية — 44813529439
CI SUCCESS وartifact14666839620، قراءة job136555949209 مباشرة. اجتاز SHA256 للأصل ISO 1793179648 B وlibmain 72073216 B، XDVDFS partition 0، XBE 2813952 B، AArch64. Disc SHA1 c5bfed25449c8212d7a976f15246e87ead57713f مقابل kImageSha1 من ELF 82d0588124e70102ee650f8969c0a56374476234: **غير متطابقين**؛ disc ليس Europe المعروف. لا يثبت السبب (تعديل عربي/إصدار/غيره) ولا source commit أو أن APK الحالي لا يعمل. لا تعطيل gate، لا استخدام offsets غير مثبتة. SRC-001 فحص نجح؛ **قبول هوية المصدر BLOCKED**. التالي SRC-002a: تحديد US hash/commit من مصدر upstream وmetadata الإصدار العربي، ومقارنة XBE header/sections مع الصورة المطابقة المرخصة إن توفرت. إن تعذر، اطلب من المالك فقط default.xbe الأصلي المطابق أو مصدر تعديلات Arabic أو log تشغيل كامل؛ لا تفترض الحاجة للثلاثة. بناء baseline ثم HOOK-001. تقرير: memory/evidence/SRC-001-identity-44813529439.json، https://github.com/alwansan/BO/actions/runs/44813529439. لا APK جديدة ولا gameplay أو اختبار هاتف.

## جلسة 03 — SRC-002a: تثبيت v1.5.0 (2026-11-21)
- APKTool 1.5.0-ar1.0/code35، META-INF revision f3754d6985a8c35204e7df071546e586b10f1806 مطابق tag v1.5.0 upstream (ليس main 8e9c6fc). المصدر المرجعي للعمل على baseline أصبح tag v1.5.0؛ المصدر العربي الكامل غير مثبت. esp.cpp غير موجود في tag؛ layout/cheats/hooks موجودة. راجع docs/V150_PROVENANCE_AR.md.
- CI [44817699656](https://github.com/alwansan/BO/actions/runs/44817699656) completed/success؛ تحقق SHA256 الرسمي، ومقارنة ELF حقيقية، job136570202480، artifact14669549225. الرسمي libmain.so 17957208 B، العربي 72073216 B؛ كلاهما AArch64 وkImageSha1=82d0588124e70102ee650f8969c0a56374476234. .dynstr/.dynsym/.rodata SHA identical؛ .text مختلف، file SHA مختلف. جميع 6 hook-symbol candidates موجودة في الاثنين (بحث substring فقط؛ لا إثبات function semantics). disc الموجود c5bfed254... لا يطابق compile image، لا نُضعف الفحص. النتيجة تثبت release provenance وcompile image match الرسمية/العربية، لا source العربي المطابق ولا compatibility للـISO. دليل memory/evidence/SRC-002a-release-comparison-44817699656.json. NET regression 44817699458 SUCCESS.
- التالي: تحقق من نسخة قانونية لـdefault.xbe SHA1=82d0588124e70102ee650f8969c0a56374476234؛ المالك قد يملكها من نسخته الأصلية. لا تستعمل encrypted upstream XBE أو تدّع تحويل XBE العربي. إن لم تتوفر، واصل شبكات C++ مستقلة/اختبارات Android دون game assets. لا APK ولا gameplay. يمكن فحص hash الدوال المرشحة في ELF كدليل إضافي فقط؛ لا كتابة ذاكرة. لا نحتاج نشر أصول جديدة في public repo. إذا رفع المالك نسخة XBE مطابقة فليكن في مسار خاص مع صلاحية مشروعة، لا تُرفع للفرع العام.

## NET-003/004
- NET-003 Linux and Android cross compile PASS 44834027711. **Android phone execution untested**. CLI no game integration, no scene actors, no 4 device test. See `memory/evidence/NET-003-cpp-android-44834027711.json`.
- Packet session token is predictable/not cryptographically authenticated; no packet encryption, replay protection on join, IP roaming, NAT traversal, congestion/reliability. Use laboratory only, private networks. Binding 0.0.0.0 on trusted LAN is opt-in.
- NET-004 needs two Android phones with matching permissions/ZeroTier, device logs/performance; no public forwarding of port 24872.

## جلسة 05 — تحقق محلي ومسار بديل للتحليل فقط
- بداية الفرع الحية `fcf4b85b0b5a5ae3057a0cec57b1c2b342706809`؛ أحدث Python CI `44834294477` completed/success.
- فحص الملفات المحلية أعاد إثبات disc XBE SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f` وcompile image `82d0588124e70102ee650f8969c0a56374476234`؛ لا تطابق.
- المالك أكد أنه لا يملك XBE الأصلي المطابق أو مصدر التعديلات العربية. لا تكرر طلبهما كأنهما متوفران.
- فحص ساكن محلي: title `45410083`، retail entry `25A3F` يطابقان config v1.5.0؛ 13 قسماً اجتازت internal SHA1 section digests. هذا لا يثبت أن الكود يطابق الصورة الأصلية أو صحة offsets.
- `weapons.cpp` مثل `esp.cpp` غير موجود في tag v1.5.0؛ المراجع السابقة من main لا تستخدم لإثبات baseline.
- أضيف workflow candidate-audit لتحليل وإعادة ترجمة ساكنة في Windows بواسطة المصدر وSDK المثبتين؛ لا يبني المحرك/Gradle/APK، ولا ينشر مشتقات اللعبة. انتظار نتيجة CI قبل أي قبول.
- محلياً: unittest المصدر 2/2 PASS وPython two-client UDP selftest PASS؛ لا هاتف/Android runtime.

### أول CI للمرشح — تصحيح toolchain
Run 44840599544 على code e0e1378843e7a727feb1cfe35d7404ebaafb0e2c فشل في بناء أدوات التحليل قبل تشغيل recompiler. سبب مثبت من log job136647712886: global LLVM Clang20 اختير عبر PATH مع MSVC14.52 الذي يطلب Clang22 (`STL1000`). التعديل يختار `$VCINSTALLDIR/Tools/Llvm/x64/bin/clang-cl.exe` صراحة، كما يفعل build.bat المرجعي، ويسجل الإصدار؛ لا تعطيل لشرط STL ولا تغيير SDK/ملف اللعبة. لا نتيجة توافق للمرشح من التشغيل الفاشل.

### SRC-002b static probe — نتيجة حقيقية وخطوة فحص أعمق
Run 44840960136، job136648896882 على code dd5261a28db3443a6bacbccc7e8fdd1114e818a2 SUCCESS؛ bundled Clang22.1.3. تحليل المرشح c5bfed... نجح بنيوياً مع config/SDK المثبتين: 15042 functions و798292 instructions، لكن statistics تحتوي تعليمات غير منفذة؛ لا baseline/توافق gameplay. artifact14680518020 metadata فقط. أضيف فحص مستقل xbe_symbols لمقارنة symbols/data/ABI، وتقرير عن دوال host الغائبة والتعليمات غير المنفذة فيها وعند hooks؛ انتظار CI الجديد. لا تحويل الصورة القديمة أو تعطيل تحقق hash.

### إغلاق فحص الرموز — 44841534370
Run 44841534370 على code 08fd55b4cb1640b4b56445d43e1801dbc635b1a3 completed/success؛ job136650848324 وartifact14680348280. استقلالاً: symbols108/154 وdata6/11 مطابقة؛ البقية46/5 غير مكتشفة أو غير محسومة. ABI11/11 غير محسومة (`?` أو غياب)، لا تعارض موجب مثبت. جميع host-targets28/28 موجودة؛ لا trap لتعليمات غير مدعومة داخل هذه الدوال ولا عند مواضع hooks، لكن بقية الناتج يحوي تعليمات غير مدعومة. **لا تعادل هذه الفحوص إثبات semantics أو layout أو spawn**. الدليل memory/evidence/SRC-002b-deep-44841534370.json.
- تقرير Android upstream #33 يسجل نفس c5bfed... مع v1.5.0 وتحميل Vulkan: https://github.com/RanggaCasper/Black-Android/issues/33 ؛ ليس إثبات تشغيل نسخة المالك أو توافق co-op، ولا دليل أن اختلاف البصمة سببه التعريب.
- سُئل المالك إن كانت نسخته تدخل مهمة؛ الإجابة/سجل الهاتف لم يصلا بعد. دليل جمع سجل التشغيل: docs/DEVICE_BASELINE_AR.md. هذا طلب تشخيص للنسخة الأصلية، لا تجربة APK جديدة أو تعديل ملفات الحفظ.
- مسار الأصل82d058... ما زال غير متاح؛ مسار candidate c5bfed... لم يغلق، يحتاج حسم static ABI/offsets ودليل runtime. لا تعيد بدء مخابر الشبكة أو طلب XBE المفقود دون دليل جديد.

## جلسة 06 — حسم معنى ABI غير المحسوم
فُحص HEAD08c545a8389f509058ff49cfd62b187dae6f6fca وActions مباشرة؛ آخر run44841534370 completed/success ولا run حي آخر. فُك ترميز x86 محلياً في مجلد مؤقت لجميع مواقع ABI11. الاستخدامات المشاهدة للسجلات/stack تدعم إعداد المصدر (edx/eax/esi/ecx/ebx وret4/16 حيث شوهدت)، لا تثبت ABI/semantics كاملة. الدليل memory/evidence/SRC-002b-manual-abi-inputs.json؛ لا game bytes/assembly في المستودع، وحذفت المشتقات المؤقتة تلقائياً. لا تعتبر46 نتيجة scanner فارغة تعارضات موجبة.

الحد الحالي: نحتاج تحديد الصورة التي تحملها النسخة المثبتة فعلاً. AssetFolders يبحث في Black المشترك وفي ملفات التطبيق؛ لذلك قد يكون ISO المرفق مختلفاً عما يقرأه الهاتف. نتيجة دخول المهمة والسجل ما زالا غير واردين. لا نطلب XBE المفقود ثانية. لا نبني gameplay فوق افتراض أن القرص المرفق هو المستخدم فعلاً، ولا نقول إن APK جماعية جاهزة.

## جلسة07 — وصول تجربة الهاتف الفعلية وبناء candidate منفصل
المالك أكد أن النسخة العربية تدخل القصة وتعمل دون مشاكل، ونسخ بيانات التطبيق محلياً. قرئ السجل الخاص دون نشره: build f3754d6، API35، Adreno730، loaded XBE c5bfed... مع another release، Exit0 بعد146s. تأكد SHA256 لنسخة ISO من الهاتف=f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62 مطابق للـmanifest. هذا يحسم أن الصورة المرفقة هي المستخدمة فعلاً؛ لا تعاود طلب تجربة الأصل أو XBE المفقود. نجاح المهمة من إفادة المالك، لا من instrumentation داخل log.
السجل يحوي `quit: ConfirmWarning, FEAudio failed` في مسار exit_menu.cpp؛ سُجل ولم يخفَ، ولا نفترض منه عطل قتال/صوت أثناء المهمة. دليل metadata فقط DEVICE-existing-owner-runtime.json؛ لا raw log أو حفظ في GitHub.

بدأ مسار بناء أساس مرشح مستقل من نفس c5bfed image الفعلي بالمصدر/SDK المثبتين. ليس إعادة إنتاج ثنائية للمصدر العربي ولا قبول ABI كامل؛ compile image سيتولد تلقائياً من المدخل الحقيقي دون تعطيل match. الحزمة org.bocoop.black.baseline، Java namespace/JNI الأصلي محفوظ، native/src/config لم تُعدل، حفظ منفصل، label NO CO-OP، updater محجوب في هذه الحزمة. لا أصول لعبة في APK. هذا اختبار baseline يلزم قبل HOOK/ACTOR، لا استبدال لهدف co-op.
خطة التنفيذ docs/superpowers/plans/2026-11-21-candidate-baseline.md؛ الاختبارات المحلية4/4 PASS (بدأ test preparer قبل وجوده ثم نجح بعد التنفيذ)، YAML/Python syntax وdiff check PASS. CI baseline مجهز ولم تثبت نتيجته بعد.

Baseline CI بدأ فعلياً: run44845056293، job136662594366، code6aa718839d912d8924af62f11c4c55999847c24e؛ تابع نفس run ولا تعيد تشغيله بسبب مهلة رصد. قبول البناء/الفحص/الهاتف لم يثبت بعد. دليل تجربة الناتج عند نجاحه docs/CANDIDATE_BASELINE_DEVICE_AR.md. Python regression44845056127 SUCCESS؛ source audit44845056215 قيد الفحص.

### Baseline أول محاولة — تشخيص Windows CLI
Run44845056293 على code6aa718839d912d8924af62f11c4c55999847c24e فشل job136662594366 قبل أي native compile: Gradle رأى Task `.5.0-baseline-candidate1` بسبب tokenization لوسيط PowerShell غير المقتبس. اقتبست وسيطي -PversionName و-PversionCode كـstrings كاملين؛ لم يتغير source/SDK أو إعداد اللعبة. الاختبارات وتجهيز الحزمة وإعادة الترجمة نجحت قبله، وAPK/metadata uploads تخطيت كما يجب. source audit44845056215 SUCCESS وUDP44845056127 SUCCESS. لا APK أو runtime acceptance من هذه المحاولة.
