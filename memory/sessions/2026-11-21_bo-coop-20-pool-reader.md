# جلسة20 — قارئ جدول الشخصيات المستقل، 2026-11-21

استمرار من21f1808. خطة docs/superpowers/plans/2026-11-21-probe3-pool.md،عقود docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md. أضيفcoop/probe3/pool_snapshot.hpp وtest_pool.cpp وCI black-coop-pool-core.yml. **ليس مرتبطاً باللعبة/APK بعد**؛Task2 (package/hooks/export/build) ما زالت غير منفذة. لا multiplayer أوspawn فعلي.

ReadPool يستخدمGuestReader الفعلي safe process_vm_readv بلاdereference،يرفضpool غيرمطابق وoverflow وcapacity/occupied غيرمتسقين،يقرأ كلholes فيsparse arrays،يتحقق كل32slot مرةواحدة وvtable وactor+250 membership،يقرأserial+374/state+384،يقارن قراءتين كاملتين بمافيهاraw arrays. calleroutput لا يتغير عندالفشل. لا ادعاءatomicity أوكشفABA،ولاserial هويةدائمة عبرreset. الحدودثابتة ولاallocations. عددنسخsyscalls قد يصل396 لكلsnapshot صالح؛قبلربطAPK قللها بقراءاتblocks ذاتحجمحدودمؤكد واختبر الأداء؛samplingمرةبالثانية فقط.

TDD: declarationstub أعادfalse وفشلtestالقبول كمايجب ثمimplementation اجتاز. strictClang21 C++20 -Wall -Wextra -Werror -pedantic محلياًPASS،والـprobe القديمtest_probe.cpp PASS. بياناتfixtureمصطنعة دونgamebytes/userlogs،لكنالقراءةمنkernelحقيقية. يتضمنPROT_NONE للmanager واختبارفشل متأخر فيarray ثانيةبعدقراءة31actor. independentreview pool_core_review كشفأنreorderداخلنفسcontainer كانيضيعقبلالمقارنة؛أضفناtest فشلREDقبلالإصلاح ثمحفظraw arrays وأصبحGREEN. workflowجديدASAN/UBSAN Linux وNDK28 ARM64 crosscompile،النتيجةلمتصلبعدعندكتابةهذاالسجل.

أكملCI ثمTask2 فيالخطة:isolatedorg.bocoop.black.probe3،snapshotبعدplayerhook بمعدل1Hzوسجلbounded،resetwrapper بعدoriginalيضيفepochفقطإذاAL1؛تحققgeneratedboundary/ctx preservation،خيارopt-in والتصديركماProbe2. حافظعلىProbe2 وbaseline والأصل والحفظ. لا تطلباختبارمالكحتىAPKجاهز.
