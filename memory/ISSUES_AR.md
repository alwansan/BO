## Work51 — حدود إثبات جسر اللعبة (2026-11-27)

- نجح إدخال TLS إلى P2 عبر جسر محلي خاص. لم يثبت ربط `BlackActivity`/JNI بالممثل. طابقت عمليتان أصليتان الجزء المؤلف؛ حالة التدمير وأثر التصادم ما زالا دون إثبات. [التفاصيل](sessions/2026-11-27_bo-coop-work51-native-input-and-prop-scope.md).

## Work49 — سجلات مشتركة تمنع تفكيك P2 المباشر (2026-11-27)

- أعاد المحرك حجزي سلاح P2 وأخرج كائناته الصوتية العشرة من السجل. أثناء تحديث P2 بقيت ثلاث قيم غير صفرية عند `+0x08` في السجلات المشتركة مع P1؛ يمسحها التفكيك الأصلي. مساحة العرض المتبادلة مثبتة، لكن يلزم عزل السجلات والمرفقات قبل التفكيك. لا تحرير شامل، ولا إثبات لتشغيل الصوت الفعلي أو تزامن callbacks.
- حرر المختبر صف السلاح والصف الفرعي بعد إثبات أن الرابط الفرعي الخارجي صفر واستعادة صف HUD المطابق. يلزم نطاق HUD آمن في المنتج وإثبات مسار الموارد الفرعية غير الصفرية قبل التعميم.
- نجح التفكيك الأصلي في المختبر بعد إزالة مؤشرات السجلات المستعارة من P2 وحده. بقيت الكاميرا وذاكرة الممثل؛ يلزم تحقق مستقل من الموارد المملوكة والتحرير والمتابعة في الإطارات الأصلية.
- فحص 16 مجالًا للمحرك لم يجد تطابقات خارجية للتخصيصات الخمسة. أُعيدت قراءة الصفحات الملتزمة بلقطة قبل النشر: بقيت كلمات الكاميرا الثلاث نفسها، ولم يظهر تطابق للممثل أو تطابق جديد بعد الاعتزال. لم يُحرّر أي تخصيص؛ المسح لا يغطي مراجع المضيف أو المؤشرات غير المحاذية.
- في تجربة لاحقة حرّر المختبر أربع كتل وأعاد تخصيصها بعناوينها وأجيال جديدة. بقيت عينات الكتل البديلة مطابقة 240 إطارًا. الكاميرا بقيت حية، ومسار تحرير المنتج لا يزال يحتاج مزودًا مثبتًا لدورة العمر.

## Work48 — حدود الدليل المتبقية (2026-11-27)

- ثبتت في مختبر Android حالتا تفعيل هدف `0 → 1` واكتمال بسيط `1 → 3` دون تغيير العداد. لم يثبت انتقال مرحلة أو عرض HUD، ولم يتحقق APK اللعبة. اختبار التصدير على المحاكي استعمل APK اصطناعيًا.
- صحة جزء حي انخفضت في استدعاء اختباري؛ لا إزالة تصادم مؤكدة. تحققت إعادة الاستخدام الطبيعية للهدف والوكيل والجسم وصف الحركة، لكن فحص المراجع شمل مجالات محددة. يبقى إثبات تقاعد بقية الموارد والتحرير الكامل والعرض وربط APK اللعبة وعالم الشبكة.

## Work43 — اكتمل دليل إعادة ربط الوحدة في Windows (2026-11-27)

- زال عائق اختبار الإزالة وإعادة الربط لوحدة واحدة في `45904704260`. ما زال تطبيق حالة عالم الهوست يتطلب هوية وعمرًا متحققين في Android وأثرًا أصليًا محددًا للعنصر؛ عدادات عضوية العناصر لا تثبت زوال التصادم.

## Work38 — طلب الإعادة لم يصل إلى قائمة الإيقاف (2026-11-26)

