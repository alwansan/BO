# عقود جدول الشخصيات — تحليل ثابت لصورة المالك

2026-11-21، امتداد جلسة19. تحليل محلي فعلي read-only للصورة SHA1 c5bfed25449c8212d7a976f15246e87ead57713f. لا إثبات هاتف جديد ولا APK/CI جديد. عناوين guest x86 فقط. هذه النتائج تصحح التسميات المبدئية في سجل جلسة19.

## Container layout المثبت بالتعليمات
كل حاوية 16بايت: +0 flags، **+4 capacity**، **+8 occupied count**، +C pointer إلى مصفوفة pointers بها holes صفرية. لا تمشِ أول occupied عناصر فقط؛ يجب مسح capacity كلها مع حدود. الدليل1B6770 و1C4B20 وتهيئة1E69B0.

|إزاحة container داخل manager|capacity عند التهيئة|flags|المعنى|
|---|---:|---:|---|
|79A0|32|0|free/reusable-like؛ تؤخذ منها خانة قبل init وتعود إليها cleanup|
|79B0|16|7|allocated/live-like قبل تغيير الحالة؛ ليست16 عدواً مرئياً مثبتاً|
|79C0|8|7|state1؛ توضع إليها بواسطة1DA240|
|79D0|8|7|state2؛ توضع إليها بواسطة1E68B0|

المصفوفات ليست inline في هذه الإزاحات؛ pointer+0xC منفصل. 32 خانة actor inline manager+A0+i*3C0. السعات16+8+8 تطابق32 لكن لا تثبت منفردة تقسيم كل الخانات في runtime.

## عقود دوال مسندة بتعليمات فعلية
- **1B6770:** ESI destination container، EBX actor؛ يبحث أول hole ضمن capacity. إن امتلأت يرجع AL0 بلا نقل. إن وجد hole يبحث actor داخل container القديم الذي يشير إليه actor+250، يصفر pointer القديم وينقص occupied. يكتب actor+250=destination وpointer الجديد ويزيد occupied ويرجعAL1. لا malloc ولا init هنا. وجود actor قديم غير موجود بالمصفوفة لا يُصلح تلقائياً count؛ افحص الاتساق في probe.
- **1C4A90:** EDX container؛ selector لا يزيل العنصر. flags0 يرجع أول nonzero ضمن capacity، وإلا يصنف المرشحين بواسطة1C4960 مع فلتر actor+39C إذا flags bit4. لا تسمه pop. لم يُحلل عقد1C4960 بعد.
- **1DA1A0:** stack arg1 manager،arg2 actor؛ret8. ينظف موارد ثم ينقل actor إلى manager+79A0 عبر1B6770 ثم يستدعي virtual+0xC. لا heap-free مثبت؛ لا تفترض actor+250 أو vtable وحده إثبات حياة.
- **1DA240:** ECX manager/EAX actor؛وجهة79C0،قد ينظف مرشحاً عند الامتلاء ثم ينقل actor ويضبط actor+384=1. لا تسمها spawn؛ هي انتقال state.
- **1E68B0:** ECX manager/EAX actor؛وجهة79D0،ينقل وينظف موارد ويكتب actor+384=2 ويصفر+8/+C. لا تسمها مجرد free.
- **1E6860:** EDX manager،تعيدAL،ولكن لها آثار جانبية عند الامتلاء (1C4A90 ثم2056A0)،لا تستدعها من الرصد.
- **1B6840:** EAX manager/ECX actor؛يصفر مراجع مساوية لهذا actor في8 مواقع manager+79E4+i*8، **ليس primitive تحرير خانة pool**.
- **1DF440:** EAX manager،وسيطي stack اثنان (ret8)، لا تفترض ESI input السابق. يتحقق من20 flags موارد، يختار وينظف مرشحاً من79B0 إذا امتلأت، يختار actor من79A0، ينقله إلى79B0، يمرر actor كأول stack arg إلى**1DA4F0** عند1DF503، ثم يحضر موارد ويرجع actor فيEAX. دليل قوي على acquisition+initialization path، وليس دعوة مسموحة لإنشاء remote player: يحتاج descriptor وموارد وABI كاملة لم تثبت بعد.
- **1DA4F0** بداية init الصحيحة. يكتب old manager+8524 إلىactor+374 ثم يزيد counter32-bit بواسطةlea eax+1. تم رصد ذلك عند1DA693..1DA6B0. لا توجد حماية overflow في هذه التعليمات، لكن reset/استخدام الهوية في بقية المحرك ما زال مجهولاً. actor+384 يصفّر أثناء init عند1DA6FB. يلزم قبول return/init النجاح قبل إعلان generation جديد.

