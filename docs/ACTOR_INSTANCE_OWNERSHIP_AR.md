# ملكية متحكم التحريك — دليل الصورة المطابقة وسجل الهاتف

## دليل الهاتف الجديد
الملخص الآمن memory/evidence/PROBE3-owner-090753-safe-summary.json. كل56لقطةpoolصالحة،zero rejects،196سطرJSON،نحو59.7ثانية دونend. رُصد17تركيبslot/serial،ومنهاحالتاتغيرserialمعاستمرارظهورالخانةفيcontainer1. إحداهماصاحبهااختلافresource320/35C. هذهإعادةتهيئةمرصودة،وليستاستدعاءspawn/deathدقيقاًأوheapfree. الانتقالاتالمشاهدة0→1عدد13،1→2عدد9،2→3عدد8،1→0عدد1. لاscopechange/reloadمرصودلأنreset_sequenceبقي1وworld_epochبقي1. لا تطلبإعادةProbe3لمجردغيابend؛استخدمالدليلوتقدمإلىملكيةالinstance.
resource320ظهرحتى4قيمغيرصفريةفياللقطة،معمشاركةبينعدةشخصيات؛35Cقيمةواحدةمشتركة. لايمكنتعريفrenderinstanceبهما.

## تحليل ثابت read-only للصورة c5bfed...
- 1DA4F0/1DA522:actor+320يساويinputstackargumentالثاني. 1C5C60يفكwrapperعبر[320]ثمينسخقوالبsharedresource+38/+40 إلىbuffersالمؤشرةبـactor+34C/+350. هذانمرشحانلمخازنكتابةخاصة؛Probe4يدعمعلاقتهماالمستقرة،ولايثبتملكيةالتخصيص.
- actor+328مرشحمتحكمتحريكحقيقي. 1E05B2..1E05EFيأخذهمنrootglobal2D12C8،array+70 وcounter+F4،معحد0x21(33)؛ينادي17EAE0للتهيئةويكتبالpointerإلىactor+328. استنتاج32NPC+1localمعقولةلكنهامحدوديةمرشحوليستترخيصلإنشاء33لاعباً.
- **17E420**ABIمنالتعليمات:EAXactor،ESIcontroller،stackarg1binding. عند17E448يكتبcontroller+0=actor؛عند17E44Aيكتبactor+328=controller؛ثمcontroller+50=binding. يعيدتهيئةmatricesعندcontroller+30/+34 وغيرهما. هذهbacklinksمثبتةثابتاً،وليستدعوةآمنةلإنشاءشخصيةمنnetworkthread.
- 17EAE0يصفرcontrollerowner+0وbinding+50ويخصصblock0x9D0إلىcontroller+54ويصفرname+5C. هذايتطابقمعالمصدرالمثبتhooks.cpp:animationowner+0،clip/runtime+54،name+5C. لا نفترضأن+54assetمشترك؛عقدهالأعمقيحتاجفحصاً.
- **1C6150** (NPCvirtualslot5) يقرأactor+320للوصفالمشترك،ويستعملactor+328→+54→+50 وactor+328→+50→+34 لملءأوصافالرسمفيbufferactor+34C،ثميناديLOD1C55A0وdrawhelper1C5B40. هذايربطcontrollerبمسارالرسم،معأنinstanceownership/runtimeالتفردلميُقبلبعد.
- actor+29Cفيالمصدرهوweapon؛لا تخلطهcontrollerالتحريكأوoffsetبنفسالاسمفيكلاسphysicsآخر.

## دليل Probe4 المقبول والخطوة التالية
سجل المالك BLACK-Probe-20261121-163248-271 قُرئ وفُحص؛ الملخص `memory/evidence/PROBE4-owner-163248-safe-summary.json`. 111/111 pool و548/548 instance links صالحة، منها437 NPC و111 local، دون اشتراك controller بينlocal وNPC في اللقطة. runtime وscratch غير صفرية في كل القراءات الصالحة. هذا يدعم علاقة controller الخاصة بالشخصية المرصودة، لكنه لا يثبت allocation ownership أو أن الذاكرة قابلة للنسخ أو آمنة للإنشاء. المالك لم يواجه مشاكل؛ لا إعادة Probe4. الالتقاط115107ms وscopeواحد ودونend، فلا دليلreload.

