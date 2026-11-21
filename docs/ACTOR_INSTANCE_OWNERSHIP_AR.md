# ملكية متحكم التحريك — دليل الصورة المطابقة وسجل الهاتف

## دليل الهاتف الجديد
الملخص الآمن memory/evidence/PROBE3-owner-090753-safe-summary.json. كل56لقطةpoolصالحة،zero rejects،196سطرJSON،نحو59.7ثانية دونend. رُصد17تركيبslot/serial،ومنهاحالتاتغيرserialمعاستمرارظهورالخانةفيcontainer1. إحداهماصاحبهااختلافresource320/35C. هذهإعادةتهيئةمرصودة،وليستاستدعاءspawn/deathدقيقاًأوheapfree. الانتقالاتالمشاهدة0→1عدد13،1→2عدد9،2→3عدد8،1→0عدد1. لاscopechange/reloadمرصودلأنreset_sequenceبقي1وworld_epochبقي1. لا تطلبإعادةProbe3لمجردغيابend؛استخدمالدليلوتقدمإلىملكيةالinstance.
resource320ظهرحتى4قيمغيرصفريةفياللقطة،معمشاركةبينعدةشخصيات؛35Cقيمةواحدةمشتركة. لايمكنتعريفrenderinstanceبهما.

## تحليل ثابت read-only للصورة c5bfed...
- 1DA4F0/1DA522:actor+320يساويinputstackargumentالثاني. 1C5C60يفكwrapperعبر[320]ثمينسخقوالبsharedresource+38/+40 إلىbuffersالمؤشرةبـactor+34C/+350. هذانمرشحانلمخازنكتابةخاصة؛تفردهماعلىالهاتفلميثبتبعد.
- actor+328مرشحمتحكمتحريكحقيقي. 1E05B2..1E05EFيأخذهمنrootglobal2D12C8،array+70 وcounter+F4،معحد0x21(33)؛ينادي17EAE0للتهيئةويكتبالpointerإلىactor+328. استنتاج32NPC+1localمعقولةلكنهامحدوديةمرشحوليستترخيصلإنشاء33لاعباً.
- **17E420**ABIمنالتعليمات:EAXactor،ESIcontroller،stackarg1binding. عند17E448يكتبcontroller+0=actor؛عند17E44Aيكتبactor+328=controller؛ثمcontroller+50=binding. يعيدتهيئةmatricesعندcontroller+30/+34 وغيرهما. هذهbacklinksمثبتةثابتاً،وليستدعوةآمنةلإنشاءشخصيةمنnetworkthread.
- 17EAE0يصفرcontrollerowner+0وbinding+50ويخصصblock0x9D0إلىcontroller+54ويصفرname+5C. هذايتطابقمعالمصدرالمثبتhooks.cpp:animationowner+0،clip/runtime+54،name+5C. لا نفترضأن+54assetمشترك؛عقدهالأعمقيحتاجفحصاً.
- **1C6150** (NPCvirtualslot5) يقرأactor+320للوصفالمشترك،ويستعملactor+328→+54→+50 وactor+328→+50→+34 لملءأوصافالرسمفيbufferactor+34C،ثميناديLOD1C55A0وdrawhelper1C5B40. هذايربطcontrollerبمسارالرسم،معأنinstanceownership/runtimeالتفردلميُقبلبعد.
- actor+29Cفيالمصدرهوweapon؛لا تخلطهcontrollerالتحريكأوoffsetبنفسالاسمفيكلاسphysicsآخر.

## خطوة عملية تالية
رصدمنفصلopt-in،بدونguestwrites/calls،يتحققlivepoolslot+serialقبلوبعدقراءةcontroller328 وbacklinkowner وruntime54/binding50 وscratch34C/350. سجّلvalidityلكلرابط،ولا تحولفشلقراءةمرشحإلىNPCdeath. افحصالتفردعبرالشخصياتوالتبدلعندإعادةاستخدامالخانة. لا تنسخbody.bcaالمقيدمنPeterson؛مرجعهWindowsTDMأفكاربنيةفقطوhead0ee3ae6411c7f0989c62cbe36b224d3f55043923لميَتغيرعندالفحصالجديد.
