# قيم تقدم أهداف المهمة — Chat-Batch03

التاريخ: 2026-11-26. الأساس البعيد `alwansan/BO:experiment/black-coop`
عند `f7551ada9e7e352f937bd24e7b5ba405b947602d`.
هذه دفعة محلية غير مدفوعة. لا بناء Android أو تنفيذ محرك أو حفظ لعبة.

## الدليل الذي بني عليه التغيير

المرجع المرخص MIT هو
[portingpete/Black-2006-Recomp](https://github.com/portingpete/Black-2006-Recomp/tree/b2512fafd3e181e4febfb298f297245b845ef628)
عند `b2512fafd3e181e4febfb298f297245b845ef628`.
قرئ [LICENSE](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/LICENSE)
وأرفق نصه مع إشعاري portingpete وsp00nz في
`coop/room_network/room_original_objective_snapshot.LICENSE`.
ترخيص المصدر لا يمنح ترخيصًا لأصول اللعبة.

في [black_checkpoint.inc](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/src/black_checkpoint.inc)،
يحفظ `BlackCheckpoint.objective_data` سجلات ذات 16 بايتًا:

| الجزء | الشكل المثبت في المرجع |
| --- | --- |
| descriptor ID | ثمانية بايتات من descriptor نفسه، دون مؤشر تخصيص |
| counter | قيمة مستقلة بعرض 32 بت |
| state | قيمة مستقلة بعرض 32 بت؛ لا معنى أصلي موثق لكل رقم |

`black_cp_save` يقرأ هذه القيم من جدول backup. `black_cp_valid` يطابق
العدد والترتيب ومعرّف كل descriptor الحالي، ويقبل counter حتى 65535
وstate حتى 3، مع حد 4096 سجلًا. هذه حدود قبول مرجع checkpoint، وليست
إثباتًا لحدود Android أو لكل انتقال حي ممكن في الهدف.

`black_cp_load` يجهّز جدول backup مع استخدام descriptor الحالي.
اختبار [test_checkpoint.c](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/tests/checkpoints/test_checkpoint.c)
يتحقق صراحةً من أن live objectives تنتظر الاستعادة الأصلية.
قرئ هذا الاختبار ولم يشغّل في الدفعة؛ لا يُستعمل `black_cp_load` كتطبيق حي.

## الفجوة في حدث BO الحالي

[RoomObjectEventWire](https://github.com/alwansan/BO/blob/f7551ada9e7e352f937bd24e7b5ba405b947602d/coop/lobby/java/com/racas/black/coop/RoomObjectEventWire.java)
ينقل objectKey وkind وstate، دون counter. ويشترط objectKey موجبًا
ضمن signed 64-bit. لذلك لا يمثل تغير counter وحده، ولا يوجد دليل يجيز
قص أعلى بت من descriptor ID كي يناسب هذا المفتاح.

لم يتغير RoomObjectEventWire أو رقم البروتوكول في هذه الدفعة. ولم تُستخدم
أرقام أحداث checkpoint بوصفها أرقام DESTROYED/ACTIVATED/OBJECTIVE؛ لا
mapping مثبت بين المجموعتين. كذلك `black_secondary_requirement_met`
في المرجع تعديل لاشتراط الأهداف الثانوية، وليس مصدرًا لحالات المزامنة.

## التنفيذ المحلي

`room_original_objective_snapshot.hpp` يقدم وظيفتين على قيم منسوخة فقط:

- `EncodeOriginalObjectiveValues`: يحفظ ID كبايتاته الثمانية دون تعديل،
  ثم counter وstate بصيغة LE32 صريحة، دون الاعتماد على padding أو ترتيب
  بايتات الجهاز المضيف. يقبل تمثيل ID صفري أو مكرر أو بأعلى بت مضبوط.
- `PrepareOriginalObjectiveValues`: يتحقق من طول السجلات وحدود المرجع
  ويطابق العدد والترتيب وكل ID مع جدول descriptors متوقع مستقل. يرفض
  المعرفات المتكررة كسياسة تعاون إضافية محافظة، ويجهز vector مستقلًا.

المخرجات تستبدل باستخدام swap بعد اكتمال التحقق. يبقى الناتج السابق
دون تغيير عند رفض البيانات أو استثناء تخصيص الذاكرة. الحالة الفارغة
مع جدول متوقع فارغ مسموحة كما في المرجع.

رفض التكرار لا يثبت أن IDs الأصلية فريدة. والتطابق لا يثبت التذكرة أو
المهمة أو عمر كائن native. عند الربط مستقبلًا يجب أخذ expectedIds من
مصدر محلي متحقق مستقل، لا اشتقاقها من البايتات الواردة نفسها.
هذه صيغة سجلات مستقلة؛ ليست ملف checkpoint كاملًا أو رسالة شبكة معتمدة.

## التحقق المنفذ

نجح `test_room_original_objective_snapshot.cpp` محليًا مع C++20 وg++ 14.2.0
والتحذيرات الصارمة وAddressSanitizer وUBSan. فحص التسرب معطل فقط بسبب
قيد ptrace المثبت في Batch01. السيناريوهات الستة:

1. Golden bytes مستقلة مع ID كامل يشمل البت الأعلى، وLE32 للعداد والحالة.
2. تغير counter من 2 إلى 3 مع بقاء state=1 والـID دون تغيير.
3. ثلاث نسخ قيم مستقلة تبقى صحيحة بعد تفريغ مخازن المدخلات.
4. رفض اختلاف ID أو الترتيب أو العدد، وتمثيل التكرار ثم رفض تحضيره الغامض.
5. رفض تجاوز counter/state والطول غير الصحيح، مع بقاء الناتج السابق حتى عند خطأ السجل الأخير.
6. الفراغ وID الصفري والحد 4096 وقيم 65535/3، ورفض 4097.

أضيفت خطوة محلية في `black-coop-room-network-preflight.yml` لتجميع الاختبار
وتشغيله عند تشغيل CI لاحقًا. تحققت صياغة YAML ووجود الخطوة؛ لم تُدفع
الملفات ولم يُشغّل workflow. الاختبارات الستة السابقة وملفاتها لم تتغير،
فحُفظت أدلتها السابقة دون إعادة تشغيل غير لازمة.

## ما يبقى قبل أي ربط بالمحرك

لا يوجد في هذه الدفعة منتج لقيم objectives الحية، أو إثبات Android لعمر
جدول descriptors، أو هوية spawn للأبواب/NPC، أو callback يطبق انتقال
هدف مع آثاره الأصلية. لم تنفذ أي قراءة/كتابة لذاكرة اللعبة أو ملفات
checkpoint أو الحفظ أو المخزون، ولم يتغير ذكاء الأعداء والحلفاء.

الخطوة التالية: تحديد مصدر موثق لالتقاط counter/state عند تغيرهما في
المهمة الحية، وإثبات هوية جدول الأهداف ودورة حياته. بعد ذلك يمكن تصميم
نقل يحفظ القيم كاملة، مع ربط صريح بتذكرة المهمة، ثم adapter أصلي مثبت.
لا تُخفى الفجوة بتمرير counter داخل state أو باستدعاء استعادة checkpoint.
لا اختبار هاتف صغير؛ تجربة واحدة بهاتفين بعد اكتمال البناء والانضمام
والمزامنة والتحقق الآلي الممكن.
