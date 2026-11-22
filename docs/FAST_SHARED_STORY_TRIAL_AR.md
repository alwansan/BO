## Session52 (2026-11-22): priority playable host story; ANY BOT LOOK temporary
DO NOT block on original ally identification, skinning, green crosshair or correct animations. Remote 2/3/4 human may display any existing original BLACK bot enemy/friend, never copy NPC AI or its live actor. Local player is original FPS.
Implemented real encrypted `RoomWire` v5 host-only `WORLD_SNAPSHOT` type13, bounded up to 4 players ticket/epoch/roster validation, latest-only fanout and 350ms TTL. [Host+3 TLS CI44972561727 PASS](https://github.com/alwansan/BO/actions/runs/44972561727), but game positions MOCKED. New original player camera eye (upstream `kPlayerEye=0x100`) staging native source read-only/10Hz, JNI to Java RoomSession, [44972768370 SOURCE PASS](https://github.com/alwansan/BO/actions/runs/44972768370); not actual phone runtime, NOT actor root/scene loaded. Real selected mission and guest stage loader remain BLOCKERS; owner pinned XBE mission candidate xrefs [44972978138 private PASS](https://github.com/alwansan/BO/actions/runs/44972978138), xref refinement first CI44973072176 failed only synthetic test length, fixed7fd43b5e. Read `memory/sessions/2026-11-22_bo-coop-52-real-original-eye-host-world-v5.md`, `docs/HOST_WORLD_SNAPSHOT_V5_AR.md`, `prompt/RESTORE_SESSION_AR.md`, `memory/STATE.json`. Android run44972799329 CHECK LIVE, room-only NO GAMEPLAY. One major owner phone trial when real same host-selected original mission/players move together; avoid all minor phone tests; safe automatic logs. Only write BO `experiment/black-coop`.

---

# BO-COOP — خطة نسخة القصة الجماعية السريعة
**2026-11-22 — توجيه المالك الجديد له الأولوية على اختيار مجسم الحليف.**

**الهدف:** يدخل لاعبون 2–4 غرفة مضيف فعلي، المضيف يبدأ القصة، كل الهواتف تحمل *المرحلة ذاتها* وتحافظ على *عالم مضيف واحد* وتُشاهد حركات بعضها. اللاعب على هاتفه يبقى بمنظور الشخص الأول الأصلي. من بعيد يُمثّل بـ**أي جسم بوت أصلي عدو أو صديق** مؤقتاً (حتى لو نفس الشكل لكل اللاعبين). لا تنتظر الحليف الصحيح أو الحركات والألوان والجلد (Skinning). لاحقاً تُصحّح الأشكال.

## المنفذ والمختبر حالياً
- RoomWire VERSION/PROFILE 4؛ حزمة input ثابتة 43B بعد عنوان إطار 8B، ضمن TLS الحقيقي، session/token/roster revision/scene epoch/seq والاتجاه والتصويب والقفز/النار. لا يوجد player ID داخل البايتات: المضيف يستنتجه من TLS socket.
- `RoomTls.Server` لا يقبل أي حركة قبل `STORY_COMMIT` ولا من عضو غير حالي؛ 60 frame/s كحد داخلي لكل ضيف، واحد أحدث عينة لكل لاعب بـTTL250ms؛ رفض القديم والعودة للمهمة السابقة. `RoomTls.Client` يضغط الحركة إلى queue من عنصر واحد على عامل إرسال خلفي (game thread لا يرسل عبر socket). `RoomSession` واجهة game-thread.
- `RoomHumanInputTlsTest` على 4 اتصالات حقيقية (مضيف + 3 ضيوف) نجح CI <https://github.com/alwansan/BO/actions/runs/44971688698>؛ مرحلة loaded/commit في الاختبار موثقة **اصطناعياً فقط**. لم يتحرك ممثل في BLACK بعد.
- `TemporaryBotVisualPolicy` عند `coop/bridge/temporary_bot_visual.hpp` يسمح ANY verified renderable original bot source (عدو/صديق) لكل human remote، ويمنع self/scene/generation أو NPC AI؛ C++ ASAN CI <https://github.com/alwansan/BO/actions/runs/44971624673>، **لا يرسم فعلياً**. لا تستبدل ممثل NPC الحي بلا حراسة lifetime/ownership.
- آخر اختبار APK/Windows <https://github.com/alwansan/BO/actions/runs/44971762426> يحتاج LIVE recheck؛ مهما نجح، النسخة الحالية عنوانها **NO GAMEPLAY**.

## P0 — ما يُنجز الآن بالترتيب
1. Hook لمهمة المضيف من اختياره الأصلي مع **دليل حقيقي** على تحميل world/scene + scene epoch (ليس pointer أو tick فقط).
2. عند PREPARE من المضيف، يبدأ الضيوف تحميل نفس mission/stage داخل لعبة BLACK؛ لا يبدأ كل هاتف قصة مستقلة. READY فقط بعد تأكيد اللعبة الحقيقي؛ ثم COMMIT من المضيف.
3. توصيل مسار TLS الجديد بمصدر يد الحركة والسلاح الأصليين وبحالة host-authoritative transform/animations الأساسية؛ لا إرسال بيانات اللاعب/المشهد قبل تحقق التذكرة.
4. إنشاء *هيئة عرض مستقلة عن الـNPC AI* لأي إنسان بعيد باستعمال أي بوت original model ثبت وجوده في stage. يُرى المضيف كبوت من هاتف الضيف وباقي الضيوف من المضيف.
5. بعد حركة لاعبين وسلوك مهمات/عدو موحد في عالم المضيف، **تجربة هاتف كبيرة واحدة**: المضيف + ضيف ثم 3 أو4، وسجلات آمنة محدودة تلقائية على كل هاتف.

## P1 — لاحقاً
تصحيح هيئة الحليف بالتفصيل، rig/RCB، الشيدرات، الأطياف، الأنيميشن، الأخضر وعدم إصابة الصديق، مؤثرات الماء/القتال.

**لا تخدع المستخدم:** room TLS وCI لا يثبتان لعب القصة. BUILD الحالي no gameplay. حافظ على المنظور الأصلي للاعب المحلي، ملفات ISO الأصلية، وعدم الكتابة إلى BO main أو CL أو biso.
