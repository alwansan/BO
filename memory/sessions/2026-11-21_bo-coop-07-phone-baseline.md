# جلسة07 — إثبات صورة الهاتف وبناء أساس مستقل

## جلسة07 — وصول تجربة الهاتف الفعلية وبناء candidate منفصل
المالك أكد أن النسخة العربية تدخل القصة وتعمل دون مشاكل، ونسخ بيانات التطبيق محلياً. قرئ السجل الخاص دون نشره: build f3754d6، API35، Adreno730، loaded XBE c5bfed... مع another release، Exit0 بعد146s. تأكد SHA256 لنسخة ISO من الهاتف=f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62 مطابق للـmanifest. هذا يحسم أن الصورة المرفقة هي المستخدمة فعلاً؛ لا تعاود طلب تجربة الأصل أو XBE المفقود. نجاح المهمة من إفادة المالك، لا من instrumentation داخل log.
السجل يحوي `quit: ConfirmWarning, FEAudio failed` في مسار exit_menu.cpp؛ سُجل ولم يخفَ، ولا نفترض منه عطل قتال/صوت أثناء المهمة. دليل metadata فقط DEVICE-existing-owner-runtime.json؛ لا raw log أو حفظ في GitHub.

بدأ مسار بناء أساس مرشح مستقل من نفس c5bfed image الفعلي بالمصدر/SDK المثبتين. ليس إعادة إنتاج ثنائية للمصدر العربي ولا قبول ABI كامل؛ compile image سيتولد تلقائياً من المدخل الحقيقي دون تعطيل match. الحزمة org.bocoop.black.baseline، Java namespace/JNI الأصلي محفوظ، native/src/config لم تُعدل، حفظ منفصل، label NO CO-OP، updater محجوب في هذه الحزمة. لا أصول لعبة في APK. هذا اختبار baseline يلزم قبل HOOK/ACTOR، لا استبدال لهدف co-op.
خطة التنفيذ docs/superpowers/plans/2026-11-21-candidate-baseline.md؛ الاختبارات المحلية4/4 PASS (بدأ test preparer قبل وجوده ثم نجح بعد التنفيذ)، YAML/Python syntax وdiff check PASS. CI baseline مجهز ولم تثبت نتيجته بعد.

## مراجعة مستقلة قبل CI
راجع baseline_review النطاق كله. أصلح خطر import إلى Black المشترك: importer وجهته assets الخاصة، أزيلت صلاحيات MANAGE/READ/WRITE_EXTERNAL_STORAGE من manifest، وحوار البداية صار اختيار ISO دون طلب صلاحية واسعة. اختبار manifest فشل قبل الإصلاح ونجح بعده، suite4/4. أزيل upload metadata always؛ الآن لا APK أو metadata artifacts إلا بعد بوابات النجاح. لم تبق findings Important معلقة.
