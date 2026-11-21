# خطة BO-COOP
| Gate | القبول | الحالة |
|---|---|---|
| NET-001 | عميلان loopback وحالة authoritative مستقلة | PASS 44808411128 |
| SRC-001 | تحقق SHA256/XBE/ELF؛ مطابقة هوية compile image | audit PASS 44813529439، identity BLOCKED (SHA1 mismatch) |
| SRC-002 | إعادة بناء upstream pinned على Windows دون أصول منشورة أو تغييرات gameplay | مفتوح |
| HOOK-001 | تسجيل محلي read-only للحالة دون crash أو تعديل الحفظ | مفتوح |
| ACTOR-001 | spawn + render + move شخصية ثانية دون singleton conflict | مفتوح |
| ACTOR-002 | لاعبان مستقلان وcamera/weapon/collision + cleanup | مفتوح |
| NET-002 | اتصال هاتفين LAN/ZeroTier؛ خسارة/ترتيب/timeout وإعادة اتصال واتصال مصادق | مفتوح |
| COMBAT-001 | authoritative AI/damage/ammo/projectile/props/objectives | مفتوح |
| COOP-001 | مهمة أولى كاملة على هاتفين مع checkpoint | مفتوح |
| COOP-002 | 4 لاعبين وبقية المهام وواجهة غرف وWAN | مفتوح |
قبل كل gate: اختبارات سالبة، نتائج صريحة، لا نسبة اكتمال وهمية. لا بناء APK منشور قبل إثبات الهوية وbaseline.

SRC-002a: provenance/image match قبل SRC-002 baseline. راجع evidence SRC-001.

SRC-002a PASS: v1.5.0 metadata and compile image identity matched official ELF. SRC-002 baseline BLOCKED on exact XBE. See memory/evidence/SRC-002a-release-comparison-44817699656.json. NET-002 isolated native lab may proceed independently.

