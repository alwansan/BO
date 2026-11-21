# ACTOR-001A — تدقيق نقاط رصد عمر الشخصية في BLACK Android

المرجع `RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806` (tag v1.5.0، **قراءة فقط**)، وملفات BO `coop/probe/probe.cpp` و`coop/probe/probe_core.hpp` على فرع `experiment/black-coop`. هذا **قرار هندسي استكشافي**، وليس دليلاً على spawn أو destructor. لا تنسخ offsets إلى أي مصدر مختلف الإصدار دون فحوص المطابقة.

## المرشحات المتحققة من موضعها ومعناها التقريبي
| مكان المصدر | ما يدعمه المصدر | ما لا يثبته |
|---|---|---|
| `src/game/cheats.cpp`، `sub_001E74B0` | hook يُستدعى عند تلقي **جندي** طلقة، `ctx.ecx` ضحية و`esp+0x18` مهاجم؛ `kCharDying=0x384` حالة قتل يستعلم عنها المصدر | ليس مخصصاً لكل موت، ولا destructor، ولا يثبت من أنشأ الجندي أو متى أزيل |
| `src/game/cheats.cpp`، `sub_001E5FA0` | تحديث اللاعب المحلي ومقارنة world+0x30 وkind=2 | ليس مؤشراً لكل الشخصيات ولا يدل على وجود player2 |
| `src/game/hooks.cpp`، `sub_0017B130` | animation controller owner+0 وclip+0x54 وname+0x5C، يُستعمل لتعديل تدفق الأنيميشن المحلي | ليس factory ولا ضمان owner lifetime |
| `src/game/hooks.cpp`، `CharacterDetail` | LOD/distance ضمن draw-related hooks | اختفاء callback قد يعني البعد أو budget؛ ليس despawn |
| `coop/probe/probe.cpp`، `sub_001C55A0` | مراقبة LOD تقرأ pointer/vtable/position/resource320/render35c، وتستدعي الدالة الأصلية بعد المراقبة | لا تثبت أن render35c هو model-instance (من Trace Probe2 يبدو مشتركاً) |
| `coop/probe/probe_core.hpp`، `Registry.World()` | `epoch++` عندما **يتغير عنوان world**، وslot serial يتغير عند assignment مختلف | تغيير المهمة مع إعادة استعمال نفس عنوان world قد لا يُرصد؛ slot serial ليس generation allocator حقيقياً |

## تصميم Probe3 المسموح فقط بعد الإثبات
1. قبل أي hook جديد، افحص صورة XBE المرشحة `c5bfed...` وتوقيع الوظيفة والـABI و`__imp__` في generated code. لا تغيّر الأصل أو تعطل image hash gate. يحظر اعتماد `0x1E74B0` باعتباره destructor.
2. يمكن **تصميم** مراقبة opt-in قليلة البيانات لحالة `dying` قبل/بعد `sub_001E74B0` مع نداء الدالة الأصلية مرة واحدة، وقراءة الآثار بآلية `GuestReader` المحمية، دون كتابة الضيف أو حفظ عناوينه في التقرير العام. لا يجري البناء/الحقن قبل اختبار حدود الإدخال وخيط التشغيل وبقاء الحالة.
3. لتأكيد لحظة التحرير الفعلية، لا بد من **دليل factory/free منفصل**. انتقال dying من صفر إلى غير صفر لا يثبت حذف actor، وغياب عينات LOD لا يدل على التحرير.
4. ضع marker للمشهد يتجاوز مجرد `world` address عند فهم semantics مصدر scene transition (قد يبقى world على العنوان نفسه). يجب ألا يستبدل هذا جيل تخصيص actor المثبت.
5. تأكد من عدم مساس اللاعب المحلي، الحفظ، الكاميرا، أو نشاط اللعب الفردي. كل عرض على الهاتف بحزمة تجريبية مستقلة عن Probe2، وتصدير sanitised فقط عند الحاجة.
6. تحاليل `coop/analysis/actor_lifecycle_triage.py` ومخزن `RemoteProxyRoster` تعالجان السجلات والحالة التجريبية ولا تحلان محل رصد allocator/death.

**النتيجة الحالية:** مرشح damage/animation/LOD قائم، **لا توجد بعد نقطة إنشاء/تحرير NPC مثبتة**؛ لذلك تظل ACTOR-001 غير مقبولة كرسم شخصيات أو multiplayer.
