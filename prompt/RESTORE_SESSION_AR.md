## جلسة36 — أحدث توجيه واجب
المالك يريد شخصيةاللعبةالأصليةعلىكلهاتف،ولايريدbodyبوتأوحليف. اقرَأmemory/sessions/2026-11-21_bo-coop-36-original-player-steering.md والخطةdefault-player-link. تجربةالهاتفالتاليةتتحققمنMODELالمحمّلفيlocalplayerالأصلي،لاتطلبNPCfactorycaptureبعدهذاالتوجيه. لاsharedStartأوبعيدمرئيبعد؛المحليوالـreviewللقارئالجديدPASS،APKالتاليقيدالإطلاق. لاادعاءأنمجردlocalplayerعلىكلجهازينسخالجسموالعالمعبرالشبكة.


## مرجع وتقدم أصول جديد بعد رفع جلسة35
اكتشفنا portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628،LICENSE MIT قرئ. راجع docs/REFERENCE_BLACK_PC_COOP_AR.md؛PC split-screen لديه player-context وpuppets لكنAIلايستهدفP2 بعد،وليسAndroid/network coop. لا نقل أعمىلـslotcopy؛+250عضويةالمجموعةيحتاجحماية. أداةBOالأصليةmodel_inventory.pyقرأَت271ملفاًمحلياًو4286MODEL،76مرشحbodyبـ10keysفريدة؛protagonistغيرمثبت.3اختباراتحدودوتصنيفPASS،لاgeometry/أصولمصدرة. نتائجآمنةفيOWNER-MODEL-CANDIDATES-20261121.json. تبقىخطةactualactor/control/AI/story،ولاStartوهمي.

FactoryAPK run44944691802 على96413485bdde48981c7000ff9f9ff6019740fa7b:coreنجحوالـgeneratedABIguardنجحوبناءARM64قيدالتجميعوقتكتابةهذاالتحديث. لا تطلبهاتفاًحتىنجاحartifactوفحصه.
# تحديث الاستعادة — جلسة35 أولاً
اقرأ memory/sessions/2026-11-21_bo-coop-35-owner-lan-pass-factory-contract.md وSTATE الحالية والخطةdocs/superpowers/plans/2026-11-21-actor-factory-observation.md قبل بقية النص أدناه. المالك أرسل تجربة ناجحة جديدة بالـAPKالقديمة: صور HOSTING/JOINED و2/4 على الطرفين وسجل guest JOIN_AUTHENTICATED؛ LANالقديم مقبول. لا تطلب تكراره لمجرد الانتظار. النسخة التلقائية الجديدة44942892701 ناجحة ومفحوصة،artifact14722613067 وSHA256b149bc2cbc96b8ea9ff072841878d09ca58334338ea5cee49a6c3b6ab1e1f6d1،لكن لا هاتف بعد. المالك طلب متابعة التطوير نحو shared Start وأجسام حقيقية، لا الوقوف عند اللوبي.

العمل الحالي: probe5 رصد قراءة فقط لمسارfactory1DF440 الأصلي، حتى32recordبدونعناوين/إحداثيات،لاspawn. الحزمةfactoryroom منفصلة وبها نفس الاتصال المبسط؛المحليوالمراجعةPASS. تابعworkflowblack-coop-room-factory-apk.yml حتىالنهاية،أصلحالفشل،افحصartifactوسجلالذاكرة،ثماطلباختبارcaptureمحددقبلتحميلالمهمةوظهورNPCثمcheckpointreloadخلال3دقائق. هذه معلوماتجديدةعنcontroller/resource/descriptorوليسإعادةProbe4. لاتفعّلStartجماعييزعموجودأجسامقبلتنفيذها. بقيةالهدفالكاملمحفوظ.

# استعادة BO-COOP — جلسة34

اقرأ AGENTS.md ثم memory/STATE.json وجلسة memory/sessions/2026-11-21_bo-coop-34-simple-auto-network.md والخطة docs/superpowers/plans/2026-11-21-simple-auto-network.md. راجع MEMORY وPLAN وISSUES وdocs/SOURCE_MAP_AR.md وcoop/README_AR.md، وافحص HEAD وActions الحيين قبل التعديل. المستودع alwansan/BO وفرع الكتابة experiment/black-coop. النسخ المحلية القديمة BO-active وBO-coop ليست أحدث مصدر؛ عمل هذه الجلسة في BO-network-fix. رسالة الاستعادة السابقة محفوظة كتاريخ في memory/sessions/2026-11-21_restore-before-34.md.

