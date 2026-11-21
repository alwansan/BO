# جلسة29 — قبول قائمة الهاتف وربط غرفة TLS فعلية

## الحالة الحية والهدف
استؤنف الهدف active. الدفعة السابقة تقدم فعلي:APK واجهة مبني ومفحوص ومرفوع. المالك أكد تشغيل اللعبة وظهور القوائم وطلب عدم التوقف حتى تجربة ثقيلة ضرورية أو اكتمال الهدف الكامل. لا نعيد اختبار UI أو Probe4. تعديل APK وحده مفضل ومأذون؛ISO فقط عند الحاجة.

## دليل المالك الجديد
قُرئت صور CL الجديدة الخمس الخاصة بـorg.bocoop.black.roomui: الزر أسفل الإعدادات في قائمتي المهمة والملف، وصفحة CO-OP / ROOM واضحة. مصدر السجل BLACK-Probe-20261121-173522-325:450JSONrecords،140/140pool و692/692instance links صالحة،منها552NPC و140local؛مدة140994ms وscopeواحد ودونend. لا دليل هاتفين أوreload. الملخص الآمن memory/evidence/ROOM-UI-owner-173522-safe-summary.json؛لا نسخ للخام أو الصور للمستودع.
black_log يؤكد الصورةc5،وفيه خطأFEAudioواحد عندConfirmWarning بعد249s. المستخدم لم يبلغ تعطل. السبب غير مثبت ولا إصلاح تخميني؛مسألةصوتمنفصلةلا تعيد اختبارالواجهة.

## التنفيذ الجديد — ليس محاكاة شبكة
خطة docs/superpowers/plans/2026-11-21-real-lobby.md. كودJava قياسي RoomWire/RoomTls ينفذ اتصالTLS socket فعلياً،مضيف+3عملاء،هويةشهادةpinnedSHA256 كاملة ورمزغرفةمشفرداخلTLS،تطابقالصورة/profile،أسماءUTF8محدودةومعرفاتبشرمنالمضيف،روسترحيheartbeatوإغلاقموارد. لاenginecalls ولاgameactors ولاAndroidadapter بعد.

اختباراتمحليةنجحت بمفاتيحTESTمولدةفي/tmpفقط:framing/UTF8/IDs،رفضPINوالشهادةوالصورةوالنسخةالخاطئة،roomfull،انقطاعوإعادةدخولبـIDجديد،تجزئةرسائلTLS،نبضاتوصفرthreadsمتبقيةبعدالإغلاق. كلاcoreJava8APIوtestsJava17مترجمانبـstrictwarnings؛المترجمالمحليالجديدحذرمنrelease8كخيارقديم،فـ-Xlint:-optionsيستثنيهذهالرسالةفقطويحافظعلىباقي-Werror.

## مراجعة وإصلاحات مثبتة
مراجعةمستقلةوجدتحالتينماديتين:publicationبعدcloseتعيدrosterقديمة،وروسـترمستمرةتخفيغيابPONG. أُعيدتالحالتانREDباختباراتحتمية/actualTLSثمأصلحتاGREEN؛publicationوcloseتحتlifecyclelock،وwatchdogلحداثةPONGمستقلعنقراءةROSTER. أُضيفأيضاًreproducerRED→GREENلـtrickledJOINdeadlineومنع تعديلsnapshotداخليةبواسطةcaller. ترتيبROSTERيُحفظبـnonblockingqueueoffersتحتقفلالمضيف؛الإغلاقوالكتابةالشبكيةخارجه. السعاتمحدودةولاgamewrites.

## المتبقي الفعلي
Actions real-lobby-core الجديد سيعيد الاختبارات علىLinux/JDK17. التالي AndroidKeyStore لهوية لكلتثبيت وfingerprintverification صريحة،اكتشافLANغيرموثوق/DirectIPv4،forms/JNI/immutable-state إلىصفحةFlash،ثمAPKمستقللاتصالغرفةحقيقي. لاطلبهاتفحتىتجربةهاتفين LAN/VPN مفيدة. إنشاءالأجساموالتحكمومزامنةالقصة/القتالمازالواهدفاًإلزامياًغيرمكتملين؛لايعتبرنجاحTLSقبولco-op.

## CI مُثبت والمرحلة التالية
run44917272041/job136885459965 **SUCCESS** على591372ba4819fd001ebb354dced27179e97ac548؛قُرئالسجلوجميعwire/actualTLS/heartbeatاختباراتPASS،coreAPIJava8وtestsJava17بـ-Werror. لاAndroidruntimeأوgameplayيثبتهذاالـCI.
أُضيفRoomIdentityAndroidKeyStoreوtestAPKsyntheticبلاGame/XBE/native،لفحصهويةopaqueتستقربينloadين واتصالTLSفعليبها. workflowblack-coop-lobby-android-platform سيجربAndroid29emulator؛الـmanifestوالـbashsyntaxمحلياًصالحان،والترجمةوالتشغيلعلىAndroidلميثبتابعد. لاتطلبمنالمالكهذاالفحصالصغيربدلاًمنالمحاكي.