المسار التالي إنشاء actor عبر المحرك مع مكوّن تحكم324 مستقل وصالح. **رُفض مسار null-brain المسجل في العالم**: مسار إصابة1E7753..1E7759 يقرأtarget324 ثم+90 دونnullguard عندما384==0. إضافةdamagehookوحيد لا تثبت بقيةcallbacks. لا تستعر324منNPC لأن1D21F0 يكتبbacklink actor إلى+8C.

التسجيل1CE1F0( EAXactor،stackworld،ret4) يربطworld5CB4 وspatial-list4968؛الإزالة1BEB40 تفترضروابط34/38صالحة. تحديث1801B0ليسpose-only لأنهينادي1CE600/15850لتحريكspatialtree. مسار1DFC30يملكnullguardلكنذلكلايحميجميعالإصابات. أكدتمراجعةمستقلةهذهالعقود.



## ملاحظة تصميم proxy لاحق — استنتاج لا تعديل مطبق
المصفوفة الفيزيائية32slot. حدودالقوائم المستخدمة16live+8phase1+8phase2=32 (freecontainercapacity32مستقل،لكنالعضويةحصرية). 1DF440ينظفمرشحاًفقطعندماlivecount==livecapacityثميفترضfreecandidateمتاحاً. رفعlivecapacityوحدهقديكسرالافتراض:يمكنامتلاء32slotبينالقوائمالثلاثدونامتلاءliveالجديدة،فيُختارfreeصفر. لا توسعcapacityعشوائياًلإضافةremoteavatars؛يلزمتصميمحصةاجماليةوprotectedremotehandlesوفحصselector/AI/questownership. مورد20flagsوحوض33controllerمرشحانبحدودثابتةمنالصورة،وليساإثباتسعةplayersمتاحة. لا تفترضأن324=AIbrain أوأنتمريرهصفراًإلىfactoryآمنقبلتحليلعقده.


## جسم الشخصية المحلية ليس slot NPC — جلسة28
قرئت بياناتvtableمنالصورةالمطابقة:local28CFE0/virtualslot5يشيرإلى1D25D0،بينماNPC28C1E8/slot5يشيرإلى1C6150. الدالةالمحليةتبنيtransformعندactor+420وتكتب+460..490؛كلهاخارجstrideNPCالبالغ3C0. **لا تستعملدالةالرسمالمحليةعلىslotNPCولا shallowcopyللكلاسالمحلي.** هذايثبتاختلافمسارالرسموحجمالكائنالمطلوب؛لايثبتتوفرfull-bodyassetمطابقللشخصيةالرئيسيةولايُثبتغيابه. يجبفحصالموردوالـrigوالأنيميشنمنأصولالمالكقبلوعدمطابقةالشكل. مطلبالمالكأنكلبشريملكجسماًمستقلاًيشملتمثيلالمضيفعندالأقران،وليساستبدالNPCمحليباللاعبالثاني.

حوض324الأصليمنفصلعن32slotللشخصيات:reset190E60يبنيسلسلة16عنصراًبـstride1FE0عندroot2D1988+2B00،والـheadعند+22CDC،والـnextعندcontroller+1F00. helper1848F0يفكheadبلاnullguard؛caller1AF880يفحصheadقبلذلك. constructor/init1D21F0يعدلbacklink+8Cويستخدمحقولdescriptor+34/+40/+44وبعضهاينقلللـAIstate؛لاborrowلـ324منشخصيةقائمةولاacquisitionبلامعرفةملكيةالمراجع. cleanup1CC980يمرعبرمكوّناتافتراضيةويعيدcontrollerلـfreelist؛لاfreeSystemيدويبدلاًعنه. هذهعقودتحليلثابت،لاspawnمطبق.