## طلب المالك الحالي
استمر في إصلاح تجربة هاتفين فشلت فيها الاستضافة والانضمام. ألغِ PIN وكلمة المرور ونسخ بصمة TLS وإدخال المنفذ من المستخدم. الاستضافة بالاسم واسم الغرفة، والانضمام باكتشاف تلقائي للأجهزة والغرف، بما فيها الأجهزة المفتوحة التي لم تستضف. IP يدوي احتياطي. إذن البحث والتجارب وActions مستمر حتى اختبار الهاتفين الذي يحتاج المالك.

## ما ثبت وما تغير
شاهدنا فعلياً ثلاث صور roomnet الأخيرة في CL، وقرأنا سجلات الطرفين. حقول PIN فارغة، والأحداث HOST_INPUT_ERROR وJOIN_INPUT_ERROR سبقت أي اتصال. أزيل PIN من بروتوكول JOIN، وأصبح Wire VERSION2 وPROFILE2؛ يجب تحديث الهاتفين معاً. Host يستخدم TCP24872 تلقائياً. RoomPresence على UDP1511 يبدأ مع BlackActivity ويعرض متاح/مضيف/منضم، والبحث يتجدد تلقائياً. TLS داخلي وتلقائي دون تحقق يدوي من الهوية. الاكتشاف محدود ببث الشبكة ومسح /24 حتى512عنواناً، وليس واجهة سرد حساب ZeroTier؛ يبقى IP احتياطياً عند حجب المرور أو اختلاف الشبكات الفرعية.

الكود الأساسي4c9f8bb اجتاز JVM44942174361 ومصدرAndroid44942174338 ومحاكيAndroid29 رقم44942174364. قرئ سجل instrumentation: زرHost الفعلي بالأسماء الافتراضية فتح المنفذ التلقائي، joinAutomatic نجح وBack لم يغلقActivity. مراجعة مستقلة وجدت JOINمعلقاً بعدLeave؛ صُحح وفُحص عدم استهلاكID لطلب ملغى. لم يبق مانع في المراجعة.

بناءAPK الأول44942174355 والثاني44942585121 توقفا قبل تجميع اللعبة بسبب قراءة/كتابة اختبارات Python بترميزWindows cp1252. أصلح UTF-8 في الجانبين، وأضيف PYTHONUTF8 إلىjob؛ نجحت16اختبارات بمحاكاةcp1252. آخرcommit تقني552161df57031f6ee98d218a16b0532dee0928d4، وبناءAPK الحالي44942892701. تحقق من النتيجة الحية ولا تعلن نجاحه قبلها. تقرير الحزمة يصف اتصالاً تلقائياً opportunistic TLS، وليس هوية موثقة خارج القناة.

## المطلوب مباشرة
تابع بناءAPK، أصلح أي فشل، ثم افحص artifact وmetadata وSHA256 والتوقيع والحزمةorg.bocoop.black.roomnet وصورةc5 وغيابISO/XBE. حدث الذاكرة ورسالة الاستعادة بالنتيجة. سلّم APK للمالك مع docs/ROOM_NETWORK_DEVICE_TEST_AR.md: فتح اللعبة على الاثنين، ظهور الجهاز الخامل، Host ثمJoin و2/4 على الطرفين، Back وLeave، ثمVPN. احصل على سجلي الغرفة وصور الحالة من الجهازين عند الفشل. لا توقف العمل على مجرد خطة أو CI قيد التشغيل.

## الهدف الكامل والمصادر
هذا لوبي وشبكة فقط؛ لا جسم لاعب ثان أو مزامنة حركة/AI/combat/story بعد. الهدف الكامل ما زال4بشر وقصة وhost authority وlate join وأسماء وfriendly fire و60ثانيةdown و5ثوانٍrevive وانتقال المراحل. لا تعلن اكتمالco-op ولا تستبدل الأجسام الحقيقية ببوتات. راجع أدلة actor/brain/factory/vtable القديمة؛ لا guest writes منnetwork thread.

مصادر المالك: https://github.com/alwansan/CL/tree/main/black%20log والصور فيimg وسجلات الهاتف الثاني فيمجلد«من الهاتف الثاني». مرجع https://github.com/Peterson047/BLACK-XBEMultiplayer هوWindows binary-only، ليس مصدرco-op مرخصاً للنسخ. biso pin9ceb6a65 يحويISO المفكوك، فلا تطلب رفعه مجدداً. XBE c5bfed25449c8212d7a976f15246e87ead57713f مثبت، baseline f3754d6 وSDK99078b1. لا حاجة لتعديلISO لهذه الدفعة. لا تنشر أصول اللعبة أو الحفظ أو المفاتيح، لا force-push، ولا تمسح العمل المحلي القديم. المالك طلب تحديث الذاكرة والاستعادة دورياً؛ حافظ عليه.
