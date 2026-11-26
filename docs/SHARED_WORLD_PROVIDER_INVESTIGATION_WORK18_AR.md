# Work18 — تحقيق موفر العالم الأصلي

التاريخ 2026-11-26. فُحص BO عند `7acb81040f93ce2ca1647f8bfb3af4f8b083c47b` على `experiment/black-coop`. هذه نتائج قراءة المصدر. لم يُنفذ فعل باب أو تدمير أو تطبيق عالم داخل اللعبة، ولم تُغيَّر بوابات الجاهزية.

## المصادر المثبتة

- مرجع Android هو `RanggaCasper/Black-Android` عند `f3754d6985a8c35204e7df071546e586b10f1806` في `/workspace/bo-tools/reference`.
- مرجع PC ذو ترخيص MIT هو `portingpete/Black-2006-Recomp` عند `b2512fafd3e181e4febfb298f297245b845ef628` في `/workspace/bo-tools/pc-reference`. جرى تنزيل checkout للقراءة والتحقق من HEAD، ولم تُعدّل ملفاته.
- ملفات BO المعنية هي `coop/lobby/java/com/racas/black/coop/RoomSession.java` و`coop/room_network/room_original_guest_host_object_contract.hpp` و`room_original_guest_host_object_witness.hpp` و`room_original_guest_canonical_source_barrier.hpp`.

## الوصلات الموجودة والفجوات المحددة

`RoomSession.offerOriginalHostObjectEventFromGameThread()` عند السطر800 يطلب episode أصليًا، تسلسلًا وtick وهوية stableKey وحالة. تعليق الواجهة يصرح بأن original object-transition hook لم يُوصل. لم يوجد منتج أصلي يستدعي هذه الواجهة في مصدر BO المفحوص.

`PollHostObjectEventsOnOriginalGuestThread()` يقرأ أحداث TLS ويربطها بمفتاح التحميل المحلي المتحقق، ثم يضع القيم في `OriginalGuestHostObjectInbox`. هذا مسار استقبال فعلي للقيم، وينتهي قبل تنفيذ أي callback على الكائن الأصلي.

`OriginalGuestHostObjectInbox::ApplyNext()` يطلب من IO إثبات المشهد وحل الهوية والعمر ثم التطبيق وإعادة قراءة الحالة. الطرق المطلوبة هي `OriginalScene` و`ResolveOriginalObject` و`StillOwnsOriginalObject` و`ApplyOriginalHostState` و`VerifyOriginalHostState`. البحث في BO وجد تطبيقات اختبارية، ولم يجد موفرًا إنتاجيًا يثبت هوية باب أو يستدعي تغيير حالته الأصلي. `OriginalGuestCanonicalApplyFrame` ينسق دفعات NPC والأحداث لكنه لا يملك caller إنتاجيًا.

إذًا المطلوب قبل الدمج هو مصدر هوية ثابتة للكائن الأصلي، وربطها بهوية التحميل والعمر، وcallback حالة أصلي موثق على خيط اللعبة. عنوان actor أو رقم خانته المحلي لا يثبت تطابق الكائن بين الهاتفين.

## ما أثبته مرجع PC وما بقي مجهولًا

في `src/recomp_manual.c` عند السطر4883 توجد wrapper باسم `sub_001E74B0`. التعليق السابق لها يحدد ECX بوصفه الجندي المتضرر و`[ESP+0x18]` بوصفه مطلق النار. تتحقق wrapper من ارتباط مطلق النار باللاعب، وتحتفظ بارتباط hit-marker ثم تستدعي `sub_001E74B0_gen()` مرة واحدة. هذه معلومة مصدرية عن إصابة الجندي. لا تثبت callback لباب أو هوية destructible أو ABI صالحًا على صورة Android.

في `src/black_checkpoint.inc` عند السطر265 تعيد الاستعادة ملء `objective_backup` وتضبط حالة checkpoint الأصلية. لا توفر هذه الدالة تطبيق objective حيًا متزامنًا أثناء استمرار العالم. استعمال استعادة checkpoint بدل فعل العالم سيغير مسار التنفيذ المطلوب.

