# ملكية صوت أسلحة لاعب الهوست الإضافي — عقود c5

2026-11-23. تحليل read-only لصورة المالك ذات SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f`، ومرجع PC MIT المثبت `portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628/src/recomp_manual.c`، دوال `black_p2_banks_make/take/swap`. لا نسخ أصول أو تفكيك كامل هنا، ولا إثبات تشغيل لاعب إضافي.

## مساحة الملكية

- مدير السلاح `[2D12D8]` حجمه D0. سجلان بطول38 عند +8/+40؛ مؤشرا الحالي/التالي +7C/+80 قد يشيران إلى داخل المدير ويحتاجان relocation عند نسخه.
- FPV من `[[[2D0D60]+D3C8]+C]`، حجمه1330. المرجع يشترط stage +130C=9 والمدير +C4 غير صفري قبل take.
- مخازن مستقلة: مدير D0، كائنان C0 للبنوك، argument20، contiguous بمقدار **293000** = اثنان×E1800 لملفات السلاح + أربعة×18000 للصوت + اثنان×38000 للبنوك.
- SDK الأصلي يوفر `mem::AllocContiguous(size,0,0xFFFFFFFF,0x1000)` و`FreeContiguous`. المواءمة والتصفير وphysical mapping من allocator نفسه، وليس `AllocSystem` بديلاً عشوائياً للبيانات الصوتية المتصلة.
- heap16 عند2D152C وheap18 عند2D1574؛ stride24 وأصل الجدول2D12EC. لا تكفي ملاحظة PC «غير مستعملين» لإثبات ملكية runtime Android. يجب فحص السجلين فعلياً على game thread، والتحقق من عدم استعمال مدير/ملف/طلب آخر لهما قبل أي كتابة، وحفظ جميع24 بايت لكل سجل للرجوع.

## ABI مثبت من الأجسام والمتصل الأصلي152180

|المدخل|السجلات والوسائط|العودة|
|---|---|---|
|141380|ESI كائن بنك C0 مستقل؛ EDI مخزن البيانات؛ ECX طول المخزن؛ stack arg1 عنوان اسم البنك|ret4؛ **EAX ليس success/HRESULT موثوقاً**|
|141AD0|ESI كائن صوت290؛ ECX argument يحتوي pointer عند+C وطول عند+10؛ لا stackargs|tail إلى1418B0 ثمret؛ لا success موثوق|
|8CE60 / 7EDB0|stack arg1 عنوان **audio object المضمّن**: bank+8 أو sound+208|cdecl: ret عادي، caller ينظف4؛ لا success موثق|

**تصحيح مهم بعد تتبع ترتيب الوسائط كاملاً:** bank+8 وsound+208 هما عنوانا audio object مضمّن، وليسا حقلي pointer إلى كائن آخر. 8CE30 يضيف type كأول وسيط إلى وسيطاته الخمسة؛7EF50 يقرأ وسيطه السادس في EBX، ويستعمله storage للكائن عند عدم الصفر، ويضع flags=4 عند object+54. لذلك cleanup يتلقى bank+8 أوsound+208 مباشرة، وليس R32 منهما. حجم audio object هو5C: bank+8..63 ضمن C0،sound+208..263 ضمن290،قبل descriptor الصوت264. لا تفسر أول word للكائن كمؤشر handle. CallGuest يعيد C++ result ثم يستعيد EAX؛ لا تقرأ `call.eax` باعتباره نتيجة.

## تصحيح نتيجة التحقيق الأول

141380 و141AD0 ليسا constructor خاليين من الموارد. كلاهما ينادي8CE30 ثم7EF50. الأخير يستعمل audio object المضمّن عند توفير الوسيط السادس (وإلا يستطيع تخصيص object)، ويستدعي callback الإنشاء، وقد ينشره في مدير الأجسام عبر81020/80FE0. كلا المدخلين يغير handoff globals:2F3A4C/2F3A50/2F3A54 و2F3B00. لذلك استعادة FPV1330 وحدها لا تلغي الإنشاء.

**تم العثور على مسار إزالة النشر الحقيقي:**8CE60 يقفز مباشرة إلى7EDB0. يقرأ object+C (نوعه) ويستدعي callback عند type+10، وينظف descriptorين عند object+10/+2C بواسطة7E1E0، ثم81030 يحذف مراجع هذا object من قوائم النشر (85A40 ثم85970)، ثم85D20 يفك روابطه/ملحقاته، ثم7EBA0 يعيد تخصيصاته السفلية. `flags & 0x4` تمنع7EBA0 من تحرير storage الكائن المضمّن نفسه؛ لا يزال ملحق +50 المحجوز داخلياً يُحرر إن وُضع `flags & 0x8`. لا تستعمل7EBA0 وحده لكائن منشور؛ تتجاوز بذلك callback وإزالة المراجع.

## callbacks الفعلية وملكية contiguous

8D1D0 يهيئ نوع الصوت عند2C2C38 ويثبت create=8CF40،destroy=8D000،read=8D010 وwrite=8D050، إضافة callbacks أخرى. 7EF50 يستدعي create على المسار نفسه قبل النشر؛7EDB0 يستدعي destroy على المسار نفسه. هذا يثبت synchronous **لهذين الاستدعاءين فقط**، وليس غياب جميع قراءات المحرك/خيوط الصوت الأخرى.

8CF40 يأخذ مخزن الصوت عبر8CE90 ويضع النتيجة عند audioObject+4C. destroy8D000 يمرر ذلك الحقل إلى8CED0. هناك خياران للـallocator:

1. إذا2C2C94 غير صفري،8CE90 يناديه، و8CED0 ينادي2C2C98. لا يصح تخمين ملكية المخزن إن كانت callbacks مجهولة.
2. 152B94..152BAC يثبت في إعداد اللعبة: allocate=B7640،free=B7630،size=B7610. B7640 يعيد مخزن handoff2F3A4C المقدم من caller ويصفر2F3A50/54؛ **لا يخصص مخزن بيانات جديداً**. B7630 يصفر2F3B00 و2F3A4C؛ **لا يحرر contiguous المقدم من caller**. B7610 يقرأ طول handoff المشروط.

بشرط التحقق الحي من تطابق هذه callbacks، يمكن تحرير audio objects الجديدة أولاً عبر8CE60، ثم تحرير contiguous الخاص بنا عبرSDK. لا تحرر handles التي جاءت من FPV الأصلي المحفوظ، ولا تتصل بتحرير شامل للنوع7E240؛ ذلك يشمل موارد الهوست.

## معاملة الإنشاء المطلوبة قبل التفعيل

**عناوين ثابتة، لا نسخ عقد منشورة:** لا ننقل طريقة PC التي تبني embedded object في FPV الأصلي ثم تنسخ bytes وتعيد FPV القديم. `7F0CC..7F0D3` يمرر عنوان object نفسه إلى publication manager؛ `80FE0/858C0` يستطيعان حفظ ذلك العنوان في عقدة خارج FPV. لاحقاً `81030/85A40` يبحثان بالقيمة نفسها للحذف. لذلك نقل bytes إلى snapshot لا ينقل المراجع الخارجية. هذا خطر استنتج من callgraph، وليس فشل PC مرصوداً. التصميم الآمن ينشئ كل bank/sound جديد في عنوان guest خاص دائم، ولا يبادل bytes الخاصة بـobject المضمّن أو عقدته؛ يجب أن تشير سجلات مدير P2 إلى تلك العناوين الثابتة.

publication registry الخارجي يبدأ من الحلقة عند575508؛81030 يمشيها ويستدعي85A40 على قائمة كل عضو لحذف القيمة المطابقة للكائن.85A40 يستعمل85970 لفك العقدة وتحريرها. يمكن تدقيق غياب عنوان embedded object الخاص بنا من هذه القوائم بعد cleanup بمشي bounded مع تحقق الروابط؛ لا يجوز اعتبار بقاء صفحات object committed دليلاً على أنه ما زال منشوراً أو أن cleanup فشل، لأن storage نفسه مملوك لنا.

نوع2C2C38 يُهيأ عبر8D1D0؛ التحقق الحي قبل الإنشاء/الحذف يشمل type+C=8CF40 وtype+10=8D000، callbacks2C2C94=B7640،2C2C98=B7630،2C2C9C=B7610. `7EF50` يخزن وسيطه الثاني (engine owner الأصلي من[2D0D60]+D3A0) عندobject+58، ويخزن النوع عندobject+C. تحقق كليهما عند cleanup، مع`flags & 0x4` وpointer البيانات object+4C إلى المخزن الخاص بنا؛ لا تسمح هذه القراءة بإزالة كائن borrowed.

1. game thread واحد، scene/FPV/manager ثابتة، FPV stage9، وقائمة تخصيصات مملوكة وغير متداخلة مع الأصل. تحقق callbacks الثلاث الفعلية قبل nativecall وقبل cleanup.
2. احفظ FPV1330، global manager pointer، سجلي heap16/18، وhandoff globals بأحجامها الصحيحة. افحص **كلا** heap قبل أول كتابة، وليس كتابة الأول ثم اكتشاف تنازع الثاني.
3. أنشئ أصواتاً/بنوكاً في تخزين مستقل كلما أمكن، وسجل فقط **عناوين embedded objects الخاصة بنا**: sound+208 وbank+8. لا تعد نجاح nativecall من EAX. تحقق handle/type/resource والملكية بعد كل خطوة. الحد الأقصى أربعة أصوات وبنكان.
4. في الفشل الجزئي: أوقف الإنشاء؛ أعد singleton/FPV الأصليين حتماً؛ حرر فقط handles الجديدة المثبتة عبر8CE60؛ أعد heap records وhandoff globals عند ثبات الملكية؛ ثم حرر التخصيصات الخاصة. إن فقدت scene/ملكية handle أو callbacks فلا تخمن cleanup: quarantine مع تعطيل سياق اللاعب الإضافي.
5. `banks_swap` لا ينشئ مورداً: يبادل manager pointer وFPV snapshot فقط. لا تمكّن swap قبل نجاح المعاملة كاملة؛ ولا تمكّن destructor عام على نسخة FPV ضحلة تحتوي صوتيها الخامس والسادس أو بقية موارد الأصل المستعارة.
6. يلزم اختبار real-engine lab للـcreate/release مع جرد النشر قبل/بعد، وفشل كل خطوة، واستعادة البايتات الأصلية؛ مع تحديد إن كانت audio polling/callbacks تقرأ عنوانFPV أثناء التبديل. لم يُنفذ initialized bank port ولم تُعلن هذه الاختبارات ناجحة حتى الآن.

## قابلية إعادة التحقيق محلياً

### قيد threading جديد وwitness حقيقي

أجسام7EF50 و7EDB0 وpublication helpers المفحوصة لا تستدعي قفلاً أو critical section. SDK المثبت `runtime/xdk/dsound.cpp` ينفذ callbacks المكتملة عادة داخلDirectSoundDoWork، لكنه عند غياب pump لمدة250ms ينفذها من host thread احتياطي؛ يترك `g_lock` قبل `Run(ctx,done)` الذي يستدعيCallGuest. لذلك mutex الصوت ليس قفلاً شاملاً على publication الأصلي، ولا يكفي فرضgame-thread على كودنا وحده لمنع كل callbacks الأخرى. هذا لا يثبت تداخلها مع constructors بعينه؛ يحتاج قياس مسار التنفيذ الحقيقي.

أضيفت وحدة خاصة جديدة `coop/engine_lab/audio_lab.py` و`audio_observer.inc` و`audio_publication.h`، مع اختبارات calibration. تلف الأجسام الأصلية7EF50/7EDB0 على checkout المختبر المؤقت بعد generation، تنادي الأصل مرة واحدة، وتقيس threads/overlap/publication refs قبل وبعد الحذف، وتفحص تغير هوية embedded objects بعد PC banks_take/restoration. لا تكتب guest memory ولا تضيف nativecalls لإنشاء صوت. تفاصيل الإعداد وحدود النتائج في `coop/engine_lab/AUDIO_LIFECYCLE_LAB.md`. نجاح اختبارات calibration ليس تشغيل engine؛ لم تُعلن بعد نتيجة private runtime لهذه الوحدة.

استعمل `coop/analysis/owner_xbe_static_slices.py` مع `BLACK_OWNER_XBE` لصورة c5 الخاصة. المديات المحددة من حدود مؤكدة: `141380:14141a`،`141ad0:141b82`،`1418b0:141927`،`152228:152371`،`7ef50:7f0e8`،`8ce30:8ce70`،`7edb0:7eded`،`7eba0:7ebe8`،`81030:8105e`،`85a40:85a70`،`85970:859a2`،`8d1d0:8d24a`،`8cf40:8d010`،`8ce90:8cf40`،`152b8f:152bb1`،`b7610:b7652`. لا ترفع stdout التفكيك أو ELF المؤقت؛ هذه الوثيقة تحفظ العقود والنتائج فقط.