## ما بقي قبل Probe3
تحقق من reset counter8524 ودالة reset list وبقيةinit حتىret وعقود slots؛تحقق ABI/recompiled function boundaries ثم اصنع read-only snapshot للمصفوفات الأربع والعضويةactor+250 والserial+374 والstate+384، مع upper bounds المثبتة وsafe read ورفض count>capacity وتكرار/مؤشر خارج pool. لا تطلق spawn/despawn على فجوة LOD. لا يكفي world-address وحده epoch إن أعادت المرحلة استعمال العناوين. الرصد قد يفوت transitions سريعة؛ logs يجب أن تعبر عن observation فقط.

## قابلية الإعادة
BLACK_OWNER_XBE=/path/default.xbe python coop/analysis/owner_xbe_static_slices.py 1b6770:1b67ec 1c4a90:1c4b17 1df440:1df593
ابدأ تفكيك1E69B0 من بدايتها أو حدود تعليمات مؤكدة؛ slices1E6B50 و1DA750 تبدأ منتصف تعليمة وأنتجت garbage في أول أسطر،استُبعدت هذه الأسطر من الأدلة. لا ترفع ELF المؤقت أو disassembly الكامل/بيانات اللعبة.


## إثبات reset — متابعة19
فُككت الدالة1D5E00 حتىret عند1D5F29 على حدود مؤكدة. EDI=manager. قد ترجعAL0 مبكراً بناءً على global2D45D8+B14/B15 أو1D52A0. على المسار الناجح تصفر قوائم79A0/B0/C0/D0 وoccupied،تمشي32 خانة بدءmanager+A0 بخطوة3C0،تصفرactor+250 ثم تنقل كل خانة إلى79A0 بواسطة1B6770. عند1D5F08..1D5F0E تكتب **manager+8524=1**،وتعيدEAX1. إذن reset ناجح يعيد serial إلى1؛world/manager pointer قد يبقى نفسه،ولا يجوز تعريف persistent identity بالـserial وحده.
مرجع call مباشر مثبت عند1EDED0: يحملglobal2D1980 إلىEDI،يناديreset،يختبرAL،ويسير state+5AE0 من7 إلى8 عند النجاح. معنى المهمة/الcheckpoint الكامل ما زال بحاجة runtime.
فحص كامل instruction disassembly بحثاً عن displacement0x8524 وجد أيضاً **1B68F0** (ECX manager) يعيدcounter القديم فيEAX ويزيده؛لا call مباشر لها في مسح تعليماتcalll إلى العنوان،وقد تستعمل بشكل غير مباشر. هذا ليس إثبات عدم استدعائها. كذلك inline counter increment داخلinit1DA4F0 كما سبق. المسح النصي ينتج false matches من عناوين0x85240 وتعليقات الأقسام؛استبعدت،ولا نزعم حصر كل reset أوكتابة aliased.
1E69B0 حتىret1E6EEB تمت مراجعتها ولا تحتوي كتابة صريحة إلى+8524 في ذلك العرض؛تهيئة قوائمها وحدها ليستserial reset.
**عقد Probe3 المقترح:** مراقبة reset wrapper بعد الأصل وإضافةepoch فقط إنAL!=0؛مراقبة المدخل وحده خاطئة لأنreset قد يفشل. يجب حفظ host state ونتيجة السياق تماماً. إن بقيتsnapshot-only،سجلcounter regression كإشارةreset غير كاملة؛فقد يفوت reset من1إلى1 بين عينتين. لا تطلق persistent IDs أو spawn/death بناءً على هذه الإشارة وحدها.