في `tools/black-formats/src/lib.rs` عند السطر381 يقرأ parser الخاص بـ`unit-v24` عدد instances بوصفه u16 عند `+0x90`، ومؤشر الجدول النسبي عند `+0x1c`، ويتحقق من سجلات بحجم48 بايت. لا يفك parser هوية instance أو علاقتها بمقبض كائن حي. مؤشر الجدول ليس عند `+0x94`.

في الملف نفسه، parser الخاص بـ`stlevel-v10` يعرض اسم سجل authored وID64 عند `record+16` وسجلًا حجمه40 بايت. هوية سجل المستوى لا تثبت هوية الباب الحي. هذان التنسيقان نقطة بحث لربط بيانات المؤلف بإنشاء الكائن، ويحتاجان تتبع المستهلك الأصلي والتحقق من إعادة التحميل والتكرارات.

بحث الكلمات door وbreakable وdestruct في مصدر Android وفي src/docs/tools/tests لمرجع PC لم يقدم callback موثقًا لتغيير حالة باب. غياب اسم دلالي في هذه الملفات لا يثبت غياب المنطق من XBE الأصلي.

## أدوات وخطوات التحقيق التالية

يمكن إعادة تتبع المصدر بالأوامر التالية من `/workspace/BO`. هذه أوامر قراءة، وقد نُفذت عمليات البحث وفحوص HEAD المكافئة أثناء التحقيق.

```sh
git -C /workspace/bo-tools/reference rev-parse HEAD
git -C /workspace/bo-tools/pc-reference rev-parse HEAD
rg -n 'ResolveOriginalObject|ApplyOriginalHostState|OriginalGuestCanonicalApplyFrame' coop
rg -n 'sub_001E74B0|black_hit_soldier' /workspace/bo-tools/pc-reference/src/recomp_manual.c
rg -n 'instance_count|instances_offset|stlevel-v10' /workspace/bo-tools/pc-reference/tools/black-formats/src/lib.rs
```

التحليل الأصلي التالي يبدأ بتحديد قارئ جدول unit ذي سجلات48 بايت، ثم constructor الكائن الذي يستهلك السجل، ثم مسار تغيير الحالة/التدمير وteardown. يلزم تتبع descriptor أو ID المؤلف عبر هذا المسار، وفحص التكرارات بين الوحدات والمراحل قبل اعتماد أي صيغة stableKey. يجب الاحتفاظ بالتعليمات الأصلية وبيانات اللعبة داخل مساحة تحليل خاصة وإخراج نتائج مشتقة محدودة فقط.

الأدوات المتوفرة في هذه البيئة هي `objdump` و`strings` وPython. يقدم `coop/analysis/story_entry_static.py` التحقق من SHA1 المثبت واستخراج حدود الأقسام التنفيذية، ويقدم `world_xref_static.py` نموذج فحص مراجع bounded و`world_writer_flow.py` أدوات تصنيف التدفق. هذه الأدوات الحالية لا تبحث عن الباب تلقائيًا، ولا تكفي نتائجها وحدها لإثبات ABI. لا يجوز تمرير ISO إلى خيار XBE.

عند وجود ملف XBE خاص متحقق مسبقًا يمكن تشغيل محلل العالم القائم كما يلي، مع استبدال المسار بملف XBE الفعلي داخل مساحة خاصة. لم يُنفذ هذا الأمر في دفعة التحقيق الحالية، ولا يُعد تحليل مرجع world دليلًا على هوية باب.

```sh
python3 coop/analysis/world_xref_static.py \
  --owner-xbe /private-analysis/default.xbe \
  --report /private-analysis/world-xrefs.json
```

بعد إثبات دالة الباب، يُضاف اختبار ABI على مصدر/صورة مثبتين، ثم موفر original IO مع revalidation للمشهد والعمر قبل وبعد callback وreadback للحالة. اختبار التكامل المطلوب هو فعل ضيف موثوق ينفذه المضيف مرة واحدة، ثم نفس الهوية والحالة الأصلية على الضيف، مع رفض replay وإعادة تحميل المرحلة وتبدل عمر الكائن. تشغيل هذا المسار داخل المحرك ما زال غير متحقق.
