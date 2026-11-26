# Work18 — استكمال مراجعة العالم وإصلاح إبطال العرض (2026-11-26)

الأساس 7acb810. أعاد المالك تأكيد الهدف: مرحلة أولى مشتركة بأجسام مرئية وتفاعل الهوست والضيوف ثم APK قبول متكامل وسجل موحد. أجاز وكلاء متوازيين وأي مجسم مناسب. Start الحالي يحمّل المرحلة على الهاتفين وفق تقريره.

أُصلح رسم packet قديمة بعد تغير roster/ABORT أثناء تخصيص GPU بإعادة قراءة facade القصة قبل التخصيص وقبل Draw ومطابقة التذكرة الكاملة وtick. regression فشل على الأساس exit134 ونجح مع ASAN/UBSAN على الإصلاح. لا يمتد قفل Java عبر GPU enqueue، لذلك قد تبقى رسوم أُجيزت قبل إلغاء متزامن بعد آخر فحص. نجحت اختبارات تجهيز المصدر الستة وmetadata واختبارات JVM/TLS المحلية. Android source units السابقة45804212528 نجحت للوحدات السبع على الأساس، وCI الجديد للإصلاح ينتظر.

أضيف schema/package/version/SHA256 للـbase APK إلى التصدير التشخيصي الصريح مع regression Android instrumentation. نجح strict Java8 compile، runtime instrumentation الجديد ينتظر CI. لا معرف جهاز أو شبكة داخل metadata.

التحقيق docs/SHARED_WORLD_PROVIDER_INVESTIGATION_WORK18_AR.md يحدد غياب producer أصلي لأحداث الأشياء وresolver/apply/verify حي لدى الضيف. استقلال actor الفيزيائي يحتاج أعمار heap الأصلية وpresentation/audio stationary ownership. لا تفعيل physical readiness أو AI أو تعديل أصول/حفظ. العالم المشترك وAPK القبول لم يكتملَا. تابع أدلة مختبر المحرك الأصلية للحصول على allocator/audio evidence ثم الربط الحي. لا اختبارات هاتف جزئية.

