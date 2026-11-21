# HOOK-001 — تجربة الرصد الاختياري للنسخة المنفصلة

**ليس لعبة جماعية.** هذا اختبار قراءة ملاحظات محدودة فقط، ولا يثبت وجود full-body أو factory أو إنشاء لاعب ثانٍ. لا يُطلب اختبار جديد للـBaseline؛ ذلك الاختبار مقبول سابقاً.

## بوابات قبل إرسال APK إلى المالك
1. [Windows Probe CI](https://github.com/alwansan/BO/actions/runs/44852419929) انتهى SUCCESS، لا يكفي مرور خطوات Linux فقط.
2. تحقق `package=org.bocoop.black.probe1`، launch Activity = `com.racas.black.BlackActivity`، label `BLACK Probe 1 - READ ONLY`.
3. تحقق توقيع Android Debug مستقل، ABI ARM64 فقط، غياب `MANAGE_EXTERNAL_STORAGE/READ_EXTERNAL_STORAGE/WRITE_EXTERNAL_STORAGE/INTERNET/ACCESS_NETWORK_STATE`.
4. تحقق `kImageSha1` المبنية من c5bfed25449c8212d7a976f15246e87ead57713f ووجود JNI `Java_com_racas_black_BlackActivity_nativeCoopProbeEnable`. وجود generated `__imp__sub_001C55A0` مبرهن في المصدر الخاص أثناء CI قبل ربط المكتبة، والـlink لا يسكت أخطاءه.
5. تحقق من `probe-metadata.json` وSHA256 الـAPK النهائي ومن عدم وجود ISO/XBE/generated source في الـAPK/artifact.
6. عند تعطل أي بوابة لا تنشر APK أو تطلب اختبار هاتف؛ أصلح على `experiment/black-coop` وكرر CI.

## تشغيل النسخة بعد قبول البناء
- تثبيت `org.bocoop.black.probe1` لا يستبدل الحزمة العربية ولا نسخة `org.bocoop.black.baseline` التي اختبرها المالك.
- ستعرض استيراد `game.iso` إلى مساحة التطبيق المستقلة عبر منتقي ملفات Android؛ لا تحتاج صلاحية جميع الملفات ولا مسح حفظ التطبيقات الأخرى.
- بعد اكتمال الاستيراد أو عند التشغيل اللاحق، نافذة اختيارية `Start capture` أو `Play without capture`. الافتراضي هو عدم التسجيل؛ لا يبدأ الرصد بدون اختيار المستخدم.
- اختيار `Start capture` يبدأ رصداً محلياً مرة واحدة خلال العملية؛ لا يتصل بأي جهاز أو سيرفر. يتوقف بعد 180 ثانية من أول world صالح، أو 2048 record أو 1 MiB أو عند تعذر القراءة أو انقضاء انتظار world (300 ثانية).
- خلال المهمة، يمكن اختبار حركة واحدة، النظر إلى جندي قريب، إطلاق النار/التحرك، الخروج، ثم إعادة تحميل مشهد (فقط إذا أردنا ملاحظة world epoch). **هذا اختبار الرصد الجديد فقط وليس إعادة اختبار baseline.**
- المخرجات الخاصة: `Android/data/org.bocoop.black.probe1/files/logs/coop_probe.jsonl`، إلى جانب سجل اللعبة العادي. لا تُرفع سجلات الـISO/الحفظ/الـJSONL الخام إلى مستودع GitHub عام؛ إذا احتاج العميل الاطلاع عليها، تُرفق بالمحادثة بشكل خاص.
- `event:start` يثبت بداية الـsampler فقط؛ `event:player/lod/animation` تحتوي ملاحظات محدودة مؤهلة لتقييم مصدر مجسمات الشخصيات؛ `event:end` مع السبب والسجلات والقراءات الفاشلة. غياب LOD/animation لا يعني عدم وجود مجسمات، وقد ينتج عن rate limits أو reject/vtable/reader. `id` هو correlation ضمن world epoch لا هوية actor دائمة.
- إذا لم تظهر النافذة رغم تشغيل اللعبة، أو سجّل `world_read_unavailable`، أو فشل app native load، لا تتجاوز قيود Android ولا تحاول root؛ نراجع الـlog ونصلح fail-closed.
- نتيجة الهاتف هي الشرط الوحيد لقبول HOOK-001 runtime، ونجاح CI لا يكفي لإثبات ذلك.

## ما بعد الرصد
- تصنيف البنى المرصودة ومقارنتها بمصدر v1.5.0؛ لا تفترض أن animation owner = character أو أن resource root هو model/factory.
- ACTOR-001 بعد إثبات نوع الشخصية وعمرها وthread context؛ لا network thread guest writes، لا تعديل AI/health/saves قبل البرهان.
- الهدف النهائي وفق docs/COOP_REQUIREMENTS_AR.md هو لاعبان ثم أربعة في المهمة نفسها، لا UI وهمية أو صوت/فيديو متزامن فقط.
