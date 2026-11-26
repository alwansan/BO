# نطاق لقطة أهداف المهمة — Chat-Batch04

الأساس: `alwansan/BO:experiment/black-coop` عند
`f7551ada9e7e352f937bd24e7b5ba405b947602d`، بعد Chat-Batch03 المحلي.
التاريخ: 2026-11-26. التغيير محلي وغير منشور.

## نتيجة البحث عن المصدر الحي

لم يعثر الفحص في المصادر المنشورة المقروءة على منتج موثق لتغيّر
`counter/state` للأهداف الحية أو دورة إنشاء/إزالة جدول descriptors.
هذا وصف لحدود الأدلة المتاحة، وليس حكمًا على كل نسخ اللعبة أو مصادرها.

مرجع PC مثبت عند `b2512fafd3e181e4febfb298f297245b845ef628`:

- [README](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/README.md)
  يستبعد أجسام دوال اللعبة المولّدة من المستودع المنشور.
- [secondary_objective_hooks وcheckpoint_hooks](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/scripts/generate.py)
  يحددان حدود قرار المتطلبات الثانوية وحد تهيئة checkpoint؛ لا يثبتان
  عملية تحديث عدّاد هدف حي أو تطبيق آثاره على عالم الضيف.
- [black_checkpoint.inc](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/src/black_checkpoint.inc)
  يجهز `objective_backup`. الفصل عن الأهداف الحية موثق في Batch03،
  ولم تُستعمل استعادة checkpoint للمزامنة.

