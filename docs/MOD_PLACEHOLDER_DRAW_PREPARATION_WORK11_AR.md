# Work-Batch11 — تجهيز مجسم مملوك للمود على واجهة SDK

هذه خطوة مباشرة في عائق العرض من Work10، وليست تحسين نقل أو إثبات ظهور لاعب. الأساس `1181690e9019e85abc0b3265d08bf8a39b117fe6`، الفرع الوحيد `experiment/black-coop`.

## الناتج

`coop/room_network/room_mod_placeholder_draw.hpp` يجهّز صندوقًا من 36 رأسًا/12 مثلثًا مملوكًا للمود، من مركز قدم وأبعاد ووحدات يزودها المستدعي. لا نموذج أصلي أو مورد لعبة أو NPC أو actor. تحفظ CPU vertices داخل الناتج، ولا تستعير مخزن إدخال.

يبني `gpu::State` جديدًا مستقلًا، مع نسخ هدف اللون والعمق والـviewport/scissor فقط من إطار المستدعي؛ view/projection مستقلان صريحان، world هو identity لأن الرؤوس عالمية. المسار غير مضاء وذو diffuse ثابت وdepth test، ولا يكتب العمق. لا يرث shaders/textures/streams/lighting/stencil/blend/occlusion من اللعبة، ولا يغير الحالة الأصلية. تدعم الحزمة لاحقًا عدة أجسام بقيم أخرى، لكن لا يوجد ربط roster حي هنا.

ترفض القيم غير المحدودة، الأبعاد الصفرية/السلبية، overflow أو انهيار الأبعاد بسبب دقة float، viewport غير صالح أو غياب target/depth. تترك الناتج السابق دون تغيير عند الرفض. المصفوفة finite/nonzero ليست إثباتًا لصحة كاميرا BLACK أو عمرها.

**لا يمكن إرسال الناتج إلى GPU مباشرة:** stream0 غير مربوط عمدًا، ولا دالة Draw أو hook أو تخصيص guest/GPU. يلزم adapter يرفع الرؤوس إلى مخزن مستقل ذي generation ويحتفظ به حتى fence، مع إعادة فحص التذكرة والمشهد/الإطار والكاميرا عند الاستهلاك. لا تستخدم مؤشرات CPU المؤقتة أو تحول FALSE في بوابات actor إلى TRUE لإظهار الصندوق.

## المرجع المصرح به

- `TGP482/XboxXBE-Recomp@99078b119b5d5e0d1d6afb1390e29437c0c984c0`: `runtime/gpu/state.h` بنية State؛ `gpu/backend.h` واجهة Draw/Fence؛ `gpu/vertex.cpp` FVF XYZ|DIFFUSE؛ `gpu/d3d12.cpp` معاني depth/color mask/cull؛ `gpu/shaders.cpp` fixed-function diffuse. استخدام API وكتابة تجهيز أصلي، لا أصول أو شفرات لعبة منسوخة.
- `RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806`: `src/d3d/d3d8.cpp` تحويل slots view2/projection3/world256 وآلية DrawUp ذات ring/fence؛ `src/d3d/render_thread.cpp` يوضح أن ذاكرة الرؤوس تُقرأ لاحقًا. لذلك العمر حتى اكتمال GPU شرط، لا مجرد حفظ state حول call.
- ما زال غير مثبت أن حالة fixed-function أو مصفوفات متاحة في hook رسم العالم هي كاميرا BLACK الحالية على Android؛ لا نسحبها من shader constants بتخمين. لا نشر ترويسات الطرف الثالث أو تعديلها في BO.

## التحقق

اختبار `test_room_mod_placeholder_draw.cpp` يُترجم مقابل ترويسة SDK المثبتة نفسها، ويتحقق من حدود الصندوق ومساحة كل مثلث، استقلال shader/texture/stream/depth، عدم تعديل حالة المصدر، والرفض الذري للمدخلات غير الصالحة. محليًا g++ C++20، تحذيرات صارمة وASAN/UBSAN، LSAN معطل في البيئة المحلية فقط. أضيفت خطوة إلى preflight المصدر لاستعمال checkout SDK المثبت الموجود في workflow. لا Vulkan فعلي أو rasterization أو Android APK أو gameplay في هذا الاختبار.

## أقرب خطوة متبقية

إثبات مصدر view/projection الخاص بإطار العالم الحالي، ثم adapter رفع مستقل fenced يملك الرؤوس عبر render-thread queue، واختبار rasterization بلا أصول على backend المصرح به قبل أي hook حي. بعد العرض تأتي وصلة فعل الضيف -> فعل أصلي على المضيف -> تطبيق/قراءة مشتركة وانتقال المرحلة؛ لم تنجزها هذه الدفعة. لا مسار ISO/APK اليدوي أو طلب هاتف جزئي.