- أثبت [45883083661](https://github.com/alwansan/BO/actions/runs/45883083661) تهيئة طلب الإعادة فقط؛ أول دليل مفقود هو جاهزية `PauseMenu`. الاختبار القديم لم يرصد بت Start بعد مرشح USB. تصحيح التوقيت ورصد التسليم والصفحة ينتظر التحقق الحي؛ صفر نداءات فصل في التقرير.

## Work36 — عضوية جدول `0xF0` لا تغطي العناصر الأخرى (2026-11-26)

- اكتُشفت مصفوفة `0x200` بدورة ربط وفصل مستقلة. لا يشهد تقرير العضوية الحالي ولا اختبار الإعادة الجاري على تعيينها أو آثار `+0x124` في التصادم. تبقى هذه الأدلة شرطًا قبل تطبيق باب أو عنصر في محرك الضيف.

## Work34 — إثبات الإزالة ينتظر اختبار الإعادة المصحح (2026-11-26)

- عضوية القائمة صحيحة في التقرير الأول. انتهت المهلة دون فصل؛ وتشغيل الإعادة `45877709542` توقف عند ترجمة GCC قبل المحرك، فلا تقرير حذف أو إعادة تحميل. إصلاح الترويسة وخيار دورة العمر المحلية موجودان.

## Work32 — هوية الهدف ليست محدد كائن الوحدة (2026-11-26)

- لا تقاطع بين 97 معرّف هدف و877 محدد كائن وحدة في ملفات المستويات الثمانية. الاستخدام المباشر لمعرّف هدف لحل هوية باب سيخفق على هذا المحتوى. مصدر الجدول مثبت ساكنًا، لكن خيط تحديث الأهداف والأثر على التصادم أو الهدف الفردي غير مرصودين.

## Work31 — الفصل وإعادة النشر لم يُرصدا بعد (2026-11-26)

- تقرير `45873530764` يثبت مطابقة الوحدة الواحدة فقط: 0 استدعاءات فصل، ولا عدادات عضوية في هذا الإصدار. اختبار العضوية جارٍ والإعادة منتظرة. مخرجات التحليل الساكن لجدول التدمير لا تحسم هوية باب فردي أو قراءة تصادمه.

## Work30 — نقص دليل الفصل الحي (2026-11-26)

- تشغيل اللاعبين الافتراضي يتوقف بعد ثماني مراحل وقد لا يفصل وحدات. خيار إعادة المهمة جاهز محليًا؛ لا يُعد مرور اختبارات الحساب أو تجهيز المصدر دليلاً على ربط/فصل حي. راجع `unit_observations` وأربع إشارات إعادة المهمة في تقرير المحرك.

## Work20 — مراجعة الاستعادة + دليل العالم المشترك (2026-11-26)

- الأساس `2e810f5`, دفع إلى `experiment/black-coop`. روابط ChatGPT المشاركة لم تُجلب (لا نص)؛ المصدر الحي + الاستعادة + الذاكرة.
- XBE خاص مطابق SHA1 `c5bfed25...`; إعادة فحص `world_xref_static`+`world_writer_flow` عبر CI `45843350617` SUCCESS؛ الأدلة `memory/evidence/WORK_BATCH20_WORLD_XREFS_PINNED_45843350617.json`: 10 أقسام، 2772164 بايت، 892 مرشح xref، كتابتان `0020BF48`/`0020BF51` register-sourced فقط. لا scene loader/mission/epoch/safe hook.
- **العالم المشترك غير مكتمل**: لا منتج أصلي + لا resolver/apply + لا `OriginalGuestCanonicalApplyFrame` caller إنتاجي + لا P2..P4 أصلية حية + لا تطبيق ضيف. لا physical readiness؛ AI/أصول/حفظ أصلية دون تغيير.
- البيئة المحلية Termux LLVM objdump بلا i386؛ اختبارات objdump عبر GitHub Actions.
- الخطوة التالية: الربط الحي على خيط اللعبة (استعادة Work19) أو متابعة Work18: قارئ unit-v24 (48B) -> constructor -> state-change/teardown -> هوية ثابتة -> موفر `OriginalScene/ResolveOriginalObject/StillOwnsOriginalObject/ApplyOriginalHostState/VerifyOriginalHostState`؛ لا ABI مخترع. ثم اختبار هاتفين كبير واحد للمالك بعد بلوغ المعيار.
- بناء APK الكامل بالمسار `black-coop-room-network-apk.yml` (يستخدم ISO المرفوعة في المستودع، لا تعديل عليها) **manual dispatch يتطلب تصريحاً صريحاً بالأصول** وفق سياسة المستودع؛ حتى ذلك الحين الـAPKs ذات NO-GAMEPLAY عبر CI هي المتاحة.

# Work19 — نتيجة المحرك الحقيقي ومراقبة الصوت (2026-11-26)

نجح CI45807466769 على74653903: generation/build وتشغيل PC الأصلي مع مراقبة الصوت. create24/destroy0/restoreChecks1/retainedFpvStorage6، ولا otherThread/overlap/eviction/busy/untracked مرصود. audio_observer_executed=true. نجحت تجربة اللاعبين السابقة:1216 ticks فيزيائية مستقلة، ready768، حركة4527/6363mm،10shots لكل لاعب وثماني مراحل input. التقرير allowlisted وحده محفوظ في memory/evidence/WORK_BATCH19_REAL_AUDIO_OBSERVER_2026-11-26.json؛ SHA25646eb9505dc99fa332eccb9b77b4daec5c84d34f4ce68e4739a0326103716d04f.

القيد الخارجي المؤكد audio_output_unavailable=true في عاملWindows؛ لم يُختبر صوت فعلي أو lifecycle teardown أو callback concurrency العادي. retainedFpvStorage انتماء عنوان، وليس برهان publication أو ملكية/restore. لا Android أو أربعة لاعبين أو shared world مثبت. محاولة45807325388 توقفت في معايرة C بسبب التقاط نسخة وسطية، صُححت في7465390 وأعيد فحص ASAN/UBSAN ثم CI الحقيقي بنجاح.

اكتملت إصلاحات إبطال العرض والتعريف الدقيق للـAPK في السجل، مع جميع فحوص6d3b5e3 الستة ناجحة. العالم المشترك ما زال محجوبًا بموفر عمر كائنات heap الأصلي وstationary audio/presentation construction، وبغياب original producer/resolver/apply للـNPC/الأشياء/الأهداف. API original destructor/identity mapping غير متوفر في المصدر المثبت. المصدر المرجعي لا يقدم حلًا لهذه الفجوة؛ لا تُخترع ABI أو IDs من pointers ولا تفتح physical readiness.

الخطوة التالية تتطلب مصدرًا/أدلة ABI أصلية لإنشاء الموارد وتحريرها وربط authored instance بالكائن الحي، وبيئة صوت/Android مناسبة للتحقق. لا APK قبول نهائي حاليًا ولا تجربة هاتف جزئية. تقرير المالك Start يبدأ المرحلة عند الجميع محفوظ؛ لا طلب إعادة إثباته. حافظ على AI/أصول/حفظ الأصل.

---

# Work19 — مراقبة عمر تخزين الصوت في مختبر المحرك الأصلي (2026-11-26)

أضيفت أدوات AUDIO_LIFECYCLE_LAB.md وaudio_lab.py وaudio_observer.inc المفقودة التي أشارت إليها وثيقة الصوت. تلتف حول original7EF50/7EDB0 بعد generation باستدعاء الأصل مرة واحدة، وتراقب counts/thread overlap وعناوين تخزين خاصة ضمن ledger محدود، ثم تصدر عدادات allowlisted فقط. لا قراءة bytes كائنات ولا كتابة guest ولا constructors جديدة. retained_fpv_storage دليل انتماء عنوان فقط، لا ملكية أو publication أو restore integrity. audio_observer_executed منفصل عن نتيجة اللعب القديمة.

نجحت36 اختبارات Python وC schedule/observer وaudio ASAN/UBSAN ومطابقة anchors المصدر المثبت. سيعمل CI الأصلي على هذه الدفعة؛ لا runtime صوت مُدّعى قبل نتيجته. آخر مختبر PC45239818055 أثبت لاعبين بحركة وسلاح مستقلين، لكنه لم يراقب ملكية الصوت/allocator. على6d3b5e3 نجحت source45806572808، Android وحدات45806572911، Android diagnostics45806572806، TLS45806572833، parallel45806572877 وUDP45806572853. الدليل WORK_BATCH18_REVOCATION_DIAGNOSTICS_2026-11-26.json.

التالي متابعة مختبر المحرك الحالي، إصلاح compilation/runtime عند الحاجة وتحليل العدادات، ثم source-proven stationary audio/game-heap ownership provider. العالم المشترك وAPK النهائي ما زالا غير مكتملين. لا طلب اختبار هاتف جزئي.

---

# Work18 — استكمال مراجعة العالم وإصلاح إبطال العرض (2026-11-26)

الأساس 7acb810. أعاد المالك تأكيد الهدف: مرحلة أولى مشتركة بأجسام مرئية وتفاعل الهوست والضيوف ثم APK قبول متكامل وسجل موحد. أجاز وكلاء متوازيين وأي مجسم مناسب. Start الحالي يحمّل المرحلة على الهاتفين وفق تقريره.

أُصلح رسم packet قديمة بعد تغير roster/ABORT أثناء تخصيص GPU بإعادة قراءة facade القصة قبل التخصيص وقبل Draw ومطابقة التذكرة الكاملة وtick. regression فشل على الأساس exit134 ونجح مع ASAN/UBSAN على الإصلاح. لا يمتد قفل Java عبر GPU enqueue، لذلك قد تبقى رسوم أُجيزت قبل إلغاء متزامن بعد آخر فحص. نجحت اختبارات تجهيز المصدر الستة وmetadata واختبارات JVM/TLS المحلية. Android source units السابقة45804212528 نجحت للوحدات السبع على الأساس، وCI الجديد للإصلاح ينتظر.

أضيف schema/package/version/SHA256 للـbase APK إلى التصدير التشخيصي الصريح مع regression Android instrumentation. نجح strict Java8 compile، runtime instrumentation الجديد ينتظر CI. لا معرف جهاز أو شبكة داخل metadata.

التحقيق docs/SHARED_WORLD_PROVIDER_INVESTIGATION_WORK18_AR.md يحدد غياب producer أصلي لأحداث الأشياء وresolver/apply/verify حي لدى الضيف. استقلال actor الفيزيائي يحتاج أعمار heap الأصلية وpresentation/audio stationary ownership. لا تفعيل physical readiness أو AI أو تعديل أصول/حفظ. العالم المشترك وAPK القبول لم يكتملَا. تابع أدلة مختبر المحرك الأصلية للحصول على allocator/audio evidence ثم الربط الحي. لا اختبارات هاتف جزئية.

---

# Work-Batch10 — توجيه علي: Start إلى عالم مشترك (2026-11-26)

الأولوية الملزمة الآن: Start الموجود يبدأ نفس المرحلة لكن العالمين منفصلان وفق تقرير علي؛ المطلوب رؤية متبادلة وفعل ضيف يغير حالة بقيادة المضيف وانتقال قصة مشترك. مجسم بسيط مصرح به مقبول. لا المزيد من النقل المعزول بلا عائق محدد. لا baseline/XBE مطلوب ولا اختبار هاتف جزئي ولا ISO/APK يدوي. العوائق المثبتة وأقرب وصلة مأذونة موثقة في `docs/SHARED_CAMPAIGN_START_BLOCKERS_WORK10_AR.md`؛ الأدلة في `memory/evidence/WORK_BATCH10_SHARED_CAMPAIGN_BLOCKERS_2026-11-26.json`. هذه دفعة توثيق وتتبع مصدر فقط؛ لا runtime جديد أو دمج حي متحقق أو APK قبول. اقرأها قبل متابعة خطة الأهداف القديمة أدناه.

---

# Work-Batch08 — حاجز استهلاك أهداف اصطناعي (2026-11-26)

الأساس المتحقق: `08eb7012542b6c72721b362752b2d9e193288d29`، فرق Work كان نظيفًا قبل fast-forward. القراءة ترفض لقطة مكتملة أقدم من 3 ثوانٍ محليًا؛ واجهة فحص اللقطة المحتفظ بها تعيد التحقق من أحدث completion، ticket، local episode، شاهد المشهد والخيط. الإلغاء/المهمة الجديدة لا يعيدان صلاحية لقطة قديمة. لا engine apply أو AI/أصول/حفظ/APK/هاتف. الأدلة: `memory/evidence/WORK_BATCH08_OBJECTIVE_CONSUMPTION_2026-11-26.json`. الخطوة التالية: توثيق مصدر الأهداف الحي وعمره قبل أي ربط بالمحرك؛ عمر المصدر/تأخر تجميع الشبكة ما زالا غير مثبتين.

---

### Session92 — 2026-11-23 — real engine lab and original soldier ownership repair

Latest owner explicitly accepts any original enemy/ally soldier appearance. START already loads the same stage on both phones; a shared physical world remains UNIMPLEMENTED. Do not present the current APK as completed co-op or request minor phone probes.

Authoritative base: 58955883939ec12a07e21afd557d85d2b0263b6d; active local checkout BO-world-sync, not stale BO-network-fix. Read latest CL host Probe051323 and guest Probe051400 plus original screenshots (host lobby and guest first-person scene). The sampled event exports end before START and do not themselves prove shared gameplay.

Fixed original visual clone copying donor intrusive membership/self/serial/AI-target state, stale pool validation, true sparse free-slot ownership and repeated unsafe retry after partial clone. Added production helper regression with ASAN/UBSAN. Fixed pre-existing source preflight assertion for isolated CallGuest context. These are source repairs, not verified on phones.

Added isolated private original-engine Windows CI lab: pinned MIT PC source b2512fafd3e181e4febfb298f297245b845ef628, original owner XBE hash validated privately, real OHCI controller scripts, original mission/player2 memory observations, per-player motion/ammo and context restore requirements. Only aggregate summary artifact. Harness tests are not original-engine acceptance; real Windows generation/build/runtime still pending at publication. Default 300-second run; no synthetic stage writes.

New host proxy adapter files implement checked original DF400/DBBE0 call contracts with explicit original actor/collider preconditions. They are NOT wired into gameplay: independent original player2 actor/camera/weapon setup and teardown remain prerequisites. ABI survey established DF400 returns AL success, not proxy pointer; DBBE0 takes proxy value; camera14380 is a constructor and generic virtual cleanup is not safe on constructor-only storage.

Next: inspect real engine lab Actions results, fix actual dependency/build/runtime blockers, port complete original P2 ownership/context safely, register AI target and physics, then implement host-authoritative NPC/object/objective application with stable lifetime identities. Guest inboxes currently have no original engine consumer. Never equate TLS, same stage, detached visual or green harness tests with shared-world completion.

References: https://github.com/alwansan/CL/tree/main/black%20log ; https://github.com/Peterson047/BLACK-XBEMultiplayer (binary-only, no borrowed restricted assets); MIT PC source above; docs/ORIGINAL_HOST_PLAYER_CONTEXT_ABI_AR.md. Images and logs continue to arrive in CL including Arabic-named guest folders. No owner assets, saves, private logs or credentials uploaded.

## Session64 — CL 2026-11-22 video + HOOK001 visual actor proof; merged touch input fix

- **User-supplied actual video analyzed completely**: `1006546724.mp4`, 162.887937s, HEVC 2400x1080 at 30fps, AAC, 187060734 bytes, SHA256 `b8cf29b0ad9c3dba12468dace342847c2017ff7ff8c4724da1acd583be72c454`; **MATCHES** CL's four-part original `Screenrecorder-2026-11-22-16-26-13-469.mp4` README (verified exact SHA). Reopen https://github.com/alwansan/CL/tree/main/black%20log/vid/Screenrecorder-2026-11-22-16-26-13-469.mp4; reconstructed by `cat part-* > video.mp4`. See **docs/BLACK_VIDEO_20261122_162613_FORENSIC_AR.md** for durable timecodes, observations, log aggregates and confidence bounds. Do not claim footage proves shared world or another human.
- Video (~16–25s) Android overlay goes `IDLE -> HOSTING, 1/4` and literally displays `STORY START: unavailable until real remote actors and story sync exist.`; (~28–43s) exits overlay to select original campaign difficulty and load LEVEL00; (~44–162s) playable original single-player first person touchscreen shooting, reloads, explosions and many original BOT NPCs. Cannot tell from video whether HOST TLS session survives overlay dismiss: `coop_room_events.log` for video timestamp is NOT included in CL uploaded 16:25 probe folder.
- Exact associated CL logs `https://github.com/alwansan/CL/blob/main/black%20log/logs/BLACK-Probe-20261122-162556-906/black_log.txt`, `https://github.com/alwansan/CL/blob/main/black%20log/logs/BLACK-Probe-20261122-162556-906/coop_probe.txt`: owner XBE SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f`, resource opens LEVEL_00 type0 at engine+66.504s and type1 at +67.904s; HOOK001 starts +68.810s. Engine loads ISO after nonfatal early "no game files" wait. Probe `535` JSONL records: 1 start, 130 local-player, 134 32-slot pool, 142 nonlocal LOD actor, 128 animation. Exactly one world epoch/reset; stable local id1 130/130 `resource_valid=false` and `render_valid=false`, health 1200→628; ALL 142 sampled original NPC LOD have `resource_valid=true` and `render_valid=true`, 19 distinct ephemeral local probe actor IDs (NOT network players). 134/134 valid pool snapshots, no rejections, occupancy evolves, so never seize NPC slot without per-slot live recheck. Original NPC animation evidence: `SH_AR_S_001`, `SH_AR_S_MOV`, `TM_AR_S_001`, `TM_AR_S_MOV`. This strongly supports ANY original NPC LOOK donor and confirms first-person local eye/hands not a full-body donor. No multiplayer test.
- **Critical source fix from video**: pinned Android `native/input.cpp::Poll(0)` combines SDL/loose controller + `touch::Apply(port,p)` and then calls `app::kGame.pad(port,p)`. Earlier session63's extra `input::Poll(0)` call on original game-thread would duplicate ORIGINAL game-pad callback, and gating `if(!report.connected)return` could fail to send finger-release neutral. Source `native/touch.cpp::Apply` sets `pad.connected=true` when touch buttons/stick active, false when released. Now [tap contract e75e843a](https://github.com/alwansan/BO/commit/e75e843a8afc287e0d573fbc09f50c7f837a7db1) and [actual staging 88fa8ea7](https://github.com/alwansan/BO/commit/88fa8ea7d06f5e78d4d08428435c953886078cff) tap already-polled port0 AFTER actual touch merge and BEFORE original callback, without extra `Poll`. Lock-free timestamped atomic sample (250ms TTL), sends neutral after no touch, fixed safe log codes65/66. Source tests [CI45029723932](https://github.com/alwansan/BO/actions/runs/45029723932) **SUCCESS** C++ + staged Java8/source transformation; Android full build [45029723927](https://github.com/alwansan/BO/actions/runs/45029723927) status pending/running at documentation, **NO GAMEPLAY** even if success.
- **NEXT P0**: Original selected mission/guest load/verified loaded-scene gate, then source-safe NPC LOOK-only remote actor/pose rendering. Preserve original local FPS camera. No owner small test, automatic safe logs; ALWAYS update restore/state. Only writable branch `experiment/black-coop`.

---
## Session63 — Original Xbox SDL gamepad -> authenticated GUEST TLS real-input source (2026-11-22)

**Agent:** BO-COOP; **ONLY WRITE** `alwansan/BO:experiment/black-coop`. **Owner orders:** update `prompt/RESTORE_SESSION_AR.md`, `memory/STATE.json`, `memory/PLAN_AR.md`, session documentation CONTINUOUSLY without being asked again. Do NOT ask owner for small phone tests. One major trial only after authentic guest stage loading, independently rendered remote HUMAN bot bodies and host shared mission/world/combat.

**Major source-gap fixed:** Prior source native host 40Hz `OriginalHostHumanInbox` consumed network movement, but the REAL Android GUEST game thread had no producer of `RoomSession.offerHumanInputFromGameThread`; only synthetic 4-party TLS harness injected commands. This blocked real motion even if STORY_COMMIT were eventually delivered. [Contract 7d8160c4](https://github.com/alwansan/BO/commit/7d8160c429abe6b372e76c82378dd7456a311fe0) adds bounded `room_original_guest_pad_contract.hpp` and a source reader from PINNED `TGP482/XboxXBERecomp@99078b1:runtime/core/input.h` `input::Pad` and `input::Poll(0)`. Raw connected local controller left stick quantized to [-1000,1000] with 4096-unit deadzone; original Xbox RIGHT trigger => FIRE, A => JUMP. YAW stays **0** because original actor world heading is NOT independently verified; don't invent it. Original SDK Poll may invoke normal game pad callback (existing source behavior). Not yet validated for Android touch-only controls; only connected SDL-visible gamepads.

[Integrated commit44f0439c](https://github.com/alwansan/BO/commit/44f0439c8712ded26bcb68415ad4b7a56638df3f): Original game's proven `sub_001E5FA0` first-person player callback already polls native 40Hz human input. New `OfferOriginalGuestController()` obtains local source report, calls Android Activity `coopOfferOriginalGuestController(IIIZZ)Z` on genuine original game thread. `RoomAppRooms.offerOriginalGuestControllerFromGameThread` checks actual JOINED phase, current exact COMMITTED `RoomStoryWire.Ticket` and bounded input, issues strict per-ticket monotonically increasing command sequences (resets on new story/room), calls `RoomSession.offerHumanInputFromGameThread` (sends via existing authenticated TLS queue, never socket I/O on original game thread). Only safe once code65 event, no controller payload/credentials/guest pointers in logs. Never creates actor, never calls NPC AI or modifies original guest memory. Full game/scene support is NOT thereby established. Confirmed [CI44983537405 SUCCESS](https://github.com/alwansan/BO/actions/runs/44983537405) native C++ tests (`ORIGINAL_GUEST_CONTROLLER_SOURCE_PASS`, bot-pool tests), full pinned Android source staging, Java8 compilation. [UDP probe44983537422 SUCCESS](https://github.com/alwansan/BO/actions/runs/44983537422).

**Session62 retained:** [commit18874aeb](https://github.com/alwansan/BO/commit/18874aeb40e664ad3cc71b941007466ba7c401cd) live original 32-slot character pool read-only NPC appearance readiness probe only once 30-tick source phase gate, uses source-pinned SDK guest `mem::IsCommitted`, safe coarse evidence codes 60..64. Not a renderer/actor. [CI44983239377 SUCCESS](https://github.com/alwansan/BO/actions/runs/44983239377). [Latest Windows ARM64 Android room-only APK44983537312](https://github.com/alwansan/BO/actions/runs/44983537312) **IN_PROGRESS at last check**; NO GAMEPLAY even if APK eventually builds. Do not ask owner to test.

**Still blocked P0:** no real host-selected original level/stage auto-launch at guest; no independent original engine scene-loaded proof/READY; room story COMMIT not driven from actual scene; no remote bot body instantiated/visible/moving in original world; no independently source-known real actor yaw, SDL touch mapping unverified; host kinematics value-only no original collision, AI/weapon/quest authority. Next resume must prioritize REAL guest level launcher and actual actor model drawing/revalidation, not just new diagnostics. Preserve original local FPS hands/gun. Any original enemy/ally bot look accepted. NEVER borrow NPC AI brain or guest network actor memory.

---

## Session62 — Original BLACK bot-LOOK 32-slot pool passive native observation (2026-11-22)

**Identity:** BO-COOP. **WRITE ONLY** `alwansan/BO:experiment/black-coop`. Owner's overriding directive: **always update prompt/RESTORE_SESSION_AR.md, memory/STATE.json, plans and session notes as you work**. **No micro owner phone trials**; only one combined physical trial once real guest stage load + visible independently controlled remote human bot bodies + shared mission goals exist.

**Confirmed evidence/source:** pinned MIT PC reference `portingpete/Black-2006-Recomp@b2512fafd` defines original character pool pointer global `0x002D1980`, 32 characters at root+`0xA0` stride `0x3C0`, model template `+0x320`, NPC controller `+0x324`, model instance `+0x328`, part buffers `+0x34C/+0x350`. Its puppet routine copies a soldier's visual state into otherwise free slot, initializes model with `sub_0017E420`, draws using original actor vtable; this PC prototype is **NOT** proven safe as Android remote-player engine adapter and has no genuine shared gameplay.

**Implemented SOURCE (not APK/phone confirmed)**:
- [commit44498fa6](https://github.com/alwansan/BO/commit/44498fa6b01afa548f94d624c09e073725129186) `room_original_bot_pool_contract.hpp` supplies strict source-look/unused-slot classification (enemy OR ally, NEVER NPC AI brain). No game memory writes, no remote actor constructor, no host COMMIT/READY.
- [commit18874aeb](https://github.com/alwansan/BO/commit/18874aeb40e664ad3cc71b941007466ba7c401cd) integrates `room_original_bot_pool_witness.hpp` into genuine original player update via existing 10Hz eye-source hook, ONLY after ≥30 original core-phase `0x1C` observations, same original stage launch episode, stable real local player and original successful `LevelDat.bin+Unit_01.bin` file-open pair. New reader uses pinned Android Xbox SDK `mem::IsCommitted` to validate every guest page; verifies root/pointer consistency and each 32-slot model/rig/buffer before deriving coarse count. Does not expose guest pointers/models/coordinates. Fixed Android-safe categories 60..64 only, at most once per category per original episode, sampling at most 0.5Hz. Correctness fix: a nonzero but invalid model template remains OCCUPIED (never misclassified as empty). Core phase original guest field read is also range-guarded. The signal is **availability HINT ONLY**, not a live remotely rendered actor or reliable scene-loaded proof.
- [Source staging / C++ sanitizer and Android Java8 preflight44983239377 SUCCESS](https://github.com/alwansan/BO/actions/runs/44983239377): includes exact `ORIGINAL_BOT_POOL_APPEARANCE_COUNTS_PASS` synthetic tests, verified no guest writer/guest function call, staging source and Java logging.
- [Windows ARM64 APK run44983239391](https://github.com/alwansan/BO/actions/runs/44983239391) **IN_PROGRESS at last live check**; room-only APK explicitly **NO GAMEPLAY**, even if this build later succeeds. Do not ask owner to test it yet. Prior Android room-only 44980350821 SUCCESS but no gameplay.
- Previous session61 canonical host-value TLS loop still exists: source feet `actor+0xA0` ≠ eye `+0x100`, 4-player v7 TLS, host-only bounded input kinematics, guest original game-thread read-only world inbox. No physics or rendered bodies.

**Next P0 tasks**: (1) actual original guest mission auto-launch from HOST-selected mission/stage (original guest loader ABI still unknown), (2) independent **actual original stage-loaded callback**/scene epoch (no camera/core phase/resource pair proves it), (3) allocation/revalidation + independently rendering remote human full body from ANY original bot visual with AI/controller never copied; host-authoritative collision, enemy combat and objectives. Only then ONE useful owner phone trial; any small details automatically captured in bounded safe logs. Maintain updates in ALL restore/state/docs without reminder.

---

## Session61 — ORIGINAL actor FEET and HOST world value loop to GUEST BLACK game thread (2026-11-22)
**Only writable:** `alwansan/BO:experiment/black-coop`. Owner: up to 4 real humans in ONE host original BLACK story mission; ANY original ally/enemy bot LOOK allowed for 2–4 human players, but NEVER borrow NPC AI brain. No incremental owner phone tests. All small diagnostics auto logs; one major device trial only when actual scene join + rendered/controlled humans.

**Real code progress:**
- [33f4a065](https://github.com/alwansan/BO/commit/33f4a065600dd93f60dbc2872fb7aa0f4f48bd3d) upgraded room TLS wire VERSION/PROFILE **7** (v6 is intentionally incompatible) and transmits authentic original source actor FEET `original_player+0xA0/+0xA4/+0xA8` separately from camera EYE `+0x100`. Corrected host snapshot source to feet rather than camera for body positions, with C++ finite/bounds checks and JNI signature (IIIIII)V. The MIT PC reference `portingpete/Black-2006-Recomp@b2512fafd` derives actor+0xA0 as character feet; original phone rendering/body fit NOT yet tested. [Android native staging44982274474 SUCCESS](https://github.com/alwansan/BO/actions/runs/44982274474), [real 4-party TLS44982274531 SUCCESS](https://github.com/alwansan/BO/actions/runs/44982274531), [Android29 Keystore/TLS44982274430 SUCCESS](https://github.com/alwansan/BO/actions/runs/44982274430).
- [9cb47de5](https://github.com/alwansan/BO/commit/9cb47de549d8280fd0d2c6d75dc549521659082d) introduced **host-only, original game-thread, value-only kinematics** with 3 separate human preview poses, seeded near authentic host original body feet; verified-source remote TLS commands move bounded at <=300cm/s, <=30cm/step, no stale/replay or scene/rejoin carryover. [779ccdfd](https://github.com/alwansan/BO/commit/779ccdfd8c30fa2bd0a5e0af93c3f84db90a9c1a) wired actual `RoomSession.drainHumanInputsFromGameThread()` into kinematics and publishes HOST-computed 4-avatar canonical snapshots rather than trusting guest-submitted body positions. This is **placeholder POSITION SIMULATION only**, no original BLACK character/physics/collision, no AI or damage.
- [2f969625](https://github.com/alwansan/BO/commit/2f96962529da813e4940a6839d28c2c5cd4f26a0) added strict `RoomGuestHostWorldSource.pack` (COMMITTED ticket, exact session/roster/revision, membership and full participant mask) and native `OriginalGuestHostWorldInbox` (2–4 humans, host must be ID1, 350ms TTL, monotonic tick, scene/self binding, no guest pointers or NPC writes) with negative synthetic tests.
- [81bbeb10](https://github.com/alwansan/BO/commit/81bbeb106714f6bde44637432607d12f5ba1456a) connected real network guest `RoomTls.Client.worldSnapshot()` -> guarded `RoomSession.readHostWorldFromGameThread()` -> Android `RoomAppRooms.pollHostWorldSnapshotFromGameThread()` -> Activity `coopPollHostWorldSnapshotFromGameThread()` -> JNI `PollOriginalGuestHostWorld()` -> native original FIRST-PERSON update 40Hz. Native fixed safe code16 only (no coordinates logged). **Source preflight44982679713 SUCCESS** with `GUEST_ORIGINAL_HOST_WORLD_INBOX_PASS` and Java8 compilation; [real TLS44982679692 SUCCESS](https://github.com/alwansan/BO/actions/runs/44982679692) tests end-to-end synthetic host snapshot.
- [Latest full Windows Android APK44982679707](https://github.com/alwansan/BO/actions/runs/44982679707) IN_PROGRESS at last inspection. Earlier APK44980350821 was SUCCESS but room-only/NO GAMEPLAY; don't claim current APK success until verified. No owner phone trial.
- No original game scene loader is called by network; no original host selected stage ID mapping proved, no actual scene-ready callback, no original remote bot model instantiated/drawn or independently moved, no authoritative engine physics, AI, shared objectives, damage, or actual shared campaign. These remain P0, **do not claim playable Co-op**.

**NEXT P0:** Safely implement original host-selected mission/guest stage start and independent original stage-loaded callback; then actual remote actor renderer and movement apply from `OriginalGuestHostWorldInbox` / `OriginalHostHumanInbox` using ANY original bot LOOK. Keep original local camera/hands/gun and never copy NPC AI. Gate all engine writes on validated actual game-thread scene and exact authenticated ticket/epoch. Inspect live Actions and update restore continuously.

---

## Session60 — native BLACK original game-thread authenticated human input + ANY-bot look planner — 2026-11-22

**Writable ONLY:** `alwansan/BO:experiment/black-coop`. Goal remains 2–4 REAL humans in ONE original BLACK host-authoritative campaign scene. **No phone micro-trials**; one combined major trial when guest original mission launch, independent remote actor render and shared combat/objectives work.

**Confirmed:** Prior [Android room-only APK44980350821 SUCCESS](https://github.com/alwansan/BO/actions/runs/44980350821) produced artifact 14737412732, not gameplay. Native TLS input bridge [93f4d6dc](https://github.com/alwansan/BO/commit/93f4d6dc98c2e580281fe05ddab31f43bd8b3583) and [4a4bcddd](https://github.com/alwansan/BO/commit/4a4bcddd577cad6d943dcd717626aa3b9261ca60): a bounded native game-thread inbox for 3 remote commands, sourced ONLY from actual TLS peer-authenticated host `RoomTls.Server.drainLatestTrustedInputs()`, with committed ticket/revision/freshness checked on Java worker, exact original player game thread guard, stale/replay/bad-value checks in C++ and all commands purged on story epoch change or loss. No network-supplied NPC pointer, bot brain, or original memory writes; fixed once-only evidence code15. [Preflight44981474772 SUCCESS](https://github.com/alwansan/BO/actions/runs/44981474772) includes `ORIGINAL_VERIFIED_TLS_INPUT_NATIVE_INBOX_PASS`, pinned staging and Java8 compilation. TLS 4-peer [44981474702 SUCCESS](https://github.com/alwansan/BO/actions/runs/44981474702), Android29 TLS [44981474706 SUCCESS](https://github.com/alwansan/BO/actions/runs/44981474706). [Performance commit2b1edb17](https://github.com/alwansan/BO/commit/2b1edb178c4eeef21f99d7eb7df16f1825edda7f) polls input at 40Hz from real original player update, independently from 10Hz original-camera diagnostic. [Source preflight44981621822 SUCCESS](https://github.com/alwansan/BO/actions/runs/44981621822). [Full Android APK44981621849](https://github.com/alwansan/BO/actions/runs/44981621849) IN_PROGRESS at last check, always verify live.

**Original bot look:** [commit7a2a5833](https://github.com/alwansan/BO/commit/7a2a5833a935aa3a0a2514ce80e6b2daf26cc93c) adds `coop/bridge/original_bot_puppet_plan.hpp`, a **source-only C++ planner** using verified MIT PC reference original character pool layout (32 slots, stride 0x3C0; model+0x320, controller+0x324, rig+0x328 and part buffers+0x34C/+0x350). Can select ANY original enemy/ally bot LOOK and three separate idle character slots, never borrowing NPC AI; game-thread ownership, validated story participants/generations and independent actual scene-loaded+COMMIT gates. [Sanitizer preflight44981706988 SUCCESS](https://github.com/alwansan/BO/actions/runs/44981706988). **NO original actor/renderer/movement physics is yet installed!** Bot pool readback/atomic original engine reservation/multiplayer actor rendering are still BLOCKED. Planner never writes game RAM.

**Next high-priority P0:** Prove original guest stage launch ABI and real scene-loaded signal (not merely 0x5AE0, two file opens, camera eye or source bytes), map host mission to original stage, then PREPARE/READY/COMMIT and create/render independently controlled remote HUMAN body in all peers with any validated original bot look; host-authoritative combat, AI and objectives. After that one major owner trial on same APK across phones. NO phone request now.

---

## Session59 — original BLACK core phase source + read-only native gate (2026-11-22)
**WRITE ONLY** `alwansan/BO:experiment/black-coop`. Owner's goal remains 2–4 real humans in ONE host-launched original BLACK mission, remote humans can use ANY original enemy/ally bot LOOK (no NPC AI brain), original local FPS hands and gun remain; **no owner phone micro-tests**. The last accepted owner device APK is NOT this diagnostic.

- Pinned MIT PC port `portingpete/Black-2006-Recomp@b2512fafd` uses `core+0x5AE0==0x1C` as a game-playing guard before its experimental PC second-player work. Private original c5 Xbox XBE [CI44980186959 SUCCESS](https://github.com/alwansan/BO/actions/runs/44980186959): fixed 2048B decoded windows detect `+0x5AE0` direct operand **2 times in `sub_001F1080` (update)**, **16 times in `sub_001EDC90` (streaming)**, 0 in `sub_0020BC40` (spawn); no immediate nearby comparison with `0x1C` in the auditor's narrowly defined rule. These are SOURCE HINTS, **not** independent proof of core-state semantics/scene loaded/guest level loader.
- [Code commit95912d73](https://github.com/alwansan/BO/commit/95912d73476b915415697e72b1d3163ae546b19c) adds original player game-thread read-only core phase `R32(world+0x5AE0)` and separate 30-consecutive-sample witness: same exact original launch episode, matching original streaming level/stage candidate, fresh original same-folder `LevelDat.bin`+`Unit_01.bin` opens, original local player stable/nonfrontend, original mission core phase exactly 0x1C; reset on any change. Emits only safe numeric event14 `ORIGINAL_CORE_PHASE_1C_RESOURCE_STREAM_STABLE_SOURCE_HINT_NOT_READY`. No actor, no game/scene writes, no guest launch, never READY or host COMMIT.
- Initial [CI44980309558 FAIL](https://github.com/alwansan/BO/actions/runs/44980309558) only an old staging test prohibited any `BothEssentialFilesOpenedForCandidate()` direct read: source witness now uses exactly one read-only fresh-pair check, not a second report. [Test fix e28f5b7a](https://github.com/alwansan/BO/commit/e28f5b7a39a8973b411aa26ed62f0f6d3e49f90b), [Native staging+C++ sanitizer+Java8 preflight44980350803 SUCCESS](https://github.com/alwansan/BO/actions/runs/44980350803). [Full Windows ARM64 Android room-only APK44980350821](https://github.com/alwansan/BO/actions/runs/44980350821) IN_PROGRESS last checked, do not claim successful binary or gameplay. Preceding [44979689352](https://github.com/alwansan/BO/actions/runs/44979689352) CANCELLED by superseding source run.
- NEXT P0: independently prove original host mission index and reliable guest level-start ABI/flow, *actual* loaded scene lifecycle and scene epoch before any READY; add independently rendered controlled human body with ANY safe original bot mesh, host-authoritative enemy AI, combat and mission objectives. PC split-screen puppet reference is a source IDEA, not a drop-in engine actor. The real co-op trial is not ready. Keep Logs automatically, ask owner one major trial only.

---

## Session58 Windows Android source packaging regression and repair — 2026-11-22
[APK44979362965 FAILED](https://github.com/alwansan/BO/actions/runs/44979362965) **BEFORE Android compilation**: Windows checkout had CRLF in pinned upstream `src/game/difficulty.cpp`; original native stage hook preparer matched an LF-containing anchor and raised `ValueError Expected exactly one unchanged anchor` in 4 preparer tests. This is source packaging, NOT BLACK gameplay failure. Added isolated normalization for the disposable staged C++ file and explicit synthetic CRLF regression; two intermediate corrections were needed (escapes and indentation), final code [ae86cddc](https://github.com/alwansan/BO/commit/ae86cddcc7095d920e6e88fbe9ccfae11246cfe0). [Source preflight44979689358 SUCCESS](https://github.com/alwansan/BO/actions/runs/44979689358) includes pinned title source staging, enforced Windows CRLF fixture, portable native C++ checks and Java compile. [Latest full Windows ARM64 Android APK44979689352](https://github.com/alwansan/BO/actions/runs/44979689352) IN_PROGRESS at final check: **DO NOT claim APK success or ask phone test**; even a SUCCESS is room-only, NO playable story. Pinned source stage input hook [2d5bfa4e](https://github.com/alwansan/BO/commit/2d5bfa4ee70897a7e4fbd4084e9652a7fa3c4c19) is read-only, original streaming call unchanged, no original game writes or scene READY. Continue urgent original stage-loaded lifecycle/guest start and independently controlled ANY bot-looking remote human, host AI/quest/combat. One big owner test only after true shared-world gameplay.

---

## BO-COOP Session58 — OWNER ORIGINAL XBE streaming level/stage BYTE source verified (2026-11-22)
**Only write** `alwansan/BO:experiment/black-coop`. Exact original c5 XBE [private CI44979193377 SUCCESS](https://github.com/alwansan/BO/actions/runs/44979193377) detects 1 direct byte-register read at each `+0x2020C` and `+0x2020E` inside `sub_001EDC90` (bounded 0x900B window). Pinned MIT PC recomp `portingpete/Black-2006-Recomp@b2512fafd` `scripts/generate.py` calls these LEVEL and STAGE and `src/black_checkpoint.inc` notes stream-entry before initial units. This is a **credible read-only stage-input candidate**, NOT actual scene completion, host mission index, valid guest loader ABI or shared co-op.

**Source integration** [commit 2d5bfa4e](https://github.com/alwansan/BO/commit/2d5bfa4ee70897a7e4fbd4084e9652a7fa3c4c19) stages original game native `sub_001EDC90` wrapper on Android, preserves unchanged original `__imp__sub_001EDC90(ctx,base)`, only reads `game+0x2020C/0x2020E` as BYTE, captures first candidate per exact original launch, and logs numeric category ONLY after same-episode successful `LevelDat.bin`+`Unit_01.bin` opens AND stable original local player. New code range 500..2029 maps source-level candidate/scene-folder 00,01,03..08; NO original writes, guest auto-start, READY, actors or AI. [Android native+Java preflight CI44979363055 SUCCESS](https://github.com/alwansan/BO/actions/runs/44979363055), [pinned SDK resource file-open source CI44979362967 SUCCESS](https://github.com/alwansan/BO/actions/runs/44979362967), [real four-peer TLS CI44979362992 SUCCESS](https://github.com/alwansan/BO/actions/runs/44979362992). New full Windows Android source APK [44979362965](https://github.com/alwansan/BO/actions/runs/44979362965) and Android29 TLS [44979362959](https://github.com/alwansan/BO/actions/runs/44979362959) were IN_PROGRESS at last inspection. No owner phone trial requested; CI first.

Next P0: verify original `sub_001EDC90` input **provenance and full load-complete lifecycle**, use source-known actual scene identity to safely initiate same original level on guest, only then source-proven READY/COMMIT; real independently controlled remote human using ANY original bot appearance and host AI/quest/combat world. In PC source `core+0x5AE0==0x1C` is a potentially useful **reference-only** mission-running gate, not verified on owner c5 image or a license to READY. No micro phone tests, one combined major gameplay trial when ready.

---

## Session57 CI verified — 2026-11-22
Source commit [`7346abc9`](https://github.com/alwansan/BO/commit/7346abc9b2e38829944f5ff3ca8aa504d9cb10f5) separates ORIGINAL XBE WORD B_BEFORE/B_AFTER around unchanged native mission-launch call, four event buckets A/B_BEFORE/B_AFTER/C, same original launch+resource pair+stable player gating. [CI44978712336 SUCCESS](https://github.com/alwansan/BO/actions/runs/44978712336) confirmed C++ sanitized pre/post source tests, pinned original source staging, and Java8 compilation. [UDP44978712333 SUCCESS](https://github.com/alwansan/BO/actions/runs/44978712333). New Android APK [44978712319](https://github.com/alwansan/BO/actions/runs/44978712319) and Android29 TLS [44978712299](https://github.com/alwansan/BO/actions/runs/44978712299) still IN_PROGRESS at last check, no result claimed. Original mission index/guest game loader/scene READY/remote body/host AI/objective/combat remain UNIMPLEMENTED. No small phone tests; only one major later shared-gameplay trial. WRITE ONLY BO experiment/black-coop.

---

## Session54 — actual Xbox level resource-open + original BLACK player, source-proven only (2026-11-22)
[Session54](memory/sessions/2026-11-22_bo-coop-54-real-stage-resource-file-open-correlated-with-original-player.md). Owner P0 unchanged: *one host-launched ORIGINAL BLACK story on 2–4 real phones, independent player bodies with ANY original bot enemy/ally appearance temporarily; local original FPS unchanged; host AI/combat/objectives*. No tiny phone trials. Code `8c1f996a` shares one atomic original `NtCreateFile/NtOpenFile` witness across original Xbox kernel and native player update, records fixed `level_00,01,03..08` candidate only after successful open of BOTH `LevelDat.bin` and `Unit_01.bin`, verified original player's JNI game-thread event, safe limited on-device log, NO assets/path/guest-memory writes. [Exact Xbox source CI44975377158 PASS](https://github.com/alwansan/BO/actions/runs/44975377158), [Android source CI44975377204 PASS](https://github.com/alwansan/BO/actions/runs/44975377204), [real 4-party v6 TLS CI44975377200 PASS](https://github.com/alwansan/BO/actions/runs/44975377200), [Android TLS44975377221 PASS](https://github.com/alwansan/BO/actions/runs/44975377221). Windows diagnostic room-only APK [44975377208](https://github.com/alwansan/BO/actions/runs/44975377208) check live, NO GAMEPLAY. ORIGINAL file OPEN does NOT mean a loaded scene, selected mission, auto guest-launch or remote rendered bot. Next P0: prove host selected campaign mission and safe guest stage engine loader, TRUE same-scene READY/COMMIT, render remote humans independently of NPC brain, replicate host AI/quests. Update RESTORE and STATE always. Write only experiment/black-coop.

---

## Session53 — original BLACK camera eye → v6 TLS guest↔host↔3 guests, stage gate remains unverified (2026-11-22)
**Important owner priority:** Real host-created, host-launched ORIGINAL BLACK story shared by 2–4 phones with independently moving human-controlled remote players; any original ENEMY or ALLY bot look acceptable temporarily, do not block on finding correct ally/skin/animation. Local first-person hands/weapon/camera stay original. No borrowing NPC brain. **NO minor phone trials**; one major useful co-op trial only after actual scene+player-body gameplay, with minor diagnostics automatically captured.
**Actual source code pushed:** `RoomWire` VERSION/PROFILE6 `HUMAN_CAMERA_EYE` type14 strict bounded 48-byte data (56 with framing), uses original pinned native local player +0x100/104/108 first-person camera sampled read-only ≤10Hz and JNI cached on actual game thread. After *previously independent real story COMMIT only*, guest sends camera to authenticated host TLS with 1-slot conflating network worker, no guest-supplied player ID, session/scene/token/revision/sequence/freshness/rate checks; host gathers latest per TLS peer (300ms), publishes 1-4 original camera-eye positions via existing `WORLD_SNAPSHOT` v5 type13, no engine actor modifications. Yaw remains 0 UNKNOWN, camera eye ≠ body/root. [Real 4-party TLS host+3 CI44973843153 SUCCESS](https://github.com/alwansan/BO/actions/runs/44973843153) with SYNTHETIC fixture coordinates/scene attestations, not actual campaign. [Android TLS44973843154 SUCCESS](https://github.com/alwansan/BO/actions/runs/44973843154), [native+Java Android preflight44973843200 SUCCESS](https://github.com/alwansan/BO/actions/runs/44973843200).
**New real engine state read-only source diagnostic**: pinned `exit_menu.cpp` `InFrontEnd()` predicate is `game && R32(game+kGameState)==game+kFrontEnd`; `room_nonfrontend_original_player.hpp` observes 30 stable original-player/world/state samples, sends ONE code `ORIGINAL_NON_FRONTEND_PLAYER_STABLE_UNVERIFIED_MISSION` (never stage proof, never PREPARE/READY/COMMIT), [Android preflight44974018224 SUCCESS](https://github.com/alwansan/BO/actions/runs/44974018224). No actual phone runtime measured. Latest Windows Android [44974018195](https://github.com/alwansan/BO/actions/runs/44974018195) check live, room-only NO GAMEPLAY.
**P0 still blocking playability:** correctly identify selected original host mission ID and actual loaded stage callback; guest start same original mission via safe engine loader and only then READY, host COMMIT; build independent remote human body using ANY verified original bot appearance and real body-root pose, host shared enemy AI/combat/objectives. Socket CI alone isn't gameplay. Read `memory/sessions/2026-11-22_bo-coop-53-original-camera-eye-v6-authenticated-world.md`, `docs/REAL_EYE_TLS_V6_AND_SCENE_PROOF_GATE_AR.md`, current `prompt/RESTORE_SESSION_AR.md`, STATE. Only write BO experiment/black-coop; never main/CL/biso/ISO/saves. Keep RESTORE updated continuously.

---

## Session52 (2026-11-22): priority playable host story; ANY BOT LOOK temporary
DO NOT block on original ally identification, skinning, green crosshair or correct animations. Remote 2/3/4 human may display any existing original BLACK bot enemy/friend, never copy NPC AI or its live actor. Local player is original FPS.
Implemented real encrypted `RoomWire` v5 host-only `WORLD_SNAPSHOT` type13, bounded up to 4 players ticket/epoch/roster validation, latest-only fanout and 350ms TTL. [Host+3 TLS CI44972561727 PASS](https://github.com/alwansan/BO/actions/runs/44972561727), but game positions MOCKED. New original player camera eye (upstream `kPlayerEye=0x100`) staging native source read-only/10Hz, JNI to Java RoomSession, [44972768370 SOURCE PASS](https://github.com/alwansan/BO/actions/runs/44972768370); not actual phone runtime, NOT actor root/scene loaded. Real selected mission and guest stage loader remain BLOCKERS; owner pinned XBE mission candidate xrefs [44972978138 private PASS](https://github.com/alwansan/BO/actions/runs/44972978138), xref refinement first CI44973072176 failed only synthetic test length, fixed7fd43b5e. Read `memory/sessions/2026-11-22_bo-coop-52-real-original-eye-host-world-v5.md`, `docs/HOST_WORLD_SNAPSHOT_V5_AR.md`, `prompt/RESTORE_SESSION_AR.md`, `memory/STATE.json`. Android run44972799329 CHECK LIVE, room-only NO GAMEPLAY. One major owner phone trial when real same host-selected original mission/players move together; avoid all minor phone tests; safe automatic logs. Only write BO `experiment/black-coop`.

---

## Session51 — OWNER OVERRIDE: ANY BOT BODY FOR FAST PLAYABLE HOST SHARED STORY
**Most recent owner directive (2026-11-22)** supersedes prioritizing matching an original friendly ally mesh/green reticle, advanced original RCB rig/skin/material/animation. For first multiplayer trial, remote human player2,3,4 may appear as **ANY existing original BLACK bot model**, enemy/friend, same generic body for everyone if easiest. Preserve independent HUMAN control and original local FPS hands/camera/weapon; **NEVER borrow or clone NPC AI brain or unsafe live actor ownership**. Urgent goal is actual host-created room + join up to four + host launches normal story and every player enters and MOVES in **same host-authoritative original mission/world**, sees other independent humans; not a names-only lobby or separate single-player worlds.
**Source work**: real TLS `RoomHumanInputWire` v4 (43B frame body, 51B total) with session/ticket/roster/scene/seq and movement, aim, fire/jump; original peer id authenticated from socket; bounded conflating outbound and latest-per-peer inbound with 250ms TTL; never forward until story COMMIT and verified game-thread adapter. [Host+3 real TLS 4-person source CI44971688698 SUCCESS](https://github.com/alwansan/BO/actions/runs/44971688698), but story scene-ready in test is MOCKED, no actual game actor movement. `temporary_bot_visual.hpp` source-gates any independently validated bot mesh as look for human remote, no NPC AI; [C++ ASAN44971624673 SUCCESS](https://github.com/alwansan/BO/actions/runs/44971624673), no actual render. First-input diagnostic once per story, no frame log spam. Latest room-only APK Windows [44971762426](https://github.com/alwansan/BO/actions/runs/44971762426) CHECK LIVE; even if build succeeds **NO GAMEPLAY**, DO NOT ask phone test.
**Critical next** find REAL host original mission selection and actual loaded game-thread scene epoch, make guests load SAME real mission and report READY, real input/snapshot actor movement, choose any bot mesh renderer; host shared AI/quest/combat. Postpone correct ally model search until after playable trial. Owner requires no small test requests; ONE major phone trial soon once real gameplay meaningful, capture minor details in safe automatic logs. See `docs/FAST_SHARED_STORY_TRIAL_AR.md`, `memory/sessions/2026-11-22_bo-coop-51-fast-four-player-any-bot.md`, `prompt/RESTORE_SESSION_AR.md`, STATE. Update continuously. ONLY BO `experiment/black-coop` writable; CL/biso/main/ISO/saves never write.

---

## Session50 — pinned original c5 XBE soldier damage paths: verified private CI (2026-11-22)
**Restore first** `memory/sessions/2026-11-22_bo-coop-50-owner-bullet-damage-xbe-audit.md`, `memory/evidence/BO-OWNER-BULLET-DAMAGE-NEIGHBORHOOD-20261122-safe-summary.json` and `docs/OWNER_ALLY_GREEN_RETICLE_RESEARCH_AR.md`. Owner-reported GREEN crosshair on native BLACK bot ALLIES and inability to shoot/kill remains a lead, **NOT yet verified original faction or invulnerability field**. Direct private owner `alwansan/biso@9ceb6a65` `Black_Xbox_Extracted/default.xbe` passed exact SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f`. New bounded XBE x86 static survey + synthetic fixture and CI [44970444776 PASS](https://github.com/alwansan/BO/actions/runs/44970444776) (initial 256B), [44970558768 PASS](https://github.com/alwansan/BO/actions/runs/44970558768) (expanded 1024B soldier). From pinned original actual `sub_001E74B0` 1024B: **266 decoded, 30 conditional branches, 2 returns**, source-known character kind refs2, health1, dying1 (one related comparison), versus 256B soldier 66 decoded/8 branch/0 ret/none of known fields. Two LOCAL-player damage method windows 256B each contained health/dying and `kCharInvulnerable=0x8F2` (local cheat only). These are **bounded linear candidate neighborhoods, NOT team/faction logic, hit immunity, game-thread reachability or original ally model proof**. Safe reports only derived counts, no XBE bytes, disassembly or owner data exported; no gameplay changes. Source `coop/analysis/ally_bullet_xbe_neighborhood.py`, workflow `black-coop-ally-bullet-xbe.yml`. **Next** disambiguate type comparison vs actual team/friendly relation and independently locate original green reticle + damage denial; associate proven friendly actor->MODEL->D3D shader/RCB, verify real host-selected loaded scene epoch before STORY_READY, then own human remote fullbody in ONE authoritative host world with synced AI/combat/quests. Preserve original FPS local; remote never borrowed NPC AI. **NO small phone tests**: CI first and one major later two-phone trial with automatically bounded safe logs. WRITE ONLY `experiment/black-coop`, never BO main, CL, biso, original ISO/XBE/saves.

---

## Session49 — Owner ally GREEN reticle / original friendly-fire immunity (2026-11-22)
**Owner steering to remember**: when aiming at an original BLACK bot ALLY the reticle becomes **GREEN** and, per owner gameplay, it cannot be shot/killed. Owner has uploaded reference ally photos in <https://github.com/alwansan/CL/tree/main/black%20log/img>. The GitHub file list was inspected: 25 JPGs, latest batch eight screenshots dated 2026-11-22 03:57–03:59; binary pixels were **NOT inspected** through the current GitHub connector. Treat green + no damage as *owner observation* and a research lead, not a proven native faction field or model identity. Original extracted owner ISO source <https://github.com/alwansan/biso>, **READ ONLY** and pinned `biso@9ceb6a65`, includes level00 StLevel.bin/StUnit01.bin/LevelDat.bin/AIWpns.awd; their team flags are NOT decoded. Latest CL log `BLACK-Probe-20261122-035406-841`: c5 image, 730 HOOK001 records and 0 read failures; sampled records contain no validated ally/team/reticle field. Latest two-phone room logs record guest ROOM_JOINED and some host ERROR_BINDEXCEPTION on rehost; **not shared story proof**.
**Actual new code** `67c88548`: `coop/bridge/ally_target_policy.hpp` and synthetic test distinguish **original AI ally** from **authenticated remote HUMAN** and original enemy. Only verified native NPC-friend relation OR trusted same-scene/same-generation host snapshot can yield `FriendlyGreen` and `DamageHint::Deny`. Human body `RemoteAllyBody` with `npcBrainOwned=false`; the phone's own player remains original first person. Unknown identities fail-closed; no copy of NPC brain, no guessed live actor/target/damage/raycast memory patch. [ASAN+UBSAN story CI44969897865 SUCCESS](https://github.com/alwansan/BO/actions/runs/44969897865), [UDP44969897868 SUCCESS](https://github.com/alwansan/BO/actions/runs/44969897868), [game-bridge preflight44969897873 SUCCESS](https://github.com/alwansan/BO/actions/runs/44969897873). **No real HUD/damage/actor/remote render integration**, no playable coop or actual scene READY. Details: `docs/OWNER_ALLY_GREEN_RETICLE_RESEARCH_AR.md`, `memory/sessions/2026-11-22_bo-coop-49-owner-green-friendly-reticle.md`.
**Priority:** independently trace pinned original BLACK XBE target acquisition → friendly relation → HUD green crosshair, and independently friendly shot/hit/damage rejection. Once original actor is proven friendly, map same actor to MODEL/real D3D draw and RCB pose. Parallel verified c5 game-thread host-selected scene-loaded epoch then real host-authoritative human co-op. NO small phone installs; future ONE substantial two-phone shared mission with safe automatic logs. ONLY WRITE `experiment/black-coop`, never main/CL/biso/owner ISO/saves.

---

## Session48 verified — owner packed 3× rows in original fullbody MODEL, Android diagnostic build PASS (2026-11-22)
Read `memory/sessions/2026-11-22_bo-coop-48-stride36-tripled-lanes-android-build.md` and safe evidence `memory/evidence/OWNER-SKIN-TRIPLE-LANES-20261122-safe-summary.json`. Original pinned `biso@9ceb6a65` 3 StLevel, 12 body candidates, 36 stride36 buffers and 38917 sampled referenced vertices. New bounded owner-private `coop/analysis/ally_skin_influence_hypotheses.py` with synthetic CI and workflow. Runs [44967973826](https://github.com/alwansan/BO/actions/runs/44967973826), [44968297799](https://github.com/alwansan/BO/actions/runs/44968297799), [44968462670](https://github.com/alwansan/BO/actions/runs/44968462670) **ALL SUCCESS** (intermediate limit and fixture errors repaired). Full +16 and +20 four-byte lanes on **38917/38917** entries have all byte values multiples of 3 with quotient under25; 0 identical four-byte lanes, 23208/72157 (32.16%) identical slots when weight candidate at +12 is nonzero. +12 4bytes sum≈255 in 83.63%. +16 active duplicate candidate12.35%, +20 47.22%. A naive raw joint-index<25 ranking favors +20 (80.19% vs +16 12.30% normalization-and-validity), but that comparison is MISLEADING if values are shader matrix row pointers ×3, in which case both lanes fit perfectly. **NOT confirmed original weight or bone index semantics, MODEL-to-actual-GPU draw identity, RCB skinning, or animated AllyBody**. Only derived counts uploaded (artifact14732965565), no original asset words.
**Verified complete Android native source diagnostic APK** [44967448414 SUCCESS](https://github.com/alwansan/BO/actions/runs/44967448414), artifact14732891166, SHA256`9aa7d608e02f2b82956e47bdbafcaa4a763616f68bae22e48888ecf14725c24a`, 21874167B, package `org.bocoop.black.roomnet`, **NO GAMEPLAY/NO SHARED STORY**. Workflow metadata JSON says v2/profile2 but actual `RoomWire` source VERSION3/PROFILE3: correct in future substantive build. C5 true host game-thread mission scene loaded/epoch, input/snapshots, remote independent human fullbody and authoritative host world/AI/combat/quests still OPEN. Priority: MODEL resource -> actual bound D3D8 draw then shader/rig binding, independently true stage load, real shared story. Preserve local original first-person, one major owner phone acceptance only, auto bounded logs. No fragmented device trials.

---

## BO-COOP Session47 — Actual original D3D8 bound draw source observer (2026-11-22)
**Only branch experiment/black-coop**. New `2ef9fd50` observes existing original DrawVertices/DrawIndexedVertices before unchanged backend draw. Requires `gpu::State.vs`, nonzero bound vertex-buffer, active stream stride36 equal to declaration stride36 and positive count; generic packed offsets +12/+20 only count when on same stream. Exactly three atomic one-time fixed safe codes 10..12 (indexed draw, nonindexed draw, packed +12/+20). Not raw assets/shader/IDs/actor/pointers, not original MODEL identity or skin weights, does NOT assert STORY_READY.
**CI** [native C++ ASAN+UBSAN 44967390703 SUCCESS](https://github.com/alwansan/BO/actions/runs/44967390703); first stage checks 44967390711/44967416619 failed introduced Python literal quoting (fixed `ac156ea7`); [final source-stage 44967448413 SUCCESS](https://github.com/alwansan/BO/actions/runs/44967448413); [isolated UDP 44967448396 SUCCESS](https://github.com/alwansan/BO/actions/runs/44967448396); [full Windows Android room-only APK 44967448414](https://github.com/alwansan/BO/actions/runs/44967448414) CHECK LIVE (last inspected in progress). Old44966638815 cancelled due superseding pushes, not a build failure. This is **NO GAMEPLAY**, no named BC1_MAT/TOM body binding, actual original RCB skin, c5 scene-loaded callback/mission stage epoch, remote human ALLY or host shared AI/combat/quests yet.
**NEXT:** establish safe original MODEL resource -> active vertex buffer/draw association; independently verify true game-thread host mission/scene loaded/epoch before any STORY_READY. Then independent human-controlled remote fullbody ally (not NPC), maintain original local first-person hands/camera/weapon, authenticated gameplay wire and single host world. **No small owner phone trials**; one large future two-phone acceptance with combined bounded automatic safe logs. Read `memory/sessions/2026-11-22_bo-coop-47-original-bound-gpu-draw-observer.md`, `docs/ORIGINAL_GPU_DECL_SAFE_WITNESS_AR.md`, RESTORE/STATE.

---

## Session46 GPU safe witness status and open actual renderer/scene blockers
C++ original GPU declaration classifier `44966575850 SUCCESS`, source staging `44966623833 SUCCESS`; full Windows Android room-only build `44966638815` CHECK LIVE; not yet a successful new APK claim and **no runtime observation**. Fixed event codes 4..9 cover declarations with stride36 and type at offsets12,16,20,24,28; created shader can be unrelated to candidate character draw. Raw owner vertex metrics `44965837041 SUCCESS` +12 sum≈255 and +20 small byte counts are **not** proven skin attributes. C++ synthetic skin math `44966105790 SUCCESS`, not original matrix convention. OPEN: actual body vertex declaration at draw and model-to-RCB skin weights; original c5 game-thread host-selected scene loaded and scene epoch; real independent remote human fullbody (not AI NPC) with authenticated snapshots/world/combat/quest. No fake READY, no owner small phone test, don't modify game assets/saves.

---

## Session46 verified weak vertex-lane patterns and blockers
`44965837041 SUCCESS` on pinned owned MODEL vertex36 metadata, 12 candidates/38917 bounded samples: +12 four bytes often sum approximately255 (83.63%), +20 four bytes often below 25 (90.85%). **These are not semantic proofs of skinning**: unknown vertex shader declaration, joint indices and weights, original RCB-to-MODEL matrix transform, normalization, materials and animation pose. Initial run44965770507 failed owner private due unlabeled ValueError (possible report size bound); fixed 256KiB derived report budget + safe reasons, 44965837041 PASS. C++ synthetic pose/skin gate `44966105790 SUCCESS` verifies only toy skeleton and weights, not rendered game actor. Original c5 actual game-thread stage loaded, same host world, independent human remote ally, authenticated input and AI/quest/combat remain blockers. Do not ask owner phone for a few data bytes; instrument safely as auto log for one future large real game test. Report docs/OWNER_VERTEX36_SKIN_RESEARCH_AR.md.

---

## Session45 resolved metadata / remaining critical gaps
- RESOLVED owner source hierarchy ambiguity: 9 serialized RCB records in 3 StLevel files correspond to 1 distinct RCB ID per file with consistent parent/lookup/matrix data; `44965062874 SUCCESS` artifact14731331749. All 12 body-model candidate records match 24/25 local skeleton hashes. **This does not prove vertex skinning or unique actual human ally model**.
- RESOLVED offline clip span validation: `44964889359 SUCCESS` 22 RCB-linked bank v8 resources, 675 clips and 61,615 encoding4 track records bounded; no runtime animation/player/pose. Asset metadata only.
- RESOLVED target stream selection: `44965229133 SUCCESS` all 12 body candidates use three stride36 vertex streams with 3 distinct raw format flags/model and one texture-name slot; **bone weights/indices/normals/UV/material meanings remain UNKNOWN**.
- OPEN CRITICAL: real game-thread stage-loaded hook/actual mission ID/host scene epoch; independent original fullbody human visual body and skin weight/bind matrices/animation pose/material shader; authenticated gameplay snapshots, world AI/combat/quest. Original first-person hands/camera must remain local; no NPC AI takeover and no fake story READY. All Actions pass only metadata and value-only contracts. No owner phone trial yet.

---

## Session44 new BODY evidence and limitations — 2026-11-22
Owner MODEL geometry private Actions44964028143 SUCCESS: 12 head/torso/leg name-tagged source candidates from 3 StLevel files, all 12 pass indices/finite XYZ, no raw model data exported; first mission keys BC1_MAT/BC1_TOM/LRG/SOL. Closed only geometry candidate issue, **NOT** verified friendly ALLY render, actual pose/skin weights/texture material, animation hierarchy, actor identity/AI ownership, or engine hooks. Derived buffer-index pairs != true vertex count and triangle strips != rendered mesh triangles. RCB skeleton parent data and MODEL skin influence mapping remain next blockers, do not copy restricted third-party animations.
Actual c5 scene-loaded callback + host shared story/quest/combat/player sync STILL OPEN. Synthetic host human input generation gate44963681576 PASS, APK room-only44962611024 PASS, owner-phone trial not needed until substantial true shared world. See docs/OWNER_ALLY_GEOMETRY_SAFETY_AR.md.

---

## Session44 remaining verified blockers
CLOSED: room-only Android staging build `44962611024 SUCCESS` (artifact14730766256), previous Windows strict source allowlist CI44962275637 failure repaired by commit561eacc0. C++ HOST_HUMAN_INPUT_GATE_PASS on `44963681576 SUCCESS`, including reentrant scene-switch/peer-leave stale commands. These are CI/synthetic only, no phone.
OPEN (critical): actual owner c5 BLACK game-thread mission and scene-loaded evidence/epoch is missing; world xref assignments EAX/EBX alone cannot prove loading, native witness event codes cannot assert READY. RoomWire v3 has no authenticated high-frequency gameplay input/snapshot channel; value-only host input gate must NOT accept raw unauthenticated UDP and cannot mutate BLACK game objects. Remote fullbody human-controlled ally renderer, original owner asset skinning/lifetime, AI/combat/quest/host world remain not implemented. Local first-person hands untouched. Keep source-only vs actual co-op distinction and owner CI-first one major device trial policy.

---

## Session43 APK packaging + external reference status
- CI44962275637 FAILURE was strict Windows packaging diff-list missing newly staged read-only `src/game/difficulty.cpp` and two `src/coop` header names, NOT compilation/gameplay or unit failure. Corrected exact finite whitelist in workflow commit561eacc0. New CI44962611024 check live.
- Peterson047 public TDM reference is binary-only; linked BLACK-Coop source 404 on attempted read. Do not claim reused source, player renderer or mission sync. Third-party `body.bca` animation file permission limited to original mod; must NOT copy. See docs/REFERENCE_BLACK_XBEMULTIPLAYER_20261122_AR.md.
- Actual host selected BLACK story, guest scene-loaded proof and world/AI/quest sync and human remote full-body visuals still OPEN. CI-first, no phone requests.

---

## Session43 research and verified limitations
- Real c5 XBE private metadata44961768279 PASS: both kWorld write sites in one executable section, 9B apart with EAX/EBX sources, direct branch to second; no validated function boundaries/ABI, no known mission ID, stage-loaded callback or scene epoch. EBX possible zero-instruction in previous linear path is NOT proof of zero across branch. Keep actual story integration blocked.
- Staging native source observer at existing original player update and difficulty wrapper added; C++ tests44962215874 and source preflight44962275672 PASS. Diagnostic codes are safe + bounded, do not indicate actual game loads, and are not connected to STORY_READY. Android full room-only APK44962275637 check live before claiming compile success. Never ask owner to install this just for these markers.
- OPEN: human-controlled remote full-body ally rendering/animation/ownership, host authoritative one shared mission/world, camera local hands unchanged, real AI/combat/objectives and replay-safe state sync. Other synthetic TLS lobby tests are NOT the missing gameplay.
- CI-first owner policy, one major two-phone acceptance only once substantial real gameplay ready.

---

## Session42 actual CI status — 2026-11-22
Closed for synthetic CI: async room diagnostics first Android failure44960759104 was a pre-flush test assumption; second44960934419 was process logger name accidentally matching bo-room network thread leak prefix. Both corrected without relaxing network teardown assertions. Android29 latest44961110132 **SUCCESS** with ROOM_LOG_ASYNC_PASS, TLS_PLATFORM_PASS, STORY_READY_ASYNC_ANDROID_PASS, ROOM_LOG_ROTATION_PASS; source44961110163 PASS. No physical device test warranted.
Unresolved: world global 0x2D45D4 sites 0020BF48 and 0020BF51 are x86 static register-source write candidates, NOT proven scene-loaded callbacks. Original game mission/stage identity, verified game-thread hook, real remote ALLY body model ownership/renderer, input/world/AI/quest/combat replication still absent. Report docs/BLACK_WORLD_POINTER_XREF_AUDIT_AR.md, Session42.

---

## Session42 residual engine and diagnostics blockers — 2026-11-22
- RESOLVED for synthetic host/guest TLS: synchronous guest READY write on future BLACK game thread replaced with capacity-one async writer and stale ticket checks. JVM44960452007/Android29 44960451936 SUCCESS. No actual BLACK scene callback yet.
- CI issue: Android29 44960759104 FAILED because instrumentation asserted files before asynchronous writer export barrier. Fixed 0de18eb19. Android29 44960934419 FAILED because persistent diagnostic thread named bo-room-safe-log-io collided with bo-room-* network worker leak assertion; renamed to bo-diagnostic-io 367052097. Check fresh run44961110132. Not a gameplay failure or request for phone trial.
- Private owner c5 XBE world global `0x2D45D4` xref audit: 892 read/write candidates, only 2 register-sourced potential writers `0x20BF48`, `0x20BF51`, not proven real world stage loader, epoch, actual mission ID or safe hook. Source docs BLACK_WORLD_POINTER_XREF_AUDIT_AR.
- OPEN: true host-owned campaign in one world, actual loaded mission scene in BOTH clients, fullbody remote human AllyBody not NPC AI, real movement/weapon/AI/quest/combat sync; no current v3 gameplay acceptance, no owner phone test until major milestone.

---

## Session41 CL-derived issues and current verification
- Historical CL roomnet logs: first-device `ERROR_BINDEXCEPTION=4` across 9 files; second 103 `SCAN_STARTED` after joining within 6 event files. New code prevents background discovery while active and duplicate HOST/JOIN, staged into APK. JVM+Android29 and source passed; NOT physically confirmed on current phones. Existing TLS Server also has setReuseAddress/1.5s bounded bind retries; don't mark all real device port behavior proven.
- Historical original player model `8/8 valid` **44 bones with arms only**, NOT an entire character; must render separate human-controlled ally body for remote players, with no NPC AI ownership. Remaining game scene loader and actor lifetime unknown. CL 22 Nov HOOK001 captured single world epoch per file and no nonlocal player samples; not evidence of remote absence from every engine path.
- Remaining actual gameplay blocker: c5 mission/stage selection and verified game-thread world-load callback, host world synchronization, remote human actor renderer/input, weapons/combat/AI/quest. Lobby-only tests cannot close.
- CI safe CL evidence44959212836 PASS; Android29 active-state scan gate44959047461 PASS; source44959047532 PASS; JVM44959047515 PASS; lobby-only APK44959047454 check live.
- Owner explicitly wants CI-first and NO small phone testing; a single substantial test later, safe automatic logs. See docs/CL_20261122_TWO_DEVICE_LOG_EVIDENCE_AR.md and docs/MAJOR_DEVICE_TEST_GATE_AR.md.

---

## Session40 Android29 test infrastructure restored; real game remains open
- RESOLVED for synthetic instrumentation: PackageManager cold-boot stall and accidental Bash replacement corruption; guarded by `bash -n` and bounded retries. Android29 `44956919973 SUCCESS`, then `44957104337 SUCCESS` with `TLS_PLATFORM_PASS` and `ROOM_LOG_ROTATION_PASS`.
- Verified current/rotated safe room log export and injected tampered-line rejection on Android29 without owner tests. Caveat: no real BLACK game/scene/remote body data or combined game telemetry yet.
- OPEN: actual host mission scene loaded event/epoch and game-thread bridge, independent fullbody real-human ally renderer, input/world/AI/combat/story. 2/4 LAN lobby and safe event transport are not playable co-op.
- User prefers CI testing; do NOT ask for phone trials until meaningful shared story/remote player milestone.

---

## CI Android29 script corruption diagnosed and guarded — Session40
Old Android29 run44956473707 failed during cold synthetic emulator PackageManager install timeout, before instrumentation. First retry44956739205 failed early because an automated JS replacement accidentally expanded a shell `$'` replacement sequence, corrupting Bash around `grep '^package:android$'`; this was a SCRIPT syntax failure, NOT Android app/TLS or game failure. Clean script commit5dd84a4a reconstructs tail explicitly; workflow commitc8c734bc runs `bash -n` BEFORE downloading SDK or emulator boot. New CI44956919973 check live. No owner phone test for CI errors.

---

## Session40 residual issues after offline owner static/XBE and diagnostics (2026-11-22)
- XBE executable entry mapping verified on actual owner c5 in CI44956138064 but a 160B disassembly window is insufficient for scene loader semantics, ABI and epoch; do not invoke as game Start or mark game-thread hook solved. Mission index0 remains valid.
- TLS v3 story diagnostic codes are automatically logged by Android room session even when overlay is dismissed; JVM test44956473684 PASS. Combined previous/current sanitized log segments are source-patched, Android29 44956473707 / lobby-only APK 44956473687 require live confirmation. No true mission scene callback means no real story event emitted yet.
- STILL UNIMPLEMENTED: remote human fullbody AllyBody and independent renderer life, shared host world/AI/combat/objectives/weapon sync, actual two-phone shared mission.
- Device policy: do not ask owner for tiny tests, new APK lobby-only or repeated model ISO. One meaningful major trial only after all acceptance gates, collecting all small details automatically. docs/MAJOR_DEVICE_TEST_GATE_AR.md.

---

## Owner test burden policy — 2026-11-22
Owner does not want small/fragmented phone tests. CI and synthetic/emulator tests are preferred. A physical two-phone test remains BLOCKED until meaningful real shared-story engine/remote-human integration exists. Automatic consolidated privacy-safe diagnostics must capture minor uncertainties during that one major test. Do not ask owner to try the successful lobby-only APK; no gameplay exists. Follow docs/MAJOR_DEVICE_TEST_GATE_AR.md and AGENTS.md.

---

## Session39 — new bridge verification and residual issues (2026-11-22)
- **PARTIAL RESOLUTION**: prior TLS v3 story transport was accessible only through RoomTls. Source commits 86faa014/5370dcae now expose a bound-thread RoomSession facade with generation reset and cover it by actual pinned-TLS JVM SessionTest. CI44954581676 SUCCESS. A caller can *claim* its thread with `bindStoryGameThread`, so actual BLACK hook/scene-loaded evidence is still essential before wiring production gameplay; this is a guard, not attestation.
- **OPEN**: exact mission start and scene-ready authoritative events inside c5 BLACK, JNI game thread contract, guest scene load. Current Android app never calls the facade. No fake READY or shared Start claims.
- **OPEN**: original owner CL shows rapid rehost BindException; source fixed-port 1.5s retry had synthetic tests, not tested again on two actual owner phones. 2/4 proves lobby only.
- **OPEN**: fullbody human-controlled remote AllyBody renderer independent from NPC AI, synchronization of input/animation/weapon/world/AI/combat/quest/saves. No player actor visible from session's owner-provided lobby screenshots.
- **CHECK**: newest Android29 44954581718 and Android lobby APK 44954581624 outcome, then prove v3 phone compatibility only with matching APKs.

---

## Session38 current residuals after real story TLS tests
- FIXED in source/CI: strict STORY PREPARE/READY/COMMIT/ABORT control carried by actual TLS; JVM44952822531 and Android29 KeyStore44952822499 PASS. NOT coupled to BLACK mission selection/loaded scene, so no playable co-op Start.
- MITIGATED in source and isolated test: CL ERROR_BINDEXCEPTION after quick Host/Leave/Host; bound 1.5s TCP retry + SO_REUSEADDR while disallowing simultaneous host, JVM44953176046/Android29_44953176054 PASS. Not yet proven on owner Xiaomi/Huawei; original event code cannot determine all contributing router/OS causes.
- INCOMPATIBILITY INTENTIONAL: RoomWire VERSION/PROFILE now3 to reject old room-only v2 clients; both real phones must run matching future build, do not mix with previous owner accepted APK.
- OPEN: actual host mission-select hook and game-thread scene load/ready, engine world snapshot, independent human-controlled FULLBODY ALLY render on other phones, AI/quest/combat/weapon animation and multi-phone same-world. 2/4 room and source/JVM/Android socket tests are not gameplay.
- OPEN: original JPG assets in CL indexed but GitHub connector refuses binary image content; don't pretend pixels verified.

---

# ACTIVE Session38 — Open and resolved distinctions (historical issues below)
- RESOLVED: Latest original-player primary-model phone test was received: 8 complete arms-only primary model samples, not absent. It is NOT proof all game model resources lack a fullbody.
- OVERRIDDEN: Session36 prohibited ally substitutes; owner now approves independent ALLY VISUAL mesh for remote real-human player ONLY, while everyone's ORIGINAL local first-person experience remains unchanged. Do not reuse AI-controlled NPC brains/actors.
- OPEN: Actual 3D ally rig skinning/material/animation, engine-owned renderer instance, cleanup and human-input ownership not validated on phones. The new RemoteAllyRoster only tracks value-only selection for both host and guest.
- OPEN: HostMissionBarrier only source/CI; need real TLS mission-control messages and actual shared BLACK stage loading/ready/host authority, then network motion/world sync; don't claim playable co-op.
- OPEN: CL host event log includes ERROR_BINDEXCEPTION during quick Host/Leave/Host; isolate TCP vs presence UDP bind/race in future.
- OPEN: New JPG metadata indexed but binaries could not be displayed via GitHub UTF-8-only tools; no fabricated visual screenshot findings.
- ACCEPTED: older two-phone room lobby 2/4 and real automatic scan JOIN_CONNECTED/ROOM_JOINED logs, not mission/actor proof. No ISO reupload needed.

---

## تسليم جلسة37 — 2026-11-22
المالك سينتقل مؤقتاً لجلسة أخرى. اقرأ memory/sessions/2026-11-22_bo-coop-37-cloud-handoff.md وdocs/CLOUD_HANDOFF_AR.md. جميع أدوات المصدر والخطط والمراجع مرفوعة؛لا تعتمد علىالجهازالمحلي. آخرAPK44946597973/artifact14724535514مفحوصة،ولا سجلاتهاتفجديدةبعدطلبها. الشخصيةالأصليةفقط،لاNPCبديل،ولاsharedStartأوgameplayبعد. افحصCLالحيةأولاًلأيcaptureجديد.


## نتيجة جلسة36 — حزمة جاهزة والتحقق مكتمل
run44946597973 على8bc7f92 SUCCESS؛قرئتjob136975283608:BUILD SUCCESSFUL وoriginalLOD/resetguardsPASS وصورةc5مطابقة. artifact14724535514 نُزل،CRCوالـAPKsha25694400c2d650d772293ab5f3a1d0c72e91d1974dea64a3004d18404e90ccb02d0 تحققااستقلالاً،الحجم21860751B. الحزمةorg.bocoop.black.defaultplayer،originalprotagonist-onlymetadata،alternate_actor_models=false،لاguestwrites/newactor/sharedStart. التقريرالآمنmemory/evidence/DEFAULT-PLAYER-APK-44946597973.json. مطلوبالآنفقطcaptureحيجديدللشخصيةالأصليةوفقdocs/DEFAULT_PLAYER_DEVICE_TEST_AR.md،لاNPCfactoryولاإعادةbaseline. لاphoneproofلهذهالحزمةبعد.
## جلسة36 — أحدث توجيه واجب
المالك يريد شخصيةاللعبةالأصليةعلىكلهاتف،ولايريدbodyبوتأوحليف. اقرَأmemory/sessions/2026-11-21_bo-coop-36-original-player-steering.md والخطةdefault-player-link. تجربةالهاتفالتاليةتتحققمنMODELالمحمّلفيlocalplayerالأصلي،لاتطلبNPCfactorycaptureبعدهذاالتوجيه. لاsharedStartأوبعيدمرئيبعد؛المحليوالـreviewللقارئالجديدPASS،APKالتاليقيدالإطلاق. لاادعاءأنمجردlocalplayerعلىكلجهازينسخالجسموالعالمعبرالشبكة.

## جلسة35 — الحالة الحية
المالك أثبتLANعلىهاتفينبالـAPKالقديمةمعPIN:HOSTING/JOINEDو2/4فيصوروسجلاتCL22:47. النسخةالتلقائيةالجديدةبُنيتوفُحصت:44942892701/artifact14722613067/SHA256b149bc2c...،ولمتُختبرعلىالهاتفبعد. لاgameplayبعد. نتابعإلىfactoryroomرصدإنشاءالأجسامالأصليopt-inمحددبـ32scalarrecord،لاspawnولاStartوهمي. اقرأmemory/sessions/2026-11-21_bo-coop-35-owner-lan-pass-factory-contract.mdوالخطةactor-factory-observation. المحليJVM/Android29/18preparer/2guardوnativeASAN/UBSANPASS؛بناءfactoryroomالتاليقيدالإطلاق.


تحقق Actions على4c9f8bb: source preflight44942174338 وJVM44942174361 وAndroid29 instrumentation44942174364 كلهاSUCCESS. سجلjob136961573450 قرئ ويؤكدTLS_PLATFORM_PASS: زرHost الفعلي بالأسماء الافتراضية فتحTCP24872، joinAutomaticنجح، وBackلميغلقActivity. APK44942174355مازالقيدالبناء؛لااختبارهاتفينبعد.
جلسة34: ثبت أن آخر محاولات الهاتفين توقفت عند تحققPIN الفارغ قبل الشبكة. أزيل الشرط وأضيف اكتشاف الأجهزة الخاملة. ما زال تحققAndroid/هاتفين مطلوباً؛ عدم اكتشاف أقرانVPN خارج/24 أو مع عزل المرور يحتاجIPيدوي. لا gameplayبعد.

## SESSION33 — حالات مفتوحة من تجربة هاتفين
- OWNER-LAN-JOIN: الجهازان على Wi-Fi نفسه لكن لا دليل authenticated JOIN/2/4؛ القديم يحتاج manual IPv4/fingerprint/PIN، ولا سجل network. الجديدة تضيف discovery+safe error codes؛ real device acceptance PENDING.
- OWNER-ROOM-UI: شاشة Activity منفصلة لا تشبه BLACK وتتسبب بمسار خروج/Back؛ source fix Dialog in BlackActivity and concise Flash buttons. لا ادعاء نجاح الجهاز قبل الاختبار. الواجهة styled وليس 100% Flash.
- OWNER-ROOM-LOGS: سجل الهاتف الثاني BLACK-Probe-20261121-194623-826/black_log.txt صفر بايت؛ تم إضافة scrubbed persistent coop_room_events.log لكل هاتف + copy; تحقق من الشمولية على الأجهزة.
- REAL-COOP: لا يوجد START قصة، remote actor+324 مستقل أو game state/mission/combat sync. عدم إظهار زر بدء خادع.
- NETWORK-TRUST: discovery UDP لا يثبت المضيف. أول اتصال يؤكد SHA256 كاملة وPIN؛ LAN broadcast قد يفشل مع AP isolation/VPN.

# المشكلات المفتوحة
- SRC-001: إصدار APK العربي والـXBE وإصدار libmain غير مثبت بالمصدر؛ استخرج hashes أولاً ولا تستعمل offsets.
- SRC-002: Windows recompiler يتطلب default.xbe صالحاً وSDK. لا نملك source الخاص بتعديلات العربية، قد تحتاج لاحقاً من المالك.
- ACTOR: singleton kWorldPlayer؛ لا دليل على قدرة إنشاء جندي متحكم به أو لاعب ثانٍ. تحديد factory/lifecycle/collision/AI/camera.
- NET: UDP probe غير مشفر وغير مصادق، لا يجرب فقد حزم أو NAT أو هاتفين؛ ليس production.
- SAVES: فصل حفظ co-op عن single player، وعدم استبدال توقيع/حزمة المالك.
- LICENSE: لا نشر game.iso/default.xbe/generated recompiled game assets؛ حقوق مستقلة عن MIT.

## نتيجة SRC-001 الفعلية — 44813529439
CI SUCCESS وartifact14666839620، قراءة job136555949209 مباشرة. اجتاز SHA256 للأصل ISO 1793179648 B وlibmain 72073216 B، XDVDFS partition 0، XBE 2813952 B، AArch64. Disc SHA1 c5bfed25449c8212d7a976f15246e87ead57713f مقابل kImageSha1 من ELF 82d0588124e70102ee650f8969c0a56374476234: **غير متطابقين**؛ disc ليس Europe المعروف. لا يثبت السبب (تعديل عربي/إصدار/غيره) ولا source commit أو أن APK الحالي لا يعمل. لا تعطيل gate، لا استخدام offsets غير مثبتة. SRC-001 فحص نجح؛ **قبول هوية المصدر BLOCKED**. التالي SRC-002a: تحديد US hash/commit من مصدر upstream وmetadata الإصدار العربي، ومقارنة XBE header/sections مع الصورة المطابقة المرخصة إن توفرت. إن تعذر، اطلب من المالك فقط default.xbe الأصلي المطابق أو مصدر تعديلات Arabic أو log تشغيل كامل؛ لا تفترض الحاجة للثلاثة. بناء baseline ثم HOOK-001. تقرير: memory/evidence/SRC-001-identity-44813529439.json، https://github.com/alwansan/BO/actions/runs/44813529439. لا APK جديدة ولا gameplay أو اختبار هاتف.

## جلسة 03 — SRC-002a: تثبيت v1.5.0 (2026-11-21)
- APKTool 1.5.0-ar1.0/code35، META-INF revision f3754d6985a8c35204e7df071546e586b10f1806 مطابق tag v1.5.0 upstream (ليس main 8e9c6fc). المصدر المرجعي للعمل على baseline أصبح tag v1.5.0؛ المصدر العربي الكامل غير مثبت. esp.cpp غير موجود في tag؛ layout/cheats/hooks موجودة. راجع docs/V150_PROVENANCE_AR.md.
- CI [44817699656](https://github.com/alwansan/BO/actions/runs/44817699656) completed/success؛ تحقق SHA256 الرسمي، ومقارنة ELF حقيقية، job136570202480، artifact14669549225. الرسمي libmain.so 17957208 B، العربي 72073216 B؛ كلاهما AArch64 وkImageSha1=82d0588124e70102ee650f8969c0a56374476234. .dynstr/.dynsym/.rodata SHA identical؛ .text مختلف، file SHA مختلف. جميع 6 hook-symbol candidates موجودة في الاثنين (بحث substring فقط؛ لا إثبات function semantics). disc الموجود c5bfed254... لا يطابق compile image، لا نُضعف الفحص. النتيجة تثبت release provenance وcompile image match الرسمية/العربية، لا source العربي المطابق ولا compatibility للـISO. دليل memory/evidence/SRC-002a-release-comparison-44817699656.json. NET regression 44817699458 SUCCESS.
- التالي: تحقق من نسخة قانونية لـdefault.xbe SHA1=82d0588124e70102ee650f8969c0a56374476234؛ المالك قد يملكها من نسخته الأصلية. لا تستعمل encrypted upstream XBE أو تدّع تحويل XBE العربي. إن لم تتوفر، واصل شبكات C++ مستقلة/اختبارات Android دون game assets. لا APK ولا gameplay. يمكن فحص hash الدوال المرشحة في ELF كدليل إضافي فقط؛ لا كتابة ذاكرة. لا نحتاج نشر أصول جديدة في public repo. إذا رفع المالك نسخة XBE مطابقة فليكن في مسار خاص مع صلاحية مشروعة، لا تُرفع للفرع العام.

## NET-003/004
- NET-003 Linux and Android cross compile PASS 44834027711. **Android phone execution untested**. CLI no game integration, no scene actors, no 4 device test. See `memory/evidence/NET-003-cpp-android-44834027711.json`.
- Packet session token is predictable/not cryptographically authenticated; no packet encryption, replay protection on join, IP roaming, NAT traversal, congestion/reliability. Use laboratory only, private networks. Binding 0.0.0.0 on trusted LAN is opt-in.
- NET-004 needs two Android phones with matching permissions/ZeroTier, device logs/performance; no public forwarding of port 24872.

## جلسة 05 — تحقق محلي ومسار بديل للتحليل فقط
- بداية الفرع الحية `fcf4b85b0b5a5ae3057a0cec57b1c2b342706809`؛ أحدث Python CI `44834294477` completed/success.
- فحص الملفات المحلية أعاد إثبات disc XBE SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f` وcompile image `82d0588124e70102ee650f8969c0a56374476234`؛ لا تطابق.
- المالك أكد أنه لا يملك XBE الأصلي المطابق أو مصدر التعديلات العربية. لا تكرر طلبهما كأنهما متوفران.
- فحص ساكن محلي: title `45410083`، retail entry `25A3F` يطابقان config v1.5.0؛ 13 قسماً اجتازت internal SHA1 section digests. هذا لا يثبت أن الكود يطابق الصورة الأصلية أو صحة offsets.
- `weapons.cpp` مثل `esp.cpp` غير موجود في tag v1.5.0؛ المراجع السابقة من main لا تستخدم لإثبات baseline.
- أضيف workflow candidate-audit لتحليل وإعادة ترجمة ساكنة في Windows بواسطة المصدر وSDK المثبتين؛ لا يبني المحرك/Gradle/APK، ولا ينشر مشتقات اللعبة. انتظار نتيجة CI قبل أي قبول.
- محلياً: unittest المصدر 2/2 PASS وPython two-client UDP selftest PASS؛ لا هاتف/Android runtime.

### أول CI للمرشح — تصحيح toolchain
Run 44840599544 على code e0e1378843e7a727feb1cfe35d7404ebaafb0e2c فشل في بناء أدوات التحليل قبل تشغيل recompiler. سبب مثبت من log job136647712886: global LLVM Clang20 اختير عبر PATH مع MSVC14.52 الذي يطلب Clang22 (`STL1000`). التعديل يختار `$VCINSTALLDIR/Tools/Llvm/x64/bin/clang-cl.exe` صراحة، كما يفعل build.bat المرجعي، ويسجل الإصدار؛ لا تعطيل لشرط STL ولا تغيير SDK/ملف اللعبة. لا نتيجة توافق للمرشح من التشغيل الفاشل.

### SRC-002b static probe — نتيجة حقيقية وخطوة فحص أعمق
Run 44840960136، job136648896882 على code dd5261a28db3443a6bacbccc7e8fdd1114e818a2 SUCCESS؛ bundled Clang22.1.3. تحليل المرشح c5bfed... نجح بنيوياً مع config/SDK المثبتين: 15042 functions و798292 instructions، لكن statistics تحتوي تعليمات غير منفذة؛ لا baseline/توافق gameplay. artifact14680518020 metadata فقط. أضيف فحص مستقل xbe_symbols لمقارنة symbols/data/ABI، وتقرير عن دوال host الغائبة والتعليمات غير المنفذة فيها وعند hooks؛ انتظار CI الجديد. لا تحويل الصورة القديمة أو تعطيل تحقق hash.

### إغلاق فحص الرموز — 44841534370
Run 44841534370 على code 08fd55b4cb1640b4b56445d43e1801dbc635b1a3 completed/success؛ job136650848324 وartifact14680348280. استقلالاً: symbols108/154 وdata6/11 مطابقة؛ البقية46/5 غير مكتشفة أو غير محسومة. ABI11/11 غير محسومة (`?` أو غياب)، لا تعارض موجب مثبت. جميع host-targets28/28 موجودة؛ لا trap لتعليمات غير مدعومة داخل هذه الدوال ولا عند مواضع hooks، لكن بقية الناتج يحوي تعليمات غير مدعومة. **لا تعادل هذه الفحوص إثبات semantics أو layout أو spawn**. الدليل memory/evidence/SRC-002b-deep-44841534370.json.
- تقرير Android upstream #33 يسجل نفس c5bfed... مع v1.5.0 وتحميل Vulkan: https://github.com/RanggaCasper/Black-Android/issues/33 ؛ ليس إثبات تشغيل نسخة المالك أو توافق co-op، ولا دليل أن اختلاف البصمة سببه التعريب.
- سُئل المالك إن كانت نسخته تدخل مهمة؛ الإجابة/سجل الهاتف لم يصلا بعد. دليل جمع سجل التشغيل: docs/DEVICE_BASELINE_AR.md. هذا طلب تشخيص للنسخة الأصلية، لا تجربة APK جديدة أو تعديل ملفات الحفظ.
- مسار الأصل82d058... ما زال غير متاح؛ مسار candidate c5bfed... لم يغلق، يحتاج حسم static ABI/offsets ودليل runtime. لا تعيد بدء مخابر الشبكة أو طلب XBE المفقود دون دليل جديد.

## جلسة 06 — حسم معنى ABI غير المحسوم
فُحص HEAD08c545a8389f509058ff49cfd62b187dae6f6fca وActions مباشرة؛ آخر run44841534370 completed/success ولا run حي آخر. فُك ترميز x86 محلياً في مجلد مؤقت لجميع مواقع ABI11. الاستخدامات المشاهدة للسجلات/stack تدعم إعداد المصدر (edx/eax/esi/ecx/ebx وret4/16 حيث شوهدت)، لا تثبت ABI/semantics كاملة. الدليل memory/evidence/SRC-002b-manual-abi-inputs.json؛ لا game bytes/assembly في المستودع، وحذفت المشتقات المؤقتة تلقائياً. لا تعتبر46 نتيجة scanner فارغة تعارضات موجبة.

الحد الحالي: نحتاج تحديد الصورة التي تحملها النسخة المثبتة فعلاً. AssetFolders يبحث في Black المشترك وفي ملفات التطبيق؛ لذلك قد يكون ISO المرفق مختلفاً عما يقرأه الهاتف. نتيجة دخول المهمة والسجل ما زالا غير واردين. لا نطلب XBE المفقود ثانية. لا نبني gameplay فوق افتراض أن القرص المرفق هو المستخدم فعلاً، ولا نقول إن APK جماعية جاهزة.

## جلسة07 — وصول تجربة الهاتف الفعلية وبناء candidate منفصل
المالك أكد أن النسخة العربية تدخل القصة وتعمل دون مشاكل، ونسخ بيانات التطبيق محلياً. قرئ السجل الخاص دون نشره: build f3754d6، API35، Adreno730، loaded XBE c5bfed... مع another release، Exit0 بعد146s. تأكد SHA256 لنسخة ISO من الهاتف=f6d15ff277eb19ef4fb9092e245599c7ae68f5a49c6ac7db60934a32ec246c62 مطابق للـmanifest. هذا يحسم أن الصورة المرفقة هي المستخدمة فعلاً؛ لا تعاود طلب تجربة الأصل أو XBE المفقود. نجاح المهمة من إفادة المالك، لا من instrumentation داخل log.
السجل يحوي `quit: ConfirmWarning, FEAudio failed` في مسار exit_menu.cpp؛ سُجل ولم يخفَ، ولا نفترض منه عطل قتال/صوت أثناء المهمة. دليل metadata فقط DEVICE-existing-owner-runtime.json؛ لا raw log أو حفظ في GitHub.

بدأ مسار بناء أساس مرشح مستقل من نفس c5bfed image الفعلي بالمصدر/SDK المثبتين. ليس إعادة إنتاج ثنائية للمصدر العربي ولا قبول ABI كامل؛ compile image سيتولد تلقائياً من المدخل الحقيقي دون تعطيل match. الحزمة org.bocoop.black.baseline، Java namespace/JNI الأصلي محفوظ، native/src/config لم تُعدل، حفظ منفصل، label NO CO-OP، updater محجوب في هذه الحزمة. لا أصول لعبة في APK. هذا اختبار baseline يلزم قبل HOOK/ACTOR، لا استبدال لهدف co-op.
خطة التنفيذ docs/superpowers/plans/2026-11-21-candidate-baseline.md؛ الاختبارات المحلية4/4 PASS (بدأ test preparer قبل وجوده ثم نجح بعد التنفيذ)، YAML/Python syntax وdiff check PASS. CI baseline مجهز ولم تثبت نتيجته بعد.

Baseline CI بدأ فعلياً: run44845056293، job136662594366، code6aa718839d912d8924af62f11c4c55999847c24e؛ تابع نفس run ولا تعيد تشغيله بسبب مهلة رصد. قبول البناء/الفحص/الهاتف لم يثبت بعد. دليل تجربة الناتج عند نجاحه docs/CANDIDATE_BASELINE_DEVICE_AR.md. Python regression44845056127 SUCCESS؛ source audit44845056215 قيد الفحص.

### Baseline أول محاولة — تشخيص Windows CLI
Run44845056293 على code6aa718839d912d8924af62f11c4c55999847c24e فشل job136662594366 قبل أي native compile: Gradle رأى Task `.5.0-baseline-candidate1` بسبب tokenization لوسيط PowerShell غير المقتبس. اقتبست وسيطي -PversionName و-PversionCode كـstrings كاملين؛ لم يتغير source/SDK أو إعداد اللعبة. الاختبارات وتجهيز الحزمة وإعادة الترجمة نجحت قبله، وAPK/metadata uploads تخطيت كما يجب. source audit44845056215 SUCCESS وUDP44845056127 SUCCESS. لا APK أو runtime acceptance من هذه المحاولة.

إعادة baseline بعد إصلاح quoting تعمل فعلياً: run44845637359/job136664530136/code5212673e5bc92bbe30a7e001cc8e26924a14f0af. اجتازت تجهيز الأدوات والاختبارات/recompiler/Android SDK ووصلت إلى Gradle Android compile عند آخر رصد. هذا handle حي يتابع، لا يعاد تشغيله بسبب timeout ملاحظة. لا APK مثبتة بعد.

### بناء candidate فعلي — 44845637359 SUCCESS
Run44845637359/job136664530136 على code5212673e5bc92bbe30a7e001cc8e26924a14f0af completed/success. Gradle BUILD SUCCESSFUL8m17s. Artifact14682519899 `black-baseline-candidate-android-arm64-NO-COOP`، APK21727055B SHA2564ead325d67772a1c877b9f073aebc0e9eaaab45e9a0028d2380645c4bc2f69f7.
فحوص APK الفعلية: packageorg.bocoop.black.baseline، activitycom.racas.black.BlackActivity، ARM64 فقط، debug signature، لا صلاحيات تخزين واسعة أو ISO/XBE/generated source في APK؛ native compile-image SHA1=c5bfed25449c8212d7a976f15246e87ead57713f يطابق XBE الحقيقي. تنزيل artifact وإعادة فحص ZIP/APK SHA وELF identity محلياً نجحا مستقلاً. metadataArtifact14683233366. لا Android runtime جديد قبل تجربة المالك، ولا co-op أو read-only gameplay hook أو spawn منفذ.
الدليل memory/evidence/BASELINE-candidate-build-44845637359.json؛ رابط artifact https://github.com/alwansan/BO/actions/runs/44845637359/artifacts/14682519899 . الملف المحلي outputs/baseline-44845637359 خارج repo. دليل التجربة docs/CANDIDATE_BASELINE_DEVICE_AR.md. لا تعيد طلب تشغيل APK الأصلية؛ ننتظر تجربة الحزمة الجديدة فقط.

## جلسة08 — تسليم عاجل لجلسة أخرى
المالك جرّب APK baseline الجديدة وأكد عدم وجود تعطل/مشاكل، وقدم بيانات org.bocoop.black.baseline محلياً. log يؤكد image c5bfed... دون another release وExit0 عند162s. missing image عند1s سبق إكمال الاستيراد عند24s وليس فشل baseline. FEAudio عندالخروج ما زال معروفاً. دليل metadata DEVICE-rebuilt-baseline-owner-runtime.json؛ raw log وISO/hdd لا تنشر. لا تطلب إعادة تجربة baseline أو XBE المفقود.

بدأ HOOK001 اختيارياً للقراءة فقط، ثم طلب المستخدم إيقاف التنفيذ وتسليم كل شيء سريعاً قبل انتهاء حد الاستخدام. كود WIP محفوظ في coop/probe/{probe_core.hpp,probe.h,probe.cpp,test_probe.cpp} وخطة docs/superpowers/plans/2026-11-21-readonly-probe.md. **ليس متصلاً بالبناء أو المصدر بعد، لا APK probe ولا CI له ولا تجربة هاتف.** core test نجح محلياً بالأمر clang++ -std=c++20 -O1 -Wall -Wextra -Werror -pedantic coop/probe/test_probe.cpp -o /tmp/black-probe-test && /tmp/black-probe-test. هذا اختبار host فقط، sanitizer/Android لم يجريا لهذا الكود.

التصميم: package جديدة org.bocoop.black.probe1 لتجنب اختلاف debug signing مع baseline والحفاظ على الأصل. opt-in زر Start capture عند التشغيل، native JNI nativeCoopProbeEnable غير مضاف إلى Java بعد؛ لذلك لا تنشط النسخة الحالية من sampler وحدها. لا شبكة أو writes إلى guest أو spawn أو استدعاء guest مجهول. process_vm_readv(getpid()) للقراءات من العملية نفسها، bounds32/host overflow/partial reads/NaN guards، لا memcpy مباشر من guest pointer. سجل محلي logs/coop_probe.jsonl. حدود:64 pointer/vtable pairs لكل world epoch،256 attempts/s،32 records/s،2048 records،180s من world صالح،1MiB؛ انتظار world محدود300s في runtime. IDs مجرد correlations؛ إعادة استعمال نفس address/vtable قد لا تكتشف. Animation owners ليست character تلقائياً، لا تقرأ pos/health منها. player health فقط عند local=world+0x30، LOD يقرأ pos +A0، resource roots +320/+35C مؤقتة الدلالة. errno وfloating environment تحفظ عبر HostState.

## ما لم يُنجز قبل التسليم
1. مراجعة runtime WIP كوحدة كاملة؛ يحتاج #include <algorithm> صريح لاستخدام std::copy. catch-all يعطل capture لكنه لا يغلق fd أو يسجل السبب؛ راجع مسار الخطأ. إعادة Enable بعد End ليست مصممة لإعادة بدء التسجيل؛ اجعل زر مرة واحدة/حالة واضحة. راجع انصاف/rate rotating-ID sampling وثبات IDs وepoch/reset عبر reload؛ لا تعتبرها lifecycle مثبتة.
2. أضف prepare_probe.py مع preflight anchors قبل أي كتابة، يرث private import/no broad storage/updater isolation من prepare_baseline.py، يغير package/label إلى probe1 وREAD ONLY، ويضيف Java opt-in/native declaration. حافظ على JNI class com.racas.black.BlackActivity. لا توقع بمفاتيح الأصل ولا تستبدل الأصل أو baseline.
3. اربط source cheats.cpp entry sub_001E5FA0 بنداء ObservePlayer(ctx.ecx,base)، وhooks.cpp entry sub_0017B130 بنداء ObserveAnimation(ctx.esp,base). لا تغير __imp calls أو guest context. انسخ probe sources إلى reference/src/coop/؛ glob الموجود src/*/*.cpp يشملها بلا تعديل CMake.
4. new wrapper sub_001C55A0 في probe.cpp يفترض generated __imp__sub_001C55A0. أكد تعريفه الحقيقي في generated code/mapping داخل CI fail-closed قبل link، وليس مجرد اسم في source. لا تبدل offsets أو تعطّل image hash check.
5. أضف workflow probe منفصل مبني على black-coop-baseline.yml: Linux host ASAN/UBSAN core tests، Python preparer tests، Android APK build/identity/package/signature/assets verification. baseline workflow لا يحتاج إعادة تشغيل لمجرد WIP غير موصول. راجع مستقل ثم Android device capture.
6. لا ترفع raw phone logs، saves، ISO/XBE، generated game source، أو signing credentials. المدخل ISO المرفوع والمقسم كاف في CI؛ راجع manifestrestore/source_audit.

## أدلة مستقلة للمواقع الجديدة
مراجعة agent read-only ثانية للـXBE ذاته:0x1C55A0 prologue/preceding padding، caller0x1C61F4 يسبقه pushEDI عند1C61F3، callee [EBP+8] عند1C55A9 ثم ret4؛ إذن entry[ESP+4] object. عند1C561E load16B vector object+A0 ثم camera distance. object+320 pointer محمل1C55AC ومفكوك1C55B9؛ +35C pointer مفكوك1C5491/1C54C0. تعريف generated ما زال يجب فحصه في CI. player ECX يؤكده1E5FAD ومصدر cheats؛ animation stack/controller+0 يؤكده1_7B130 disassembly وhooks.cpp. لا full-body model أو factory أو playable second actor مثبت. مشتقات التحليل المؤقتة حُذفت.

## مصادر reader
AOSP Android15 SYSCALLS.TXT يحتوي process_vm_readv all، app/common blocklists لا تحتويه. قرئت المصادر الأساسية https://android.googlesource.com/platform/bionic/+/refs/tags/android-platform-15.0.0_r9/libc/SYSCALLS.TXT وSECCOMP_BLOCKLIST_APP.TXT وSECCOMP_BLOCKLIST_COMMON.TXT. ليس هذا إثبات runtime على هاتف المالك أو كل OEM؛ reader يجب أن يفشل مغلقاً إذا رجع EPERM/ENOSYS. لا root أو تجاوز صلاحيات أو معالجة SIGSEGV للحيلولة دون حماية الذاكرة.

## جلسة09 HOOK001 active risks
- Fresh Windows probe1 build pending run 44852419929; confirm generated __imp__sub_001C55A0 and JNI symbol; fail closed rather than bypass. No device capture yet.
- Bounded read-only observational IDs are not actor lifetimes, no proven full-body factory. process_vm_readv on Android must be device tested later, no root/bypass. Java one-shot optional prompt.
- Uploaded ZIP parts present only in this session; repeated ISO content already exists in BO manifest; see docs/OWNER_ISO_UPLOAD_AR.md. No raw asset upload to GitHub.

## Windows Probe APK — first result PASSED (not phone acceptance)
- [44852419929](https://github.com/alwansan/BO/actions/runs/44852419929) completed SUCCESS, job136686638837: real Windows/NDK Android ARM64 build with newly injected read-only observer hooks, generated LOD symbol preflight, JNI export, source change allowlist. Debug APK package `org.bocoop.black.probe1` with separate storage and label BLACK Probe 1 - READ ONLY, no internet/network/broad storage permission, no game ISO/XBE in artifact. Built compile image c5bfed25449c8212d7a976f15246e87ead57713f matches owner ISO. APK 21,738,227 B, SHA256 `531f6cf29fdcc29db1cb5ab39971fab2670672a389e33b0b2e50597ecfaae0fb`; artifact14686665295, metadata14685973542. Exact CI evidence `memory/evidence/HOOK001-probe-candidate-44852419929.json`.
- This first build belongs to source `f911ef9e5b6b83ff19aac39406af8c70d3814f74`. A small kernel namespace convention patch was committed after that run; newer [44852913117](https://github.com/alwansan/BO/actions/runs/44852913117) still pending/in-progress at writing. Do not claim updated head APK verified until its result. **No probe phone test or gameplay/co-op acceptance.**

## HOOK001 session10 risk & correction
- Local player could starve under 64 observations. Reserved first slot; source and host tests pass 44854667613, Android new rebuild not yet confirmed. ID1 may be reused across local respawn in same world; identifiers remain correlations only, not actors.
- Actual phone support for process_vm_readv/getpid under OEM Android and LOD/vtable filtering, log directory/access and clock limits is **not demonstrated by CI**. Do not infer from build alone.

## تحديث جلسة10 — تحليل الخصوصية والنتائج الحية
- [44854855170](https://github.com/alwansan/BO/actions/runs/44854855170) completed SUCCESS for privacy-safe probe log analyzer tests (synthetic only), on code bcbf1964b7fb378ad17fed2724aaeefbab91de46. Added bounded diagnostic reason-code flags: capture_end_not_seen, no_samples, no_player_samples, no_lod_samples, no_animation_samples, reader_unavailable, local_output_error. No raw pointer, position, names, thread/epoch IDs or raw logs in summaries. Tests include empty/permission and truncated capture. These flags DO NOT prove model absence or factory success.
- [44854855083](https://github.com/alwansan/BO/actions/runs/44854855083) Python UDP regression SUCCESS. Android candidate run44854667587 was in progress (Gradle native compile) at this documentation update. Do not infer Android runtime from host CI.

## Session11 remaining risks
- Old reserved-slot registry fails to promote previously NPC-classified local object; fixed with slot serial in f7abf7de, host tests pass44855984868. Android current run44855984997 in_progress; runtime unproven until phone.
- Serial prevents stale per-slot cooldown but **does not establish actor lifetime**. Registry matches address/vtable and resets on changed world pointer; same-pointer reload reuse may go unnoticed.
- New NET-004 inbox is isolated and not connected to guest/thread hooks. No secure session identity, delivery reliability, overflow recovery, or real-world scene epoch yet. Must not be treated as production co-op.

HOOK001 starvation under LOD: reserved per-second/lifetime player opportunity without expanding global limits in e9ad056; host CI44856368591 PASS. Android run44856368592 pending. User phone runtime still unverified; sample absence may be caused by no hook, reader restriction, vtable check or timing.

## NET004-PREP composition test (2026-11-21)
- Code commit a5d164ca6f5f19bb807851429c7ba294a5d572b4 wires two previously isolated *test components* under coop/bridge/test_transport_pipeline.cpp. A deterministic host validates Join/Accept and Input token/endpoint/sequence; a transport worker stages value-only commands into SceneInbox; the owning simulated game thread drains them into a mock scene. Negative tests cover forged endpoint/token, duplicate replay, wrong drain thread, and scene reload dropping stale input. No actual network socket is opened by this composition test (separate Linux UDP socket tests exist), no BLACK hooks/actor memory access/gameplay, no authentication or phone test.
- [44856542339](https://github.com/alwansan/BO/actions/runs/44856542339) completed SUCCESS with ASAN/UBSAN and strict C++20; input path tested synthetically only. Keep wording «mock host→scene pipeline», not co-op, and do not treat a pseudorandom lab token as secure authentication.

## جلسة12 — remaining runtime blockers
- Newest Probe1 fairness APK [44856368592](https://github.com/alwansan/BO/actions/runs/44856368592) CI SUCCESS, but never exercised on phone. Android security restrictions may cause process_vm_readv to fail closed; malformed logs or missing LOD are not evidence of no actor.
- `correlation_overlap_counts` compares per-(world epoch, id) **temporary** correlations; same-address reuse and callback timing may create false overlaps/absence. Synthetic test success alone does not establish a full-body model, an allocator/factory or correct game-thread ownership.
- Production co-op still lacks actor spawning/ownership, combat & mission sync, secure multiplayer transport and two-phone runtime tests. Do not describe mock scene inbox as game integration.

## Final APK extraction verified, session12 (2026-11-21)
- Downloaded GitHub Actions **artifact14687833595** from run44856368592 through GitHub connector into this session's isolated container. ZIP extraction yielded actual `black-probe1-candidate-READ-ONLY-NO-COOP.apk` at **21,738,563 bytes** SHA256 **371de65dd7972d1505713d31d2a41080aa99877f848ac35a24fefe8b825ca2d7**, exact CI metadata match. Checked APK ZIP has16 entries and three ARM64 native libraries: libSDL3.so, libc++_shared.so, libmain.so; no files ending .iso/.xbe/.enc/.cpp. Original JNI/signature/package checks were verified in Windows job, not re-run by the container.
- Delivered temporary sandbox APK link in chat `/mnt/data/BLACK-Coop-Probe1-ReadOnly-20261121.apk` after verifying exact path; local container files **are not guaranteed durable across sessions**. Durable download remains GitHub Actions run44856368592 (retention 14 days). Do not commit APK or ISO to BO. No phone runtime report yet.

## Session13 issues
- Current CL copy of `coop_probe.jsonl` size **0**, although owner's app file exists: Shizuku shell uid2000 cannot read app-private mode0600. No full record analysis possible. A `run-as` extraction command was provided as optional attempt; success unknown.
- Android15 storage: exporting through MediaStore.Downloads by the owning app needs no all-files permission; Android OEM runtime still requires phone verification. Shared copies accessible to other apps, user opt-in required and text disclosed. If copy fails, preserve original and log minimal generic failure.
- New standalone Probe2 package avoids replacing Probe1 with a potentially different debug certificate, but requires importing ISO into Probe2's own app-specific assets via existing picker. Don't request missing XBE.
- Room menu contains read-only planned host/join status; no actual multiplayer, room creation, reliable network transport or actor cloning; prove Android build then UI runtime.

## جلسة14 — unresolved despite CI success
- Despite APK successful compile and binary presence, **MediaStore Downloads export has not been exercised on Android 15**; provider handling/permissions, file-manager discoverability and exact path may vary. Private log still mode0600 and CL copy 0 bytes. No assertion of export success or actor samples.
- CO-OP / ROOM is currently a sixth internal OPTIONS page, **not** a top-level Flash pause entry immediately below SETTINGS. Verify on owner's UI, then consider safe separate menu hook; don't misrepresent hierarchy.
- Probe2 app has separate app-private assets; import via SAF needed once on that package; baseline saves/ISO remain untouched. No secure production sessions, guest actor creation or gameplay co-op.

## جلسة15 — مخاطر بعد إثبات الرصد الحقيقي
- Raw public CL `coop_probe.txt` exposes guest pointers, positions and animation labels; **do not copy raw lines into BO**. Next user logs should be private or anonymized with `summarize_probe.py` if possible.
- LOD sample's read success for resource320/render35c doesn't prove ownership; observed render35c has only1 distinct nonzero value across13 LOD groups (possibly shared). No body rig ownership guaranteed.
- clip_hash field in source actually reads `clip+0x18`, not proven hashed clip ID; a changing value is **not** 12 animation clip transitions; safe analyzer corrects the name.
- Epoch count only1 and no end event, so object disappearance/reappearance and safe allocator/factory remain unresolved. One observed callback thread is not proof of single-threaded game.
- Current screenshots show only main menu and Probe2 opt-in, not the injected in-OPTIONS page; requested direct menu option below Settings still open.

## جلسة15 — تصحيح CI ومرشح القائمة العليا
- After privacy analyzer field renamed to explicitly `nonzero_unverified_clip_field_0x18_count`, synthetic test used its old key, causing [44863951613](https://github.com/alwansan/BO/actions/runs/44863951613) FAILURE (KeyError only). Fixed test expectation in code `77e5c031ecc1d55cbefdc73a7a1139a714150b82`; [44864020398](https://github.com/alwansan/BO/actions/runs/44864020398) privacy analyzer SUCCESS and [44864020404](https://github.com/alwansan/BO/actions/runs/44864020404) isolated UDP SUCCESS. No Android build needed (only offline analysis/test).
- Pinned v1.5.0 upstream `src/game/exit_menu.cpp` was inspected read-only: title menu already duplicates Options text clip into QuitButton below settings using Flash `CopyButton/Follow/JoinRing`. Strong candidate for independent CO-OP top-level button **between OPTIONS and QUIT**, with no claim implementation yet. Plan and hazards in `docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md`; keep current Probe2 APK intact until isolated Probe3 experiment and device UI proof.

## جلسة16 — source availability and identity caveats
- `alwansan/biso` is **source of extracted game assets**, branch `main` read-only, pinned commit `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`. It is not equivalent to generated C++ source and not proof of actor factory.
- 574 names/byte sums and 5 independent file SHA256 match owner ISO, but a full 574/574 SHA256 comparison was not yet completed. `coop/biso_manifest_audit.py --iso` requires both owner ISO and manifest locally; public CI must remain metadata-only.
- Repacked XISO from restored file tree may differ bytewise from original even when playable; don't bypass original-image identity gates or mutate default.xbe protection without explicit separate reason.
- /mnt/data parts and extracted ISO are **currently** available only; not guaranteed durable across conversations. Check GitHub BO manifest and pinned biso before asking owner to re-upload anything.

## Clarification of source identity gate
- 574 file SHA256 values from the actual ISO match pinned biso **manifest** in an order-sensitive 256-bit digest. The posted `part-0000*` data blobs themselves have not been downloaded and hashed. Source consistency is strong; do not state repository blob corruption was excluded by independent hash verification.

## جلسة17 — ACTOR-001A observation coverage + remote proxy staging (2026-11-21)
- Returned to owner priority after biso validation. Read actual source (`coop/probe/probe.cpp`, `coop/probe/probe_core.hpp`, `coop/bridge/scene_inbox.hpp`, `coop/native/net_core.hpp`), Probe2 evidence and latest plan. Current `Registry.Id` is an epoch, pointer/vtable **temporary correlation**, with serial internal to slot but not a guaranteed object generation. Round-robin non-player sampling may produce silence even if NPC remains active.
- Derived aggregate time-window analysis from owner CL Probe2 capture **without copying raw file into BO**: 13 LOD correlations, 13 animation-related; 9 first observed >=10 sec after first captured sample, 2 with >=10 sec internal observation gaps, zero with last sample >=10 sec before final observed event, 1 epoch, no end. **No proof of NPC birth/death, disappearance, reuse, or resource ownership.** Sanitized evidence `memory/evidence/ACTOR001A-coverage-owner-Probe2-20261121.json`.
- Added `coop/analysis/actor_lifecycle_triage.py` (commit `31240e47f64ca890c3844b1ad9a66aaecba3bfc8`): bounded PRIVATE JSONL reader, per-(epoch,id) gap windows, multi-epoch observations and explicitly false lifecycle/factory claims, outputs anonymous counts only. Tests for partial capture, stale samples, scene epoch split, malformed private input, privacy; privacy workflow [44867782174](https://github.com/alwansan/BO/actions/runs/44867782174) **SUCCESS**. No owner raw log in CI, no APK modifications.
- Added `coop/bridge/remote_proxy_roster.hpp` + `test_remote_proxy_roster.cpp` (commit `33f08e654ba624d4fb44cbdb562128f65fb44894`) and extended `test_transport_pipeline.cpp` (commit `15092111638cd68ba38715712ebe0b0399b8630f`). C++20 **value-only** authoritative toy host roster for player IDs2–4, owner-game-thread guard, scene epoch, monotonic tick, timeout, full-roster absence, increasing generation on return, strict bounded positions/IDs and rollback rejection. Zero guest pointers, no NPC spawn, no render, no combat and **no secure session authentication**. First isolated sanitizer bridge CI [44867858529](https://github.com/alwansan/BO/actions/runs/44867858529) **SUCCESS**; integrated toy host→proxy+inbox rerun [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) **pending at documentation time**. Do not mark final integration accepted until checked.
- Latest BO app still owner-accepted Probe2 for game import/export, top-level CO-OP/ROOM button below settings not yet implemented; current page is within OPTIONS. No Android build warranted for offline-only code. biso pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, original 574 files already matched ISO manifest; no reupload needed.

### Session17 caveats
- Sample first-appearance and gaps do not identify actor spawn/death; limited round-robin samples and one incomplete epoch are confounders.
- RemoteProxyRoster is a bounded **toy value store** (slots2-4), not a real actor handle or render proxy; existing toy transport lacks secure authentication, no host/client game integration. Only ApplyTrustedSnapshot after eventual verified host/transport validation.
- Room menu top-level placement and co-op mission synchronization remain open.

## ACTOR-001A integrated synthetic pipeline CI closure
- [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) completed SUCCESS for source `15092111638cd68ba38715712ebe0b0399b8630f`, job136735683825. Verified job log explicitly prints PASS for bounded SceneInbox, toy UDP Host→ProxyRoster+SceneInbox→mock scene, and value-only remote roster scene/owner/generation/stale tests. C++20 strict `-Wall -Wextra -Werror -pedantic`, ASAN/UBSAN on Ubuntu. **No Android build, no BLACK runtime, no real-player actor, no authenticated transport, no host+phone session**. New tests do not justify declaring ACTOR-001 complete.

## Pinned source review for ACTOR001A death/ownership hooks
- Reviewed real upstream v1.5.0 `src/game/cheats.cpp`, `src/game/hooks.cpp`, `src/game/layout.h` by pinned GitHub commit, and BO's probe observer/Registry. Candidate `sub_001E74B0` handles soldier **damage**, not necessarily final death or destructor; `kCharDying=0x384` is an observed dying flag but cannot prove deallocation. `sub_0017B130` references animation owner/clip, not actor factory. LOD observer is a render-distance hook, absent samples are not actor death. `Registry.World` only increments epoch on changed **world address**, not necessarily new mission if address reused. Precise review `docs/ACTOR001A_SOURCE_HOOK_DECISIONS_AR.md` is **design/reading only**, no changed APK or hook instrumentation.

## جلسة18 — Peterson047 BLACK-XBEMultiplayer reference review (2026-11-21)
- User-supplied repo `https://github.com/Peterson047/BLACK-XBEMultiplayer`; read README, third-party notices, bundled tool scripts `extract_mp_map.py`, `prepare_pvp_audio.py`, `prepare_pvp_weapons.py`, all Windows bat scripts and inspected full91-entry tree. Pin `0ee3ae6411c7f0989c62cbe36b224d3f55043923`. **Windows binary-only TDM alpha** for up to8 users, not Android or story coop. External claimed source `Peterson047/BLACK-Coop:feat/windows-multiplayer@095488c` returns **404** via connected GitHub; don't claim we accessed original native C++ or copy it. No top-level LICENSE; body.bca animation derived from Kevin Iglesias explicitly restricted to that mod, no re-distribution.
- Useful design: remote-body animation concept, standalone net worker, LAN discovery port1510 separate from game port1508, direct IP, host/dedicated topology, strict version rejection, owner ISO-derived isolated map and weapon packs with speech.slb tables; some audio offsets unverified for BO. Keep owner phone HOST, campaign-specific NPC/mission/quest/save separately. Detailed architecture/risks `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`; evidence `memory/evidence/REFERENCE-BLACK-XBEMultiplayer-20261121.json`.
- Cross-check **actual owner biso manifest**: all 13 required `level_00` core/stage files exist, their sizes match Peterson's published map profile, plus 16 DB/WDD weapon pairs. Independent **original** Python `coop/analysis/tdm_reference_profile.py` tests this metadata with negative synthetic tests; [44868801462](https://github.com/alwansan/BO/actions/runs/44868801462) SUCCESS, no source scripts, ISO, XBE or TDM assets copied; code `49d987bc5b8afcff0b796dfda087a80e4422d9cf`. Existing privacy CI [44868801450](https://github.com/alwansan/BO/actions/runs/44868801450) SUCCESS.
- Priority remains ACTOR-001A actual NPC/remote-avatar lifetime and safe body ownership. Next independent LAN discovery codec/rate-limit/ports on native host test, separate top-level menu and game integration later. No playable co-op claimed.

### Third-party TDM reference risks
- Peterson047/BLACK-XBEMultiplayer has **no top-level license** and binary-only native TDM; referenced full source Peterson047/BLACK-Coop unavailable 404. THIRD-PARTY-NOTICES licenses components, not blanket permission to copy mod code/art. `assets/anim/body.bca` explicitly no standalone redistribution. No direct code/binary/media copies.
- Windows TDM design for 8 players and optional dedicated server does **not** establish story co-op NPC, quests, checkpoint/save behavior or Android ABI. Do not adopt offsets or hook semantics blindly. LAN discovery has **UDP port collision** hazard (1508 game/1510 discovery). Discovery untrusted and NOT WAN authentication.

## جلسة18 — LAN discovery codec مستقل نجح في CI (2026-11-21)
- Developed original `coop/native/lan_discovery.hpp` and `test_lan_discovery.cpp`, source code `33bff07535a85f256c50a6a2842c19714148d5fe`. Inspired by Peterson047's *documented architecture only* (game and discovery ports distinct, LAN browser concept), **no source/assets copied**.
- Wire format `BCLN` version1: short query/reply with 32-bit challenge (correlation, **not auth**), game port, occupied/capacity (1..4), printable room name max32, length/byte/version/duplicate port guards and outgoing 1Hz RateGate. Only **codec/state validation**, **no UDP broadcast socket, actual browser, phone JNI integration, encrypted/authenticated session or playable game network**. Suggested game1508/discovery1510 are defaults referenced by Peterson project, configurable later. Android might require system multicast/broadcast considerations; not yet proven on phone.
- [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957) **SUCCESS BOTH JOBS**: native-linux job136739329989 printed passing strict packet parsing, port/name/rate guard,5000 deterministic malformed packets; legacy UDP loopback 2 clients passes. Android ARM64 job136739329666 cross-compiled LAN test binary and `udp_cli` with NDK28 and confirmed AArch64 ELF; **not executed on Android phone**. Existing unrelated UDP mock [44869092971](https://github.com/alwansan/BO/actions/runs/44869092971) SUCCESS.
- Remote author source `Peterson047/BLACK-Coop` remained 404; do not copy its restricted `body.bca`, binary, badges or unlicensed scripts. Actor lifetime/rig/weapon/quest sync remains primary main blocker. Safe discovery design `docs/NET_LAN_DISCOVERY_AR.md`.

### NET-LAN security status
- `BCLN-v1` challenge is correlation only, NOT authentication. No session crypto, trusted peer, WAN safety, real LAN socket or Android broadcast validation. Outgoing rate limiter is not an inbound DDoS limiter; add per-IP limit and source validation before network listener.
- Keep `host_port` distinct from `discovery_port`, test SDK Android/permissions when binding sockets on user device; don't confuse cross-compile success with execution.

## جلسة18 — POSIX UDP LAN discovery loopback فعلي (2026-11-21)
- Added original `coop/native/lan_socket.hpp` and `test_lan_socket.cpp`, source commit `d97c3ffe3bb19c2950acca7db7db3943fee16d2d`. Portable Linux/Android-NDK POSIX socket RAII, default loopback bind, no SO_REUSEADDR to prevent duplicate port consumption, bounded MSG_TRUNC recv, source IP+port checks, 16-slot inbound per-endpoint 500ms PeerRateGate/5s slot expiry, outgoing1Hz budget from BCLN-v1, strict version/nonce/room/port validation. **No broadcast, app/JNI integration, WAN auth/encryption or actual BLACK player**.
- [44869388876](https://github.com/alwansan/BO/actions/runs/44869388876) **SUCCESS** BOTH JOBS on source `d97c3ffe`: Linux job136740257311 used ASAN/UBSAN and validated **actual UDP IPv4 loopback** Query/Reply, source+challenge, duplicate binding rejected, bounded timeout, incoming/outgoing rate caps and synthetic 5000 malformed datagram decodes. Android ARM64 job136740257470 successfully NDK28 cross-compiled test and checked ELF AArch64; **not executed on actual phone**. Separate toy UDP CI [44869388854](https://github.com/alwansan/BO/actions/runs/44869388854) SUCCESS.
- Peterson047 reference remains read-only and unlicensed external binaries/media unavailable for copying. Their Windows Team Deathmatch source `Peterson047/BLACK-Coop` 404; the independent BO room discovery now has a bounded real UDP loopback transport, but it **cannot be marketed as LAN game hosting** until Android sockets interface, LAN network discovery, authenticated join and a real NPC-player render hook exist.

### LAN socket implementation boundaries
- Now real UDP **loopback-only** tests, but still no real subnet broadcast or Android runtime integration. Host NDK compile isn't device test. Fixed per-source table not spoof-resistant; challenge is not auth. Require authenticated session and visibility controls before enabling gameplay.


## ACTOR-001A — تحديث جلسة19
pool32 ثابت وserial مرشح لا يثبتان active lifetime أو remote body؛ دوال acquisition/retire/list ما زالت فرضيات. تفصيل العناوين في memory/sessions/2026-11-21_bo-coop-19-cloud-handoff.md.


### تصحيح عقود pool — جلسة19
اقرأ docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md: +4 capacity/+8 occupied؛ sparse lists32/16/8/8،actor+250 membership،acquisition1DF440→init1DA4F0. إثبات ثابت فقط؛reset/overflow/runtime/model ما زالت مفتوحة.


### متابعة19 — reset مثبت ونطاق المنتج
دالة1D5E00 على مسارAL1 تعيد32 خانة إلى79A0 وتكتبcounter8524=1؛لا هوية دائمة بالserial وحده، ولاepoch بمجرد دخولreset الذي قد يفشل. التفاصيل docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md. نص هدف المالك الكامل محفوظ docs/OWNER_PROJECT_OBJECTIVE_AR.md. تحليل ثابت فقط،لاAPK/CI جديد.


## جلسة20 — pool reader
راجع memory/sessions/2026-11-21_bo-coop-20-pool-reader.md والخطةProbe3. core مستقلbounded sparse snapshot محليPASS،review fix rawarrayreorder مطبق،CIpending؛لاgameintegration/APK بعد. أداءsyscalls وepoch/reset وpackage/export ضمنTask2.


## جلسة21 — Probe3 Android integration
راجع memory/sessions/2026-11-21_bo-coop-21-probe3-device-build.md؛coreCI44871689814SUCCESS،دمجProbe3واختباراتمحليةومراجعةPASS،APKworkflowجديدقيدالإطلاق. لاdeviceproofولاcoop. تابعبناءProbe3حتىالنهايةثمطلباختبارNPCdeath/reload/logمحدد.


### Probe3 core CI مغلق
44872596855 SUCCESS لكلاjob،logs مقروءة؛الدليل memory/evidence/PROBE3-core-44872596855.json. APK44872596889 لايزالجارياً؛تابعهقبلأيطلبهاتف،ثم docs/PROBE3_DEVICE_TEST_AR.md.


## جلسة22 — generatedguardfalsepositive
APK44872596889FAILقبلGradle؛توقعاتصيغةea/castsفيالفاحصصححتوفقSDK،Python12/preparers6PASSومراجعةPASS. اقرأ memory/sessions/2026-11-21_bo-coop-22-reset-guard-format.md وتابعالبناءالجديد. لاAPKجديدةجاهزةبعد.


## جلسة23 — Probe3 جاهز للهاتف
APK44872995034SUCCESS/core44872995111SUCCESS،artifact14695275729 نُزلوفُحصsha256352d78ed1811b918b91f344e0ba613eb7a0a9f1cc43ba93ae952fccb83ffec44. اقرأ memory/sessions/2026-11-21_bo-coop-23-probe3-apk-ready.md؛التالياختبارمالكوفقdocs/PROBE3_DEVICE_TEST_AR.md ثمlifetime/modelanalysis. لاco-op/ISOجديدبعد.


### بوابة الجهاز الحالية
Probe3جاهز؛بعد3goalturnsلايزالcoop_probe.txtالجديدغيرمتاحفيCL/attachments. التنفيذblockedانتظاراختبارمالكNPCdeath/reloadوفقdocs/PROBE3_DEVICE_TEST_AR.md،وليسبناءنسخةمطابقةأوتوسيعmocknet. الهدفالكاملغيرمكتمل.


## جلسة24 — حل انتظار Probe3
وصلcapture090753،56/56poolvalid و2serialreplacements؛الملخصالآمنوالخطةفيmemory/sessions/2026-11-21_bo-coop-24-probe3-owner-analysis.md. actor328controllerbacklinkودخولهdrawمثبتانثابتاً؛التاليinstancegraph. لاتكررطلبProbe3أوbaseline؛لاspawn/render/coopمقبولبعد.


## جلسة25 — رصد instance graph
محلل Probe3 CI44875336266SUCCESS. إضافة Probe4 منفصلة لروابط controller328/owner/runtime/scratch؛محلياً native/JSON/preparer9tests ومراجعة PASS،CI/APK pending. راجع memory/sessions/2026-11-21_bo-coop-25-instance-graph-build.md؛لا duplicateProbe3 ولا co-op claim.


## جلسة26 — Probe4 ready
APK44876183662SUCCESS؛artifact14696342069 نُزلوفُحصhash. راجع memory/sessions/2026-11-21_bo-coop-26-probe4-ready.md؛التالياختبارownerinstancegraph وفقdocs/PROBE4_DEVICE_TEST_AR.md،بلاrepeatProbe3/baseline. analyzergraphextensionsمحلياً19+11testsPASSومراجعةPASS؛CIتحليلهذهالدفعةقيدالإطلاق. لاco-opجاهز.


### محلل instance graph — CI مثبت
44908343745SUCCESS على399cf730؛لاrunجارٍلهذهالدفعة. Probe4APK44876183662/artifact14696342069جاهز،التاليphonegraphcaptureفقطقبلbody/proxyتجريبيحقيقي.


## تحديث جلسة27 — Probe4
قُبل سجل المالك الجديد ضمن الرصد:111/111 pool و548/548 links صالحة،لا مشاكل حسب المالك،لا reload proof أوco-op. لا إعادة Probe4. الخطوة الحالية تدقيق تسجيل/إزالة مجسم إضافي ومسارات إصابته؛full factory مع null controller غير آمن. التفاصيل في `memory/sessions/2026-11-21_bo-coop-27-probe4-owner-graph.md`.

قرار جلسة27 بعد المراجعة: رُفض مجسم null-brain المسجل؛1E7753يقرأ324+90دونguard. التاليacquisitionأصليمن1AF880مع324مستقلوفحصdescriptor/questIDsوالسعات. لاAPKجديد.


## جلسة28 — طلب المالك وتنفيذ title UI
اختبارات الهاتف السابقة فردية فقط؛الآخرونNPC. يريد المالك أجسام بشر مستقلين وانتقالهم مع المضيف بينالمراحل. إذن جديد بفرع تجريبيbiso وتعديلISOعندالحاجة،دونmain. CO-OP تحتOptionsنُفذفيprepare_room_ui.py،مباشرةإلىالغرفةمعBackللـtitle؛12preparer+12rootPython+native routePASSومراجعةمستقلةواضحة،NDKوالهاتفلميثبتابعد. HOST/JOINغيرمتاحينولاجسملاعبثانبعد. راجع`memory/sessions/2026-11-21_bo-coop-28-title-room-ui.md`.

جلسة28/إصلاحCI:run44913282838فشلقبلcompileبسببCRLFفيexit_menu.cpp. اختبارreproducerREDثمLFnormalizationGREEN،13/13preparerPASS؛إعادةCIمطلوبة. المصدرالأولbf9a5c8ليسAPKجاهزاً.

جلسة28/النتيجة: UI APK جاهز،run44913654405 SUCCESS على96f69b6،artifact14710883570،APK SHA256 af133c9b2a74a9714d62ca943bdd209ffc552267a053951085980ad7addfebfe وحجمه21753039B؛نُزل وفُحص مستقلاً. المطلوب أول فحص لموضعCO-OP والدخول والعودة على الهاتف حسبdocs/ROOM_UI_DEVICE_TEST_AR.md. لا Host/Join فعليين أو أجسام بشر متصلين،ولا ISO جديدة. خريطةالربطdocs/COOP_INTEGRATION_ROADMAP_AR.md.


## جلسة29 — قبول UI وTLS فعلي
المالك قبل القوائم وعمل اللعبة؛قُرئت5صوروجميع140pool/692linksمنسجل173522-325صالحة. لا إعادةUI/Probe4. خطأFEAudioواحدبلاcrashمبلغ،السببغيرمثبت. RoomWire/RoomTlsالجديدانينفذانغرفةTLSفعليةعلىJVMمضيف+3عملاءمعpin/PINوheartbeatورفضنسخة/امتلاءوإغلاق. مراجعةمستقلةوجدتpublication-after-closeوmissingPONG؛reproducersREDثمfixesGREENوsuitePASS. Android/discovery/JNIوالأجسام/القصةغيرمكتملة. التفاصيل`memory/sessions/2026-11-21_bo-coop-29-real-tls-core.md`؛الخطةreal-lobby.md.

جلسة29/CI:run44917272041/job136885459965 SUCCESS على591372ba؛logsreadوالـTLSsockettestsPASS. التاليRoomIdentityAndroidKeyStoreواختبارAndroid29emulatorبـAPKsyntheticبلاgameassets؛لميثبتAndroidبعد. لاheavyphonetestقبلحزمةاتصالغرفةفعلية.

## جلسة30 — استعادة تقدم الجلسة الأخرى وDirect TLS Android (2026-11-21)
- Owner supplied shared-chat URL `https://chatgpt.com/s/cx_6b01ba1d7a048191b3880e16288211c7`, but public URL fetch was disabled. **Do not claim transcript was read.** Instead read newest live `alwansan/BO:experiment/black-coop` HEAD starting `c9e4db52ac802371dc6e8ac4fd6b97343f797c4d`, latest `memory/sessions/2026-11-21_bo-coop-29-real-tls-core.md`, STATE, RESTORE, verified CI and docs. State29 supersedes previous session18: owner UI+Probe4 device accepted, code now real JVM TLS room/3 peers+UDP discovery, native body/quest/combat not yet.
- Implemented `coop/lobby/java/com/racas/black/coop/RoomDirect.java` and `coop/lobby/test/DirectTest.java`: strict DNS-free IPv4:port numeric parser, rejects ambiguous zeros/broadcast/multicast/out-of-range/hostname/Unicode control; full 32-byte SHA256 fingerprint in canonical hex or colon notation, then passes into existing `RoomTls.pinnedContext`/real join. Wrong PIN/full pin tested against **real TLS socket**, no game assets/JNI/Flash or gameplay actor. Initial [44920652328](https://github.com/alwansan/BO/actions/runs/44920652328) failed strict `javac -Werror` test-only unused resource warning; corrected on `8df7a68346aed269bbd762449db7b7af5af6ea3f`. [44920763536](https://github.com/alwansan/BO/actions/runs/44920763536) **SUCCESS**, [44921276865](https://github.com/alwansan/BO/actions/runs/44921276865) and [44921892219](https://github.com/alwansan/BO/actions/runs/44921892219) subsequent core regressions SUCCESS.
- Identified Android emulator platform sequence from real logs: 44919439734 cancelled after adb wait hung 18min; bounded every ADB get-state/boot/installer/instrumentation call with `timeout -k` and diagnostic tails in `coop/lobby/run_android_platform_test.sh` (source `a954306c2b4afdde2b3cd5f7ffc85c9401f07226`). Android29 emulator then **BOOTED and installed test APK successfully**, but real Conscrypt TLS handshake failed `SSLHandshakeException: Connection closed by peer` [44920763565](https://github.com/alwansan/BO/actions/runs/44920763565) and [44921276909](https://github.com/alwansan/BO/actions/runs/44921276909). This is **not** a simulated Android pass.
- Added AndroidKeyStore DIGEST_NONE for prehashed TLS signatures per Android KeyGenParameterSpec guidance (source `650d50eb1eae630ea879fa0bb28747601e74630b`), but Android platform [44921495502](https://github.com/alwansan/BO/actions/runs/44921495502) still failed. Added sanitized pre-JOIN server error/key-type diagnostics `903bb225ab1a923b30deffb9be0b0a91c512591b`: [44921892101](https://github.com/alwansan/BO/actions/runs/44921892101) failure revealed `SSLHandshakeException/SSLProtocolException Handshake failed` and **Android Conscrypt requested key type `EC`**, while server identity/keymanager was RSA-only. No secret/PIN/private key logged.
- Implemented separate opaque `AndroidKeyStore` EC P-256 identity alias `BLACK_COOP_TLS_EC_V1` and EC-only X509KeyManager (source `af67599d8cbe7db7f99bf36f1ebc139f0ec4e0c0`) instead of falsely returning RSA key for EC. New platform run [44922289967](https://github.com/alwansan/BO/actions/runs/44922289967) **PENDING at handoff**. Do not mark Android TLS success until CI instrumentation emits `TLS_PLATFORM_PASS`. A new alias avoids silent reuse of old certificate pin; avoid silent identity rotations in any production-installed package.
- Existing owner acceptance remains: Room UI APK package `org.bocoop.black.roomui`, owner photographs show CO-OP below Options in title/pause menu and room page; Probe4 reports 140 pool/692 links on latest owner capture, NO player2/actor factory. No baseline/Probe4/UI retest requested. ISO in BO and `biso` already verified; no reupload needed.

## Session30 Android KeyStore platform blocker
- Android29 emulator boots and installs synthetic APK, but RSA-only KeyManager handshake fails even with DIGEST_NONE. Observed server SSLHandshakeException/SSLProtocolException and requested EC keyType in 44921892101; not a certificate pin bypass issue. EC P-256 identity is being tested, no PASS yet.
- Avoid treating test-only self-signed certificate and full SHA256 pin as production PKI; user must verify pin out-of-band. Do not silently replace already-pinned identities on devices.
- Shared-chat URL inaccessible directly; GitHub session29 live source used. No claim of exact transcription from link.

## Session31 remaining issues
- Android TLS core passed emulator 44922935535. Separate run44923264409 failed at `adb shell pm install` (timeout 124 after staged APK push); runner/emulator install flake, not proven TLS defect. New UI/platform test run44923426931 pending.
- Android Host/Join/Leave forms added as isolated synthetic Activity. No integration with BlackActivity/Flash menu yet; no two-phone LAN test; do not release this as co-op gameplay.
- Real actor creation still blocked by independent +324 ownership, 16-element pool, unguarded damage access and full-body/animation resource verification. Preserve story saves, NPCs, ISO, and main.
## Session32 unresolved gates
- The game Flash/Android bridge is a **source-verified candidate**, [44924220185](https://github.com/alwansan/BO/actions/runs/44924220185) success; Windows/NDK APK [44924346601](https://github.com/alwansan/BO/actions/runs/44924346601) running. No claim that in-game HOST/JOIN has booted on device.
- Network room remains session-only (real authenticated humans, not playable gameplay actors), with physical LAN/firewall and Android lifecycle yet to verify. Flash room UI text is read-only roster, not an actor roster. PIN transmitted inside TLS after full fingerprint verification; it is not public WAN-ready hosting.
- The process-owned `RoomAppRooms` persists when form closes and shuts down with game Activity destroy; lifecycle, pause/resume and player disconnect need device testing. Original `RoomLobbyActivity` test-only stops onStop; the game variant intentionally changes this in staged source.
- Original `roomui` remains signed separately with no network permissions; game branch isolated as `roomnet`. Do not overwrite game saves/ISO.

APK44942174355فشلفيWindowsقبلالبناء: اختبارroom_network_prepare_checksقرأRoomOverlayالعربيباستخدامcp1252الافتراضي. أُصلحread_textبـUTF-8صريح؛ليسفشلاًفيAndroidأوالشبكة. مراجعةالإلغاءالمستقلةأُعيدتولمتجدمانعاًمتبقياً.

## مرجع وتقدم أصول جديد بعد رفع جلسة35
اكتشفنا portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628،LICENSE MIT قرئ. راجع docs/REFERENCE_BLACK_PC_COOP_AR.md؛PC split-screen لديه player-context وpuppets لكنAIلايستهدفP2 بعد،وليسAndroid/network coop. لا نقل أعمىلـslotcopy؛+250عضويةالمجموعةيحتاجحماية. أداةBOالأصليةmodel_inventory.pyقرأَت271ملفاًمحلياًو4286MODEL،76مرشحbodyبـ10keysفريدة؛protagonistغيرمثبت.3اختباراتحدودوتصنيفPASS،لاgeometry/أصولمصدرة. نتائجآمنةفيOWNER-MODEL-CANDIDATES-20261121.json. تبقىخطةactualactor/control/AI/story،ولاStartوهمي.

FactoryAPK run44944691802 على96413485bdde48981c7000ff9f9ff6019740fa7b:coreنجحوالـgeneratedABIguardنجحوبناءARM64قيدالتجميعوقتكتابةهذاالتحديث. لا تطلبهاتفاًحتىنجاحartifactوفحصه.
## Work50 — تصويب TLS غير مفعّل بعد على ممثل المضيف

- ثبت النقل لمحوري التصويب، وتغيّر الاتجاه أفقيًا في مختبر المحرك بشكل مستقل. لم تُوصل قناة TLS بالممثل الأصلي في تطبيق Android بعد؛ التصويب الرأسي والعرض والعالم المشترك وAPK القبول مفتوحة.
