# جلسة25 — تنفيذ رصد روابط التحريك وتجهيز Probe4

خطة العمل docs/superpowers/plans/2026-11-21-probe3-evidence-and-instance.md؛ دليل الشفرة المطابقة docs/ACTOR_INSTANCE_OWNERSHIP_AR.md. محلل Probe3 نجح في CI44875336266 علىf0a66a8dd74f2e610be7dc914f7a301d84364c8f. السجل الحقيقي مقبول كـ56 لقطة pool صالحة، وليس طلباً لإعادة اختبار Probe3.

أضيف coop/probe4/instance_graph.hpp. ReadInstance يتحقق من vtable المحدد ومن serial/container/state0 للشخصية الحية، ويقرأ backlink مرتين ثم يعيد فحص هوية الشخصية والحقول المختارة. ثماني نسخ kernel كحد أقصى، دون dereference أو guest calls/writes؛أي فشل يترك output كما هو. LOCAL له vtable مثبت مستقل ولا يفرض عضوية pool. رصد موارد runtime54/binding50/scratch34C/350 هو رصد قيم pointers فقط؛لا يثبت readable allocation أو uniqueness.

sampler في Probe4 يراقب حتى4 NPC أحياء بالتناوب بالإضافة للاعب المحلي كل ثانية. مجمل kernel copies أقل من300 (core pool212 + global fences4 + local serial1 + graph5*8=257). world/manager/reset fences بعد رصد كل الروابط؛لا ادعاء atomicity/ABA. JSON pool schema السابق لا يتغير، ويضاف instance_schema1 وinstances؛tuple موثق في docs/PROBE4_DEVICE_TEST_AR.md. فشل الرابط لا يفسر موتاً ولا يحول بيانات غير مثبتة إلى actor handle صالح.

prepare_probe4.py يعمل على stage مبني من prepare_probe3 ويخرج15 ملفاً فقط، packageorg.bocoop.black.probe4 وlabelProbe4؛ملفات Probe3 وworkflow الخاصة به لم تتغير. workflow منفصل black-coop-probe4-apk.yml يبدأ LinuxASAN/UBSAN core/observer tests قبل jobWindows/NDK/APK. يحتفظ بفحص c5/generated-reset/package/signature/JNI/no-network/assets exclusions.

اختبارات TDD: stub ReadInstance فشل قبول الرسم كما يجب ثمstrictClangC++20 PASS لروابط owner وتغيير serial أثناء القراءة وwrong class/container/overflow/local distinction وعدم تلويث output. actual observer harness PASS وربط NPC/local بالموارد الصحيحة وJSON/rate/errno. preparers9/9 PASS؛YAML وembeddedPython syntax PASS. pool_core_review لم يجد عيباً مهماً، ونبّه لحدود valid الموثقة أعلاه. CI/APK لم يثبتا بعد لهذه الدفعة؛تابع run الجديد بعد push ولا تطلب الهاتف قبل نجاحه.
