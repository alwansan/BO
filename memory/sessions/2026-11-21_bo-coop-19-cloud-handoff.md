# جلسة19 — تسليم عاجل إلى جلسة سحابية، 2026-11-21

طلب المالك حفظ المعرفة فوراً بسبب قرب انتهاء حدود الجلسة. هذه دفعة توثيق فقط؛ لم ننفذ Probe3 أو ننتج APK/ISO جديداً ولم نشغل CI جديداً. نقطة البداية المفحوصة `0a5746a7b3231f02ec81d66706c2929147aeeb2c`.

## حالة مؤكدة
Baseline يعمل على هاتف المالك بلا تعطل؛ لا تعيد طلب اختبار baseline أو XBE الأصلي المفقود. الصورة المطابقة المتاحة SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f`، الحجم 2813952. Probe2 يعمل وتصدير Android15 نجح. يوجد 400 حدث حقيقي: player131/lod172/animation97، epoch واحدة؛ 13 ارتباط LOD+animation وواحد player+animation. هذه ارتباطات مؤقتة وليست إثبات spawn/death أو multiplayer. مصدر السجل في مستودع المالك CL: `black log/logs/BLACK-Probe-20261121-064812-388/coop_probe.txt`. لا تنسخ السجل الخام إلى BO.

## اكتشاف التحليل الثابت الجديد — ACTOR-001A
تحليل read-only لصورة c5 المطابقة، وليس offsets من إصدار آخر. عناوين أدناه guest x86، لا مؤشرات Android native.
- vtable المرصود لشخصية NPC/LOD `0x28C1E8`، والمحلي `0x28CFE0`. أول NPC slot يشير `0x1B7720` وهو ret4، **ليس destructor مفترضاً**.
- تهيئة `0x20BDDE`: طلب حجم `0x8540` ثم call `0x1F3FC0` (allocation-like، لم يثبت عقده). يكتب manager vtable `0x28D254`، يبدأ ECX=manager+`0xA0`، EDX=`0x20`، ثم عند `0x20BE02` يكتب NPC vtable ويزيد ECX بـ`0x3C0` حتى32 مرة. يخزن manager في global `0x2D1980` عند `0x20BE11`، ومسار الفشل يكتب صفراً عند `0x20BE18`.
- هذا دليل ثابت قوي على32 خانة NPC مهيأة داخل pool؛ **لا يثبت نشاط الخانة أو حياتها أو مالك المجسم**.
- تهيئة vtable المحلي عند `0x14490`: `mov [ebx+0x30],0x28CFE0`، يدعم وجود local-player subobject في world+30، لا يثبت remote body.
- مراجع global manager تشير إلى count محتمل manager+`0x79B4` وجدول pointers محتمل +`0x79BC`؛ يلزم إثبات المعنى وحدود count قبل أي قراءة جديدة.
- `0x1DA680` يقرأ manager، وعند `0x1DA693` يقرأ counter+`0x8524`، يزيده عند `0x1DA6A7`، ويكتب القيمة القديمة في actor+`0x374` عند `0x1DA6B0`. **مرشح serial/generation حقيقي**، لا يُعتمد حتى فهم reset/reuse/overflow واختبار الهاتف.
- دوال للفحص التالي: `0x1DA1A0` remove/retire-like (تأتي بعد تصفير actor+8/+C في مسارات cleanup)، `0x1DA240` insert-like (ECX manager/EAX object)، `0x1E6860` capacity-like (EDX manager، AL مختبر)، `0x1E68B0` cleanup-like (ECX manager/EAX object)، `0x1E69B0` init-like (ESI manager)، `0x1DF440` acquisition/setup-like (EAX manager/ESI آخر)، `0x1D5E00` mission-transition (EDI manager)، `0x1B6840` teardown/update helper، `0x1CCBB0` cleanup-like. **كل التسميات فرضيات**.
- آخر محاولة عرض disassembly لهذه الدوال تجاوزت حد المخرجات ولم تُراجع؛ أعد slices صغيرة ولا تدع اكتمال الفحص.

## طريقة إعادة التحليل في السحابة
استعد default.xbe من biso pinned وفق docs/OWNER_BISO_SOURCE_AR.md وتحقق SHA1 قبل التحليل. الملف المحلي `/home/a/Desktop/black/biso/Black_Xbox_Extracted/default.xbe` ليس مطلوباً رفعه مجدداً. الأداة المساعدة المؤقتة `/tmp/black_actor_slices.py` محفوظة أيضاً داخل `coop/analysis/owner_xbe_static_slices.py` لأجل قابلية الاستمرار؛ تفحص SHA1، تغلف الأقسام في ELF مؤقت وتشغّل llvm-objdump. لا ترفع ELF أو disassembly كامل أو محتوى XBE. الاستخدام: `python coop/analysis/owner_xbe_static_slices.py 1da1a0:1da240` بعد تعيين متغير البيئة BLACK_OWNER_XBE إلى مسار الصورة. ابدأ على حدود دوال صحيحة؛ بدء التفكيك منتصف التعليمة يضلل.

## خطة التنفيذ التالية
1. إثبات دوال pool/list/serial والـscene reset بالتحليل الثابت قبل اعتماد offsets.
2. تصميم Probe3 منفصل read-only يراقب lifetime/generation والمجسم/ownership؛ bounded safe reads وfail-closed SHA، بلا network writes. اختبارات للمنطق المستقل ثم مراجعة وCI Android.
3. فقط بعد نجاح البناء اطلب اختبار هاتف محدداً: ظهور أعداء، ضرر/موت، checkpoint/reload وإعادة الاستخدام. حافظ على baseline/Probe2 والحفظ.
4. بعد ACTOR proof انتقل remote actor/render ثم combat/quests/late join/revive. لا تستبدل ذلك بمزيد mock network.
5. حدث STATE/MEMORY/PLAN/ISSUES/RESTORE بعد الدفعات ونتائج Actions.

## GitHub والمصادر
الكتابة حصراً BO:experiment/black-coop؛ لا force push ولا main أو مراجع. biso pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b` مصدر مفكك مع restore_github_upload.py. لا حاجة لإعادة رفع ISO أو Android/data في هذه المرحلة. المرجع Peterson047/BLACK-XBEMultiplayer pin `0ee3ae6411c7f0989c62cbe36b224d3f55043923` Windows TDM binary-only؛ native source BLACK-Coop أُعيد فحصه وظل404. لا تنسخ scripts/assets/binaries غير المرخصة أو body.bca المقيد.
آخر CI المعروف44869388876 ناجح Linux UDP loopback + NDK AArch64 crosscompile فقط؛ ليس Android runtime أو LAN فعلي. راجع أحدث runs عند الاستعادة. لا توجد APK/ISO جديدة بهذه الدفعة.

