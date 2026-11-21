# BO-COOP — استعادة أحدث حالة، جلسة26، 2026-11-21

## الهدف والفرع
تطوير story co-op حقيقي لـBLACK Android، وليس TDM أو محاكاة شبكة. المتطلبات الكاملة في `docs/OWNER_PROJECT_OBJECTIVE_AR.md`. **لم يكتمل co-op بعد**: لا لاعب ثانٍ حقيقي أو COMBAT/AI/quest sync مقبول، ولا ISO معدل جديد. الكتابة حصراً `alwansan/BO:experiment/black-coop`؛main وكل المراجع للقراءة فقط،لاforcepush. المالك يجيز البحث والتنفيذ وActions حتى الحاجة لاختبار هاتف.

ابدأ بـAGENTS.md ثمmemory/STATE.json و`memory/sessions/2026-11-21_bo-coop-26-probe4-ready.md`،ثمMEMORY/PLAN_AR/ISSUES_AR. تحقق منHEAD وActions الحيين؛لا تستعد حالة تاريخية قديمة بدلاً منالحالة هنا.

## الخطوة التالية الآن
**Probe4 APK جاهز ومثبت، ويحتاج اختبار روابط التحريك على هاتف المالك.** نفّذ `docs/PROBE4_DEVICE_TEST_AR.md`:Startcapture،حركةوإطلاقناربقربعدةNPC،قتلعدووcheckpoint/reloadخلال90ثانيةإنأمكن،ثمbackground/exportوأحدثcoop_probe.txt. قبلإعادةطلبالبياناتافحصCLوالـattachments؛قديرفعالمالكالسجلدونإرفاقهنصاً. لا تطلبإعادةbaseline/Probe2/Probe3 أوISO/Androiddata.

