# جلسة27 — سجل Probe4 مقبول ضمن نطاق الرصد

أبلغ المالك أن الاختبار بلا مشاكل وأرسل BLACK-Probe-20261121-163248-271 في alwansan/CL. قُرئ السجل فعلياً وفُحص Git blob SHA وSHA256؛ الملخص الآمن في `memory/evidence/PROBE4-owner-163248-safe-summary.json`. لا ينسخ الخام إلى BO.

439 سجلاً: 111/111 pool صحيحة، 548/548 instance links صحيحة (437 NPC و111 local)، صفر rejects. حتى أربعة controllers مختلفة لـNPC في اللقطة دون اشتراك مع local. المدة 115107ms، scope واحد، لا end ولا دليل reload. هذه قراءات مستقرة للعلاقات المختارة وليست إثبات allocation ownership أو atomicity أو co-op.

## قرار الخطوة التالية
لا تطلب إعادة baseline أو Probe3 أو Probe4. انتقل إلى مجسم محلي إضافي مؤقت قبل الشبكة، مع تدقيق التسجيل والإزالة أولاً. لا APK جديد في هذه الدفعة.

## نتائج ثابتة تمنع استدعاءً خاطئاً
- factory 1DF440 يمرر arg1 إلى 1D21F0 الذي يكتب actor في controller+8C دون null guard؛ تمرير nullptr غير آمن.
- init 1DA4F0 يأخذ تسعة stack args وEAX، ret24؛ ليس بديلاً كاملاً عن factory.
- poser 1801B0 يستدعي 1CE600 الذي يعيد ربط spatial tree عبر 15850؛ ليس pose-only.
- cleanup 1DA1A0 يستدعي 1BEB40 الذي يفك spatial links دون null guards في مسار actor+30==0؛ لا تستعمل cleanup على slot غير مسجل.
- تدقيق لاحق: 1CE1F0 EAX=actor، stack world، ret4، يربط world+5CB4/actor+B0 ثم يصفر actor+30/+44، يكتب actor+54=self، ويصل actor+34/+38 بعقدة world+4968. هذا يفسر شرط cleanup، لكنه لا يثبت سلامة مجسم بلا brain تجاه الإصابة/الانفجار.
- renderer 1CCA60 يبني قائمة الرسم من live79B0 وphase1 79C0؛ عضوية pool لا تعفي من spatial registration الصحيح.

التالي: تدقيق مسارات الإصابة/الانفجار لـ324 الفارغ وتسلسل register/update/unregister؛ بعدها تنفيذ حزمة محلية معزولة opt-in فقط إن ثبتت invariants. لا AI brain shallow copy، ولا زيادة live capacity منفردة، ولا ادعاء لاعب2 من هذه التجربة.
