# BO-COOP — ORIGINAL HOST HUMAN PLAYER CONTEXT / ABI — تنفيذ P0 الأساسي

آخر مراجعة 2026-11-23، الفرع `experiment/black-coop`.

## هدف التنفيذ
ليس كافياً أن تدخل هواتف متعددة إلى نفس TLS lobby، أو تحمل نفس `level_00/stg_0001`، أو يصل موقع لاعب عبر الشبكة. القبول: هوست واحد يملك محاكاة BLACK الأصلية مع 2–4 لاعبين بشخصيات مستقلة داخل **نفس** collision/AI/weapon/objective world. كل لاعب يرى جندي الآخر ويتحرك ويطلق ويدمر الباب في العالم ذاته.

## مصدر التنفيذ المتحقق منه
مرجع PC منشور MIT: `portingpete/Black-2006-Recomp@b2512fafd3/src/recomp_manual.c`، أقسام ~1291–1360،1363–1394،1811–1845،1916–1988،1990–2007،2039–2130،2132–2195،2238–2248،2250–2390.
- **مصدر فقط وليس ABI Android مؤكداً**: محرك BLACK الواحد يشير إليه `[0x002D45D4]`. اللاعب الأصلي `core+0x30` حجم `0x900`، camera owner `[0x002D12B8]` حجم `0x16C0`، input source `0x40`، weapons pointer slots 16 ×4، actor model/part buffers مستقلة.
- تابع PC يُدخل اللاعب الثاني بتبديل الجسم `0x900` في خانة core مع shadow، لكن **لا يبدل** روابط قائمة الكائنات والتصادم الخاصة بعقدة `core+0x30`: world grid node `+0x20..+0x57` وروابط doubly-linked المرتبطة بعناوين الأصل. يجب فحص أي روابط راجعة قبل النسخ، وليس استبدال `memcpy(core,shadow,0x900)` مباشرة.
- camera owner swap يغير `[0x002D12B8]` ويعيده، مع first-person viewmodel/animation/cache/weapon banks/sounds/muzzle flash/physical shells/player use state. نسخ actor وحده لا يجعل سلاحه مستقلاً، وقد يسبب تلوث inventory وشفط الموارد الصوتية.
- `sub_00014380` تأسيس camera owner؛ `sub_001E0E40` تهيئة اللاعب الأصلية على عدة ticks؛ `sub_000DF400` طلب proxy و`sub_000DBBE0` إرجاعه؛ original actor step `sub_001F1080`. عدد proxy 20 مع occupancy `[[0x002D45F4]+0x2960]`; احتفظ بسعة للأعداء. AI targets من `[[0x002D1988]+0xFAC]`، 32×12، والمؤشر `actor+0x378`.
- مرجع PC P2 نفسه **لا يحقق AI target الكامل**؛ لذلك نجاح shadow actor منفرداً ليس نجاح co-op نهائياً.

## تقدم كود Android المرتبط فعلاً
- `RoomHostHumanKinematics` يتلقى TLS guest feet ويعطي قيم canonical محدودة السرعة، ليس engine actor.
- `OriginalHumanBotVisual` يستعير **جندي original NPC منفصل بصرياً** دون controller أو collision، ويستدعي source-linked renderer؛ نجاح العرض على هاتف لم يثبت بعد.
- Commit `7951e38158e8c71c169e48f43bc826c531fce523` أضاف `OriginalRemoteHumanMotionPlan` وتحويل idle/move بجملة `TM_AR_S_001` و`TM_AR_S_MOV` باستخدام `sub_0017E130` المنقول من PC بعد فحص `dispatch::Find`. يخضع الموديل لفحص orig-scene وgame-thread والمكان. فحص C++ source/UBSan نجح `45095381428`. ما زال renderer على هاتف غير مثبت؛ لا ادعاء بوجود collider/actor.
- Commit `f8edb2fb` يفحص مخزون proxy read-only بدون تشغيل الـengine allocator. بعده Commit `22c8a07cda4751d2b547be6558a5c66b514ff637` أضاف source-only audit الخاص بملف owner XBE SHA1 pinned: ثمانية function-entry executable candidate windows أصلية، private numeric-only artifact؛ إجراء `45095558435` **SUCCESS** في synthetic + owner-private jobs. **الوجود في executable section ليس صحة استدعاء Android ولا proof player allocation**.
- بناء الـWindows Android `45095381425` ظل **IN_PROGRESS** عند آخر فحص، ولا يُنشر باعتباره co-op مكتمل.

## بوابات تنفيذ لا يجوز تجاوزها
1. إثبات calling convention الفعلي على Android ووضع rollback للـoriginal context switch، مع حماية stack/x86 regs والفوترة الصوتية/أصوات الأسلحة.
2. تخصيص `shadow actor`، `camera owner`، `input source`، `weapon array`، `part buffers` مستقلة، وحجز guest allocator lifecycle موثّق؛ لا مشاركة عناوين player 1. صلاحية actor/owner/model/proxy وملكية current thread شرط إلزامي.
3. إنشاء اللاعب 2 فقط أولاً، في `HOST original game thread` بعد real COMMIT/independently matched mission: setup الأصلي حتى ready ثم physics proxy أصلي، والأصل يحدّثه بمدخلات الـguest TLS؛ رفض التنفيذ عند نقص free proxy أو RAM pages. لا تُمرر network payload كعنوان أو NPC slot.
4. الحفاظ على روابط core/world grid أثناء كل swap؛ إن فشل أي شرط يتم تعطيل الـextra-actor وإعادة original host الواحد بدون تخريب العالم أو save. لا يؤثر PC joystick أو أي thread على اللعبة أثناء التبديل.
5. بعد نجاح P2، عمّم roster slots 3,4 بملكية مختلفة، مع reservation تمنع تنازع proxy وAI target table وweapon slots.
6. إثبات AI targeting: أعداء الهوست يهاجمون كل اللاعبين، والرصاص/الأبواب والأهداف تُحسب في عالم الهوست فقط؛ event TLS v9 وguest object inbox متاحان، لكن producer native stable door identity وguest native safe apply **غير موجودين**.
7. القبول النهائي على هاتفين أو حتى أربعة، اختبار واحد شامل عند وجود أجسام/تصادم/طلقات/باب/عدو/هدف فعلي؛ CI الاصطناعي غير كافٍ.

## أخطار مُراقبة
مخزون NPC character 32 منفصل عن مخزون physical proxies 20. ألا يُعامل body appearance puppet كـphysics player أو enemy target. لا يكفي خريطة host NPC slot ↔ guest NPC slot؛ IDs محلية ومتغيرة. لا تفعل `sub_000DF400` لمجسم puppet غير مهيأ لتكون له actor lifecycle. تجنب الاستدعاء من Java/UI/network threads. لا تعديل `main` و`CL` و`biso` والملفات الأصلية أو saves.

