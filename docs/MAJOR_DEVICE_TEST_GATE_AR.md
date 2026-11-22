## BO-COOP Session47 — Actual original D3D8 bound draw source observer (2026-11-22)
**Only branch experiment/black-coop**. New `2ef9fd50` observes existing original DrawVertices/DrawIndexedVertices before unchanged backend draw. Requires `gpu::State.vs`, nonzero bound vertex-buffer, active stream stride36 equal to declaration stride36 and positive count; generic packed offsets +12/+20 only count when on same stream. Exactly three atomic one-time fixed safe codes 10..12 (indexed draw, nonindexed draw, packed +12/+20). Not raw assets/shader/IDs/actor/pointers, not original MODEL identity or skin weights, does NOT assert STORY_READY.
**CI** [native C++ ASAN+UBSAN 44967390703 SUCCESS](https://github.com/alwansan/BO/actions/runs/44967390703); first stage checks 44967390711/44967416619 failed introduced Python literal quoting (fixed `ac156ea7`); [final source-stage 44967448413 SUCCESS](https://github.com/alwansan/BO/actions/runs/44967448413); [isolated UDP 44967448396 SUCCESS](https://github.com/alwansan/BO/actions/runs/44967448396); [full Windows Android room-only APK 44967448414](https://github.com/alwansan/BO/actions/runs/44967448414) CHECK LIVE (last inspected in progress). Old44966638815 cancelled due superseding pushes, not a build failure. This is **NO GAMEPLAY**, no named BC1_MAT/TOM body binding, actual original RCB skin, c5 scene-loaded callback/mission stage epoch, remote human ALLY or host shared AI/combat/quests yet.
**NEXT:** establish safe original MODEL resource -> active vertex buffer/draw association; independently verify true game-thread host mission/scene loaded/epoch before any STORY_READY. Then independent human-controlled remote fullbody ally (not NPC), maintain original local first-person hands/camera/weapon, authenticated gameplay wire and single host world. **No small owner phone trials**; one large future two-phone acceptance with combined bounded automatic safe logs. Read `memory/sessions/2026-11-22_bo-coop-47-original-bound-gpu-draw-observer.md`, `docs/ORIGINAL_GPU_DECL_SAFE_WITNESS_AR.md`, RESTORE/STATE.

---

## Session46 actual original D3D8 declaration fixed-code bundle for ONE future big test
Added a source-only read-only observer after the pinned original `gpu::ParseDeclaration(*vs)` in staged Android `src/d3d/d3d8.cpp`: fixed `GPU_V36_DECL_CANDIDATE` and `GPU_V36_SLOT12_PACKED_DECL`, `GPU_V36_SLOT16_PACKED_DECL`, `GPU_V36_SLOT20_PACKED_DECL`, `GPU_V36_SLOT24_NORMPACK_DECL`, `GPU_V36_SLOT28_FLOAT2_DECL` codes, each at most once per running process. The codes identify **shader declaration categories** only and do NOT assert that this shader was bound to a particular original body MODEL or scene, NOR infer skinning weights. C++ ASAN/UBSAN `44966575850 SUCCESS`, source staging CI `44966623833 SUCCESS`, Windows room-only APK compile `44966638815` CHECK LIVE. NO standalone phone mini-test is justified. In the future major two-phone real co-op acceptance, read these codes along with host-selected mission/real engine loaded scene, human remote body/animation ownership, authenticated gameplay/AI/quest events; all safely bounded. `docs/ORIGINAL_GPU_DECL_SAFE_WITNESS_AR.md` documents exact limits; never export raw shaders, vertex words, matrices, PIN, peer IP, game assets or saves.

---

## Session46 original skinning research is CI only (NOT a user phone-test trigger)
Owner original stride36 38,917 sampled indexed vertices/6 lanes verified privately CI `44965837041 SUCCESS`, synthetic rig/4-weight pose contract CI `44966105790 SUCCESS`. The +12 byte-sum≈255 and +20 small-byte hypotheses do NOT prove original skinning nor real remote human body. **No physical owner test now.** During a future substantial joint story/human body test automatically log only fixed categories: true engine game-scene/stage/epoch proof, actual character shader vertex declaration kinds/offset categories, human remote body generation and material/pose status, authenticated input/snapshot, host AI/objectives/combat. Reject logs with raw shader, bytes, positions, PIN, peer addresses, saves or original game assets. Continue CI source tests and static safe owner data checks.

---

## Session45 additional CI gates, still NOT a phone test trigger
Owner original RCB `44965062874 SUCCESS` (9 serialized skeleton records but 1 per-file distinct ID across 3 stages), animation bank `44964889359 SUCCESS` (22 bank, 675 clip and 61,615 track boundary checks), MODEL stride36 `44965229133 SUCCESS` (12 bodies, each 3 stride36 buffers). These are offline asset STRUCTURE proofs, **not** fullbody animation/renderer running on phone, actual host story scene or synchronized combat. They add no owner trial. The eventual one major 2-phone acceptance should automatically capture safe fixed codes for host mission attempt, true scene epoch/loaded proof, both client scene state, remote human body generation/visible animation, bounded input/snapshot status and world/combat/objective progress. Do not export raw game model, animation, peer IP, room PIN or save.

---

## Session43 event witness prepared for ONE future substantial phone acceptance (not now)
The staged Android room-only source includes three **automatically collected bounded fixed codes**, using existing original BLACK source wrappers with no guest writes:
`WORLD_MISSION_LAUNCH_ATTEMPT` (max eight), `WORLD_LOCAL_PLAYER_UPDATE_SEEN`, `WORLD_POINTER_CHANGED_UNVERIFIED`. They are emitted from original mission attempt/player update callbacks through JNI to `RoomDiagnostics.event`, which uses a bounded async disk worker. They contain NO mission ID, world pointer, character coordinates, IP, name, PIN, raw XBE or scene. **NONE of these alone proves actual stage loaded or authorizes STORY_READY**. This enables correlating future major user sessions' mission request and first local gameplay update without asking the owner to test another small button or manual log. C++ synthetic guard CI44962215874 and source staging CI44962275672 both PASS; full Android APK build for new native wrappers must be separately confirmed, currently CI44962611024 check live. No install request until actual host world and human remote AllyBody are implemented and CI verified.

---

## CI achievement: safe combined log export verified without owner participation (2026-11-22)
Run `44957104337` Android29 **SUCCESS** with `ROOM_LOG_ROTATION_PASS`: automatically generated 900 synthetic safe events; previous+current rotated room event log exported once through MediaStore; began/ended markers found; tampered previous log refused. TLS/Keystore and rehost also PASS. This validates ROOM transport diagnostics, not the FUTURE real BLACK scene/actor/combat/world instrumentation. Do not ask owner to test until the actual substantial shared-world milestone is ready.

---

# BO-COOP — سياسة تقليل تجارب المالك وتجربة القبول الموحّدة (2026-11-22)

## توجيه المالك الملزم
يفضّل المالك الاعتماد على GitHub Actions والمحاكيات والاختبارات الصناعية وتحليل المصدر إلى أقصى حد. **لا تُطلب منه تجربة هاتف إلا عند إنجاز مرحلة تكامل كبيرة يصعب إثباتها من CI**. كل التفاصيل الصغيرة التي قد نحتاجها تُجمع تلقائياً في logs آمنة ومحدودة أثناء **تجربة قبول واحدة**، بدل الطلب المتكرر لتجربة زر أو شاشة أو إشارة مفردة. لا تُكرر طلب ISO أو التجربة المؤكدة لنموذج اليدين. لا يُطلب تثبيت APK الغرفة وحدها.

## البوابة الإلزامية قبل طلب تجربة هاتف
1. تأكيد تطابق المصدر مع صورة المالك c5 ومعمارية التطبيق، وخلو البناء من أعطال CI، ونجاح اختبارات Java TLS/Android KeyStore والمصدر C++ تحت ASAN/UBSAN.
2. وجود رابط رسمي موحّد لحزمة Android حديثة **على الهاتفين نفس البروتوكول**، مع هوية commit وبصمة APK وتوقيع وسبب تجربة ذي قيمة.
3. مسار قصة مضيف حقيقي في محرك BLACK، اختيار مهمة 0..7 من اللعبة الأصلية، وتحميل scene فعلي للضيف والمضيف، وعدم المزج بين عالمين فرديين. يجب إثبات game-thread scene epoch قبل إرسال READY، لا الاكتفاء باللوبي.
4. على الأقل تجربة مفهوم مفيدة للمشاهد الآخر: مجسم حليف كامل مملوك للاعب البشري البعيد، مع تحكمه بالشبكة، من دون brain NPC أو نسخ ممثل حي. المحافظة على اليدين والكاميرا والسلاح الأصليين على كل هاتف.
5. تشخيصات تلقائية مدمجة تكفي للإجابة عن الأسئلة الصغيرة، ومفحوصة صناعياً دون البيانات الخاصة.

**الحالة الآن: البوابة 3 و4 و5 غير مكتملة. لا حاجة لأي تجربة من المالك.**

## حزمة الأدلة الموحدة المطلوبة قبل تجربة القبول
الحقول الآمنة العامة (كلها رموز أحداث وأرقام صغيرة دون أسماء/هوية/عناوين):
- build/profile/protocol/schema ونسخة البرنامج ونتيجة فحص الأصول محلياً (bool دون معلومات الأصل).
- حالات host/scan/auth/roster/rehost/leave ومقاييس الأوقات والمدد الإجمالية وقطع الاتصال.
- مرحلة القصة: HOST_CHOICE, STORY_PREPARE, SCENE_LOAD_BEGIN, SCENE_LOADED_VERIFIED, READY, COMMIT, ABORT وسبب الرفض *فقط كرمز*، مع رقم المهمة 0..7، هوية المشهد المحلية غير الحساسة/عداد epoch، وعدد البشر 1..4.
- مكان اللاعب البعيد المرئي: حالة إنشاء/حذف renderer والتبعية للإنسان ووجود rig/bones/material/animation بحقول منطقية/عدادات؛ دون أسماء مفاصل أو بيانات مجسم.
- مقاييس حركة/إدخال/سلاح/AI/ضرر/تقدم هدف المرحلة مع عدادات وعدد الرسائل المرفوضة، لا مواقع أو لقطات خام.
- مستوى الأداء والتحمل: حدود frames, ping/jitter/loss، انقطاعات، أخطاء JNI/Crash/ANR على شكل تصنيفات، وليس ملفات مستخدمين.
- الالتزام بالحجم: حد/تدوير السجل، تجميع معدل الأحداث المتكررة، عدم الكتابة كل frame أو على خيط اللعبة؛ مستخرج واحد منظّم عند نهاية الاختبار، من كل هاتف، بنفس الجلسة.

لا تسجّل PIN أو IP أو fingerprint أو TLS keys أو أسماء أو رمز غرفة سري أو raw save أو ISO/XBE أو بنية خاصة باللاعب. لا تنشر سجلات خام قد تتضمن هذه البيانات في Actions/artifacts.

## تجربة القبول الموحدة المستقبلية (ليست تعليمات للمستخدم الآن)
عندما يصبح تكامل القصة وRemoteAlly صالحاً للاختبار، تُقدّم نسخة واحدة للهاتفين مع خطة واحدة: المضيف يفتح الغرفة، الضيف ينضم، المضيف يختار مهمة من القصة ويبدأها، الطرفان يرون العالم نفسه والشخصية البعيدة ويختبران الحركة/السلاح/التفاعل وتقدم الهدف، ثم يختبران خروج/إعادة اتصال واحدة آمنة عند الحاجة. بعد انتهاء التجربة يصدران **حزمة أدلة واحدة لكل هاتف** من داخل التطبيق، تُجمع في CL مع الصور المهمة عند وجود فشل بصري. لا تُطلب إعادة التجربة من أجل تفاصيل صغيرة؛ يحلل العميل سجلات التجربة السابقة ويعالجها أولاً.

## دليل التقدم الحالي
- ما ثبت: واجهات اللوبي واتصال 2/4 على هاتفين بإصدار سابق، بروتوكول TLS v3 ومراحل PREPARE/READY/COMMIT واختبارات إعادة الاستضافة الصناعية، واختبار RoomSession facade.
- ما لم يثبت: hook تحميل المشهد الحقيقي على c5، عالم مضيف واحد، جسم حليف مستقل للاعب البعيد، مزامنة القتال والقصة، اختبار v3 على الهاتفين.
- أعمال Actions القادمة: فحص ثابت لمرشحي الدخول إلى المهمة من الصورة المملوكة للمالك داخل Runner من دون تصدير أصولها، مع Tests وتقارير آمنة فقط.

اقرأ دائماً AGENTS.md وprompt/RESTORE_SESSION_AR.md وmemory/STATE.json والخطة الحديثة قبل اقتراح أي تجربة هاتف.