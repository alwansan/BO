# رسالة الاستعادة — BO-COOP
اسمك BO-COOP. أكمل مشروع BLACK Android story co-op في https://github.com/alwansan/BO، فرع الكتابة الوحيد **experiment/black-coop**. لا تعدل main أو أي مستودع مرجعي. المالك أعطاك حرية التجارب وCI والقرارات الهندسية؛ لا تطلب إذناً لكل دفعة. الهدف لاعبان حقيقيان داخل مهمة القصة، ثم 4+، المضيف هاتف لاعب، LAN وZeroTier/Tailscale وعبر الإنترنت دون سيرفر ألعاب مخصص، الفردي محفوظ.
اقرأ بالترتيب:
https://github.com/alwansan/BO/blob/experiment/black-coop/AGENTS.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/MEMORY.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/STATE.json
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/PLAN_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/ISSUES_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/prompt/SESSION_PROTOCOL_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/docs/SOURCE_MAP_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/sessions/2026-11-20_bo-coop-02.md
ثم راجع HEAD وActions الحيين https://github.com/alwansan/BO/actions . المصدر المرجعي pinned 8e9c6fc7d570503ea68d039cff16c09339ee6d30: https://github.com/RanggaCasper/Black-Android/tree/8e9c6fc7d570503ea68d039cff16c09339ee6d30
## آخر نقطة
NET-001 run 44808411128 نجح loopback فقط. SRC-001 أضيف له مختبر تحقق split SHA256 وXDVDFS وXBE وELF identity؛ run 44813529439: synthetic SUCCESS؛ identity completed عند الفحص. NET regression 44813529325 SUCCESS. راجع نتيجة black-coop-source-audit run وأصلح أي فشل دون تجاوز hash. لا لاعبان داخل اللعبة ولا APK جديدة. لا تنشر game assets. إذا نجح audit، حدد صلاحية matching للـXBE، ثم baseline بناء المصدر Windows قبل Hook read-only. راجع source map؛ لا تستخدم offsets من إصدار غير مثبت. إذا تعذر مصدر/توقيع العربية اطلب الملف الضروري فقط.
حدّث هذه الرسالة وSTATE/MEMORY/PLAN/ISSUES وسجل الجلسة في GitHub بعد كل دفعة أو فشل تلقائياً. لا تعد العمل الخلفي دون automation. لا تدّع نجاح اختبار لم يحصل.

## تحديث CI
- [44813529325](https://github.com/alwansan/BO/actions/runs/44813529325) NET-001 regression SUCCESS على 6a82564229247ed21ef526cfbfb221a499802805.
- [44813529439](https://github.com/alwansan/BO/actions/runs/44813529439) SRC-001 synthetic job SUCCESS؛ identity audit completed / success عند الفحص. لا نتيجة XBE مثبتة قبل قراءة تقرير identity.json أو log. لا APK أو اختبار هاتف.

## نتيجة SRC-001 الفعلية — 44813529439
CI SUCCESS وartifact14666839620، قراءة job136555949209 مباشرة. اجتاز SHA256 للأصل ISO 1793179648 B وlibmain 72073216 B، XDVDFS partition 0، XBE 2813952 B، AArch64. Disc SHA1 c5bfed25449c8212d7a976f15246e87ead57713f مقابل kImageSha1 من ELF 82d0588124e70102ee650f8969c0a56374476234: **غير متطابقين**؛ disc ليس Europe المعروف. لا يثبت السبب (تعديل عربي/إصدار/غيره) ولا source commit أو أن APK الحالي لا يعمل. لا تعطيل gate، لا استخدام offsets غير مثبتة. SRC-001 فحص نجح؛ **قبول هوية المصدر BLOCKED**. التالي SRC-002a: تحديد US hash/commit من مصدر upstream وmetadata الإصدار العربي، ومقارنة XBE header/sections مع الصورة المطابقة المرخصة إن توفرت. إن تعذر، اطلب من المالك فقط default.xbe الأصلي المطابق أو مصدر تعديلات Arabic أو log تشغيل كامل؛ لا تفترض الحاجة للثلاثة. بناء baseline ثم HOOK-001. تقرير: memory/evidence/SRC-001-identity-44813529439.json، https://github.com/alwansan/BO/actions/runs/44813529439. لا APK جديدة ولا gameplay أو اختبار هاتف.

## جلسة 03 — SRC-002a: تثبيت v1.5.0 (2026-11-21)
- APKTool 1.5.0-ar1.0/code35، META-INF revision f3754d6985a8c35204e7df071546e586b10f1806 مطابق tag v1.5.0 upstream (ليس main 8e9c6fc). المصدر المرجعي للعمل على baseline أصبح tag v1.5.0؛ المصدر العربي الكامل غير مثبت. esp.cpp غير موجود في tag؛ layout/cheats/hooks موجودة. راجع docs/V150_PROVENANCE_AR.md.
- CI [44817699656](https://github.com/alwansan/BO/actions/runs/44817699656) completed/success؛ تحقق SHA256 الرسمي، ومقارنة ELF حقيقية، job136570202480، artifact14669549225. الرسمي libmain.so 17957208 B، العربي 72073216 B؛ كلاهما AArch64 وkImageSha1=82d0588124e70102ee650f8969c0a56374476234. .dynstr/.dynsym/.rodata SHA identical؛ .text مختلف، file SHA مختلف. جميع 6 hook-symbol candidates موجودة في الاثنين (بحث substring فقط؛ لا إثبات function semantics). disc الموجود c5bfed254... لا يطابق compile image، لا نُضعف الفحص. النتيجة تثبت release provenance وcompile image match الرسمية/العربية، لا source العربي المطابق ولا compatibility للـISO. دليل memory/evidence/SRC-002a-release-comparison-44817699656.json. NET regression 44817699458 SUCCESS.
- التالي: تحقق من نسخة قانونية لـdefault.xbe SHA1=82d0588124e70102ee650f8969c0a56374476234؛ المالك قد يملكها من نسخته الأصلية. لا تستعمل encrypted upstream XBE أو تدّع تحويل XBE العربي. إن لم تتوفر، واصل شبكات C++ مستقلة/اختبارات Android دون game assets. لا APK ولا gameplay. يمكن فحص hash الدوال المرشحة في ELF كدليل إضافي فقط؛ لا كتابة ذاكرة. لا نحتاج نشر أصول جديدة في public repo. إذا رفع المالك نسخة XBE مطابقة فليكن في مسار خاص مع صلاحية مشروعة، لا تُرفع للفرع العام.

أحدث سجل جلسة: https://github.com/alwansan/BO/blob/experiment/black-coop/memory/sessions/2026-11-21_bo-coop-03.md