- APKrun44876183662 **SUCCESS**،source `1b27ecc26e5417b91eb84208479e039e9b792521`؛LinuxASAN/UBSAN وWindows/NDK/APKنجحا وقرئتlogs.
- package `org.bocoop.black.probe4`،labelBLACK Probe4 READ ONLY،21749755bytes،SHA256 `bfd7968b04851d8c814bdb88563b1efb8e8b9b88695d2c161c077111bd3c9897`.
- [artifact14696342069](https://github.com/alwansan/BO/actions/runs/44876183662/artifacts/14696342069)،metadata14697085027؛artifactالتطبيقينتهي2026-12-05. نُزلZIPوفُحصZIP/APKhashمستقلاً؛الأدلة `memory/evidence/PROBE4-apk-44876183662.json`. لاsignedURLsفيالمستودع.
- نسخةمحلية `/home/a/Desktop/black/Probe4-44876183662/black-probe4-candidate-READ-ONLY-NO-COOP.apk` ومعهاmetadata/NOTICES،ولايفترضوجودهافيجلسةسحابيةجديدة.
- أحدثanalyzerCI44908343745 **SUCCESS** على `399cf730f95dfb6a52c76f02cca30b353545cadd`؛تحديثPythonفقط،لايغيّرAPKالمذكور. لاCIمطلوبإعادةبدئهلمجردالاستعادة.

## ما أثبته سجل المالك السابق
وصلسجلProbe3 `alwansan/CL:main/black log/logs/BLACK-Probe-20261121-090753-249/coop_probe.txt` وقرئفعلياً. 89203bytes،SHA256 `d95bddaedf2edcbe4d670f3aa776060fabc622982e6a17862574477a4e803e0e`،blobSHA `02ab58b296a4ae09f27f8ef173e244936423a7ba`. لاتنسخالخامإلىBO.
196JSONrecords تشمل56/56poolصالحة وzero rejects؛رُصدت17slot/serialcombinations وتبدلاserialفيالخانةنفسهامعظهورهاoccupied. الانتقالاتالمشاهدة0→1عدد13،1→2عدد9،2→3عدد8،1→0عدد1. هذايدعمإعادةالتهيئةالمرصودة،وليسcallbacksدقيقةلـspawn/death. اللقطةحوالي59.7ثانيةبـscopeواحدودونend،فلاreloadproofداخلنافذتها. ملخصآمن `memory/evidence/PROBE3-owner-090753-safe-summary.json` وجلسة24.
resource320مشتركبينعدةشخصيات(حتى4قيمغيرصفريةفياللقطة)،و35Cقيمةواحدةمشتركة؛ليساإثباتmodelinstance. **لا تطلبProbe3نفسهمجدداً.** baseline وProbe2قبلهيعملانعلىهاتفالمالك؛لا تكررطلبXBEالأصلي82d058غيرالمتوفر.

## معرفة المحرك التي يبنى عليها العمل
اقرأ `docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md` و`docs/ACTOR_INSTANCE_OWNERSHIP_AR.md` قبلأيguestwrite أوfactorycall.
- الصورةالمطابقةc5bfed25449c8212d7a976f15246e87ead57713f. globalmanager2D1980؛32NPCslotsبداية+A0،stride3C0. vtableNPC28C1E8 وlocal28CFE0. أولvtableليسdestructorمفترضاً.
- containers79A0/free،79B0/live-like،79C0/phase1،79D0/phase2؛+4capacity/+8occupied/+Cpointer،arraysبهاholes. actor+250membership،+374serial،+384state. reset1D5E00ينجحAL1ويعيدcounter8524إلى1؛الفشلAL0لايعدreset. لاهويةدائمةبعنوانpointerأوserialمنفرد.
- factory-like1DF440وinit1DA4F0مشروحانثابتاً؛لايستدعيهماالرصد. زيادةlivecapacityوحدهاقدتكسرfree-slotinvariant؛لاتستدعfactoryبـdescriptor أو324مفترضين.
- **actor328هوcontrollerمرشححقيقي**:17E420يربطactor328↔controller.owner0؛runtime54/binding50 وscratch34C/350يستخدمهاdraw1C6150. Probe4يفحصbacklinkوهويةالشخصيةقبلوبعدالقراءة؛حتى4NPC+localكلثانية،حد300kernelcopies،دونguestwrites/calls.
- instance_schema1 وinstances tuple `[local,slot,serial,valid,controller,runtime,binding,scratchA,scratchB]`. validيعنيbacklinkوالحقولالمختارةاستقرتفيالقراءة؛**لايثبتreadabletargets أوallocationownership أوuniqueness أوatomicity/ABA**. الفشلpointeroutputsصفر. بعددليلالهاتفانتقللتجربةbody/proxyحقيقيةبخيطاللعبةوحمايةscope/slot/serial؛لا تستبدلهابتوسيعmocknetwork.

## الأدوات والكود
`coop/analysis/pool_capture.py`يحللProbe3/4ويخرِجaggregatesفقط؛10pooltests،19analysis test_*.py و11legacyprivacychecksمحلياًPASS ومراجعةمستقلةواضحة. `coop/probe4/instance_graph.hpp`وpool_observer.inc هماالرصدالجديد؛prepare_probe4.pyيعملفيstageمؤقتويحافظعلىProbe3. الخطة `docs/superpowers/plans/2026-11-21-probe3-evidence-and-instance.md`.
NET/SceneInbox/RemoteProxyRoster وLANPOSIXالقديمةنجحتضمنحدودloopback/synthetic/NDKفقط؛لاauthأوAndroidLANruntimeأوgameactorintegrationمقبول. CO-OP/ROOMصفحةOPTIONSمؤقتة،زرالقائمةالعلياطلبمفتوح.

## المصادر والمراجع الإلزامية
- ownerISOموجودمجزأفيBO ومفككفي `alwansan/biso@9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`؛`docs/OWNER_BISO_SOURCE_AR.md` وrestore_github_upload.py. لاتطلبإعادةرفعالأصول،ولاتنشُرISO/XBE/generatedsource/keysفيcommitsأوartifacts.
- Black-Androidbaseline `f3754d6985a8c35204e7df071546e586b10f1806` وSDK `99078b119b5d5e0d1d6afb1390e29437c0c984c0` للقراءةفقط. يعادcompileلـc5المطابق؛لاbypassلـimagegateولاادعاءمطابقةمصدرالتعديلاتالعربيةالكامل.
- مرجعالمالك `Peterson047/BLACK-XBEMultiplayer@0ee3ae6411c7f0989c62cbe36b224d3f55043923` مازالheadنفسهعندجلسة24. اقرأ `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`:WindowsTDMbinary-only،nativeBLACK-Coopالمشارإليه404،لاLICENSEشامل،body.bcaمقيدبذلكالمود. استفدمنأفكارالبنيةواكتبكوداًمستقلاً؛لاتنسخassets/binaries/scriptsغيرالمرخصة.

## الذاكرة والتسليم
بعدكلدفعة/قرار/CIحدّثSTATE/MEMORY/PLAN/ISSUESحسبالحاجة والجلسة وRESTOREوادفعنفسالفرعبـCASدونforce. احفظSHAالكودالمفحوصوRunIDs،لاSHAcommitالتوثيقنفسهداخله. المستودعالحيهوالمصدرالمرجعي.
محلياًBO-activecheckoutقديمHEADولهملفاترفعتعبرGitHubAPI؛لا تفترضأنgitstatusالمحليفروقمستجدة. فيجلسةجديدةاستنسخالفرعالحينظيفاًبـsparsecheckoutلـcoop/docs/memory/prompt/.githubبدلاًمنجلبكلالأصول. **لاتعملresetأوحذف لـBO-coopالقديم** ولهتعديلاتمحليةمسبقة. تاريخالجلساتمحفوظفيmemory/sessions؛لايُفهمحفظالذاكرةكعملفيالخلفيةخارجالجلسة.
