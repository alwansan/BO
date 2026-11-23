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
