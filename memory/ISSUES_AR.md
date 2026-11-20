# المشكلات المفتوحة
- SRC-001: إصدار APK العربي والـXBE وإصدار libmain غير مثبت بالمصدر؛ استخرج hashes أولاً ولا تستعمل offsets.
- SRC-002: Windows recompiler يتطلب default.xbe صالحاً وSDK. لا نملك source الخاص بتعديلات العربية، قد تحتاج لاحقاً من المالك.
- ACTOR: singleton kWorldPlayer؛ لا دليل على قدرة إنشاء جندي متحكم به أو لاعب ثانٍ. تحديد factory/lifecycle/collision/AI/camera.
- NET: UDP probe غير مشفر وغير مصادق، لا يجرب فقد حزم أو NAT أو هاتفين؛ ليس production.
- SAVES: فصل حفظ co-op عن single player، وعدم استبدال توقيع/حزمة المالك.
- LICENSE: لا نشر game.iso/default.xbe/generated recompiled game assets؛ حقوق مستقلة عن MIT.

## نتيجة SRC-001 الفعلية — 44813529439
CI SUCCESS وartifact14666839620، قراءة job136555949209 مباشرة. اجتاز SHA256 للأصل ISO 1793179648 B وlibmain 72073216 B، XDVDFS partition 0، XBE 2813952 B، AArch64. Disc SHA1 c5bfed25449c8212d7a976f15246e87ead57713f مقابل kImageSha1 من ELF 82d0588124e70102ee650f8969c0a56374476234: **غير متطابقين**؛ disc ليس Europe المعروف. لا يثبت السبب (تعديل عربي/إصدار/غيره) ولا source commit أو أن APK الحالي لا يعمل. لا تعطيل gate، لا استخدام offsets غير مثبتة. SRC-001 فحص نجح؛ **قبول هوية المصدر BLOCKED**. التالي SRC-002a: تحديد US hash/commit من مصدر upstream وmetadata الإصدار العربي، ومقارنة XBE header/sections مع الصورة المطابقة المرخصة إن توفرت. إن تعذر، اطلب من المالك فقط default.xbe الأصلي المطابق أو مصدر تعديلات Arabic أو log تشغيل كامل؛ لا تفترض الحاجة للثلاثة. بناء baseline ثم HOOK-001. تقرير: memory/evidence/SRC-001-identity-44813529439.json، https://github.com/alwansan/BO/actions/runs/44813529439. لا APK جديدة ولا gameplay أو اختبار هاتف.
