# خريطة نقل اللاعب الثاني إلى محرك BLACK الأصلي على Android

Session109 — 2026-11-23. هذه خريطة مصادر وعقود تنفيذ، **وليست** تقرير قبول اللعب التعاوني.

## المراجع المثبتة

- مرجع BLACK PC الأصلي MIT: [`portingpete/Black-2006-Recomp@b2512fa/src/recomp_manual.c`](https://github.com/portingpete/Black-2006-Recomp/blob/b2512fafd3e181e4febfb298f297245b845ef628/src/recomp_manual.c)؛ السطور: `black_p2_start` عند 1920، `black_p2_make` عند 1968، `black_p2_take_proxy` عند 2134، `black_p2_setup` عند 2178، `black_p2_tick` عند 2393، `black_p2_swap` عند 1812، مخازن الصوت/anim قبل ذلك.
- مصدر Android المثبت: [`RanggaCasper/Black-Android@f3754d6`](https://github.com/RanggaCasper/Black-Android/tree/f3754d6985a8c35204e7df071546e586b10f1806)، SDK مختلف عن Windows PC: `TGP482/XboxXBERecomp@99078b1`.
- كود BO الحالي `coop/room_network/room_original_host_player_storage.hpp` و`room_original_host_player_storage_kernel.hpp` يملك تخصيصات منفصلة بطول `0x16C0` مالك الكاميرا، `0x900` actor، `0x40` input، `0x40` weapons، `0x630` أجزاء/موديل. `room_original_host_proxy_engine.hpp` يملك فيزياء player slot بعد original setup مع `OriginalProxySceneLedger`. جرى تجميعها مع Android في [45206563984](https://github.com/alwansan/BO/actions/runs/45206563984)، لكن **لم تُفعّل بعد**.

## ماذا يفعل مرجع PC فعلاً

1. عند ثبوت original story running phase`0x1C`، يخصص كتل actor وcamera وweapons وparts منفصلة؛ يشغّل مُنشئ الكاميرا الأصلي `0x14380` بالـESI المناسب.
2. ينسخ حالة player1 إلى player2 actor shadow، لكن **يعزل مخزن الأسلحة** ويصحح self-links وcamera owner pointers وpart buffers؛ لا يُسمح بنسخ lifetimes أو intrusive collision lists للمالك الأول.
3. عند تهيئة الشخصية يستخدم original engine routine `0x1E0E40` عدة updates، عبر تبديل آمن لسياق اللاعب والكاميرا وسلاحه والأنيميشن والصوت. **يجب** إعادة player1 context بعد كل خطوة حتى لو لم يكتمل setup.
4. بعد التهيئة يطلب proxy جديداً من محرك الفيزياء الأصلي `0xDF400`، ويطلقه بـ`0xDBBE0` مع إثبات الاستقلال وعدم سرقة آخر slot من NPC. كما يجهز مجسم soldier كـrepresentation مرئي للآخرين.
5. على Android لا يكفي نقل تلك العناوين: نقل PC `black_guest_call` أو نسخ shadow حرفياً يتجاهل ABI مختلفاً والمراجع الفعلية ودورة حياة original cache؛ ينبغي استعمال `kernel::CallGuest(X86Context&, ...)` من SDK Android المثبت وبمراجعة register/return ووظائف fallback.

## بوابات القبول للتنفيذ القادم

- **مرحلة A:** original host game-thread يثبت original world/episode/physics/NPC pool والمشهد المحلي (لا تساوه بـroom TLS epoch) ويحتفظ `OriginalProxySceneLedger` طيلة lifetime الأصلي، ولا يخلق ledger جديداً بعد فشل native call.
- **مرحلة B:** إنشاء `OriginalHostPlayerStorage` مستقلاً لكل إنسان (max3 إضافيين)، إثبات owner+actor allocation عبر `OwnsActor` وعدم تقاطعها مع P1/core/NPC/physics، ثم تهيئة camera owner وإنشاء **full swap bundle**.
- **مرحلة C:** تبديل P1/P2/P3/P4 atomic context بما في ذلك actor/owner/camera/weapon slots/animation/voice/banks؛ استدعاء original setup `0x1E0E40` فقط بعد إثبات كل تلك الأجزاء. الاستثناء أو partial native side effect يوجب حجر المشهد. يجب إثبات إعادة P1 إلى سياقه قبل أي frame جديد.
- **مرحلة D:** real player proxy، AI target ownership والـsoldier model pose/animation، إدخال input حقيقي لكل لاعب على خيط اللعبة الأصلي، لا مجرد TLS packet logs؛ لا يزوّر لاعب/هدفاً/جاهزية.
- **مرحلة E:** المضيف وحده يقرر original NPC/weapon damage/doors/objectives وmission progression، وAndroid guest يطبّقها على نفس المرحلة الأصلية بعد مطابقة host-authoritative epoch/asset hashes، لا أربع مراحل منفصلة.
- **مرحلة F:** CI يفحص السلوك الآمن حتى الحد الممكن، ثم **تجربة هاتفين أو أكثر واحدة كبيرة** يقود فيها HOST START كل اللاعبين إلى **نفس original BLACK stage** وتظهر أجسامهم ويتزامن القتال. لا تطلب اختباراً لتفصيل بسيط.

**الحالة الصادقة 2026-11-23:** لا توجد original actor2–4 initialized runtime ولا authoritative NPC/door/objective guest native application بعد. بناء APK الحالي نجح فقط كـroom lobby وشيفرة مساعدة source.

راجع كذلك `docs/ORIGINAL_PC_HDD_PARTITION_PROBE_AR.md` لإصلاح إقلاع مختبر Windows **المنفصل عن Android**. لا تلمس main، CL، biso، ولا ملفات المستخدم الأصلية أو raw logs.
