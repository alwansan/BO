# ذاكرة BO-COOP
- الهدف: طور قصة تعاوني مستقل، Host هاتف، إنترنت اختياري وLAN/ZeroTier، واجهة غرف مشابهة Eden.
- الأصل BO main b1ed1745e3b6b81192581e2824eb0f8daaa977c9. APK مفكك مرتين؛ package com.racas.black.arabic، ARM64، SDL3/Vulkan، INTERNET موجود.
- المصدر المرجعي المثبت 8e9c6fc7d570503ea68d039cff16c09339ee6d30 من RanggaCasper/Black-Android؛ README يثبت إعادة ترجمة x86 XBE إلى C++ ثم ARM64، لا محاكي. NOTICE MIT موجود في APK. XBE/ISO ليست MIT.
- المصدر يبنى Windows VS2022 + JDK17 + Android platform34 NDK28.2.13676358 CMake3.31.6؛ SDK recompiler 99078b119b5d5e0d1d6afb1390e29437c0c984c0. upstream CI يحتاج XBE_KEY؛ لا نطلب مفتاحه أو نحاول فك تشفيره؛ نستخدم ملف اللعبة المرخص عند المالك.
- upstream native/game_files.cpp يخزن kImageSha1 ويدعم تحويل Europe SHA1 796ad8369354e48aa15644d4707dd70bd3bc3b4c إلى US. config/game.toml title 45410083. لا تطابق offsets عربي قبل الفحص.
- NET-001 run 44808411128 SUCCESS لعميلين على loopback؛ ليس اختبار هاتف/محرك. لا توجد APK جماعية.
- مصادر مرجعية لنظام الذاكرة: raf clients/GT/RESTORE_AR.md وCP Native-Compose AGENTS.md وprompt/RESTORE_SESSION_AR.md. CB 404؛ CP هو المتاح.

## تحديث CI
- [44813529325](https://github.com/alwansan/BO/actions/runs/44813529325) NET-001 regression SUCCESS على 6a82564229247ed21ef526cfbfb221a499802805.
- [44813529439](https://github.com/alwansan/BO/actions/runs/44813529439) SRC-001 synthetic job SUCCESS؛ identity audit completed / success عند الفحص. لا نتيجة XBE مثبتة قبل قراءة تقرير identity.json أو log. لا APK أو اختبار هاتف.

## نتيجة SRC-001 الفعلية — 44813529439
CI SUCCESS وartifact14666839620، قراءة job136555949209 مباشرة. اجتاز SHA256 للأصل ISO 1793179648 B وlibmain 72073216 B، XDVDFS partition 0، XBE 2813952 B، AArch64. Disc SHA1 c5bfed25449c8212d7a976f15246e87ead57713f مقابل kImageSha1 من ELF 82d0588124e70102ee650f8969c0a56374476234: **غير متطابقين**؛ disc ليس Europe المعروف. لا يثبت السبب (تعديل عربي/إصدار/غيره) ولا source commit أو أن APK الحالي لا يعمل. لا تعطيل gate، لا استخدام offsets غير مثبتة. SRC-001 فحص نجح؛ **قبول هوية المصدر BLOCKED**. التالي SRC-002a: تحديد US hash/commit من مصدر upstream وmetadata الإصدار العربي، ومقارنة XBE header/sections مع الصورة المطابقة المرخصة إن توفرت. إن تعذر، اطلب من المالك فقط default.xbe الأصلي المطابق أو مصدر تعديلات Arabic أو log تشغيل كامل؛ لا تفترض الحاجة للثلاثة. بناء baseline ثم HOOK-001. تقرير: memory/evidence/SRC-001-identity-44813529439.json، https://github.com/alwansan/BO/actions/runs/44813529439. لا APK جديدة ولا gameplay أو اختبار هاتف.

## جلسة 03 — SRC-002a: تثبيت v1.5.0 (2026-11-21)
- APKTool 1.5.0-ar1.0/code35، META-INF revision f3754d6985a8c35204e7df071546e586b10f1806 مطابق tag v1.5.0 upstream (ليس main 8e9c6fc). المصدر المرجعي للعمل على baseline أصبح tag v1.5.0؛ المصدر العربي الكامل غير مثبت. esp.cpp غير موجود في tag؛ layout/cheats/hooks موجودة. راجع docs/V150_PROVENANCE_AR.md.
- CI [44817699656](https://github.com/alwansan/BO/actions/runs/44817699656) completed/success؛ تحقق SHA256 الرسمي، ومقارنة ELF حقيقية، job136570202480، artifact14669549225. الرسمي libmain.so 17957208 B، العربي 72073216 B؛ كلاهما AArch64 وkImageSha1=82d0588124e70102ee650f8969c0a56374476234. .dynstr/.dynsym/.rodata SHA identical؛ .text مختلف، file SHA مختلف. جميع 6 hook-symbol candidates موجودة في الاثنين (بحث substring فقط؛ لا إثبات function semantics). disc الموجود c5bfed254... لا يطابق compile image، لا نُضعف الفحص. النتيجة تثبت release provenance وcompile image match الرسمية/العربية، لا source العربي المطابق ولا compatibility للـISO. دليل memory/evidence/SRC-002a-release-comparison-44817699656.json. NET regression 44817699458 SUCCESS.
- التالي: تحقق من نسخة قانونية لـdefault.xbe SHA1=82d0588124e70102ee650f8969c0a56374476234؛ المالك قد يملكها من نسخته الأصلية. لا تستعمل encrypted upstream XBE أو تدّع تحويل XBE العربي. إن لم تتوفر، واصل شبكات C++ مستقلة/اختبارات Android دون game assets. لا APK ولا gameplay. يمكن فحص hash الدوال المرشحة في ELF كدليل إضافي فقط؛ لا كتابة ذاكرة. لا نحتاج نشر أصول جديدة في public repo. إذا رفع المالك نسخة XBE مطابقة فليكن في مسار خاص مع صلاحية مشروعة، لا تُرفع للفرع العام.

## جلسة 04 — مختبر الشبكات الأصلي (2026-11-21)
- نجاح C++20 أصلي NET-003: مصدر `coop/native/net_core.hpp` و`test_net.cpp` و`udp_cli.cpp`، المضيف يمنح 3 معرفات ضيوف (2–4)، tick/snapshot، UDP IPv4 loopback حقيقي، packet bounds وdedup وsequence/timeout. هذه محاكاة منطقية مستقلة فقط، لا BLACK ولا 4 هواتف فعلية.
- التشغيل [44833884316](https://github.com/alwansan/BO/actions/runs/44833884316) SUCCESS؛ [44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) SUCCESS (Linux CLI separate processes + Android NDK cross compile). Artifact 14677732935 SHA256 `0ca758a2ae4ac13a322cfa5106ff49ab58f753cb5758d43e7d9a296a2ccbe100`، binary غير مجرب على هاتف.
- الفشلان السابقان 44833697806 و44833772092 عيوب في توقعات الاختبار وصُححا؛ لا تُحذف من التاريخ.
- الأخطار: UDP لا تشفير/مصادقة، لا reliability/jitter/NAT، ZeroTier لم يُختبر. راجع `docs/NET_003_NATIVE_LAB_AR.md` و`memory/evidence/NET-003-cpp-android-44834027711.json`.
