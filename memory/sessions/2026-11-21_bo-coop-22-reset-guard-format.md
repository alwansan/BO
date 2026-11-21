# جلسة22 — إصلاح توقعات فاحص generated reset

APK run44872596889 على96b9c8366a80dd321be47857dc67f1000213fe7f انتهىFAILURE،job136750222861؛قرئتlogsبالموصل. فشلstepVerify generated original LOD and pool-reset definitions قبلGradle،بسببValueError Successful counter store missing or wrong destination/value. LODdefinitionPASS. لاAPKمبنيةفيهذاrun،ولاruntimefailureمثبت.

## سبب مثبت وإصلاح محدود
قراءةSDKpin99078b119b5d5e0d1d6afb1390e29437c0c984c0 tools/recomp/codegen_int.cpp EmitInteger وcodegen.cpp AddrExpr/Write/RegRead أثبتت أنMOVmemoryينشئlocaluint32_t ea=(uint32_t)(ctx.edi+0x8524u)،ثمX86_MEM32(ea)=(uint32_t)(ctx.eax). الفاحصكانيتوقعinlineaddress. وكذلكXORrdيولدcastإضافياًإلىuint8_tحولRegRead،لميشملهالـfixtureالأول.

حُدث handwrittenfixture إلىصيغةSDKالمثبتة (ليستgamebytes/sourceمنالناتج)،وفشلpositivecaseREDقبلالإصلاح. حُدثverify_probe3_reset.py ليطلبإسنادeaالدقيق **والـstoreالموافقإليه**،معإبقاءEAX1/ALzeroحفظupperEAX وثلاثret4 checks. أضيفتnegativecasesلـEDI→ECX وstoreEAX→ECX بجانبmissingstore/wrongsuccess/OR/ret12. Python12/12PASS وpreparers6/6PASS وdiffcheckPASS. راجعpool_core_reviewالإصلاحوأكدأنهformatcorrectionدونخفضالشروط.

هذاcodefixيحتاجrunجديدعلىcommitالجديد؛لاrerunللـrunالقديمالذييثبتcommitخاطئ. قارئpoolوالـobserverلميتغيرا،وقبولهماcoreCI44872596855SUCCESSلايزالصحيحاًلنطاقه. تابعAPKالجديدحتىالنهاية؛لاطلباختبارهاتفحتىAPKartifactوتحققهوية/package/signature. إذاقفلتبوابةشكلأخرىافحصSDK/الناتجخصوصياًولا تطبعfullgeneratedcodeأوتنشره.


## البناء الجديد بدأ فعلياً
source678536b205506be6cfcc4ca86a18afe3391e0aa9،APK44872995034in_progress وcore44872995111queuedظهرافيGitHubAPI. تابعنفسrunبدلاًمنrestart.
