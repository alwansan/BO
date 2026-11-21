# جلسة21 — دمج Probe3 وبناء Android قيد الإطلاق

Core run44871689814 علىe5b7999db4de3680b48065243895308e622258a2 completed SUCCESS بكلاjob LinuxASAN/UBSAN وNDK28 ARM64 crosscompile. لا هاتف/لعبة في ذلكCI.

## التغيير الحقيقي في هذه الدفعة
- prepare_probe3.py يبني transform فيstage مؤقت ثم14ملفاً فقط إلىsource checkout؛packageorg.bocoop.black.probe3 وlabelProbe3. Probe2/الأصل/الحفظ لمتتغير. الشبكةمعطلةكماProbe2،التقاطاختياري وتصديرMediaStoreكماقبولAndroid15.
- pool_observer.inc يدخلداخلnamespacebo_probeanonymous بعدWrite،ويرصدمنplayerhook بعدObserve القديم بحد1Hz شاملاًمحاولاتالفشل. نفسlog/export وحدود1MiB/180ثانية/2048records،وسجلpool واحدفيالثانية منreservedbudget كيلايجوعهLOD. samplerيتحققworld/managerbefore/after وresetsequencebefore/after،ويرفضالاختلاف. لاatomicity/ABAproof.
- poolsnapshot صارت212kernelcopies للقراءة المزدوجة الكاملة،مقيدةباختبار<=220. مجموعالـsamplerمعglobals<=220. arraysتقرأblocksبطولcapacityفقط؛metadataمن+320..+387 نسخة104bytes واحدةمحدودة **ليستatomic**. تتضمنserial/state وresource320/35C لنفسslot؛لاpointerdereferenceإضافي.
- JSONeventpool يحتويvalid,ms,world_epoch,reset_sequence,counter,occupied[4],actors32. كلtuple: **[physical_slot,container_index,serial,state,resource320,resource35c]**. freegroup0يصفّرآخر4قيم لأنهاstaleوليستهويةحياة. pointervaluesخاممرصودةوليستإثباتrendererinstance/modelownership. invalidrecordلاactors.
- resetwrapper001D5E00 ينادي__imp__original أولاً ثميفحصAL1 فقطويزيدatomiccounter. لاctxwritesجديدةولااستدعاءguestمنsampler. reset_sequence عددsuccessfulhookreturnsأثناءopt-in،ليسnetworkidentityأودليلاًعلىرصدكلtransitionعابر. registryworld_epoch القديميبقىpointercorrelation؛استخدمreset_sequenceمعه ولا تدّعlifetimeدائم.
- verify_probe3_reset.py يفحصشكلعملياتgeneratedbodyالمثبتةبالـVA: EAX1success،storeفعليEDI+8524←EAX،ALzeroXORفيearlyfailوثلاثreturnsesp+=4. workflowيرفضmissing/duplicate/trappingdefinitions. اختباراتسالبةتمنعconstant/commentبدونstore وret12 وsuccess0 وORبدلXOR. **staticshapeguard وليستنفيذABI/runtimeproof**؛إعادةالترجمةالفعليةوبناءAPKلميثبتابعدوقتكتابةالسجل.
- workflowجديدblack-coop-probe3-apk.yml منفصلعنProbe2،يعيدتركيبownerISOبـrunnerفقط،ويفحصهويةc5والحزمةوالتوقيعوالصلاحياتوJNIويصدرAPKProbe3+metadata/NOTICESفقط. لاgameassets/ISO/XBEفيartifact.

## أدلة محلية ومراجعة
strictClang21 C++20 -Wall -Wextra -Werror -pedantic:pooltestPASS،observerالحقيقي.inc معGuestReaderkernelreadوحوضذاكرةمصطنعPASS؛JSON3recordsvalidtrue/false/true وratelimitوفريmetadatazero وerrnoحُفظ وslot31/serial131/resourcesارتبطتمباشرة. Pythontest_*.py12/12PASS،preparers6/6PASS،YAMLوembeddedPythoncompilePASS. سببفشلcompileمحليأولكانformat-securityعندappendبدونarguments؛أصلحناcallsإلى%sونجحالـharness. reviewpool_core_review كشفعدمربطresourcesبالslots وضعفgeneratedguard؛كلاهماعولجوأكدمراجععدموجودملاحظاتمهمةإضافية. لا اختبارdeviceجديد.

## التالي الحتمي
تابعActionsبعدpush حتىنهايةcore+APK؛عندفشلgeneratedguardافحصbodyالمؤقتعلىrunnerبلاpublishgamecodeولاخفضشروطهعشوائياً. عندنجاحAPKاطلباختبارمالكواحدمحدد: ابدأcaptureفيProbe3،شاهدظهورأعداءواقتلواحداً،أعدcheckpoint/المهمةثمصدّرالسجل. حافظعلىbaseline/Probe2؛لااختبارnetworkالآن. لاcoopفعليولاISOمعدلجديدبهذهالدفعة؛هذهبوابةACTORlifetime/model evidenceلإكمالremoteactorلاحقاً.
