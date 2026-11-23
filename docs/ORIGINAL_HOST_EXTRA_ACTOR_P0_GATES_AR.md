# BLACK Co-op — بوابة التحويل إلى لاعب أصلي فعلي داخل محرك الهوست

تحديث جلسة 88 — 23 تشرين الثاني 2026.

**المطلوب:** لاعب واحد أصلي عند `core+0x30` و1–3 لاعبين إضافيين بشخصيات أصلية تمتلك physics proxy مستقلاً وتستعمل input من اتصال TLS وتتحكم بالأسلحة والأضرار في **محرك هوست واحد**. وجود مجسم مرسوم فقط أو مصدر مواقع ليس نجاحاً.

## التحقق الذي أصبح موجوداً
- بيانات TLS تحمل جلسة موقّعة وCOMMIT وموقع/حركة/اتجاه وحالة أزرار لاعبين 2–4.
- الاتجاه صار مستخرجاً من متجه إطلاق الرصاص الأصلي `player+0xF0`، بعد التأكد من finite والمجال. لا تستعمل زاوية عصا Xbox على أنها دوران اللاعب الفعلي.
- `OriginalHostExtraActorPreconditions` يجمع شرط original stage match وCOMMIT epoch المنفصل عن input epoch مع fresh commands ومساحة الفيزياء/الـAI/المجسم؛ `2127` معناه **سعة مرشحة فقط** لا Actor.
- XBE الأصلي المثبت هويةً لديه مداخل وظائف مرشحة: `sub_00014380` camera owner setup، `sub_001E0E40` player setup، `sub_000DF400` physics proxy alloc، `sub_000DBBE0` release، `sub_001F1080` player tick. التدقيق الخاص `45095558435` يثبت وقوع عناوين التنفيذ داخل صورة XBE فقط، وليس calling convention الآمن على Android.

## ما يجب تنفيذه فعلياً بالترتيب (لا تتخطَّ السلامة)
1. **Player 2 فقط أولاً**: أنشئ guest-safe owned shadow actor مستقل (PC source `0x900` bytes) وcamera owner مستقل (`0x16C0`) وinput source (`0x40`) وweapon array (16 pointers)، وقسم animation/part buffers مستقل داخل الحاوية الصحيحة، بعد إثبات ABI ومالك الذاكرة واستعادة المشهد عند unload. لا تلمس world grid linked list المملوكة للـcore الأصلي.
2. وصل full controller input الموثق من peer2 بالـoriginal actor وليس Java preview. `OriginalHostHumanInputs().Fresh(2)` يعرض *أوامر قيمة فقط*؛ لا يُعتبر Player2 تحرك حتى يشغّل ORIGINAL player movement engine instance ويملك proxy.
3. أنشئ physical proxy بواسطة API الأصلي بعد حراسة pool وABI وlife-cycle؛ احجز خانة للأعداء. لا تكرّر سياق المالك الأصلي داخل المخزون نفسه. في الانسحاب أو موت/تبديل المرحلة: نظف كل actor/weapon/proxy وأعد أسلحة/صوت/كاميرا الهوست الأساسي.
4. شغّل original player update ثانية مع context-safe swap يحفظ self-linked nodes والـowner/weapon+model/animation banks and audio, original game code and x86 context. لا تنسخ `memcpy` على كل `core+0x30`: روابط الـcollision/list ستفسد. مرجع PC المصدر `portingpete/Black-2006-Recomp@b2512fafd3/src/recomp_manual.c` موثّق.
5. اربط original AI target registry 32×12 بشخصية P2، ثم اختبر إصابة P2 والأعداء وطلقاته (المرجع PC نفسه ما يزال قاصراً في AI targeting). لا يكفي إضافة record مفردة.
6. عمّم P2 إلى peer3 وpeer4 من دون مشاركة owner/weapon/state. استخدم منظومة HOT START/COMMIT الجديدة حتى يبدأوا نفس المرحلة، وابنِ actor cleanup/disconnect/reconnect.
7. **عالم المضيف فقط يحسب القتال/الأبواب**: عرّف native stable door instance identity وnative original destroy hook producer، وطبّق أحداث TLS v9 عبر original guest engine APIs؛ لا تستخدم raw pointer أو NPC slot المحلي كهوية عبر الشبكة. ثم شارك أهداف/checkpoint والـcutscenes.
8. CI والتجربة: sanitizer + real TLS 2–4 + native compile + Windows ARM64 APK جميعها نجاح، ثم **تجربة هاتفين كبيرة واحدة** عندما يعمل اصلاً P2/القتال/الباب/الأعداء. لا تطلب تجربة تفعيل عارض فقط.

## فاصل القبول
نجاح CI على `45096922154` يعني **source gating compiling**, ولا يعني وجود لاعب إضافي في محرك BLACK، ونجاح `45096922149` إن تحقق فهو APK تجريبي يحوي lobby أصلي وليس تعاوني حقيقي. يجب الإبلاغ بدقة عن هذا الفصل.

