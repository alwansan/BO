# Work19 — مراقبة عمر تخزين الصوت في مختبر المحرك الأصلي (2026-11-26)

أضيفت أدوات AUDIO_LIFECYCLE_LAB.md وaudio_lab.py وaudio_observer.inc المفقودة التي أشارت إليها وثيقة الصوت. تلتف حول original7EF50/7EDB0 بعد generation باستدعاء الأصل مرة واحدة، وتراقب counts/thread overlap وعناوين تخزين خاصة ضمن ledger محدود، ثم تصدر عدادات allowlisted فقط. لا قراءة bytes كائنات ولا كتابة guest ولا constructors جديدة. retained_fpv_storage دليل انتماء عنوان فقط، لا ملكية أو publication أو restore integrity. audio_observer_executed منفصل عن نتيجة اللعب القديمة.

نجحت36 اختبارات Python وC schedule/observer وaudio ASAN/UBSAN ومطابقة anchors المصدر المثبت. سيعمل CI الأصلي على هذه الدفعة؛ لا runtime صوت مُدّعى قبل نتيجته. آخر مختبر PC45239818055 أثبت لاعبين بحركة وسلاح مستقلين، لكنه لم يراقب ملكية الصوت/allocator. على6d3b5e3 نجحت source45806572808، Android وحدات45806572911، Android diagnostics45806572806، TLS45806572833، parallel45806572877 وUDP45806572853. الدليل WORK_BATCH18_REVOCATION_DIAGNOSTICS_2026-11-26.json.

التالي متابعة مختبر المحرك الحالي، إصلاح compilation/runtime عند الحاجة وتحليل العدادات، ثم source-proven stationary audio/game-heap ownership provider. العالم المشترك وAPK النهائي ما زالا غير مكتملين. لا طلب اختبار هاتف جزئي.

