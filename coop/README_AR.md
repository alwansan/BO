# BLACK Co-op: مرحلة الجدوى
الفرع التجريبي فقط؛ main محفوظ. الهدف: شخصيتان مستقلتان في مهمة القصة، ثم 4 لاعبين؛ المضيف هاتف اللاعب. لا وجود لطور جماعي مثبت حالياً.
## الأدلة
- BO: AndroidManifest يثبت الحزمة com.racas.black.arabic وصلاحية INTERNET. BLACK_FILE_MANIFEST.json: libmain.so (72073216 bytes)، game.iso (1793179648 bytes، 39 أجزاء)، نسختان لهما نفس مسار الأصول الشجري.
- مرجع المصدر: https://github.com/RanggaCasper/Black-Android (إعادة ترجمة Xbox XBE، وليس محاكي تقليدي). مراجعة src/game/{hooks,esp,weapons}.cpp وconfig/game.toml تشير إلى نقاط مراقبة الشخصيات والأسلحة وإلى افتراض لاعب محلي واحد. لا تنقل offsets إلى Arabic قبل مطابقة XBE.
- Eden: https://github.com/eden-emulator/mirror/blob/master/docs/user/Multiplayer.md ; يحاكي LDN موجود مسبقاً في ألعاب Switch.
- النقل: https://docs.zerotier.com/protocol/ ؛ https://tailscale.com/docs/reference/connection-types ؛ https://github.com/ValveSoftware/GameNetworkingSockets/blob/master/README_P2P.md
## الخطة وبوابات القبول
1. NET-001: UDP localhost بإدخال لاعبين مستقلين وحالة مضيف مشتركة (محاكاة مجردة ليست اللعبة). تشغيل python3 coop/net_probe.py selftest. يسمح أيضاً بهوست وعميلين يدويين على LAN/ZeroTier عبر عنوان المضيف. لا تشغله على الإنترنت العام؛ لا تشفير أو مصادقة في هذا المختبر.
2. SRC-001: تحقق SHA256 حسب manifest ثم استخرج default.xbe من نسخة مرخصة؛ طابق SHA1 مع upstream وحدد commit ومناطق تعديلات Arabic. فحص عناوين الذاكرة في المصدر، لا افتراض التطابق. إعادة بناء upstream وقياس baseline دون تعديل gameplay.
3. HOOK-001: قراءة حالة اللاعب المحلي فقط وعرضها في سجل محلي (position, yaw, pitch, mission, health, weapon). فصل thread الشبكة عن game thread باستعمال queues محدودة.
4. ACTOR-001: إنشاء شخصية proxy ثانية باستخدام منشئ الشخصيات/نموذج جندي موجود، دون تبديل singleton اللاعب أو الكاميرا، ثم تأكيد الحركة والرسم والتنظيف عند إعادة تحميل المهمة. لم يثبت هذا بعد.
5. COMBAT-001: المضيف يحكم إصابات/RNG/AI/ضرر، الزبون يرسل input ويرسم snapshots مع interpolation. مزامنة التحطيم والأبواب والمخزون والأهداف وcheckpoint؛ هوية stable ID لا pointers. تجربة مهمة واحدة كاملة ثم بقية المهام.
6. NET-002: LAN unicast + discovery، ZeroTier/Tailscale عبر IP، ثم ICE/STUN/TURN اختياري. لا سيرفر ألعاب مخصص؛ relay/control plane قد يحتاج طرفاً ثالثاً. اختبار أجهزة وشبكات مستقلة. بروتوكول إنتاجي موثوق/غير موثوق ومصادق عليه (ENet أو GameNetworkingSockets حسب اختبارات Android).
7. UI: إنشاء/انضمام/جاهزية/اختيار مهمة/كلمة مرور/اتصال وping. طور القصة الفردي دون تغيير. لا host migration مبدئياً.
## حدود المختبر
لا يقرأ ISO أو يحقن مكتبة أو يشغل BLACK. لا يثبت إمكانية لاعب ثان داخل المحرك. ملف Python مستقل لا حاجة إلى تنزيل ملفات اللعبة لتشغيله؛ CI يستخرج ملف المختبر وحده من Git، لا يعيد تجميع ISO. لا تنشر أصول اللعبة المحمية في artifacts؛ استخدم نسخاً مرخصة.