## ملاحظات محلية
`/home/a/Desktop/black/BO-active` checkout نظيف من الفرع الصحيح قبل هذه الدفعة. `/home/a/Desktop/black/BO-coop` قديم وله تعديلات محلية؛ لا reset/delete ولا تخلطه مع HEAD الحديث. مراجع read-only: Black-Android f3754d6985a8c35204e7df071546e586b10f1806، SDK XboxXBE-Recomp 99078b119b5d5e0d1d6afb1390e29437c0c984c0.


## متابعة قصيرة بعد التسليم — عقود أكثر دقة
فُحصت فعلياً slices `1da1a0:1da2d0` و`1e6860:1e69b0` بواسطة الأداة على SHA المطابق:
- **0x1E6860 ليست predicate نقية آمنة للاستدعاء**: تقارن count+79B4/capacity+79B8 في container+79B0، وعند الامتلاء تستدعي1C4A90 ثم2056A0 قبل اختبار20 flags في global2D45F4+2960. لا تستدعها من probe؛ راقب فقط.
- 0x1DA240 تنقل actor إلى container manager+79C0، تفحص actor+250، وقد تستدعي1DA1A0 عند الامتلاء. تضبط actor+384 إلى1؛ **لا تسمها spawn أو active-list insertion مؤكداً**، قد تكون مرحلة تقاعد/موت.
- 0x1E68B0 تستخدم container+79D0، وقد تستدعي1DA1A0 عند الامتلاء، تفك موارد+324/+B4، تضبط+384 إلى2، وتصفّر+8/+C. هذا يدعم وجود قوائم حالات متعددة، وليس قائمة active وحيدة.
- 0x1DA1A0 تأخذ manager ثمactor كوسيطين stack وتنتهي ret8؛ تنظف موارد+324/+B4، تستدعي1B6770 على manager+79A0 ثم actor virtual slot+0xC. هذا عقد cleanup قوي، لا free heap مثبت.
هذه أدلة ثابتة فقط. يجب فحص1B6770 و1C4A90 و1DF440 ودالة init/serial لفهم الحاويات قبل أي إضافة hooks أو قراءة count كأنه عدد الأحياء.
