# إثبات ملكية الذاكرة الأصلية للاعب BLACK الإضافي — Session108

2026-11-23. الإصلاح [2910ab90](https://github.com/alwansan/BO/commit/2910ab90004b40749008ca8d82daa7787d3935e4) على الفرع `experiment/black-coop`، واختبار C++ المصدر [45204058892](https://github.com/alwansan/BO/actions/runs/45204058892) **SUCCESS**.

## السبب

قبل التصحيح كان `OriginalHostProxyEngine::actor()` يتحقق من `vtable/kind/serial/collider` وصلاحية صفحة الذاكرة وخروج actor عن نطاق world/NPC/physics، ولكنه لا يثبت أن actor خُصص من original player storage. يمكن لكائن مزيف أن ينسخ كلمات header الصحيحة في نطاق committed مجهول ثم يصل إلى استدعاء `DF400`. هذا ليس مجرد اختلاف بصري: محرك الفيزياء الأصلي يحتفظ بروابط عناوين مرجعية تحتاج lifetime مثبتاً.

## التنفيذ المصدر

- `OriginalHostProxyEngine::actor()` يطلب الآن `io.OwnsActor(actor,0x900)` **قبل** قبول header واستدعاء أي original engine guest call.
- `OriginalHostProxyKernelIO` العام يرفض تلقائياً كل actor (`OwnsActor=false`) لأنه ليس مالكاً لتخصيص independent human.
- `OriginalPlayerStorageKernelIO` يعطي الإثبات فقط إذا كان عنوان actor ومساحة `0x900` مسجلين في ledger التخصيصات لهذا storage adapter، وتثبت `mem::AllocationSize` عدم انتهاء ذلك التخصيص. ليست `mem::IsCommitted` وحدها شهادة ملكية.
- الأصل المستهدف `OriginalHostPlayerStorage::Allocate` يسجل خمسة أجزاء مستقلة، ومنها actor `0x900`. وفي teardown الآمن `Free` يمحو سجل التخصيص.
- أُضيفت اختبارات رفض header مزيف خارج سجلات التخصيص، رفض actor لم يعد مملوكاً قبل Acquire، ورفض Release بعد سحب شهادته مع حجر scene؛ الاختبار تحت ASAN/UBSAN نجح ضمن المصدر.

## ما لا يثبته الاختبار

- لم تنفذ بعد original setup مكتمل أو نشر هذا adapter على خيط Android الأصلي مع lifecycle الجاهز؛ `setupComplete` لا يزال يحتاج شهادة مستقلة فعلية لا يأتي بها عنوان التخصيص وحده.
- `OriginalProxySceneLedger` لا يجوز استبداله عند failure أو انتقال room/network؛ يجب أن يبقى مع owner original scene حتى تحرير مثبت. هذا دليل مصدر fixture لا صورة حقيقية للاعب في مهمة.
- Android APK الحالي room-only وليس `2–4 players original world`. المحرك Windows الخاص لا يزال يخرج عبر Dashboard ERROR قبل القائمة؛ اختبار الفئة الدقيقة [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) منتظر عند مراجعة Session108.

## الخطوة القادمة

استخراج فئة Dashboard error الدقيقة دون تسريب launch path/titleid/guest stack، ثم إصلاح native boot على أساس الدليل؛ تفعيل actor2–4 وسلاحهم وتحريكهم وفيزيائهم وAI ومزامنة host original NPC/mission + guest original state على خيط اللعبة الحقيقي؛ وبعد نجاح التكامل فقط تجربة كبيرة على هاتفين أو أكثر. لا تعديل `main` أو `CL` أو `biso` أو محتويات المالك الأصلية.
