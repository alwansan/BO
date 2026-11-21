# BO-COOP Session38 — 2026-11-22 — نتائج الهاتفين واتجاه جسم الحليف وبدء المضيف

## الحالة الحية
فرع الكتابة الوحيد: alwansan/BO experiment/black-coop. بداية الجلسة HEAD 37b897153992be66fc7facc936e936ccf1896724. لا تعديل main ولا ISO ولا ملفات الحفظ. اقرأ AGENTS ثم RESTORE ثم CLOUD_HANDOFF وSTATE والجلسات37/36 والخطة الجديدة.

## اكتشاف سجلات جديدة لم تكن قد وصلت في جلسة37
المصدر CL: black log/logs/BLACK-Probe-20261121-235407-267/coop_probe.txt (369904 B، blob a9ade07f8ffecb051969d3dc00a092c5459bf2ad)، 675 حدث: start1, player168, pool171, default_player_model8, animation136, lod191. **جميع قراءات default_player_model الثماني: valid/has_model/complete=true، 44 مفصلاً، 2 buffers، draw_list_mask=32 أي draw-list5، arms=true، head/torso/legs=false، body_candidate=false.** النتيجة تخص primary model فقط actor+0x320→wrapper[0]، ولا تنفي وجود موارد أجسام كاملة أخرى.

الهاتف الأول Xiaomi 23013PC75G Android15، سجل black log/logs/BLACK-Probe-20261121-235407-267/black_log.txt blob eaaaa544...، والهاتف الثاني Huawei JSN-L22 Android10، سجل black log/من الهاتف الثاني/BLACK-Probe-20261121-235449-609/black_log.txt blob 59a989... . الاثنان يشغلان org.bocoop.black.defaultplayer وصورة اللعبة c5bfed25449c8212d7a976f15246e87ead57713f. لا ملف default_player_model مماثل مرفوع من الثاني. لا داعي لتكرار التجربة الأولى.

سجلات الاتصال المتأخرة من الهاتف الثاني تتضمن SCAN_RESULTS_1 وJOIN_CONNECTED وROOM_JOINED؛ سجل المضيف فيه HOST_DISCOVERY_READY وHOSTING وأحياناً ERROR_BINDEXCEPTION بعد إعادة استضافة سريعة. هذا يؤكد اتصال غرف، لا gameplay أو مشتركية العالم. النجاح السابق 2/4 على هاتفين صحيح ويظل قبول غرفة، لا طور قصة. سجل الصورة الجديدة: أسماء JPG في img و«من الهاتف الثاني» وSHAs موجودة، لكن محتواها الثنائي لم يمكن فتحه عبر GitHub text-only ولا raw web؛ لا ندعي فحصها بصرياً. الملخص في memory/evidence/DEFAULT-PLAYER-owner-20261122-235407-safe-summary.json.

## توجيه المالك الأحدث — يلغي منع الحليف كتمثيل بصري في جلسة36
على كل جهاز **الشخصية الأصلية هي اللاعب المحلي بمنظور الشخص الأول كاملاً**، باليدين والسلاح والكاميرا والـHUD. عندما ينظر لاعب إلى البشر الآخرين (بما فيهم المضيف عند الضيف) يمثلون **بجسم حليف كامل بصرياً**، لكن يظل التحكم إنسانياً عبر الشبكة، دون AI حليف، دون نسخ ممثل NPC حي أو مشاركة brain+324، ودون تحويل منظورك أنت إلى حليف.

## تنفيذ حقيقي على المصدر (ليس gameplay)
- coop/bridge/remote_ally_roster.hpp + test_remote_ally_roster.cpp: حالة scene + self-player واعية؛ host يشاهد الضيف، والضيف يشاهد host، ولا نسخ local self، مع generation/timeout/epoch ورفض snapshots غير صالحة، ودون أي guest pointer/actor/render.
- coop/story/mission_barrier.hpp + test_mission_barrier.cpp: مسار host PREPARE/READY/COMMIT، لا COMMIT قبل كل المشاركين المتوقعين والتأكد من نفس mission/stage/scene، ويرفض القديمة والتزوير ويُلغي عند leave. لا يفتح مشهداً في BLACK حالياً.
- .github/workflows/black-coop-story-contracts.yml: CI44951425780 SUCCESS C++20 ASAN/UBSAN مع اختبارات scene inbox وproxy القديمة.
- وثيقة الخطة: docs/superpowers/plans/2026-11-22-host-story-remote-ally.md.

## الخطوات التالية
1) ربط هذه العقود برسائل TLS الحية بطريقة Host-authoritative، دون ادعاء لعب؛ 2) تحديد API الحقيقية لبدء/تحميل المرحلة/الـready داخل XBE على game-thread، 3) تدقيق rig مجسم الحليف وملكية instance مستقلة للضيف والمضيف + تنظيفها، 4) إرسال إدخال/حركة وسلاح البشر ثم world/combat/AI/quest والانتقالات. لا Start وهمياً يشغل لعبتين منفصلتين. يحافظ القائد على طور سنغل بلاير.
5) لا APK جديدة خرجت لهذا الكود الجديد؛ آخر APK مبني ومفحوص run44946597973/artifact14724535514. لا تطلب ISO جديداً.

راجع العقود: renderer المحلي1D25D0 مختلف عن NPC1C6150؛ NPCstride3C0 والـbrain+324/controller+328 وmembership+250 لا تُنسخ عشوائياً. مرجع portingpete MIT للقراءة والتعلم مع نسبة عند نسخ كود مرخص، ليس Android co-op جاهزاً. أجهزة المالك ذات الأصول موجودة BO/biso، لا نشر binary/geometry/ISO.
