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
