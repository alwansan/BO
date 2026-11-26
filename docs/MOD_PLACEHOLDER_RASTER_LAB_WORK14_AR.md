# Work14 — نتيجة الرسم الفعلي محفوظة ومتحققة (2026-11-26)

نجح run [45788110496](https://github.com/alwansan/BO/actions/runs/45788110496) على الكود cbf0144cde648de4a2311a8e0ca57faa38fd80ab: SDK D3D12 الأصلي المثبت مع threaded backend وWARP الإلزامي؛ 11 مقارنة بكسل ناجحة، occlusion صحيح، المجسم لا يكتب العمق، و4 تخصيصات/4 تحريرات بعد fence. أُعيد التحقق محليًا من SHA256 للأرتيفاكت 15101237016: `51b5d33e71a13c97543d055c9ccc8421975694bbceabfb846411e2543b7c70fa`. الدليل: `memory/evidence/WORK_BATCH14_ACTUAL_SDK_RASTER_2026-11-26.json`؛ الأدوات: `coop/render_lab/` والـworkflow موجودان في GitHub.

نجحت اختبارات نفس الكود: source45788110359، parallel45788110387، privacy45788110494، UDP45788110385. هذا يغلق انتظار CI المذكور أدناه في Work13/14. نتيجة التحضير `rasterization_verified:false` داخل source manifest سبقت التنفيذ؛ runtime result هو دليل نجاح الاختبار المعزول فقط.

لا يثبت ذلك ظهور لاعب داخل BLACK أو Android/Vulkan أو عالمًا مشتركًا. التالي: ربط مجسم المود بكاميرا Work13 وبحالة القصة/الغرفة والمشهد والإطار الحقيقيين؛ ثم فعل الضيف في حالة الهوست الأصلية وتطبيقها لدى الضيف وانتقال القصة. لا تُفعّل مسار NPC القديم أو بوابات physical readiness بمجرد نجاح الرسم. لا AI/أصول/حفظ، ولا dispatch يدوي لـISO/APK أو تجربة هاتف صغيرة. يجب إكمال العوائق الأصلية قبل تجربة الهاتفين الثقيلة. تابع على experiment/black-coop فقط، وتحقق من الرأس قبل الكتابة.

---

# Work14 — اختبار rasterization فعلي بلا أصول: جاهز لـCI، النتيجة لم تثبت بعد

2026-11-26، يعتمد على مجسم Work11 ورفع Work12 ومصدر كاميرا Work13. لا أصل لعبة أو recompiler أو XBE/ISO أو APK أو تعديل AI/حفظ في هذا المسار.

`coop/render_lab/` يبني GPU backend الفعلي من SDK99078b119b5d5e0d1d6afb1390e29437c0c984c0، مع shaders/vertex/texture وrender_thread وallocator وImGui1.91.0 المثبت. يحتاج Windows2025 وclang-cl لأن SDK يستعمل GNU builtins/attributes؛ عاملان و20 دقيقة كحد أقصى.

الاختلاف الوحيد عن SDK الإنتاجي اختيار adapter: نسخة cpp مؤقتة خارج checkout تطلب EnumWarpAdapter وتتحقق من software flag، دون hardware fallback. يفحص prepare_sdk.py الـcommit وSHA256 وanchor المطابق، ويرفض الكتابة داخل SDK أو فوق نواتج موجودة. لا تُنسخ أو تعدل ترويسات طرف ثالث في BO. فحص التحضير المحلي نجح وتأكد بقاء SDK نظيفًا؛ هذا لا يثبت Windows build أو rasterization.

التنفيذ ينشئ HWND خفيًا، وذاكرة/أسطح لون وعمق مملوكة للاختبار، ثم يستخدم CreateThreadedBackend(CreateBackend()) الحقيقي وModPlaceholderUploadKernel الحقيقي. يفحص لون الصندوق والبكسلات الخارجية عبر Backend::Sync، وocclusion من clear depth قريب، وأن الصندوق نفسه لا يكتب العمق (صندوق أبعد يبقى ظاهرًا). لا يتوفر depth readback في Sync، فلا ندعي فحص dump للعمق. يفحص !Busy قبل Free وتحرير أزواج header/data وعدد تخصيصات allocator وبقاء State المدخلة دون تغيير.

Workflow الجديد `black-coop-mod-render-lab.yml` يعمل على source/SDK فقط؛ لا يُشغل مسارات ISO/APK اليدوية. artifacts سجلات الاختبار الاصطناعي وmetadata/licensing فقط، لا executable أو ملفات لعبة. إذا فشل compiler/Init/readback يجب إصلاح السبب قبل اعتبار المسار متحققًا؛ fixture أو source hash أو إنشاء run ليس نجاح rasterization.

حتى النجاح المستقبلي يخص WARP/D3D12 وSDKqueue، وليس Android/Vulkan أو كاميرا BLACK الحية أو رؤية بشر على هاتفين. المصوفات في هذا الاختبار اصطناعية مقصودة لاختبار المسار بلا أصول. الوصلة الحية التالية تحتاج signed story/scene/frame وأهداف الرسم الصحيحة من Work13، ثم consumer مرئي؛ فعل الضيف الأصلي والحالة المشتركة وانتقال المرحلة ما زالت مطلوبة للقبول النهائي.
