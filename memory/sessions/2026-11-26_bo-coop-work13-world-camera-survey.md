# Work-Batch13 — تحقيق كاميرا العالم قيد العمل (2026-11-26)

عُثر على المنتج الأصلي2B830 الذي يرسل view/projection الفعليتين إلى XDK SetTransform0/1؛ SDK يحتفظ بهما في transforms2/3. أضيف hook مصدري للقراءة فقط بعد original مرة واحدة، مع entryESP+8 وbooleanEAX=1 موثّقين. يحفظ لقطة تحت d3d::Lock ويُبطل الكاميرا الثانوية/الفاشلة ويعيد فحص الهوية والمصفوفات والtargets عند القراءة. لا كتابة لعبة/State أو GPU submit أو أصل شُغّل. اختبار memory ASAN/UBSAN وSDK syntax-only وخمسة source-stage checks نجحت محليًا؛ CI الجديد غير متحقق بعد. لا signed-room authorization أو camera/runtime أو rasterization أو لعب مشترك مثبت. التالي ربط المستهلك بـstory scope وإطار العالم ثم rasterization بلا أصول قبل العرض. التفاصيل docs/ORIGINAL_WORLD_CAMERA_SOURCE_WORK13_AR.md والأداة/الأدلة محفوظة. لا AI/تعديل أصول/حفظ أو workflow ISO/APK يدوي أو اختبار هاتف جزئي.

---

