# مرشحات فحص الشخصية — معلومات ساكنة، ليست spawn منفذة

المرجع Android v1.5.0 f3754d6985a8c35204e7df071546e586b10f1806 وXBE المالك c5bfed25449c8212d7a976f15246e87ead57713f. تحقيق read-only مستقل أثناء بناء baseline؛ لم يعدل المستودع أو الأصول، واستخرج مشتقات مؤقتة صغيرة ثم حذفها. ثُبتت هنا مراجع المصدر؛ بقية دلالات XBE مرشحات تتطلب تحققاً مستقلاً قبل الاستخدام.

| الموقع | الملاحظة والدلالة المحدودة |
|---|---|
| config/game.toml hook0x1C5655، CharacterDetail في hooks.cpp | Hook قائم لتعديل مسافة التفاصيل، وليس observer خالصاً: يعدل xmm1 بحسب draw_distance. دالة XBE المرشحة0x1C55A0 تقرأ object+0xA0 وتحسب مسافة وتكتب+0xC0/+0xC8؛ مرشح ارتباط render/LOD، لا إثبات نوع CPlayer أو مجسم كامل. |
| object+0x320 ثم descriptor | تحليل XBE يرجح مورد LOD/model: descriptor byte+0x6B وحدود مسافة من+0x24. لم يثبت skeleton أو allocator/factory أو ownership/lifetime. |
| object+0x35C ودالة0x1C5490 | مرور على count+0x24 وجداول+0x1C/+0x20/+0x38/+0x40 وإرسال عبر0x15A820 إلى draw lists؛ دلالة render مرجحة فقط. |
| world+0x4960، callback0x1C48D0 | تحديث اللاعب0x1E6076–0x1E6089 يمرر البنية/callback/player إلى0xCC1F0. callback يقرأ node+0x34 ثم object ويقارن مواضع ويخزن مرشحاً في0x2F3BDC. هذا spatial query مفلتر مرجح، لا actor list مثبتة؛ ممنوع traversal/نسخها كأنها قائمة عامة. |
| sub_0017B130 في hooks.cpp | animation controller: owner+0، clip+0x54، name+0x5C، from+0x90. المصدر يفرق بين owner واللاعب المحلي؛ مرشح لاكتشاف مالكي animation غير اللاعب. |
| sub_001EEFD0 في hooks.cpp | weapon update، owner+0xF0 وplayer weapon+0x29C في المصدر؛ يدعم correlation ولا يثبت playable actor إضافياً. |

## ما لم يثبت
لا full-body model للاعب الأول، ولا actor factory أو clone آمن، ولا كاميرا/إدخال/سلاح مستقلان لشخصية ثانية. inventory ISO574 ملفاً، وأسماء ملفات chars لا تثبت وحدها وجود جسم لاعب. نجاح baseline compile أو عناوين source المتطابقة لا يغلق ACTOR-001.

## الفحص التالي بعد قبول baseline على الهاتف
صمم probe منفصلاً opt-in دون كتابة guest memory أو استدعاء guest functions غير مثبتة. تحقق من bounds والعمر وfinite vectors قبل القراءة؛ عيّن هوية محلية مؤقتة للأجسام مع reset عند reload. اجمع عدداً محدوداً من العينات في buffer محلي، وسجل thread IDs أولاً؛ لا تفترض أن CharacterDetail/animation hooks تعمل جميعها في game thread ذاته.

قارن الثبات/المشي/الدوران/الرمي، حركة وموت NPC قريب، checkpoint reload والخروج من المهمة. اربط animation owners وLOD/resource changes دون تصدير pointers إلى شبكة أو افتراض أن object يماثل اللاعب. يلزم إثبات الاختفاء/إعادة الإنشاء قبل البحث عن lifecycle/factory. لا شبكة أو spawn أو تعطيل original hooks من هذا التحقيق.

## جلسة16 — موارد المالك المفككة
`alwansan/biso@9ceb6a65fb5a4414c106ca3961295a1f8f7e558b` يتيح دراسة الملفات الأصلية دون استخراج ISO كل مرة: `chars/trans_ch.bin`, `chars/trans_fp.bin`, `levels/level_XX/Unit_01.bin`، `LevelDat.bin` و`export/frontend/FEMain.bin`. فحص الأسماء والأحجام ثابت، وليس إثبات معنى تنسيق البيانات أو ارتباطها بهويات runtime التي ظهرت في Probe2. خريطة مستقلة docs/ACTOR_BISO_ASSET_INDEX_AR.md، وبوابة مراجعة lifecycle قبل أي spawn باقية.
