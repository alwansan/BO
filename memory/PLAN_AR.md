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
