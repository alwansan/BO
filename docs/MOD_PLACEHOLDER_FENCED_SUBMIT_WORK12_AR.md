# Work12 — رفع ورسم fenced لمجسم مستقل

2026-11-26، الأساس e809cbd4041a6d9c2bbfc8a486713374634a4156. امتداد مباشر لمجسم Work11؛ لا تعديل AI/أصول/حفظ، ولا ISO/APK يدوي، ولا hook حي أو ادعاء ظهور جسم في BLACK.

`SubmitModPlaceholder` ينسخ الرؤوس إلى lease مستقلة، يربط stream0 في نسخة منفصلة من State، وينادي SDK Backend::Draw للـtriangle list ثم Fence/WaitFence. لا يحرر الذاكرة قبل تأكيد انتهاء القراءة المؤجلة. التنفيذ الأول متزامن ويتعمد انتظار fence بدل تحسين الأداء الآن. يُرفض shader/texture/stream مستعار، وتُفحص هوية backend وحاجز المستدعي قبل التخصيص وقبل الإرسال.

`ModPlaceholderUploadKernel` bridge فعلي إلى AllocContiguous وAllocSystem، وترويسة vertex buffer ذات common=1 وphysical data من PhysicalFromVirtual؛ نفس عقد UpVertices في Android baseline المثبت. لا يستعمل ذاكرة اللعبة أو مخزن DrawUP الأصلي. يحتفظ بثماني leases كحد أقصى، ويرفض خيطًا آخر. استثناء Draw أو fence غير صالح/ما زال pending يعزل التخصيصات؛ لا تحرير تخميني. Drain يحتاج نفس backend الحي ويؤكد fence لاحقًا قبل تنظيف العزل. يجب أن يعيش backend وIO حتى نهاية كل lease؛ destructor لا يحرر ذاكرة قد يقرأها render thread.

التحقق المحلي: RED عند غياب helper ثم GREEN بعد التنفيذ، ASAN/UBSAN (LSAN معطل محليًا)، مع backend اصطناعي يؤخر قراءة الرؤوس إلى WaitFence. يفحص عدم تحريرها مبكرًا، رفض المشهد الملغى بين التخصيص والإرسال، الاستثناء وpending fence، backend آخر، وعدم تغيير State الأصلية. bridge الفعلي تُرجم syntax-only ضد SDK99078b119b5d5e0d1d6afb1390e29437c0c984c0. أضيف الاختباران إلى source preflight.

هذه ليست rasterization حقيقية: fixture لا يشغل GPU. كما أن bool حاجز المستدعي لا يثبت كاميرا BLACK. لم يُربط أي hook أو roster حي. التالي: إثبات مصدر view/projection الحالي على حد رسم العالم والتذكرة/المشهد/الإطار وأهداف اللون/العمق، ثم rasterization بلا أصول على backend المصرح به، ثم ربط حي. لا تجاوز بوابات renderer القديم أو إعداد actor كي يظهر الصندوق. يبقى فعل الضيف الأصلي والحالة المشتركة وانتقال القصة غير مكتملة.

حُفظت تعديلات Session102 المحلية القديمة في git stash قبل fast-forward؛ لا تطبق lab.py القديم على الرأس الحديث. أدوات الصوت المحلية التي لم تنشر بعد محفوظة أيضًا هناك، وتحتاج مقارنة مع تحقيقات Work الحالية قبل نقلها؛ ليست خطوة حرجة الآن ولا تصريحًا بتشغيل مسارات الأصول القديمة.

حُفظت أدوات الصوت المحلية السبعة في coop/archive/session102_audio_witness مع README أرشيفي، دون تفعيل أو تشغيل مساراتها؛ للمقارنة والاستعادة فقط، لا لاستبدال lab الحالي.