## مرحلة NET-003 (21 نوفمبر)
| Gate | النتيجة | الدليل |
|---|---|---|
| NET-003a codec/game-neutral Host | PASS Linux ASAN/UBSAN UDP loopback | [44833884316](https://github.com/alwansan/BO/actions/runs/44833884316) |
| NET-003b CLI two processes | PASS Linux loopback | [44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) |
| NET-003c Android ARM64 compile | PASS cross compile, **device untested** | artifact 14677732935 |
| NET-004 device LAN/ZeroTier | OPEN | no Android runtime test |
| NET-005 authenticated reliable transport | OPEN | no production-grade crypto/reliability |
| HOOK-001/ACTOR-001 | BLOCKED by exact game baseline | original XBE missing |

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

إعادة baseline بعد إصلاح quoting تعمل فعلياً: run44845637359/job136664530136/code5212673e5bc92bbe30a7e001cc8e26924a14f0af. اجتازت تجهيز الأدوات والاختبارات/recompiler/Android SDK ووصلت إلى Gradle Android compile عند آخر رصد. هذا handle حي يتابع، لا يعاد تشغيله بسبب timeout ملاحظة. لا APK مثبتة بعد.

### بناء candidate فعلي — 44845637359 SUCCESS
Run44845637359/job136664530136 على code5212673e5bc92bbe30a7e001cc8e26924a14f0af completed/success. Gradle BUILD SUCCESSFUL8m17s. Artifact14682519899 `black-baseline-candidate-android-arm64-NO-COOP`، APK21727055B SHA2564ead325d67772a1c877b9f073aebc0e9eaaab45e9a0028d2380645c4bc2f69f7.
فحوص APK الفعلية: packageorg.bocoop.black.baseline، activitycom.racas.black.BlackActivity، ARM64 فقط، debug signature، لا صلاحيات تخزين واسعة أو ISO/XBE/generated source في APK؛ native compile-image SHA1=c5bfed25449c8212d7a976f15246e87ead57713f يطابق XBE الحقيقي. تنزيل artifact وإعادة فحص ZIP/APK SHA وELF identity محلياً نجحا مستقلاً. metadataArtifact14683233366. لا Android runtime جديد قبل تجربة المالك، ولا co-op أو read-only gameplay hook أو spawn منفذ.
الدليل memory/evidence/BASELINE-candidate-build-44845637359.json؛ رابط artifact https://github.com/alwansan/BO/actions/runs/44845637359/artifacts/14682519899 . الملف المحلي outputs/baseline-44845637359 خارج repo. دليل التجربة docs/CANDIDATE_BASELINE_DEVICE_AR.md. لا تعيد طلب تشغيل APK الأصلية؛ ننتظر تجربة الحزمة الجديدة فقط.

## جلسة08 — تسليم عاجل لجلسة أخرى
المالك جرّب APK baseline الجديدة وأكد عدم وجود تعطل/مشاكل، وقدم بيانات org.bocoop.black.baseline محلياً. log يؤكد image c5bfed... دون another release وExit0 عند162s. missing image عند1s سبق إكمال الاستيراد عند24s وليس فشل baseline. FEAudio عندالخروج ما زال معروفاً. دليل metadata DEVICE-rebuilt-baseline-owner-runtime.json؛ raw log وISO/hdd لا تنشر. لا تطلب إعادة تجربة baseline أو XBE المفقود.

بدأ HOOK001 اختيارياً للقراءة فقط، ثم طلب المستخدم إيقاف التنفيذ وتسليم كل شيء سريعاً قبل انتهاء حد الاستخدام. كود WIP محفوظ في coop/probe/{probe_core.hpp,probe.h,probe.cpp,test_probe.cpp} وخطة docs/superpowers/plans/2026-11-21-readonly-probe.md. **ليس متصلاً بالبناء أو المصدر بعد، لا APK probe ولا CI له ولا تجربة هاتف.** core test نجح محلياً بالأمر clang++ -std=c++20 -O1 -Wall -Wextra -Werror -pedantic coop/probe/test_probe.cpp -o /tmp/black-probe-test && /tmp/black-probe-test. هذا اختبار host فقط، sanitizer/Android لم يجريا لهذا الكود.

التصميم: package جديدة org.bocoop.black.probe1 لتجنب اختلاف debug signing مع baseline والحفاظ على الأصل. opt-in زر Start capture عند التشغيل، native JNI nativeCoopProbeEnable غير مضاف إلى Java بعد؛ لذلك لا تنشط النسخة الحالية من sampler وحدها. لا شبكة أو writes إلى guest أو spawn أو استدعاء guest مجهول. process_vm_readv(getpid()) للقراءات من العملية نفسها، bounds32/host overflow/partial reads/NaN guards، لا memcpy مباشر من guest pointer. سجل محلي logs/coop_probe.jsonl. حدود:64 pointer/vtable pairs لكل world epoch،256 attempts/s،32 records/s،2048 records،180s من world صالح،1MiB؛ انتظار world محدود300s في runtime. IDs مجرد correlations؛ إعادة استعمال نفس address/vtable قد لا تكتشف. Animation owners ليست character تلقائياً، لا تقرأ pos/health منها. player health فقط عند local=world+0x30، LOD يقرأ pos +A0، resource roots +320/+35C مؤقتة الدلالة. errno وfloating environment تحفظ عبر HostState.

## ما لم يُنجز قبل التسليم
1. مراجعة runtime WIP كوحدة كاملة؛ يحتاج #include <algorithm> صريح لاستخدام std::copy. catch-all يعطل capture لكنه لا يغلق fd أو يسجل السبب؛ راجع مسار الخطأ. إعادة Enable بعد End ليست مصممة لإعادة بدء التسجيل؛ اجعل زر مرة واحدة/حالة واضحة. راجع انصاف/rate rotating-ID sampling وثبات IDs وepoch/reset عبر reload؛ لا تعتبرها lifecycle مثبتة.
2. أضف prepare_probe.py مع preflight anchors قبل أي كتابة، يرث private import/no broad storage/updater isolation من prepare_baseline.py، يغير package/label إلى probe1 وREAD ONLY، ويضيف Java opt-in/native declaration. حافظ على JNI class com.racas.black.BlackActivity. لا توقع بمفاتيح الأصل ولا تستبدل الأصل أو baseline.
3. اربط source cheats.cpp entry sub_001E5FA0 بنداء ObservePlayer(ctx.ecx,base)، وhooks.cpp entry sub_0017B130 بنداء ObserveAnimation(ctx.esp,base). لا تغير __imp calls أو guest context. انسخ probe sources إلى reference/src/coop/؛ glob الموجود src/*/*.cpp يشملها بلا تعديل CMake.
4. new wrapper sub_001C55A0 في probe.cpp يفترض generated __imp__sub_001C55A0. أكد تعريفه الحقيقي في generated code/mapping داخل CI fail-closed قبل link، وليس مجرد اسم في source. لا تبدل offsets أو تعطّل image hash check.
5. أضف workflow probe منفصل مبني على black-coop-baseline.yml: Linux host ASAN/UBSAN core tests، Python preparer tests، Android APK build/identity/package/signature/assets verification. baseline workflow لا يحتاج إعادة تشغيل لمجرد WIP غير موصول. راجع مستقل ثم Android device capture.
6. لا ترفع raw phone logs، saves، ISO/XBE، generated game source، أو signing credentials. المدخل ISO المرفوع والمقسم كاف في CI؛ راجع manifestrestore/source_audit.

## أدلة مستقلة للمواقع الجديدة
مراجعة agent read-only ثانية للـXBE ذاته:0x1C55A0 prologue/preceding padding، caller0x1C61F4 يسبقه pushEDI عند1C61F3، callee [EBP+8] عند1C55A9 ثم ret4؛ إذن entry[ESP+4] object. عند1C561E load16B vector object+A0 ثم camera distance. object+320 pointer محمل1C55AC ومفكوك1C55B9؛ +35C pointer مفكوك1C5491/1C54C0. تعريف generated ما زال يجب فحصه في CI. player ECX يؤكده1E5FAD ومصدر cheats؛ animation stack/controller+0 يؤكده1_7B130 disassembly وhooks.cpp. لا full-body model أو factory أو playable second actor مثبت. مشتقات التحليل المؤقتة حُذفت.

## مصادر reader
AOSP Android15 SYSCALLS.TXT يحتوي process_vm_readv all، app/common blocklists لا تحتويه. قرئت المصادر الأساسية https://android.googlesource.com/platform/bionic/+/refs/tags/android-platform-15.0.0_r9/libc/SYSCALLS.TXT وSECCOMP_BLOCKLIST_APP.TXT وSECCOMP_BLOCKLIST_COMMON.TXT. ليس هذا إثبات runtime على هاتف المالك أو كل OEM؛ reader يجب أن يفشل مغلقاً إذا رجع EPERM/ENOSYS. لا root أو تجاوز صلاحيات أو معالجة SIGSEGV للحيلولة دون حماية الذاكرة.

## جلسة09 — HOOK001 (read-only)
| Gate | State |
|---|---|
| source preflight + C++ guarded reads ASAN/UBSAN | PASS [44852312025](https://github.com/alwansan/BO/actions/runs/44852312025) |
| Android probe1 opt-in JNI + original generated hook verify | CI [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) pending |
| probe APK verification / signature / isolated storage / no network permissions | pending Windows artifact |
| device mission capture | NOT REQUESTED, only after verified probe APK |
| full-body actor creation / co-op | not started |
Owner ISO uploaded 5 parts and fully SHA-verified. See docs/OWNER_ISO_UPLOAD_AR.md; GitHub copy equivalent.

## تحديث جلسة09 — تجميع Windows ومتغير namespace
- Hook C++ sources الأصلية المستعملة في الإصدار v1.5.0 تستخدم `using namespace kernel;` أمام X86_EXTERN_FUNC/X86_FUNC. أضيفت المطابقة إلى `coop/probe/probe.cpp` commit5c5be3972e2131d4b34bad273f41b89e54de5c74؛ إجراء وقائي وليس دليلاً أن البناء القديم فشل.
- Windows الأول [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) ما زال قيد التجميع عند هذا التوثيق، والثاني بالشفرة الأحدث [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) pending/queued (concurrency). لا تعتبر الأول build نهائياً بعد تغيير source؛ اختبر الثاني وأصلح سجلات الفشل الحقيقية. تحديث preflight [44852913135](https://github.com/alwansan/BO/actions/runs/44852913135) يحتاج تحقق نهاية.

## Windows Probe APK — first result PASSED (not phone acceptance)
- [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) completed SUCCESS, job136686638837: real Windows/NDK Android ARM64 build with newly injected read-only observer hooks, generated LOD symbol preflight, JNI export, source change allowlist. Debug APK package `org.bocoop.black.probe1` with separate storage and label BLACK Probe 1 - READ ONLY, no internet/network/broad storage permission, no game ISO/XBE in artifact. Built compile image c5bfed25449c8212d7a976f15246e87ead57713f matches owner ISO. APK 21,738,227 B, SHA256 `531f6cf29fdcc29db1cb5ab39971fab2670672a389e33b0b2e50597ecfaae0fb`; artifact14686665295, metadata14685973542. Exact CI evidence `memory/evidence/HOOK001-probe-candidate-44852419929.json`.
- This first build belongs to source `f911ef9e5b6b83ff19aac39406af8c70d3814f74`. A small kernel namespace convention patch was committed after that run; newer [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) still pending/in-progress at writing. Do not claim updated head APK verified until its result. **No probe phone test or gameplay/co-op acceptance.**

## جلسة09 — تحليل سجلات الرصد دون نشر البيانات
- `coop/analysis/summarize_probe.py` يدقق start header/حدود2MiB و8192 سطر و4096B للسطر، يحصي event/epoch/thread IDs كأعداد إجمالية فقط، يحذف من المخرجات أي raw position/vtable/animation names/actor pointer/ID/phone-specific path. `capture_analysis_checks.py` يختبر التسريب والحالات السالبة بما فيها سبب End غير قابل للتجزئة.
- [44853385306](https://github.com/alwansan/BO/actions/runs/44853385306) و[44853421222](https://github.com/alwansan/BO/actions/runs/44853421222) completed SUCCESS على synthetic inputs فقط. لا raw phone logs، ولا device runtime proof. تعليمات docs/PROBE_DEVICE_GUIDE_AR.md.

## جلسة 10 — HOOK001 coverage
- [x] Android namespace-aligned previous Probe1 APK CI [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) SUCCESS; phone execution open.
- [x] Prevent local-player sample starvation by reserving local slot; host sanitizer + pinned-source [44854667613](https://github.com/alwansan/BO/actions/runs/44854667613) SUCCESS.
- [ ] Verify Android rebuild [44854667587](https://github.com/alwansan/BO/actions/runs/44854667587) and associated signed APK metadata before recommending artifact.
- [ ] HOOK001 phone runtime: optional capture only, store logs privately, summarize without raw locations; **not** baseline retest.
- [ ] ACTOR-001 (full body proxy) remains unproven; no networked gameplay yet.

## تحديث جلسة10 — تحليل الخصوصية والنتائج الحية
- [44854855170](https://github.com/alwansan/BO/actions/runs/44854855170) completed SUCCESS for privacy-safe probe log analyzer tests (synthetic only), on code bcbf1964b7fb378ad17fed2724aaeefbab91de46. Added bounded diagnostic reason-code flags: capture_end_not_seen, no_samples, no_player_samples, no_lod_samples, no_animation_samples, reader_unavailable, local_output_error. No raw pointer, position, names, thread/epoch IDs or raw logs in summaries. Tests include empty/permission and truncated capture. These flags DO NOT prove model absence or factory success.
- [44854855083](https://github.com/alwansan/BO/actions/runs/44854855083) Python UDP regression SUCCESS. Android candidate run44854667587 was in progress (Gradle native compile) at this documentation update. Do not infer Android runtime from host CI.

## Session11 gate update
- [x] Windows verified reserved-slot Probe1 candidate [44854667587](https://github.com/alwansan/BO/actions/runs/44854667587) SUCCESS; not phone-tested. Artifact14687571586 and memory/evidence/HOOK001-player-slot-44854667587.json.
- [x] Local-promotion and slot serial negative tests [44855984868](https://github.com/alwansan/BO/actions/runs/44855984868) PASS; current Windows [44855984997](https://github.com/alwansan/BO/actions/runs/44855984997) in_progress at recording.
- [x] Game-neutral scene inbox [44856160545](https://github.com/alwansan/BO/actions/runs/44856160545) PASS; no device/BLACK integration.
- [ ] Verify latest APK and optionally first HOOK001 device capture (not baseline re-test).
- [ ] Validate actor lifetimes/full-body before ACTOR-001; no guest writes or production auth claims.

HOOK001 fairness source e9ad056: host sanitizer + pinned-source [44856368591](https://github.com/alwansan/BO/actions/runs/44856368591) PASS, Android [44856368592](https://github.com/alwansan/BO/actions/runs/44856368592) pending. Next verify APK SHA/artifact; older f7 build 44855984997 still running. No device acceptance.

## NET004-PREP composition test (2026-11-21)
- Code commit a5d164ca6f5f19bb807851429c7ba294a5d572b4 wires two previously isolated *test components* under coop/bridge/test_transport_pipeline.cpp. A deterministic host validates Join/Accept and Input token/endpoint/sequence; a transport worker stages value-only commands into SceneInbox; the owning simulated game thread drains them into a mock scene. Negative tests cover forged endpoint/token, duplicate replay, wrong drain thread, and scene reload dropping stale input. No actual network socket is opened by this composition test (separate Linux UDP socket tests exist), no BLACK hooks/actor memory access/gameplay, no authentication or phone test.
- [44856542339](https://github.com/alwansan/BO/actions/runs/44856542339) completed SUCCESS with ASAN/UBSAN and strict C++20; input path tested synthetically only. Keep wording «mock host→scene pipeline», not co-op, and do not treat a pseudorandom lab token as secure authentication.

## جلسة12 — Verified Android build and ACTOR triage
- [x] Close pending Windows [44855984997](https://github.com/alwansan/BO/actions/runs/44855984997) SUCCESS (intermediate promotion), verify APK SHA and artifact.
- [x] Close final Windows [44856368592](https://github.com/alwansan/BO/actions/runs/44856368592) SUCCESS, APK SHA256 371de65dd7972d1505713d31d2a41080aa99877f848ac35a24fefe8b825ca2d7, artifact14687833595, source e9ad056805cabacadcf3882d5ff7aeb5f45df914; report memory/evidence/HOOK001-final-fairness-build-44856368592.json.
- [x] Implement anonymous event-kind correlation analysis under `coop/analysis`; synthetic privacy CI [44857976254](https://github.com/alwansan/BO/actions/runs/44857976254) SUCCESS. No APK rebuild.
- [ ] First private read-only **Probe1** device runtime capture; never repeat accepted baseline. Use docs/PROBE_DEVICE_GUIDE_AR.md.
- [ ] Interpret LOD/animation/position & thread observations conservatively. Confirm actor identity/lifetime and safe factory before ACTOR-001 writes or networked gameplay.

## Final APK extraction verified, session12 (2026-11-21)
- Downloaded GitHub Actions **artifact14687833595** from run44856368592 through GitHub connector into this session's isolated container. ZIP extraction yielded actual `black-probe1-candidate-READ-ONLY-NO-COOP.apk` at **21,738,563 bytes** SHA256 **371de65dd7972d1505713d31d2a41080aa99877f848ac35a24fefe8b825ca2d7**, exact CI metadata match. Checked APK ZIP has16 entries and three ARM64 native libraries: libSDL3.so, libc++_shared.so, libmain.so; no files ending .iso/.xbe/.enc/.cpp. Original JNI/signature/package checks were verified in Windows job, not re-run by the container.
- Delivered temporary sandbox APK link in chat `/mnt/data/BLACK-Coop-Probe1-ReadOnly-20261121.apk` after verifying exact path; local container files **are not guaranteed durable across sessions**. Durable download remains GitHub Actions run44856368592 (retention 14 days). Do not commit APK or ISO to BO. No phone runtime report yet.

## جلسة13 — Actual device evidence + new export/UI gates
- [x] Inspect owner's Probe1 black.log in CL + screenshots: import, game running, probe capture activated, Exit0. Private JSONL not readable; CL file 0B. memory/evidence/DEVICE-probe1-owner-20261121.json.
- [x] Implement bounded MediaStore Downloads export opt-in (`coop_probe.txt`, `black_log.txt`) in Probe2 separate package preserving installed Probe1.
- [x] Add in-game CO-OP/ROOM page as read-only in OPTIONS (native Flash extension, pinned source); host/preparer CI [44860875040](https://github.com/alwansan/BO/actions/runs/44860875040) SUCCESS.
- [ ] Android package2 compile [44860874976](https://github.com/alwansan/BO/actions/runs/44860874976) pending. Verify JNI, native image, package, no game assets/network/storage permissions and exporter compiled; obtain artifact/hash.
- [ ] On phone: optional new Probe2 session and public snapshots at exact Download path + actual menu screenshot. Existing baseline remains tested, no repeat.
- [ ] Use capture samples for ACTOR-001 read-only lifecycle evidence; no guest writes/host/join feature until validated.

## جلسة14 — Probe2 CI/artifact verified and next device gate
- [x] Android final [44860874976](https://github.com/alwansan/BO/actions/runs/44860874976) SUCCESS, source `d8dba8d8da2da69c62718a87f3e6f03f16be0d2f`, artifact 14690036713, APK SHA256 `861fd501497f2251feeb544c56430e084c3b48d3afa87e0d3a65c12e8a1115c0`, verified actual archive/DEX/native strings. Evidence `memory/evidence/PROBE2-android-44860874976.json`.
- [x] Prior e2 Android build 44860718925 SUCCESS but superseded; intermediate 44860822227 cancelled. Host ASAN/UBSAN 44860875040 PASS.
- [ ] First **Probe2 only** Android15 run: check actual MediaStore directory, non-empty `coop_probe.txt`, `black_log.txt`, assess if sampler ends/has records; avoid pushing raw logs to public CL.
- [ ] Verify native in-game SETTINGS -> CO-OP / ROOM sixth page, scroll and back; design change later if owner wants directly under main/pause SETTINGS button.
- [ ] Only after real character trace: pursue ACTOR-001 lifecycle proof. No gameplay write/network integration until then.

## جلسة15 — الرصد الحقيقي للـACTOR على جهاز المالك
- [x] Read CL Probe2 exported JSONL: 400 valid observations (131 player,172 lod,97 animation), one epoch, one observed thread,14 correlations; end absent => partial. Evidence `memory/evidence/DEVICE-probe2-owner-20261121-064812-safe-summary.json`.
- [x] Verify MediaStore export to `Download/Games/cl/black log/logs` **works on device**; Probe2 capture actually recorded events.
- [x] Enrich privacy-safe analyzer: player/LOD vs NPC-like LOD/animation groups, resource/render coverage, in-memory anonymous position variance, animation label changes; field at clip+0x18 semantics **unproven**. Privacy [44863742397](https://github.com/alwansan/BO/actions/runs/44863742397) SUCCESS; UDP [44863742412](https://github.com/alwansan/BO/actions/runs/44863742412) SUCCESS. No APK rebuild needed.
- [ ] ACTOR-001A: prove appearance/disappearance/respawn and model ownership across scene/epoch before factory/clone.
- [ ] In-game CO-OP/ROOM page still not pictured/confirmed; top-level option immediately below `الإعدادات` not yet built. Review safe UI hook points separately.
- [ ] ACTOR-001/COMBAT-001/gameplay network remain unimplemented; no guesswork about remote render proxy.

## جلسة15 — تصحيح CI ومرشح القائمة العليا
- After privacy analyzer field renamed to explicitly `nonzero_unverified_clip_field_0x18_count`, synthetic test used its old key, causing [44863951613](https://github.com/alwansan/BO/actions/runs/44863951613) FAILURE (KeyError only). Fixed test expectation in code `77e5c031ecc1d55cbefdc73a7a1139a714150b82`; [44864020398](https://github.com/alwansan/BO/actions/runs/44864020398) privacy analyzer SUCCESS and [44864020404](https://github.com/alwansan/BO/actions/runs/44864020404) isolated UDP SUCCESS. No Android build needed (only offline analysis/test).
- Pinned v1.5.0 upstream `src/game/exit_menu.cpp` was inspected read-only: title menu already duplicates Options text clip into QuitButton below settings using Flash `CopyButton/Follow/JoinRing`. Strong candidate for independent CO-OP top-level button **between OPTIONS and QUIT**, with no claim implementation yet. Plan and hazards in `docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md`; keep current Probe2 APK intact until isolated Probe3 experiment and device UI proof.

## جلسة16 — مصادر المالك المفككة والتطابق
- [x] Pin `biso@9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, review manifest and restore script; metadata574 original files/578 stored parts, 3 large split files,0 missing,0 size mismatches.
- [x] Independently read owner's **actual ISO** currently mounted; SHA256 `f6d15ff...3ec246c62` passes, XDVDFS574 files/40 dirs, exact byte-sum match, XBE+four additional file SHA256 samples match `biso`.
- [x] Add pinned metadata-only + synthetic XDVDFS CI [44867209688](https://github.com/alwansan/BO/actions/runs/44867209688) SUCCESS; `coop/biso_manifest_audit.py` supports `--checkout` and `--iso` local full verification when permitted.
- [x] Save pinned sources to `memory/STATE.json`, `docs/OWNER_BISO_SOURCE_AR.md`, `docs/ACTOR_BISO_ASSET_INDEX_AR.md`, and restore. Do not rely on sandbox persistence.
- [ ] If needed for ACTOR-001A, perform full per-file SHA comparison of owner ISO and restored biso locally; check format of selected model/animation candidate files without asserting unproven meaning.
- [ ] Continue ACTOR ownership/lifetime and isolated CO-OP top-level UX without modifying successful Baseline/Probe2 or game saves. No playable multiplayer yet.

## Full ISO manifest identity improvement
- [x] SHA256 every one of 574 ISO files and confirm canonical ordered 256-bit digest `b3e56e650e8a2b91af1380204599e9faf8722e98ef9d5d22790b69e798302d6f` equals pinned biso manifest.
- [x] Separate path/size canonical FNV64 digest `dad4e7e9643c8bfb` identical.
- [ ] Independently validate **remote biso blob bytes**, rather than just its manifest entries and tree byte lengths, only when needed through a local checkout/restore to avoid copying 1.7GB publicly.

## جلسة17 — ACTOR-001A observation coverage + remote proxy staging (2026-11-21)
- Returned to owner priority after biso validation. Read actual source (`coop/probe/probe.cpp`, `coop/probe/probe_core.hpp`, `coop/bridge/scene_inbox.hpp`, `coop/native/net_core.hpp`), Probe2 evidence and latest plan. Current `Registry.Id` is an epoch, pointer/vtable **temporary correlation**, with serial internal to slot but not a guaranteed object generation. Round-robin non-player sampling may produce silence even if NPC remains active.
- Derived aggregate time-window analysis from owner CL Probe2 capture **without copying raw file into BO**: 13 LOD correlations, 13 animation-related; 9 first observed >=10 sec after first captured sample, 2 with >=10 sec internal observation gaps, zero with last sample >=10 sec before final observed event, 1 epoch, no end. **No proof of NPC birth/death, disappearance, reuse, or resource ownership.** Sanitized evidence `memory/evidence/ACTOR001A-coverage-owner-Probe2-20261121.json`.
- Added `coop/analysis/actor_lifecycle_triage.py` (commit `31240e47f64ca890c3844b1ad9a66aaecba3bfc8`): bounded PRIVATE JSONL reader, per-(epoch,id) gap windows, multi-epoch observations and explicitly false lifecycle/factory claims, outputs anonymous counts only. Tests for partial capture, stale samples, scene epoch split, malformed private input, privacy; privacy workflow [44867782174](https://github.com/alwansan/BO/actions/runs/44867782174) **SUCCESS**. No owner raw log in CI, no APK modifications.
- Added `coop/bridge/remote_proxy_roster.hpp` + `test_remote_proxy_roster.cpp` (commit `33f08e654ba624d4fb44cbdb562128f65fb44894`) and extended `test_transport_pipeline.cpp` (commit `15092111638cd68ba38715712ebe0b0399b8630f`). C++20 **value-only** authoritative toy host roster for player IDs2–4, owner-game-thread guard, scene epoch, monotonic tick, timeout, full-roster absence, increasing generation on return, strict bounded positions/IDs and rollback rejection. Zero guest pointers, no NPC spawn, no render, no combat and **no secure session authentication**. First isolated sanitizer bridge CI [44867858529](https://github.com/alwansan/BO/actions/runs/44867858529) **SUCCESS**; integrated toy host→proxy+inbox rerun [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) **pending at documentation time**. Do not mark final integration accepted until checked.
- Latest BO app still owner-accepted Probe2 for game import/export, top-level CO-OP/ROOM button below settings not yet implemented; current page is within OPTIONS. No Android build warranted for offline-only code. biso pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, original 574 files already matched ISO manifest; no reupload needed.

### Next gates — ACTOR-001A (session17)
- [x] Synthetic coverage privacy tool and actual sanitized Probe2 gap aggregate (9 late,2 long), run44867782174 PASS; no raw owner data in BO.
- [x] Value-only actor proxy roster scene/thread/generation/stale checks, host ASAN/UBSAN run44867858529 SUCCESS.
- [ ] Integrated toy network→proxy+SceneInbox CI44867924228 pending; inspect logs and fix on branch if failure.
- [ ] Prove actual NPC lifetime/ownership before applying any proxy into BLACK, design minimally invasive opt-in hook for death/reload proof.
- [ ] Implement main-menu CO-OP entry under Settings via separate isolated source-gated experiment; don't touch accepted Probe2.

## ACTOR-001A integrated synthetic pipeline CI closure
- [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) completed SUCCESS for source `15092111638cd68ba38715712ebe0b0399b8630f`, job136735683825. Verified job log explicitly prints PASS for bounded SceneInbox, toy UDP Host→ProxyRoster+SceneInbox→mock scene, and value-only remote roster scene/owner/generation/stale tests. C++20 strict `-Wall -Wextra -Werror -pedantic`, ASAN/UBSAN on Ubuntu. **No Android build, no BLACK runtime, no real-player actor, no authenticated transport, no host+phone session**. New tests do not justify declaring ACTOR-001 complete.

## Pinned source review for ACTOR001A death/ownership hooks
- Reviewed real upstream v1.5.0 `src/game/cheats.cpp`, `src/game/hooks.cpp`, `src/game/layout.h` by pinned GitHub commit, and BO's probe observer/Registry. Candidate `sub_001E74B0` handles soldier **damage**, not necessarily final death or destructor; `kCharDying=0x384` is an observed dying flag but cannot prove deallocation. `sub_0017B130` references animation owner/clip, not actor factory. LOD observer is a render-distance hook, absent samples are not actor death. `Registry.World` only increments epoch on changed **world address**, not necessarily new mission if address reused. Precise review `docs/ACTOR001A_SOURCE_HOOK_DECISIONS_AR.md` is **design/reading only**, no changed APK or hook instrumentation.

## جلسة18 — Peterson047 BLACK-XBEMultiplayer reference review (2026-11-21)
- User-supplied repo `https://github.com/Peterson047/BLACK-XBEMultiplayer`; read README, third-party notices, bundled tool scripts `extract_mp_map.py`, `prepare_pvp_audio.py`, `prepare_pvp_weapons.py`, all Windows bat scripts and inspected full91-entry tree. Pin `0ee3ae6411c7f0989c62cbe36b224d3f55043923`. **Windows binary-only TDM alpha** for up to8 users, not Android or story coop. External claimed source `Peterson047/BLACK-Coop:feat/windows-multiplayer@095488c` returns **404** via connected GitHub; don't claim we accessed original native C++ or copy it. No top-level LICENSE; body.bca animation derived from Kevin Iglesias explicitly restricted to that mod, no re-distribution.
- Useful design: remote-body animation concept, standalone net worker, LAN discovery port1510 separate from game port1508, direct IP, host/dedicated topology, strict version rejection, owner ISO-derived isolated map and weapon packs with speech.slb tables; some audio offsets unverified for BO. Keep owner phone HOST, campaign-specific NPC/mission/quest/save separately. Detailed architecture/risks `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`; evidence `memory/evidence/REFERENCE-BLACK-XBEMultiplayer-20261121.json`.
- Cross-check **actual owner biso manifest**: all 13 required `level_00` core/stage files exist, their sizes match Peterson's published map profile, plus 16 DB/WDD weapon pairs. Independent **original** Python `coop/analysis/tdm_reference_profile.py` tests this metadata with negative synthetic tests; [44868801462](https://github.com/alwansan/BO/actions/runs/44868801462) SUCCESS, no source scripts, ISO, XBE or TDM assets copied; code `49d987bc5b8afcff0b796dfda087a80e4422d9cf`. Existing privacy CI [44868801450](https://github.com/alwansan/BO/actions/runs/44868801450) SUCCESS.
- Priority remains ACTOR-001A actual NPC/remote-avatar lifetime and safe body ownership. Next independent LAN discovery codec/rate-limit/ports on native host test, separate top-level menu and game integration later. No playable co-op claimed.

### Reference adoption gates — Peterson047
- [x] Reviewed public repo tree, docs, scripts, notices; source ref to BLACK-Coop inaccessible (404); pinned commit `0ee3ae6411c7f0989c62cbe36b224d3f55043923`.
- [x] Resource profile metadata-only against owner biso:13 stage-file sizes and16 DB/WDD pairs; CI44868801462 SUCCESS.
- [ ] Add independent LAN discovery codec and port distinctness/rate-limit tests (do NOT expose unauthenticated WAN).
- [ ] NPC actor/render/animation ownership proof for story player2, then optionally remote-body visuals after licenses/format support.
- [ ] Top-level CO-OP room menu + Android package only after safe source integration; no asset/binary source copying.
