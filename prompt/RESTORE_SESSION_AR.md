## تسليم جلسة37 — 2026-11-22
المالك سينتقل مؤقتاً لجلسة أخرى. اقرأ memory/sessions/2026-11-22_bo-coop-37-cloud-handoff.md وdocs/CLOUD_HANDOFF_AR.md. جميع أدوات المصدر والخطط والمراجع مرفوعة؛لا تعتمد علىالجهازالمحلي. آخرAPK44946597973/artifact14724535514مفحوصة،ولا سجلاتهاتفجديدةبعدطلبها. الشخصيةالأصليةفقط،لاNPCبديل،ولاsharedStartأوgameplayبعد. افحصCLالحيةأولاًلأيcaptureجديد.

# استعادة BO-COOP — جلسة36، الشخصية الأصلية فقط

اقرأ AGENTS.md ثم memory/STATE.json وmemory/sessions/2026-11-21_bo-coop-36-original-player-steering.md والخطة docs/superpowers/plans/2026-11-21-default-player-link.md. راجع MEMORY وPLAN وISSUES وdocs/SOURCE_MAP_AR.md وdocs/REFERENCE_BLACK_PC_COOP_AR.md. افحص HEAD وActions الحيين قبل أي تعديل. الكتابة في alwansan/BO فرعexperiment/black-coop، دونforce-push أوتغييرmain. عمل هذه الجلسة فيBO-network-fix؛BO-active وBO-coop نسخ محلية قديمة لا تنقلها فوقremote.

## أحدث طلب للمالك
كل هاتف يجب أن يستخدم شخصية اللعبة الرئيسية الأصلية، بحركاتها ومزاياها؛ لا يريد موديلات بوتات أوحلفاء بديلاً للاعبين. يريد Host يبدأ المهمة وبقية البشر معه، ويريد متابعة العمل حتى تجربة هاتف ضرورية. هذا يتقدم على اقتراحNPC-factory في جلسة35. لا تطلب منه تجربةfactoryroom الآن؛أدواتها محفوظة للبحث فقط. لا تضفStart يزعم لعباً مشتركاً لمجرد إغلاق نافذة الغرفة.

## تقدم مثبت
المالك أثبت LAN على هاتفين بالـAPK القديمة بعد إدخالPIN: شاهدنا صورتَيCL22:47، المضيفHOSTING والضيفJOINED وكلاهما2/4،وسجلguest JOIN_AUTHENTICATED. هذا قبولاللوبيالقديم، لاVPN أوgameplay.

النسخة المبسطة تلغيPIN والبصمة اليدوية وإدخال المنفذ من المستخدم. أسماءفقط،TCP24872 داخلي،اكتشافأجهزةخاملةوغرف علىUDP1511،manualIP احتياطي. TLS داخلي وتلقائي،Wire2/Profile2 يتطلب تحديث الطرفين. الاكتشافbroadcast و/24 محدود، لا سرد لكلحسابVPN. AndroidKeyStore EC P256 محفوظ. كود4c9f8bb اجتازJVM8suitesوAndroid29 Hostbutton/automaticJOIN/Back؛source7ثم19preparerنجحت. نسخةالاتصال552161d بُنيتوفُحصت:run44942892701/artifact14722613067،APKsha256b149bc2cbc96b8ea9ff072841878d09ca58334338ea5cee49a6c3b6ab1e1f6d1،21854231بايت. لا اختبار هاتف لهذه النسخة الجديدة بعد. فشلWindowsالقديم كانترميزcp1252فيالاختبارات وأصلحUTF-8قراءةوكتابةوPYTHONUTF8.

## العمل الحالي المطلوب متابعته
كود8bc7f92e17c1d35c6c125013bc4057afe320527f يضيفحزمةorg.bocoop.black.defaultplayer باسمBLACK ORIGINAL PLAYER TEST. تعتمداتصالroomnetنفسه،وتراقبMODELالمحمّلفيشخصيةاللعبالأصليةفقط منhookتحديثهاالموجود،دونNPCfactoryhook أوactorcreationأوguestwrites. حتى8سجلاتdefault_player_model/1Hz:counts وbooleans لتصنيفالمفاصلوdraw-list؛لا أسماءأوعناوينأوهندسة. القارئيستعملrelocatedpointersويعيدفحصهويةplayer/model/world. body_candidate قرينةskeleton لا إثباتfullbodyمرئي أوهوية بطل.

