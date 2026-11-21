# مرشحات فحص الشخصية — معلومات ساكنة، ليست spawn منفذة

المرجع Android v1.5.0 f3754d6985a8c35204e7df071546e586b10f1806 وXBE المالك c5bfed25449c8212d7a976f15246e87ead57713f. تحقيق read-only مستقل أثناء بناء baseline؛ لم يعدل المستودع أو الأصول، واستخرج مشتقات مؤقتة صغيرة ثم حذفها. ثُبتت هنا مراجع المصدر؛ بقية دلالات XBE مرشحات تتطلب تحققاً مستقلاً قبل الاستخدام.

| الموقع | الملاحظة والدلالة المحدودة |
|---|---|
| config/game.toml hook0x1C5655، CharacterDetail في hooks.cpp | Hook قائم لتعديل مسافة التفاصيل، وليس observer خالصاً: يعدل xmm1 بحسب draw_distance. دالة XBE المرشحة0x1C55A0 تقرأ object+0xA0 وتحسب مسافة وتكتب+0xC0/+0xC8؛ مرشح ارتباط render/LOD، لا إثبات نوع CPlayer أو مجسم كامل. |
| object+0x320 ثم descriptor | تحليل XBE يرجح مورد LOD/model: descriptor byte+0x6B وحدود مسافة من+0x24. لم يثبت skeleton أو allocator/factory أو ownership/lifetime. |
| object+0x35C ودالة0x1C5490 | مرور على count+0x24 وجداول+0x1C/+0x20/+0x38/+0x40 وإرسال عبر0x15A820 إلى draw lists؛ دلالة render مرجحة فقط. |
| world+0x4960، callback0x1C48D0 | تحديث اللاعب0x1E6076–0x1E6089 يمرر البنية/callback/player إلى0xCC1F0. callback يقرأ node+0x34 ثم object ويقارن مواضع ويخزن مرشحاً في0x2F3BDC. هذا spatial query مفلتر مرجح، لا actor list مثبتة؛ ممنوع traversal/نسخها كأنها قائمة عامة. |
| sub_0017B130 في hooks.cpp | animation controller: owner+0، clip+0x54، name+0x5C، from+0x90. المصدر يفرق بين owner واللاعب المحلي؛ مرشح لاكتشاف مالكي animation غير اللاعب. |
| sub_001EEFD0 في hooks.cpp | weapon update، owner+0xF0 وplayer weapon+0x29C في المصدر؛ يدعم correlation ولا يثبت playable actor إضافياً. |

## ما لم يثبت
لا full-body model للاعب الأول، ولا actor factory أو clone آمن، ولا كاميرا/إدخال/سلاح مستقلان لشخصية ثانية. inventory ISO574 ملفاً، وأسماء ملفات chars لا تثبت وحدها وجود جسم لاعب. نجاح baseline compile أو عناوين source المتطابقة لا يغلق ACTOR-001.

## الفحص التالي بعد قبول baseline على الهاتف
صمم probe منفصلاً opt-in دون كتابة guest memory أو استدعاء guest functions غير مثبتة. تحقق من bounds والعمر وfinite vectors قبل القراءة؛ عيّن هوية محلية مؤقتة للأجسام مع reset عند reload. اجمع عدداً محدوداً من العينات في buffer محلي، وسجل thread IDs أولاً؛ لا تفترض أن CharacterDetail/animation hooks تعمل جميعها في game thread ذاته.

قارن الثبات/المشي/الدوران/الرمي، حركة وموت NPC قريب، checkpoint reload والخروج من المهمة. اربط animation owners وLOD/resource changes دون تصدير pointers إلى شبكة أو افتراض أن object يماثل اللاعب. يلزم إثبات الاختفاء/إعادة الإنشاء قبل البحث عن lifecycle/factory. لا شبكة أو spawn أو تعطيل original hooks من هذا التحقيق.

## جلسة16 — موارد المالك المفككة
`alwansan/biso@9ceb6a65fb5a4414c106ca3961295a1f8f7e558b` يتيح دراسة الملفات الأصلية دون استخراج ISO كل مرة: `chars/trans_ch.bin`, `chars/trans_fp.bin`, `levels/level_XX/Unit_01.bin`، `LevelDat.bin` و`export/frontend/FEMain.bin`. فحص الأسماء والأحجام ثابت، وليس إثبات معنى تنسيق البيانات أو ارتباطها بهويات runtime التي ظهرت في Probe2. خريطة مستقلة docs/ACTOR_BISO_ASSET_INDEX_AR.md، وبوابة مراجعة lifecycle قبل أي spawn باقية.

## جلسة17 — ACTOR-001A observation coverage + remote proxy staging (2026-11-21)
- Returned to owner priority after biso validation. Read actual source (`coop/probe/probe.cpp`, `coop/probe/probe_core.hpp`, `coop/bridge/scene_inbox.hpp`, `coop/native/net_core.hpp`), Probe2 evidence and latest plan. Current `Registry.Id` is an epoch, pointer/vtable **temporary correlation**, with serial internal to slot but not a guaranteed object generation. Round-robin non-player sampling may produce silence even if NPC remains active.
- Derived aggregate time-window analysis from owner CL Probe2 capture **without copying raw file into BO**: 13 LOD correlations, 13 animation-related; 9 first observed >=10 sec after first captured sample, 2 with >=10 sec internal observation gaps, zero with last sample >=10 sec before final observed event, 1 epoch, no end. **No proof of NPC birth/death, disappearance, reuse, or resource ownership.** Sanitized evidence `memory/evidence/ACTOR001A-coverage-owner-Probe2-20261121.json`.
- Added `coop/analysis/actor_lifecycle_triage.py` (commit `31240e47f64ca890c3844b1ad9a66aaecba3bfc8`): bounded PRIVATE JSONL reader, per-(epoch,id) gap windows, multi-epoch observations and explicitly false lifecycle/factory claims, outputs anonymous counts only. Tests for partial capture, stale samples, scene epoch split, malformed private input, privacy; privacy workflow [44867782174](https://github.com/alwansan/BO/actions/runs/44867782174) **SUCCESS**. No owner raw log in CI, no APK modifications.
- Added `coop/bridge/remote_proxy_roster.hpp` + `test_remote_proxy_roster.cpp` (commit `33f08e654ba624d4fb44cbdb562128f65fb44894`) and extended `test_transport_pipeline.cpp` (commit `15092111638cd68ba38715712ebe0b0399b8630f`). C++20 **value-only** authoritative toy host roster for player IDs2–4, owner-game-thread guard, scene epoch, monotonic tick, timeout, full-roster absence, increasing generation on return, strict bounded positions/IDs and rollback rejection. Zero guest pointers, no NPC spawn, no render, no combat and **no secure session authentication**. First isolated sanitizer bridge CI [44867858529](https://github.com/alwansan/BO/actions/runs/44867858529) **SUCCESS**; integrated toy host→proxy+inbox rerun [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) **pending at documentation time**. Do not mark final integration accepted until checked.
- Latest BO app still owner-accepted Probe2 for game import/export, top-level CO-OP/ROOM button below settings not yet implemented; current page is within OPTIONS. No Android build warranted for offline-only code. biso pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, original 574 files already matched ISO manifest; no reupload needed.
