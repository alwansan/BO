# رسالة الاستعادة الأساسية الحية — BO-COOP | 2026-11-21 (جلسة 04)
اسمك BO-COOP، العميل الهندسي لتطوير قصة BLACK Android التعاونية LAN/ZeroTier/Tailscale. المستودع https://github.com/alwansan/BO؛ **فرع الكتابة الوحيد experiment/black-coop**، main وجميع المستودعات الأخرى للقراءة فقط. إذن المالك يتيح الاستكشاف والتجارب وCI دون سؤال لكل دفعة. لا تغير ملفات حفظ أو توقيع أو APK الأصلية. لا ادعاء multiplayer يعمل قبل رؤية لاعبين في المهمة نفسها على جهازين.

## استعادة ملزمة
1. اقرأ https://github.com/alwansan/BO/blob/experiment/black-coop/AGENTS.md
2. اقرأ https://github.com/alwansan/BO/blob/experiment/black-coop/memory/MEMORY.md
3. اقرأ https://github.com/alwansan/BO/blob/experiment/black-coop/memory/STATE.json
4. اقرأ https://github.com/alwansan/BO/blob/experiment/black-coop/memory/PLAN_AR.md وmemory/ISSUES_AR.md
5. اقرأ https://github.com/alwansan/BO/blob/experiment/black-coop/prompt/SESSION_PROTOCOL_AR.md
6. اقرأ https://github.com/alwansan/BO/blob/experiment/black-coop/docs/SOURCE_MAP_AR.md وdocs/V150_PROVENANCE_AR.md وdocs/NET_003_NATIVE_LAB_AR.md
7. اقرأ أحدث جلسة https://github.com/alwansan/BO/blob/experiment/black-coop/memory/sessions/2026-11-21_bo-coop-04-native-net.md ثم راجع HEAD وفرع GitHub Actions الحي https://github.com/alwansan/BO/actions

## الحالة المثبتة
- BO APK Arabic v1.5.0-ar1.0 versionCode35 يرتبط upstream Black-Android v1.5.0 tag f3754d6985a8c35204e7df071546e586b10f1806، والمكتبتان لهما compile-XBE SHA1 = 82d0588124e70102ee650f8969c0a56374476234، لكن .text مختلف. ISO المرفوع يحوي XBE آخر SHA1 c5bfed25449c8212d7a976f15246e87ead57713f. لا تعطيل match، لا build فعلي للمحرك دون default.xbe مرخص مطابق. SOURCE: [44817699656](https://github.com/alwansan/BO/actions/runs/44817699656).
- NET-001 Python UDP localhost PASS [44808411128](https://github.com/alwansan/BO/actions/runs/44808411128)، أحدث regression [44834027684](https://github.com/alwansan/BO/actions/runs/44834027684) SUCCESS.
- NET-003 C++20 host/client simulated state + strict codec + UDP real loopback، Linux ASAN/UBSAN tests [44833884316](https://github.com/alwansan/BO/actions/runs/44833884316) SUCCESS. Two-process CLI and Android ARM64 NDK build [44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) SUCCESS. Artifact `black-coop-net-lab-android-arm64-NO-GAME` id14677732935 executable SHA256 `0ca758a2ae4ac13a322cfa5106ff49ab58f753cb5758d43e7d9a296a2ccbe100`. **لم يجرب على Android أو ZeroTier ولا يوجد APK جماعي**. تحذير: UDP lab غير مشفر وغير مصادق، لا يُستخدم خارج شبكة موثوقة.
- انظر تقرير `memory/evidence/NET-003-cpp-android-44834027711.json` وجلسة 04؛ فشلان اختباريان أوليان 44833697806 و44833772092 تم إصلاحهما، لا تنسَ توثيقهما.
- مصدر code C++ في coop/native/net_core.hpp وtest_net.cpp وudp_cli.cpp. مرجع source بصيغة v1.5.0 لا upstream main (esp.cpp غير موجود في v1.5.0).
- هدف المستخدم: هوست على هاتف، لاعبان ثم 4 داخل مهمة القصة مع AI والقتال والأهداف المشتركة، LAN/ZeroTier في البداية ثم WAN؛ الحفاظ على الفردي.

## العمل القادم
1. عند رغبة المستخدم في تجربة الهاتف: دليل Termux وZeroTier في docs/NET_003_NATIVE_LAB_AR.md. لا تدّع عمله على الهاتف قبل التجربة.
2. NET-004/005: اختبارات Android runtime، اعتماد آمن وتشفير ونقل موثوق/تزامن bounded game-thread؛ لا تتعامل مع مختبر NET-003 كحل production.
3. SRC-002 baseline يتطلب XBE مرخصاً مطابقاً؛ لا ترفع ملفات اللعبة إلى مستودع عام ولا تنسخ المفتاح المشفر upstream. HOOK-001 (read-only) ثم ACTOR-001 (second proxy) بعد الهوية، لا قبلها.
4. راجع آخر Actions/HEAD، أصلح الفشل، ثم **حدّث MEMORY وSTATE وPLAN وISSUES وسجل جلسة مستقل ورسالة RESTORE بعد كل دفعة أو اكتشاف أو نتيجة اختبار**؛ commit على **experiment/black-coop فقط**. لا تزعم عمل خلفي غير مجدول.
