# Work14 — اختبار rasterization فعلي بلا أصول: جاهز لـCI، النتيجة لم تثبت بعد

2026-11-26، يعتمد على مجسم Work11 ورفع Work12 ومصدر كاميرا Work13. لا أصل لعبة أو recompiler أو XBE/ISO أو APK أو تعديل AI/حفظ في هذا المسار.

`coop/render_lab/` يبني GPU backend الفعلي من SDK99078b119b5d5e0d1d6afb1390e29437c0c984c0، مع shaders/vertex/texture وrender_thread وallocator وImGui1.91.0 المثبت. يحتاج Windows2025 وclang-cl لأن SDK يستعمل GNU builtins/attributes؛ عاملان و20 دقيقة كحد أقصى.

الاختلاف الوحيد عن SDK الإنتاجي اختيار adapter: نسخة cpp مؤقتة خارج checkout تطلب EnumWarpAdapter وتتحقق من software flag، دون hardware fallback. يفحص prepare_sdk.py الـcommit وSHA256 وanchor المطابق، ويرفض الكتابة داخل SDK أو فوق نواتج موجودة. لا تُنسخ أو تعدل ترويسات طرف ثالث في BO. فحص التحضير المحلي نجح وتأكد بقاء SDK نظيفًا؛ هذا لا يثبت Windows build أو rasterization.

التنفيذ ينشئ HWND خفيًا، وذاكرة/أسطح لون وعمق مملوكة للاختبار، ثم يستخدم CreateThreadedBackend(CreateBackend()) الحقيقي وModPlaceholderUploadKernel الحقيقي. يفحص لون الصندوق والبكسلات الخارجية عبر Backend::Sync، وocclusion من clear depth قريب، وأن الصندوق نفسه لا يكتب العمق (صندوق أبعد يبقى ظاهرًا). لا يتوفر depth readback في Sync، فلا ندعي فحص dump للعمق. يفحص !Busy قبل Free وتحرير أزواج header/data وعدد تخصيصات allocator وبقاء State المدخلة دون تغيير.

Workflow الجديد `black-coop-mod-render-lab.yml` يعمل على source/SDK فقط؛ لا يُشغل مسارات ISO/APK اليدوية. artifacts سجلات الاختبار الاصطناعي وmetadata/licensing فقط، لا executable أو ملفات لعبة. إذا فشل compiler/Init/readback يجب إصلاح السبب قبل اعتبار المسار متحققًا؛ fixture أو source hash أو إنشاء run ليس نجاح rasterization.

حتى النجاح المستقبلي يخص WARP/D3D12 وSDKqueue، وليس Android/Vulkan أو كاميرا BLACK الحية أو رؤية بشر على هاتفين. المصوفات في هذا الاختبار اصطناعية مقصودة لاختبار المسار بلا أصول. الوصلة الحية التالية تحتاج signed story/scene/frame وأهداف الرسم الصحيحة من Work13، ثم consumer مرئي؛ فعل الضيف الأصلي والحالة المشتركة وانتقال المرحلة ما زالت مطلوبة للقبول النهائي.
