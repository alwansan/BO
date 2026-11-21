# ACTOR-001A — فهم دورة حياة الجندي وربط حالة لاعب بعيد بأمان
**الحالة: تحقيق قراءة فقط + مختبر C++ مستقل، ليس لاعباً ثانياً داخل اللعبة.**

## ما ثبت من الهاتف
المالك شغل Probe2 واستخرج `coop_probe.txt` إلى Downloads. 400 عينة فعلياً: player131، LOD172، animation97، epoch واحدة فقط، 13 correlations من LOD+animation، 1 من player+animation. الأدلة المجمّعة في `memory/evidence/ACTOR001A-coverage-owner-Probe2-20261121.json` و`docs/ACTOR_DEVICE_TRACE_FINDINGS_AR.md`؛ لا نعيد نسخ السجل العام أو مؤشرات الذاكرة هنا.

تحليل ACTOR-001A: من الـ13 مجموعة LOD، **9** لم تظهر في أول عشر ثوانٍ من نافذة **العينات** و**2** لديها انقطاع رصد 10 ثوانٍ أو أكثر تلاه ظهور جديد في **عينات الرصد**، ولا توجد مجموعة ينتهي رصدها قبل نهاية جميع العينات بعشر ثوانٍ. **لا يثبت** هذا spawn أو موت أو despawn أو إعادة استعمال الـheap؛ لأن sampler يدور بين IDs ويأخذ عينات محدودًا ويتوقف قبل انتهاء اللعبة.

## ماذا جهزنا في الكود؟
- `coop/analysis/actor_lifecycle_triage.py`: يقرأ سجل المستخدم محلياً فقط، يعطي إحصاءات الفجوات/المشاهد/الأعداد دون IDs/إحداثيات/pointers أو أسماء أو توقيتات خام. حقول `npc_lifetime_proven` و`actor_factory_proven` تظل false. CI اصطناعي لا يستعمل ملفات مالك.
- `coop/bridge/remote_proxy_roster.hpp`: 3 slots قيمة فقط للاعبين 2–4، scene epoch وgeneration وmonotonic snapshot tick وstale timeout وحماية خيط اللاعب المضيف. مصدر snapshot يجب أن يمر مصادقة جلسة حقيقية **غير موجودة بعد**؛ لا تقبل النقل التجريبي على WAN. لا تعادل IDs عنوان actor، ولا يوجد إنشاء/حذف لشخصيات اللعبة.
- الاختبار المركب `coop/bridge/test_transport_pipeline.cpp` يمرر Snapshots مختبر الشبكة إلى proxy roster، ومدخلات إلى `SceneInbox` قبل محاكاة مشهد زائف. اختبار على host فقط، لا هاتفين ولا gameplay.

## بوابات قبل player2 الفعلي
1. رصد موثّق لانتقال مشهد أو موت/عودة جندي مع حدث مراقبة صريح لدورة الحياة (وليس sample silence)، والتحقق من خيط التحديث وpointer reuse/generation.
2. تحقق من هوية وملكية مورد render الحقيقي أو NPC body instance عبر قراءات محدودة؛ النتيجة السابقة `render35c` مشتركة غالباً في13 مجموعة، لذلك لا تستعمله كـactor handle.
3. تحديد factory أو allocator/destructor صحيح ومناسب لإصدار c5bfed...، مع تجارب معزولة ومراجعة offsets/ABI، دون تعديل الأصل والحفظ أو إلغاء gating.
4. ربط `RemoteProxyRoster` بمشهد اللعبة فقط بعد وجود identity+ownership وضمان تطبيق على game thread. ثم نموذج visual proxy، وبعدها حركة/animation/sound/weapon/combat/mission/checkpoint sync.
5. الشبكة الإنتاجية تحتاج مصادقة وتشفير وتحكم انقطاع وحدود وإعادة اتصال؛ toy Host/UDP الحالي ليس آمناً للنشر.
6. واجهة CO-OP أسفل الإعدادات في القائمة الرئيسية ما زالت في مرحلة خطة `docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md`؛ لا تستدعِ HOST/JOIN قبل backend الحقيقي.

**لا تكرر تجربة الـBaseline/Probe2 فقط لمجرد التصدير.** إن احتجنا إلى رصد دورة الحياة لاحقاً، سننتج Probe معزولة فقط بعد تحديد hooks آمنة ومثبتة وبناء ناجح.