محلياًnativeASAN/UBSANو19preparerوالمراجعةالمستقلةPASS. بناءAPK44946597973 على8bc7f92e17c1d35c6c125013bc4057afe320527f اكتملSUCCESS؛قرئتlogs وفيهاBUILD SUCCESSFUL وLOD/resetguards والصورةمطابقة. artifact14724535514 نُزلوفُحصCRCوالـAPK SHA25694400c2d650d772293ab5f3a1d0c72e91d1974dea64a3004d18404e90ccb02d0 والحجم21860751B. الحزمةوالتوقيعوالـJNIوAArch64وصورةc5وغيابأصولISO/XBE تحققتبالـCIوالـartifact. التقريرmemory/evidence/DEFAULT-PLAYER-APK-44946597973.json. رابطالتحميلالدائم https://github.com/alwansan/BO/actions/runs/44946597973/artifacts/14724535514 . لا تعِدبناءنفسالحزمةأوتكرارالفحوص دونتغييرأوفشل؛التالياختبارهاتفجديدوليسCIقيدالتشغيل. لاphoneproofلهذهالحزمةبعد.

نطاقdefault_player_modelهوprimarydrawmodelفقط:actor320 ثمwrapper[0]. body_candidate=falseلايثبتغيابجسمآخرفيمواردالنسخة،وcompleteيصفmetadataللنموذجالأولالمقروءلاكلالشخصية. لاافتراضأنwrapperمخصص200بايتلمجردوجودPCcopy.

التجربةالمطلوبةالآنعلىالهاتفالمحددةفيdocs/DEFAULT_PLAYER_DEVICE_TEST_AR.md:هاتفواحد،Startcaptureقبلتحميلمهمة،اللعب20–30ثانيةبالشخصيةالأصلية،ثمcoop_probe.txt وblack_log.txtالجديدان منDownloads/Games/cl/black log/logs/BLACK-Probe-[date]. لا تطلبإعادةbaselineأوProbe4القديم. يمكنفحصالاتصالالمبسطعلىالطرفينبنفسAPKكتجربةمنفصلة. النتيجةالحيةتحسمماهوالنموذجالافتراضيقبلنسخملكيةactor/context.

## المراجع والمصادر
- CL/black log: https://github.com/alwansan/CL/tree/main/black%20log،صورimg وسجلات«من الهاتف الثاني». فهرسآمنmemory/evidence/SESSION34-owner-two-phone-source-index.json.
- مرجعجديدMITقرئترخيصه:portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628. اقرأdocs/REFERENCE_BLACK_PC_COOP_AR.md. PC player-context معسلاح/كاميرا/HUD مستقل،والأعداءلا تستهدفP2فيه بعد. لاAndroid/networkجاهزاً،ولا نسخABIأوslot+250بلااختبار. لمينسخBOكودruntimeمنه؛أرفقالنسبةوMITإذااستُعمللاحقاً.
- تحليل271ملفاًمحلياًبقارئBOأصلي أعطى4286MODEL و76مرشحbodyبـ10keys. هذه معلوماتعنالمحتوى،لا إذنلاستبدالشخصيةولا إثباتprotagonist. الملخصOWNER-MODEL-CANDIDATES-20261121.json؛لاgeometryأصولمصدرة.
- Peterson047/BLACK-XBEMultiplayer مرجعWindowsTDMbinary-only،ليسco-opsourceمرخصاً. لا تنسخEXE/DLL/body.bca.
- biso pin9ceb6a65 يحويISOمفكوكاً؛لاطلبإعادةرفع. XBEc5bfed25449c8212d7a976f15246e87ead57713f،baselinef3754d6 وSDK99078b1. لا حاجةلتعديلISO لهذهالدفعات. لا تنشرأصولاللعبةأوالحفظأومفاتيح.

## ما لم ينجز من الهدف
لا جسمبشريبعبد،لاsharedStart أوmission prepare/load/ready،ولاstatewireللشخصية/game-threadremotecontext بعد. ثمAI/combat/story/latejoin/friendlyfire والأسماء و60sdown/5srevive وانتقالاتالمراحل. وجودlocalactorعلىكلجهازلا يزامنالعالمتلقائياً؛لا تسوّق لُعبتينمنفصلتينكـco-op. تابعالهدفالأصليبأربعةبشر،معحفظالسنغلبلايروالأصل. حدثSTATE/MEMORY/PLAN/ISSUES/RESTOREبعدكلدفعةوCI.
