# حماية فيزياء اللاعبين الأصليين عبر كل الـadapters

Session106، 2026-11-23. **آخر تغيير فعلي:** `OriginalProxySceneLedger` في `coop/room_network/room_original_host_proxy_engine.hpp` الآن سجل واحد يجب مشاركته بين adapters اللاعبين الإضافيين في نفس مشهد المضيف الأصلي، بدلاً من الاعتماد حصراً على `uncertain_` المحلي.

## ما أُنجز في Session106

- تنفيذ مصدر [81e623fc](https://github.com/alwansan/BO/commit/81e623fc1768e151e6671db9538eede91cc868de) واختبارات [50b1d0e2](https://github.com/alwansan/BO/commit/50b1d0e2ec7ad91557798240508ca678907f3086).
- السجل يبقى طوال مشهد المحرك المثبت ويعرف `scene/world/physics`، ويسجل حتى ثلاثة actor/proxy بشرية مستقلة. يمنع ربط actor أو proxy مكرر، يمنع إنشاء لاعب رابع إضافي فوق الحد المخصص، ويُبقي حالة native-call معلقة حتى إثبات نجاحها أو إثبات التراجع عنها.
- إذا ضاعت ملكية proxy أو تغيّر المشهد أثناء استدعاء guest أو فشل rollback، يبقى السجل محجوراً؛ حذف adapter وإنشاؤه مرة ثانية **لا يمحو سجل المشكلة**. عند إتلاف adapter وهو يملك proxy لم يحرره، يُحجر السجل أيضاً.
- اختبار حقيقي من `g++` مع ASAN/UBSAN في [45199349609](https://github.com/alwansan/BO/actions/runs/45199349609) **SUCCESS**، ويتضمن 3 أجسام مستقلة + محاولة جسم رابع، إعادة استخدام slot بعد تحرير موثوق، والتأكد من استمرار الحجر بعد حذف adapter أو استثناء native جزئي.
- المنشأ الأصلي لـactor2 وscene generation وcorrect original resource teardown لم يثبت داخل Android بعد. السجل ليس engine-global static ولا يحل المشكلات إذا دمره caller وأعاد إنشاء سجل مختلف. يجب أن يعيش ضمن *مالك طور القصة* لا داخل player adapter، ولا يُعاد إنشاؤه إلا بعد teardown مثبت من المحرك الأصلي. لذلك هذا مصدر اختبار تحضيري وليس co-op gameplay ناجحاً.

## الحدود الأساسية

لا تقم بتفعيل هذا المسار داخل Android قبل معرفة lifetime أصلي للبنى التالية: actor/camera/animation/weapon/audio/physics/AI. لا يكفي أن يكون العنوان committed أو header actor ظاهرياً. كل أعمال الفيزياء وعمليات إنشاء/إزالة ledger تستدعي على خيط اللعبة الأصلية فقط. لا تسرب ملفات اللعبة أو بيانات runtime الخاصة في السجلات.

## عائق الإقلاع

المختبر الأصلي Windows [45193373405](https://github.com/alwansan/BO/actions/runs/45193373405) أعاد `HalQuickRebootRoutine` بعد 202ms؛ فشل قبول gameplay. اختبار parser الخاص [45197279738](https://github.com/alwansan/BO/actions/runs/45197279738) كان قيد التنفيذ عند آخر فحص. يجب معرفة إن كان launch-page مطلوباً أو فشل موارد قبل أي تجاوز لطلب إعادة التشغيل. بناء Android الحالي room-only، لا عالم مشترك.


---

# حماية استدعاء الفيزياء الأصلي عند الفشل الجزئي

Session104، 2026-11-23 — `OriginalHostProxyEngine` في `coop/room_network/room_original_host_proxy_engine.hpp`.

## التنفيذ الحالي

يجب أن يبدأ إنشاء proxy مستقل للاعب مضيف إضافي فقط بعد تحقق actor وscene والـphysics pool على خيط اللعبة الأصلي. تم تصحيح حالة مهمة: استدعاء `DF400` أو `DBBE0` قد يعدل جدول الفيزياء قبل أن يرجع خطأ أو يرمي استثناء؛ لا يجوز إعادة الاستدعاء آلياً عند فقدان الدليل. قبل أول استدعاء engine يتم تعيين `uncertain_`، ولا تُرفع إلا بعد Acquire موفق بملكية موثقة أو Release/Rollback ثبتت نتائجهما في نطاق المشهد نفسه.

إذا تعذر التحقق من snapshot/backlink أو تبدل world أو فشل الإلغاء، ترجع الدالة فشلًا مع `RequiresEngineCleanup()` وتحجب المحاولات التالية **على نفس كائن adapter**. مسار Throw في كل من create/release محمي باختبارات. لا تُحرر أي proxy بذاكرة عمياء، ولا تفترض أن EAX يشير إلى proxy؛ النجاح الأصلي يعتمد على contract الموثق للـAL والـbacklink.

## حدود مهمة جداً

- `uncertain_` مخزن داخل C++ adapter **فقط**. تبديل الكائن بكائن جديد يزيل علم المنع محلياً لكنه لا يزيل original proxy ولا يعيد مورد engine. لذلك يجب أن يبقى adapter حيّاً طوال ملكية actor/match، ولا يعاد إنشاؤه بعد أي فشل غير مؤكد قبل cleanup engine مثبت أو تفكيك مشهد ثبت تحرير موارده الأصلية.
- لم تُنفذ بعد آلية engine-global persistent quarantine ledger، ولا إدماج Android native actor2. يجب إضافة ledger مشترك يحفظ ownership عبر كل adapters قبل التشغيل في production.
- لا يكفي `mem::IsCommitted` أو guest header يبدو صحيحاً لإثبات ملكية proxy أو actor. لا يجوز مشاركة proxy اللاعب الأول أو الـNPC.
- CI `45195066451` فحص C++ source/ASAN/UBSAN **SUCCESS**، لكنه لا يثبت قبول engine runtime أو وجود عالم مشترك.

## المتبقي

1. الحصول على نتيجة مختبر Windows الحقيقي بعد إضافة تصنيف `HalReturnToFirmware` المحدود وإصلاح الإقلاع بناء على الدليل.
2. إكمال التخصيص/التهيئة الأصلية للاعب2 والسلاح والصوت والـanimation وإضافة engine-global lifetime ledger؛ بعدها الربط بالـoriginal physics وAI.
3. تفعيل مرسل HOST authoritative للقتال/الأعداء/الأبواب/الأهداف، ومستهلك guest original-engine الآمن، ثم تجربة كبيرة على هاتفين أو أكثر.

لا تعديل `main` أو `CL` أو `biso` أو ملفات اللعبة الأصلية أو الحفظ.