وفي BO، [دليل أعمار تخصيص النظام](https://github.com/alwansan/BO/blob/f7551ada9e7e352f937bd24e7b5ba405b947602d/coop/prepare_original_system_lifetime.py)
يستبعد صراحةً إثبات عمر suballocation عشوائي في heap اللعبة.
[مصدر NPC](https://github.com/alwansan/BO/blob/f7551ada9e7e352f937bd24e7b5ba405b947602d/coop/room_network/room_original_host_npc_source_witness.hpp)
و[مراقب نهاية إطار العالم](https://github.com/alwansan/BO/blob/f7551ada9e7e352f937bd24e7b5ba405b947602d/coop/room_network/room_original_post_frame_witness.hpp)
لا يقدمان في المسار المفحوص منتج أهداف أو عمر جدولها أو تطبيقها الحي.
لم تُشغّل أدوات توليد المصدر أو المحرك أو ملفات اللعبة خلال هذا البحث.

## القرار والتنفيذ

تربط `room_original_objective_story_scope.hpp` سجلات القيم المكتملة من
Batch03 بهوية مهمة المود كاملة. تستخدم الحقول الفعلية في
[RoomStoryWire.Ticket](https://github.com/alwansan/BO/blob/f7551ada9e7e352f937bd24e7b5ba405b947602d/coop/lobby/java/com/racas/black/coop/RoomStoryWire.java)،
دون اختزالها إلى هوية stage أو مؤشر native.

| الحقل | عقد Ticket المستخدم |
| --- | --- |
| session | 16 بايتًا؛ الصفر مسموح في هذا العقد |
| token | موجب حتى INT64_MAX |
| rosterRevision | 1 إلى 0xffffffff |
| mission | 0 إلى 7 |
| stage | 0 إلى 65535 |
| sceneEpoch | موجب حتى INT32_MAX |
| participants | 3 إلى 15، مع وجود بت المضيف؛ يقبل masks غير متجاورة مثل 5 و9 و13 |

هذه حدود بيانات المود. لا تعني أن كل نطاق مقبول في Ticket تدعمه
شواهد المحرك الحالية؛ بعض الشواهد الأصلية أضيق. لا نضيّق عقد Ticket
ضمنيًا، ولا نمرر هذه القيمة مباشرة إلى تلك الشواهد.

`CaptureOriginalObjectiveStorySnapshot` يبني لقطة تملك scope وسجلاتها
باستخدام `EncodeOriginalObjectiveValues` الموجود.
`PrepareOriginalObjectiveStorySnapshot` يتحقق من صلاحية ومطابقة الحقول
السبعة مع نطاق حالي مستقل، ثم يستدعي `PrepareOriginalObjectiveValues`
مع جدول descriptors مستقل. ينشر قيم النتيجة ونطاقها بعد اكتمال التحقق.
رفض نطاق أو payload لا يغيّر الناتج السابق.

نطاق الناتج محفوظ معه كي يستطيع المستهلك إعادة مطابقته عند الاستعمال.
إذا تغيرت التذكرة بعد التحضير، يجب على المستهلك تجاهل النتيجة القديمة؛
هذه الوظائف لا تراقب التذكرة تلقائيًا ولا تمنح الناتج تصريح تطبيق دائمًا.
كلمة «بعد اكتمال التحقق» تصف سلوك الفشل، ولا تدّعي عملية atomic بين خيوط
متزامنة. يظل استدعاء الدوال وحماية البيانات مسؤولية المستهلك.

## التحقق القابل للإعادة

`test_room_original_objective_story_scope.cpp` يختبر خمس مجموعات:

1. نجاح نطاق مطابق، وملكية النسخ، وتمثيل تغير العدّاد.
2. تبديل حقل واحد في كل مرة من الحقول السبعة، مع payload سليم وعداد
   مختلف؛ يجب رفضه وإبقاء النتيجة السابقة كاملة.
3. الانتقال إلى تذكرة جديدة وقبول قيمها، ثم رفض لقطة التذكرة السابقة.
4. حدود Ticket الواسعة والقيم الخارجة عن العقد.
5. رفض سجلات أو descriptors غير متطابقة دون نشر جزء من النتيجة.

`check_objective_story_scope_parity.py` يقارن قواعد C++ مع constructor
و`same` في كود Java الحقيقي، ويقارن الاثنين بتوقعات مستقلة للحالات.
لا يكتفي باتفاق تنفيذين. يطلب Java 8 افتراضيًا عبر `--release 8`، ولا يحتاج
Android SDK أو اتصال شبكة. طول session ثابت في نوع C++ عند 16 بايتًا؛
ليس اختبارًا لإمكان تمثيل طول مختلف داخل هذا النوع.

أوامر الإعادة من الحزمة المرفقة:

```sh
python3 run_local_checks.py --source-root source --output scope-results --disable-leak-check --test test_room_original_objective_story_scope
python3 source/coop/room_network/check_objective_story_scope_parity.py --source-root source --output scope-parity
```

رفض المترجم المتاح هنا `--release 8` برسالة `release version 8 not supported`.
لهذه البيئة فقط يمكن إضافة `--java-current-runtime` صراحةً لتشغيل مقارنة
القيم على Java 21 المتاح. لا يوجد fallback ضمني، ويظل CI يطلب Java 8.
اختبار runtime المحلي لا يثبت توافق Java 8 أو بناء Android؛ حُفظ سجل
المحاولة الأولى منفصلًا عن نتيجة المقارنة على runtime الحالي.

تفاصيل التنفيذ الفعلي والنتائج وبصمات المصادر محفوظة في أدلة الدفعة.
تسجيل الخطوتين في workflow المحلي لا يعني نشره أو تشغيله على GitHub.

النتيجة المحلية: نجح اختبار C++ الجديد 1/1 بمجموعاته الخمس مع
ASan وUBSan؛ فحص التسرب معطل بسبب قيد ptrace المثبت سابقًا.
ونجحت مقارنة Java 21/C++ في 157/157 حالة: 64 للصلاحية و93 للمساواة.
هذه المقارنة scalar-only دون sanitizers، بينما اختبار الوحدة مستقل
بالـsanitizers. لم تُعد الاختبارات السبعة السابقة؛ حُفظت نتائجها بعد
التحقق من اعتمادياتها. لا نتيجة نجاح لهدف Java 8 في البيئة المحلية.

## الحدود والخطوة التالية

- المطابقة لا تتحقق من TLS أو المصادقة؛ نطاق المهمة الحالي يجب أن يأتي
  من مسار المضيف الموثوق المستقل، لا من اللقطة التي نتحقق منها.
- لا يوجد sequence أو clock في هذه الدفعة. يمكن تحضير لقطة أقدم داخل
  **نفس Ticket**؛ لا تسمّى هذه الوظيفة freshness أو ضمانًا للأحدث.
- جدول descriptors المتوقع يجب أن يأتي من مصدر مستقل متحقق. لا إثبات
  هنا لعمر جدول اللعبة أو دلالات حالات الأهداف أو آثار تطبيقها.
- لا نوع رسالة أو نسخة بروتوكول جديدة، ولا JNI أو native callback، ولا
  تعديل لقواعد إكمال المهمة أو ذكاء الأعداء والحلفاء أو ملفات الحفظ.

الخطوة التالية الممكنة في منطق المود: تصميم نقل محدود لسجلات الأهداف
الكاملة مع ترتيب خاص باللقطات وتجميع كامل قبل التحضير. يجب مراعاة
[RoomWire.MAX_FRAME](https://github.com/alwansan/BO/blob/f7551ada9e7e352f937bd24e7b5ba405b947602d/coop/lobby/java/com/racas/black/coop/RoomWire.java)
الحالي البالغ 512 بايتًا للإطار الكامل؛ 4096 سجلًا من 16 بايتًا لا تدخل
في إطار واحد. لا ترفع الحد ولا تختزل ID أو counter لتجاوز هذه الفجوة.
يبقى اختبار النقل بمنتج اصطناعي صريح منفصلًا عن ادعاء منتج أهداف حي.

الربط بالمحرك ينتظر دليل مصدر مستقل للالتقاط والعمر والتطبيق الأصلي.
لا اختبار هاتف صغير؛ تجربة قبول واحدة بهاتفين بعد اكتمال بناء Android
والانضمام والمزامنة والتحقق الآلي الممكن. لا دفع أو نشر في هذه الدفعة.
