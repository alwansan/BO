# Work15 — مصدر عرض ذري مرتبط بتذكرة القصة والمشهد الأصلي (2026-11-26)

أضيف `RoomSession.otherHumanPresentationFromGameThread()` وواجهة RoomAppRooms/BlackActivity المجهزة للمصدر. يجمع الإصدار1، originalEpisode، level/stage، epoch/revision، token64، participants/self، tick/count وsession128 مع مواضع البشر الأصلية المصدر/المعتمدة من الهوست، تحت قفل RoomSession واحد. يعيد صفراً عند عدم COMMIT أو اختلاف التذكرة/المشهد أو صلاحية خيط الأصل أو فقدان العضو. يسجل episode وقت attestation الأصلية، ويرفض تحميلًا جديدًا لنفس المرحلة إذا حاول استعارة readiness قديمة. يعيد فحص التذكرة وCOMMIT بعد قراءة المصدر القائم؛ لا بروتوكول شبكة جديد أو إدخال يدوي للمستخدم.

نجح محليًا Java8 core وJava17 tests بالتحذيرات كأخطاء؛ اختبار TLS الحقيقي بأربعة أطراف (مدخلات محرك اصطناعية) تحقق من header/session/nonce والأدوار والنسخ المستقلة، رفض خيط آخر، إبطال إعادة تحميل المرحلة ذاتها، ABORT/rejoin وعدم العرض قبل READY الجديدة. نجحت اختبارات source staging الخمسة مع BO_UPSTREAM_REFERENCE المثبت. إضافة الاختبار بدأت بفشل compile لغياب الواجهة ثم نجحت بعد التنفيذ. نجح CI على الكود9448a73: source45799322972 وrealTLS45799322977 وAndroid Keystore الاصطناعي45799323195 وUDP45799323049. الأدلة memory/evidence/WORK_BATCH15_SCOPED_PRESENTATION_2026-11-26.json؛ لا لعبة أصلية أو عرض حي في هذه الاختبارات.

العرض الحي لم يُربط بهذا المصدر بعد. التالي كتابة consumer أصلي يطابق originalEpisode/world/frame ومصدر كاميرا Work13 مع هذه الحزمة، ثم يرفع مجسم المود عبر المسار Work12 المثبت في WARP/Work14. لا تضبط بوابات physical readiness على true؛ لا تعديل AI/أصول/حفظ أو dispatch يدوي ISO/APK. العالم المشترك والأفعال الأصلية وانتقال القصة ما زالت مطلوبة قبل تجربة الهاتفين الثقيلة. أدلة الرسم الفعلي صارت محفوظة في memory/evidence/WORK_BATCH14_ACTUAL_SDK_RASTER_2026-11-26.json. كل التغييرات على experiment/black-coop فقط.

---

