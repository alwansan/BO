# استعادة BO-COOP — 2026-11-21، نهاية جلسة07

الهدف الكامل: قصة BLACK Android تعاونية فعلية، هاتف هوست، لاعبان ثم4، LAN/ZeroTier ثم WAN، غرف واكتشاف واتصال يدوي وlate-join، AI/قتال/أهداف، نوك60s/إنعاش5s، friendly fire، أسماء ومجسمات وحفظ فردي مستقل. المتطلبات وبراهينها docs/COOP_REQUIREMENTS_AR.md. لا co-op منفذة أو APK جماعية بعد.

## اقرأ أولاً
AGENTS.md ثم memory/STATE.json وMEMORY.md وPLAN_AR.md وISSUES_AR.md وprompt/SESSION_PROTOCOL_AR.md وأحدث memory/sessions/2026-11-21_bo-coop-07-phone-baseline.md. راجع HEAD/Actions الحيين. الكتابة فقط alwansan/BO experiment/black-coop؛ لا force push ولا تعديل أصول/حفظ/توقيع الأصل أو مستودعات المرجع. حدّث الذاكرة/الجلسة/RESTORE بعد كل دفعة أو نتيجة. لا عمل خلفي خارج الجلسة.

## العقبة السابقة حُسمت — لا تكرر طلب الأصل
المالك أكد أن APK العربية تدخل المهمة وتعمل، ونسخ بيانات التطبيق محلياً. السجل buildf3754d6 يحمل XBEc5bfed25449c8212d7a976f15246e87ead57713f مع another release؛ SHA256 كامل ISO المنسوخ من الهاتف مطابق manifest f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62. دليل sanitized metadata في memory/evidence/DEVICE-existing-owner-runtime.json؛ raw log/hdd خاص لا ينشر. log exit-menu FEAudio failed محفوظ كقيد، Exit0؛ لا نفترض عطل قتال.
الأصل libmain compile-image82d0588124e70102ee650f8969c0a56374476234 ما زال مختلفاً، ومصدر التعريب/XBE الأصلي غير متوفرين عند المالك. لم يتحول c5bfed إلى82d058 ولم يعطل hash check. لا تدع إعادة إنتاج المصدر العربي.

## الجديد المثبت: APK أساس منفصلة جاهزة لاختبار الهاتف
- run44845637359/job136664530136 على code5212673e5bc92bbe30a7e001cc8e26924a14f0af completed SUCCESS، Gradle8m17s. أول محاولة44845056293 فشلت قبل native compile بسبب quoting PowerShell، وصُححت دون تغيير اللعبة. لا تعيد run الناجحة بلا تغيير/فشل ذي صلة.
- reference Android tagv1.5.0 SHAf3754d6985a8c35204e7df071546e586b10f1806 وSDK99078b119b5d5e0d1d6afb1390e29437c0c984c0 مثبتان. esp.cpp/weapons.cpp غير موجودين في هذا tag. المترجم Windows هو Clang22 المرفق بـVS؛ global Clang20 سبب فشل سابقSTL1000. Android NDK28.2.13676358/CMake3.31.6/JDK17.
- baseline candidate تعيد ترجمة XBE الحقيقيc5bfed...، وcompiled kImageSha1 داخل APK يطابقه فعلياً. native/src/config لم تُعدل؛ packaging/import/label/updater/resource limits فقط. هذا مسار candidate جديد، لا binary-equivalent Arabic.
- packageorg.bocoop.black.baseline، activity/JNIcom.racas.black.BlackActivity، labelBLACK Baseline - NO CO-OP. الحفظ مستقل، الاستيراد إلى assets الخاصة فقط، لا MANAGE/READ/WRITE_EXTERNAL_STORAGE، updater محجوب. لا ISO/XBE/generated sources في APK/artifact.
- فحوص real APK: ARM64، package/activity، debug signature، source unchanged، no original assets، وembedded native identity PASS؛ تنزيل ZIP/APK وإعادة SHA/ELF محلياً PASS.
- APK21727055bytes، SHA2564ead325d67772a1c877b9f073aebc0e9eaaab45e9a0028d2380645c4bc2f69f7. artifact14682519899، metadataArtifact14683233366. الدليل memory/evidence/BASELINE-candidate-build-44845637359.json. رابط التحميل https://github.com/alwansan/BO/actions/runs/44845637359/artifacts/14682519899 . نسخة محلية /home/a/Desktop/black/outputs/baseline-44845637359/black-baseline-candidate-NO-COOP.apk إن بقي الجهاز نفسه.
- suitePython4/4، actual source packaging preview PASS، مراجعة مستقلة أصلحت مخاطر shared import ورفع artifacts قبل verify. source audit44845056215 وUDP44845056127 SUCCESS.

## المطلوب الآن من المالك
اختبار **APK الجديدة** فقط وفق docs/CANDIDATE_BASELINE_DEVICE_AR.md: تثبيت مستقل، Choose ISO من Downloads، دخول مهمة والمشي/الرمي/ضرر/إعادة محاولة/خروج، ثم أحدث log من Android/data/org.bocoop.black.baseline/files/logs/. لا تطلب منه إعادة تجربة APK الأصلية أو XBE المفقود. نجاح CI لا يثبت runtime الجديدة.

## بعد وصول نتيجة baseline
قبول HOOK-001 يحتاج runtime ودليل قراءة bounds/lifecycle/thread ownership. مرشحات الجسم/animation وLOD في docs/ACTOR_FEASIBILITY_AR.md معلومات ساكنة لا factory مثبتة. لا full-body player أو spawn آمن مثبت. لا تعتمد network thread لكتابة ذاكرة، ولا تصدر pointers، ولا تدع لاعباً ثانياً قبل ACTOR/COMBAT على جهازين.
NET003 مستقل فقط: Linux UDP وAndroid crosscompile44834027711 PASS؛ لا هاتفين أوZeroTier أومصادقة/تشفير. docs/NET_003_NATIVE_LAB_AR.md. لا تستبدل هدف co-op بمختبر أو baseline أو قائمة غرف.
