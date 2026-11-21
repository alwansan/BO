# مرجع جديد للاعب الثاني وموارد الأجسام — جلسة35

المصدر المقروء والمثبت: [portingpete/Black-2006-Recomp](https://github.com/portingpete/Black-2006-Recomp/tree/b2512fafd3e181e4febfb298f297245b845ef628)، commit `b2512fafd3e181e4febfb298f297245b845ef628`. قرئ LICENSE الفعلي: MIT، copyright2026 portingpete وsp00nz. ليس هو Peterson047 المرجع السابق المغلق. المصدر العام لا يمنح ترخيصاً لأصول اللعبة. لم ننسخ أي مصدر C أو مورد لعبة منه إلى المحرك؛ إذا استُعمل كود منه لاحقاً، أرفق نسبته ونص MIT وإشعارات اعتماده الملائمة.

## ما يفيد مشروعنا
[خطة split-screen](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/docs/plans/split-screen-coop.md) و[src/recomp_manual.c](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/src/recomp_manual.c) يقدمان تجربة PC للاعب ثان، حالة مستقلة للسلاح والكاميرا وHUD، وتمثيلاً مرئياً بواسطة soldier puppet. الوثيقة تقول صراحة إن الأعداء لا تستهدفP2 بعد، والشبكة مستقبلية. هذا مرجع تنفيذ وتحقيق، لا co-opAndroid جاهزاً ولا مصدر بطل كامل مثبتاً.

تحققنا محلياً من صورةc5:1F4540 يكتب2 إلى[2D45F0]+20208، بينما عقودpool1B6770 تطابق قراءتنا السابقة. تشابه العناوين لا يثبت كل واجهات المشروعين أو حجم كل كلاس. كودPC يستعمل سجلات وإطار ذاكرة وallocator مختلفة؛ يجب نقل المعنى عبرX86Context وgame-thread وواجهاتAndroid المثبتة، وليس نسخglobals الخاصة بـWindows.

## حدود تمنع النقل الأعمى
- التمثيل المرئي في المرجع يأخذ قالب جندي من المرحلة؛ لا يثبت أنه جسمKellar مطابق.
- `black_puppet_make` ينسخ حقول slot إلا قائمة استثناءات؛ لا يحمي+250 ضمن تلك القائمة. إذا كان القالب حياً فيcontainer مختلف، قد يتغير backlink العضوية بينما slot ما زال فيfree array. قراءتنا لـ1B6770 تدعم احتمال بقاءfree entry عند النقل اللاحق. هذا خطر مشروط استنتجناه من المصدر، لا عطلPC مثبت على جهاز؛ يحتاج اختبار المحافظة على العضوية قبل أي نقل.
- null brain في puppet مرسوم مباشرة وخارج تسجيل القتال يختلف عن actor مسجل قابل للإصابة. رفضنا السابق لـnull-brain factory مسجل يبقى صحيحاً. لا ننقل الحماية من مسار إلى آخر بالافتراض.
- تثبيت حياةactor/context والـphysics والـAI والأهداف والمراحل يبقى مطلوباً. بدء لعبتين بمهمة واحدة دون مزامنة العالم ليس co-op.

## تحليل ملفات المالك الجديد
استفدنا من توثيق [حاويات BIN/DB](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/docs/file-formats/bin-db.md) و[MODEL](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/docs/file-formats/models.md) لكتابة قارئBO أصلي للـmetadata فقط: `coop/analysis/model_inventory.py` مع3اختبارات. قرأ محلياً271ملفاً من نسخةbiso دون تغييرها، وفحص4286وقوعMODEL.76وقوعاً (10مفاتيح فريدة) تملك أسماء مفاصل للرأس والجذع والساقين. كل المرشحات هنا25bone/3buffer. هذا يؤكد مرشحات أجسام بشرية، لا مظهر البطل ولا صحةskin/material/animation كاملة.

الملخص الآمن `memory/evidence/OWNER-MODEL-CANDIDATES-20261121.json`. لا geometry أوvertices أومصفوفاتمفاصل أوأصول فيBO/artifacts. المفتاحKELL لم يظهر بين المفاتيح المحللة؛ ذلك لا يثبت غياب جسم بطل باسم مختلف أوحاوية غيرمدعومة.

## أثر المرجع على الخطة
خطة البدء الجماعي تحتاج فصل player-context عن NPC factory. سجلfactoryroom الجديد يقيس سلوكاً حياً إضافياً ويختبر ثباتhook الأصلي؛ ليس إذناً لإعادة تشغيلdescriptor ولا شرطاً لكل تصميم محتمل. قبل إضافةكتابات أوتغييرsingleton، تحقق على الهاتف من الحزمة المعزولة الحالية، ثم صمّم حصةأجسام لا تسرقslot أوcontroller منNPC، وطبقةgame-thread لاستقبال بيانات البشر وmission prepare/load/ready. لا تضفStart مزيفاً.
