# BLACK Co-op — تحليل Probe2 الحقيقي للمجسمات والأنيميشن (2026-11-21)

**المدخل الحقيقي:** <https://github.com/alwansan/CL/tree/main/black%20log/logs/BLACK-Probe-20261121-064812-388> من تجربة المالك على `org.bocoop.black.probe2`. جرى الاطلاع على الملفين الفعليين `coop_probe.txt` و`black_log.txt` في GitHub، وليس الاكتفاء بلقطات الشاشة. **لا ننسخ المحتوى الخام إلى BO**؛ الأرقام الآتية مشتقة للتحليل فقط، والدليل الآلي `memory/evidence/DEVICE-probe2-owner-20261121-064812-safe-summary.json`.

## قبول محدد على جهاز Android 15
- مسار تصدير MediaStore إلى Downloads **نجح فعلياً**: الملف النصي الأول 130,243 بايت، والثاني 2,253 بايت. هذا يغلق مشكلة `Permission denied` التي منعت قراءة ملف Probe1 الخاص، بالنسبة إلى *نسخ* السجلات المصدّرة الجديدة فقط.
- `black_log.txt` يؤكد تحميل ISO الصحيحة SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f` وبدء HOOK001. رسالة عدم وجود game files ظهرت قبل اكتمال الاستيراد وليس دليلاً على فشل هذه التجربة.
- الصور الحالية تظهر القائمة الرئيسية ومربع حوار `BLACK Probe 2 - READ ONLY`. **لا تثبت بعد فتح صفحة CO-OP / ROOM داخل SETTINGS**. الطلب الأصلي كان زرّاً تحت «الإعدادات» في الواجهة الأساسية؛ النسخة الحالية تضع صفحة سادسة *داخل* OPTIONS، لذلك موضع الزر المطلوب ما زال بند عمل.
- السجل الحالي لا يحتوي `end`، فهذه لقطة تصدير جزئية وليست شهادة انتهاء capture؛ لا يوجد `Exit(0)` موثّق هنا كما كان في تجربة Probe1 السابقة.

## نتائج الرصد الحقيقي المجهولة الهوية
| المقياس | النتيجة |
|---|---:|
| أسطر JSONL | 401 (بدون أسطر معطوبة) |
| `start` | 1 |
| `player` | 131 (كلها `local=true`) |
| `lod` | 172 |
| `animation` | 97 (منها 13 `local=true`) |
| `end` | 0 |
| مجموع عينات player/lod/animation | **400** |
| مشاهد/epochs مرصودة | 1 |
| خيوط تنفيذ مرصودة في callbacks | 1 |
| مجموع هويات correlation المؤقتة في epoch | 14 |
| نطاق وقت العيّنات | قرابة 139 ثانية |

### علاقة المكوّنات (نفس epoch+ID المؤقت، بلا نشر IDs)
- **13** مجموعة لديها `lod` و`animation`، بلا `player`.
- **مجموعة واحدة** لديها `player` و`animation`، بلا `lod`.
- لا توجد مجموعة مشتركة ثلاثية ولا تطابق مباشر بين `player` و`lod` في السجل الحالي.
- القراءات صالحة لحقول موضع اللاعب في 131/131 وحقول LOD في 172/172؛ نجحت أيضاً قراءة `object+0x320` و`object+0x35C` في 172/172 عينة LOD. **قراءة المؤشر ليست إثبات أنه يملك نموذج شخصية منفصلاً**.
- تغير حقل الموضع بأكثر من 0.1 وحدة مصدرية لدى 13/13 مجموعة LOD؛ ظهرت أكثر من تسمية أنيميشن مرصودة لدى 11/13 منها. تغير الحقل المقروء من `clip+0x18` لدى 12/13، ولكن معنى هذا الحقل **غير مثبت** وقد يتغير داخل المقطع نفسه؛ **لا تفسره على أنه 12 انتقال أنيميشن أو 12 animation clips**.
- ظهر 4 قيم غير صفرية مختلفة لمؤشر المورد المرصود `+0x320`، وقيمة واحدة غير صفرية لمؤشر `+0x35C` ضمن عينات LOD. قد تكون موارد مشتركة؛ لا تفسر قيمة `+0x35C` على أنها instance render object منفصل لكل شخصية.

## التفسير والخطوة الهندسية المقبلة
1. لدينا دليل فعلي على أن **هوية اللاعب المحلي المرتبطة بأنيميشن تختلف عن مجموعات LOD المرتبطة بأنيميشن**. لا يجوز افتراض أن شخصية اللاعب ذات منظور أول تملك body render قابل للنسخ عبر نفس المؤشر.
2. افحص فرضية استعمال نموذج NPC/LOD مرئي كبداية لـ **remote-avatar proxy**، لكن عبر قراءات فقط أولاً: تحديد ownership والعمر (ظهور/اختفاء/إعادة ظهور) ثم الكاميرا والسلاح والتحكم، ولا تكتب guest memory ولا تستدعِ factories مجهولة.
3. يلزم دليل read-only بعد تبديل scene أو NPC death/reload لتثبيت lifetime وgeneration. هويات `(epoch,id)` في هذا السجل مجرد correlations، وقد يعاد استخدامها.
4. ظهور callback thread واحد في 400 عينة لا يعني أن كل منطق اللعبة على خيط واحد؛ يجب التحقق من سياسات hook/game-thread قبل أي دمج شبكي.
5. **الحذر من الخصوصية:** ملف `coop_probe.txt` الخام المنشور في CL يحتوي حقولاً تقنية (مواضع/مؤشرات وأسماء أنيميشن). شارك التجارب التالية خاصاً قدر الإمكان، أو شارك فقط مخرجات `coop/analysis/summarize_probe.py` المنقّاة.

## ما حدث في البرمجة
- أُضيفت إحصاءات cohort منفصلة لا تعتمد على قائمة هويات صريحة، و`capture_is_partial_snapshot` وقياسات صلاحية المورد والرندر وتغير الموضع والقيم المرتبطة بالأنيميشن. النتيجة لا تُصدر مواقع أو مؤشرات أو أسماء حركات أو هويات.
- `clip_hash` في السجل مصدره قراءة `clip+0x18` عبر `coop/probe/probe.cpp`، وليس Hash موثّق لهوية clip؛ لذلك سُمّي في التقرير الآمن `lod_animation_clip_field_0x18_changed`. لا يثبت دوام هوية actor أو إمكان إنشاء player2.
- اختبار الخصوصية [44863742397](https://github.com/alwansan/BO/actions/runs/44863742397) PASS على كود `063ce80d...`، واختبارات UDP غير المتأثرة [44863742412](https://github.com/alwansan/BO/actions/runs/44863742412) PASS.
- **لا حاجة لإعادة بناء APK لهذه التعديلات**: جميع التغييرات في أداة تحليل سجلات مستقلة خارج APK المصدّق سابقاً [44860874976](https://github.com/alwansan/BO/actions/runs/44860874976).

## جلسة15 — تصحيح CI ومرشح القائمة العليا
- After privacy analyzer field renamed to explicitly `nonzero_unverified_clip_field_0x18_count`, synthetic test used its old key, causing [44863951613](https://github.com/alwansan/BO/actions/runs/44863951613) FAILURE (KeyError only). Fixed test expectation in code `77e5c031ecc1d55cbefdc73a7a1139a714150b82`; [44864020398](https://github.com/alwansan/BO/actions/runs/44864020398) privacy analyzer SUCCESS and [44864020404](https://github.com/alwansan/BO/actions/runs/44864020404) isolated UDP SUCCESS. No Android build needed (only offline analysis/test).
- Pinned v1.5.0 upstream `src/game/exit_menu.cpp` was inspected read-only: title menu already duplicates Options text clip into QuitButton below settings using Flash `CopyButton/Follow/JoinRing`. Strong candidate for independent CO-OP top-level button **between OPTIONS and QUIT**, with no claim implementation yet. Plan and hazards in `docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md`; keep current Probe2 APK intact until isolated Probe3 experiment and device UI proof.

## جلسة17 — ACTOR-001A observation coverage + remote proxy staging (2026-11-21)
- Returned to owner priority after biso validation. Read actual source (`coop/probe/probe.cpp`, `coop/probe/probe_core.hpp`, `coop/bridge/scene_inbox.hpp`, `coop/native/net_core.hpp`), Probe2 evidence and latest plan. Current `Registry.Id` is an epoch, pointer/vtable **temporary correlation**, with serial internal to slot but not a guaranteed object generation. Round-robin non-player sampling may produce silence even if NPC remains active.
- Derived aggregate time-window analysis from owner CL Probe2 capture **without copying raw file into BO**: 13 LOD correlations, 13 animation-related; 9 first observed >=10 sec after first captured sample, 2 with >=10 sec internal observation gaps, zero with last sample >=10 sec before final observed event, 1 epoch, no end. **No proof of NPC birth/death, disappearance, reuse, or resource ownership.** Sanitized evidence `memory/evidence/ACTOR001A-coverage-owner-Probe2-20261121.json`.
- Added `coop/analysis/actor_lifecycle_triage.py` (commit `31240e47f64ca890c3844b1ad9a66aaecba3bfc8`): bounded PRIVATE JSONL reader, per-(epoch,id) gap windows, multi-epoch observations and explicitly false lifecycle/factory claims, outputs anonymous counts only. Tests for partial capture, stale samples, scene epoch split, malformed private input, privacy; privacy workflow [44867782174](https://github.com/alwansan/BO/actions/runs/44867782174) **SUCCESS**. No owner raw log in CI, no APK modifications.
- Added `coop/bridge/remote_proxy_roster.hpp` + `test_remote_proxy_roster.cpp` (commit `33f08e654ba624d4fb44cbdb562128f65fb44894`) and extended `test_transport_pipeline.cpp` (commit `15092111638cd68ba38715712ebe0b0399b8630f`). C++20 **value-only** authoritative toy host roster for player IDs2–4, owner-game-thread guard, scene epoch, monotonic tick, timeout, full-roster absence, increasing generation on return, strict bounded positions/IDs and rollback rejection. Zero guest pointers, no NPC spawn, no render, no combat and **no secure session authentication**. First isolated sanitizer bridge CI [44867858529](https://github.com/alwansan/BO/actions/runs/44867858529) **SUCCESS**; integrated toy host→proxy+inbox rerun [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228) **pending at documentation time**. Do not mark final integration accepted until checked.
- Latest BO app still owner-accepted Probe2 for game import/export, top-level CO-OP/ROOM button below settings not yet implemented; current page is within OPTIONS. No Android build warranted for offline-only code. biso pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`, original 574 files already matched ISO manifest; no reupload needed.
