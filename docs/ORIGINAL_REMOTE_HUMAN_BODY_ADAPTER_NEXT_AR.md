### Session67 verified owner-native XBE lookup candidates / separate Android APK still pending
- Original pinned owner c5 SHA-1 `c5bfed25449c8212d7a976f15246e87ead57713f` private XBE [45032934537 SUCCESS](https://github.com/alwansan/BO/actions/runs/45032934537): synthetic positive/negative and owner-private jobs both succeeded. Direct numeric source logs in bounded 768-byte windows: PC name `soldier_visual_init@0x0017E420` executable=1 calls=1 returns=0; `soldier_animation_request@0x0017E130` executable=1 calls=14 returns=3; `soldier_pose_update@0x001801B0` executable=1 calls=16 returns=3. **Caution:** windows may include adjacent functions, and 0 returns in first window means zero *observed* only; none of these counts proves entry ABI/valid call function boundary, animation or rendering. No callable guest API, original asset bytes or disassembly exported. No remote actor created.
- The source-only bot rig plan preflight [45032729987 SUCCESS](https://github.com/alwansan/BO/actions/runs/45032729987); real TLS [45032641442 SUCCESS](https://github.com/alwansan/BO/actions/runs/45032641442), Android29 platform [45032641361 SUCCESS](https://github.com/alwansan/BO/actions/runs/45032641361), UDP [45032729950 SUCCESS](https://github.com/alwansan/BO/actions/runs/45032729950).
- Room-only separate Android build [45032729971](https://github.com/alwansan/BO/actions/runs/45032729971) last checked still IN_PROGRESS in `Compile isolated Android baseline with bounded parallelism`; DO NOT CLAIM APK SUCCESS. It has no shared story even if success.
- Next P0 engineering: actual host-selected stage/game-scene loader, true guest auto-launch, authenticated actual scene loaded epoch, and body from any live original BOT LOOK in own visual slot with engine-safe lifetime/renderer, then combat/AI/mission synchronization. Keep local FIRST PERSON, no NPC brain copied, no small phone tests; one major real-coop owner validation later.

---

### Session67 original owner XBE visual/animation/pose function private audit
- Added `coop/analysis/original_human_look_xbe.py`, synthetic positive/negative tests, and GitHub Action `black-coop-original-human-look-xbe.yml`. Reuses pinned owner c5 XBE read-only within the runner; inspects only 768 private bytes each at PC MIT candidate addresses `0x0017E420`, `0x0017E130`, `0x001801B0`. The workflow checks executable section mapping and bounded decode/field-reference count, never exports raw code, disassembly, guest pointers or owner private files. NO function boundaries, return/register ABI, safe guest call, engine-created remote character or drawable bot is asserted.
- Its next meaningful outcome is whether the 3 PC-source candidate offsets exist in this exact owner XBE; on success it narrows next *dynamic* ABI validation, not a safe permission to call these functions. Maintain one major owner test only when shared story works.

---

# BLACK Co-op — المخطط الهندسي القادم لجسم الإنسان من مظهر بوت اللعبة الأصلي

**مصدر مباشر مراجع:** `portingpete/Black-2006-Recomp@b2512fafd`، الملف `src/recomp_manual.c` (الأسطر التقريبية 2009–2130)، وخطة `docs/plans/split-screen-coop.md`. مصدر PC تجريبي وهو **ليس** برهان صلاحية الاتصال بدوال محرك BLACK على أندرويد.

### ما يفيدنا فوراً
- مصفوفة الشخصيات 32 خانة من `[0x002D1980] + 0xA0`، حجم الخانة `0x3C0`. الحصول على نموذج مظهر من أي بوت موجود. لا مانع إن كان عدوّاً أو حليفاً.
- `black_puppet_make` ينسخ بيانات البوت المرئية إلى خانة مملوكة مستقلة، ويستبعد الحقول الخاصة بقوائم العالم والعقدة والمتحكم والـbuffers الخاصة بالجهة المستهدفة. يزيل `+0x324` متحكم البوت ويمسح أجزاء أخرى، ويجهز النسخة المرئية بـ `sub_0017E420`. هذا **ليس** نسخاً لذكاء البوت أو تحويلاً لعدو قائم إلى إنسان.
- `black_puppet_follow` يجعل المجسم يتبع موضع جسم الإنسان (قدماه عند `actor+0xA0` وليس العين `+0x100`) ويطلب وضع الوقوف/الحركة عبر `sub_0017E130` ثم وضعية الجسم بـ `sub_001801B0`.
- يعيد فحص ملكية الخانة عندما يستولي محرك اللعبة عليها لبوت جديد. هذا مهم جداً مع الأدلة الفعلية من HOOK001 أن إشغال الخانات يتغير أثناء اللعب.
- يحتفظ اللاعب المحلي بمنظوره الأصلي الأول وسلاحه؛ يظهر مجسم البوت **للآخرين فقط**. كل لاعب عبر الشبكة يمتلك خانة وحالة مستقلة، والخادم هو السلطة على الحركة والضرر والأهداف.

### ما لا يجوز افتراضه
لا يجوز نسخ هذه الإجراءات من PC إلى أندرويد مباشرة، أو استدعاء دوال الضيف أو الكتابة إلى خانة من دون التحقق من نفس نسخة XBE (SHA1 c5)، ABI الاستدعاء، تهيئة مراجع النموذج/المخازن، سياق الخيط الأصلي الفعلي وإشارة المشهد الجاري. قيمة `core+0x5AE0 == 0x1C` وفحص الملفين وقراءة الحقول ولو تطابقت ليست وحدها إذناً لإنشاء جسم أو إشارة READY.

### التنفيذ التالي
العمل أولاً على إثبات مصدر دوال `0017E420 / 0017E130 / 001801B0` وطريقة استدعائها في Android guest، والتحقق الذري من أن خانة الهدف فارغة وصالحة قبل تهيئة أي مجسم. ثم التحديث والرسم على خيط اللعبة الفعلي من مواقع الجسم التي يقررها المضيف بعد COMMIT، مع تطابق التذكرة والمشهد والجيل؛ وبعدها مزامنة التصادم والعدو والقتال والأهداف. لا تُطلب تجربة هاتف صغيرة؛ تُجمع مؤشرات المشاكل تلقائياً في تجربة كبيرة واحدة بعد توفر عمل جماعي فعلي.
