### Session84 verification/packaging amendment (2026-11-23)
- Original v9 host-only ordered object-event TLS implementation `101b3ec9`: JVM 4-peer real TLS **SUCCESS** run [45093109148](https://github.com/alwansan/BO/actions/runs/45093109148); Android29 platform **SUCCESS** run [45093109200](https://github.com/alwansan/BO/actions/runs/45093109200); source preflight **SUCCESS** run [45093109118](https://github.com/alwansan/BO/actions/runs/45093109118); UDP **SUCCESS** run 45093109084. These are transport/preflight results, NOT original BLACK shared-world acceptance.
- Android artifact metadata initially remained hard-coded v8 after `RoomWire` moved to v9; repaired `.github/workflows/black-coop-room-network-apk.yml` in [`0aa5bc1ba25ddd06eeaf28a5b403d3122586e02f`](https://github.com/alwansan/BO/commit/0aa5bc1ba25ddd06eeaf28a5b403d3122586e02f). Build [45093547238](https://github.com/alwansan/BO/actions/runs/45093547238) scheduled for corrected v9 metadata, **NOT verified complete** at this note. Earlier [45093109167](https://github.com/alwansan/BO/actions/runs/45093109167) v9 build used old v8 metadata and must not be published as correct.
- Next work MUST implement native stable original object identity/engine event producer and safe guest object application; plus 2–4 real host game actors, independent weapons and physics, AI enemy targeting, synchronized damage/doors/mission. An event arriving in TLS without corresponding native game engine implementation does not change door state.
- The owner-authorized hourly autonomous continuation `6b03909067548191a740f2121625a041` is **ENABLED** after explicit check/update. Keep CI tests and source edits real; no small phone experiments. Still not an accepted playable co-op APK.


# P0 — تحويل BLACK الأصلية إلى عالم تعاوني واحد حقيقي

تحديث: 2026-11-23 — BO-COOP Session83. **هذه خطة تنفيذ محرك حقيقي وليست تصريحاً بأن APK الحالي يعمل تعاوُنياً.**

## حقيقة تجربة الهواتف
- TLS START/READY/COMMIT يسمح للهاتفين بتحميل `level_00/stg_0001` ولكنه لا يدمج محركين منفصلين. تجربة كسر الباب على الهوست وعدم تغيّره عند الضيف دليل على غياب حالة عالم واحدة، لا دليل على خلل بالشبكة فقط.
- جسم اللاعب الأصيل محلي من منظور أول ولا يمثل مجسماً شبكياً؛ `OriginalHumanBotVisual` يحتاج إكمال إثبات الإنشاء والرسم على الجهاز. أي جندي BOT أصلي مقبول مؤقتاً.
- [SESSION83] تم توصيل **قدمي اللاعب الأصلي على الضيف** إلى **قيم المنظر الموثَّقة والمحددة السرعة عند الهوست**؛ لا يصير بتلك الخطوة Actor حقيقي، ولا تصير الأبواب/الأضرار متزامنة.

## أدلة ABI ومعلومات يحق للتنفيذ التحقّق منها
المصدر البحثي المنشور MIT: `portingpete/Black-2006-Recomp@b2512fafd3`، ملف `src/recomp_manual.c` وخطة `docs/plans/split-screen-coop.md`. صورة XBE الخاصة بالمالك ثابتة بهوية SHA1 `c5bfed25449c8212d7a976f15246e87ead57713f`، لكن التطابق لا يثبت سلامة استدعاءات Android الأصلية.

| الأصل | إثبات من مصدر PC (يحتاج حراسة واختبار Android) |
|---|---|
| محرك القصة | `[0x002D45D4]`، جسم اللاعب الواحد عند `core+0x30` بحجم `0x900` |
| Camera owner | `[0x002D12B8]` وحجم كيان `0x16C0` |
| هدف الأعداء | `[0x002D1988]+0xFAC`، 32 × 12 byte، كائن الهدف بالمكان `entry+8` |
| Character pool | `[0x002D1980]`، 32 شخصية، الفهرس محلي لكل محرك **ولا يجوز نقل المؤشر أو فهرس NPC إلى ضيف** |
| نماذج الجنود | شخصية مستعارة بدون AI controller، `model+0x320`، `controller+0x324=0`، الرسم بواسطة virtual method slot5 في استدعاء grid draw |
| proxy physics | `sub_000DF400` لإنشاء proxy، و`sub_000DBBE0` لإرجاعه؛ عدد 20 داخلياً في `world+0x2320`، ينظر إلى إشغال `world+0x2960` |
| تهيئة اللاعب الثاني على PC | `sub_00014380` لـ camera owner، `sub_001E0E40` لاكتمال player load، `sub_001F1080` تحديث اللاعب |
| قصور PC المنشور | لاعب P2 يتحرك ويطلق؛ milestone3 غير منجز لأن الأعداء ما زالوا لا يستهدفونه بالطريقة الأصلية |

## شروط التحويل البرمجي الحقيقية، بالتسلسل

### P0-A — لاعبو الهوست الأصليون
1. على original BLACK game-thread بعد COMMIT فقط، أنشئ حتى ثلاثة *Managed original player contexts* لكل slot 2–4؛ لكل لاعب shadow actor وcamera owner وweapon inventory وinput source وframe-state وسجلات عمر؛ ليس تكرار `world+0x30` بطريقة عمياء.
2. تحقق أولاً من XBE ABI، heap allocation، thread affinity، world pointer، setup function arguments وتحرير موارد المراحل، مع Guard صارم لكل pointer range.
3. امنح كل Actor physics proxy حقيقياً في عالم الهوست، واحفظ ارتباطه وإلغاءه عند disconnect/world unload. استهلاك `RoomHumanInputWire` يكون مرة واحدة من الشبكة، لا من تحديث UI، مع حد 2–4 اللاعبين ونسخة واحدة للمشهد.
4. كل لاعب يتحرك ويصوّب ويطلق سلاحه بمحاكاة الهوست الأصلية؛ لا تعتمد على Java kinematic preview إلا كواجهة مؤقتة. يُرسل الهوست التحويل الحقيقي للـActor، Health/Ammo/Fire/Hit باعتبارها القيم المرجعية.
5. افصل رسوم منظور أول الشخصية المحلية عن جسمها المعروض للآخرين: استخدم BOT soldier للأقران بألوان/مؤثرات أصلية إن أمكن. لا تُفعّل Enemy AI على puppet الخاص بالعرض.

### P0-B — أعداء وضرر موحّدان
1. عرّف registry لمعرفات NPC ثابتة داخل **المشهد الواحد** بالهوست. لا تربط `host NPC slot` بـ `guest NPC slot` حسب نفس الرقم؛ ابحث عن هوية آمنة قابلة للإثبات (spawn identity + resource/model + generation) ورفض التصادم/التباس المطابقة.
2. أصلح AI Target pool للـActor2–4 وأثبت أن الهدف الأصلي يختار جميع البشر. جدول target وحده لا يكفي؛ تحقق أيضاً من world grid/attack fallback.
3. اسمح للمحرك الأصلي للهوست فقط بإجراء hit/damage/death/AI/physics/destructible calculations؛ ضيف القصة يعرض/يصلح لحالة الهوست، وليس محاكاة مستقلة تفرض ضرراً متضارباً.
4. ضمان عمر الحالة: scene epoch + token + ticket + roster revision + monotonic tick؛ tombstone حقيقي عند الانسحاب أو تغيير المرحلة.

### P0-C — أبواب وتكسير وأهداف القصة
1. عند الهوست، اربط حدث كسر/ضرر الباب بكائن اللعبة الأصلي وهويته المستقرة، وليس بمكان ثابت أو index قابل للتغير دون إثبات.
2. حين يطلق اللاعب الثاني، لا تُعطِ ضيفاً صلاحية تقرير أن الباب انكسر؛ الهوست يحسب الرصاصة/تصادم الباب ثم ينشر حالته/حدثه المرجعي.
3. في الضيف طَبِّق الحدث بواسطة استدعاء محرك أصلي مثبت وآمن، لا بكتابة عمياء على حقول غير معروفة. راجع animations/particles/audio حتى يظهر نفس التأثير.
4. اربط checkpoint والمهمات والأهداف والـcutscenes بالهوست؛ لا تسمح للضيف بالانتقال إلى مرحلة أخرى إلا بتذكرة host story جديدة.

### P0-D — تجربة القبول الكبيرة الوحيدة
- مضيف + ضيف على هاتفين، START مرة واحدة، القوائم تختفي، العالم الأصلي نفسه، كل لاعب يرى جسم الآخر ويتحرك معه مستقلاً.
- يقفان في مكان متفق عليه ويطلق كلاهما النار على باب/هدف: الضرر والتكسير والحطام يظهران على الجهازين، بنفس الترتيب والوقت المعقول.
- الخصم يستهدف الاثنين ويصيبهما ويستقبل ضررهما؛ موت NPC واحد لا يبقى حياً عند جهاز آخر.
- ينجح checkpoint وهدف القصة للمجموعة؛ reconnect/leave يؤدي إلى تنظيف actor/weapon/proxy والـTLS world بدون duplication أو انهيار.
- اجمع من تجربة واحدة logs آمنة تلقائية من الجهازين، بدون عناوين ذاكرة، بيانات حساب أو أسرار.

## قواعد CI والنتيجة المقبولة
- Native C++ sanitizer/ABI contracts وJVM genuine TLS 2–4 peers وAndroid29 KeyStore والـsource preflight والبناء الكامل ARM64 جميعها SUCCESS.
- ***القبول النهائي لا يعتمد على CI الاصطناعي فقط***؛ يلزم تقرير تشغيل أصلي على هاتفين يحتوي رسم أجسام اللاعبين وأثر الباب والأعداء والأهداف.
- لا تُلغِ بناء Windows الطويل بدفعات source متكررة؛ افحصه حتى ينجح أو يصل خطأ حقيقي ثم أصلحه.
- حصراً فرع `experiment/black-coop`. `main` و`CL` و`biso` وISO/XBE والـsaves محمية.

### Session84 — 2026-11-23 — authenticated ordered HOST object-event transport (NOT original BLACK world replication)

- Owner target unchanged: **two to four original BLACK players, ONE host-authoritative playable story, independent original engine actors, physics/AI/weapons, visible BOT bodies, shared destructible doors, enemies and mission objectives**. Do NOT ask for small phone tests; only one meaningful combined acceptance test once engine integration is complete.
- Source branch: `experiment/black-coop`; source implementation [`101b3ec9e8d575936b9afbbc9300f88c888c2a3e`](https://github.com/alwansan/BO/commit/101b3ec9e8d575936b9afbbc9300f88c888c2a3e). Protocol `RoomWire.VERSION/PROFILE=9` so older v8 room APKs MUST NOT join a new v9 room.
- New `RoomObjectEventWire`: 50-byte host event body, exact 16-byte room session + story token + roster revision + scene epoch + strictly increasing sequence/tick, strictly positive **opaque stable object identity** (not raw original memory pointer/NPC slot), enumerated kind/state. `RoomTls.Server.offerObjectEvent` only after story COMMIT, full 2–4-player fanout via encrypted sockets, never guest-originated, no silent dropping. `RoomTls.Client` rejects duplicates or gaps, buffers up to 64 ordered events, empties on ABORT/roster/close. `RoomSession` exposes native **verified game-thread-only** host source publishing and signed guest polling; no original object hook uses it yet.
- Added `RoomObjectEventWireTest` and **real four-peer loopback TLS** `RoomObjectEventTlsTest` and CI commands. Android packaging copies the new class in `coop/prepare_room_network.py`. At the time of this note source/test build runs `45093109148` (TLS), `45093109200` (Android29), `45093109118` (preflight), `45093109167` (APK **NO GAMEPLAY**) were in progress and **must be rechecked**, not marked success.
- Previous phone evidence: two independent local BLACK missions, guest room overlay, no remote human or same-door damage. Guest overlay auto-dismiss gate commit `dd11c771` and authentic original guest feet host visual reconciliation commit `37b581a0` exist; latest v8 full APK `45092061435` was SUCCESS but **did not produce actual co-op gameplay**.
- Important source finding: PC MIT `portingpete/Black-2006-Recomp@b2512fafd`, `src/recomp_manual.c`, implements experimental shadow P2 actor + physics proxy (`sub_000DF400`) and soldier puppets, but `docs/plans/split-screen-coop.md` says enemies still do not target P2; its context swaps are PC-only, NOT a proven Android 2–4 actor ABI.
- **P0 still OPEN:** (A) allocate/update original host player actors 2–4 with real physics collision/independent weapons; (B) original NPC AI targets all humans and reconciles enemy damage; (C) find proven native original door/object destruction event + stable per-spawn identity, publish it via newly available TLS object-event stream, verify safe matching original guest engine apply; (D) host-authoritative checkpoint/mission state. Transport alone is NOT a shared world.
- Safety: no blind guest NPC/object memory writes, unproven guest loader calls, or copying cross-engine pointers; do not mutate `main`, `CL`, `biso`, game ISO/XBE, saves. Keep GitHub Actions running; avoid burst pushes cancelling long APK builds.


### BO-COOP Session85 — 2026-11-23 — host event -> REAL ORIGINAL GUEST game-thread bridge (NO native door writes)

- Owner objective: Android BLACK first mission with 2–4 actual humans *simultaneously in one authoritative host world*, each a separate physics actor and soldier body, with original weapons/hits/enemies/destructible doors/objectives replicated and one host START. Do not request minor phone trials. Only one significant end-to-end trial after real original engine integration.
- All code **only** `experiment/black-coop`; latest source commits [`0d5699e1`](https://github.com/alwansan/BO/commit/0d5699e1f7f007a2474714a5d995d2c1f5fcd7af) host-object guest JNI/native inbox, [`ccb3ac1c`](https://github.com/alwansan/BO/commit/ccb3ac1ccfb751fdde3a7ec2deb954efff9571a1) test fixture fix, [`e7c3c0e2`](https://github.com/alwansan/BO/commit/e7c3c0e2408a821c97e102d22a510006b078cccd) actual TLS RoomSession->source-scoped 10-int event regression and monotonic sourceTick on host and guest.
- `RoomSession.pollOriginalHostObjectEventFrameFromGameThread()` now emits `[-1]` for uncommitted/abort/foreign thread/source loss, `[0]` for authenticated committed with no new event, `[epoch,rosterRevision,tokenHi,tokenLo,seq,tick,objectKeyHi,objectKeyLo,kind,state]` for authenticated host-world object state. Original source identity is independently checked against staged original episode and exact signed story ticket.
- `RoomAppRooms.pollHostObjectEventFromGameThread` and `coopPollHostObjectEventFromGameThread` in the injected Android `BlackActivity` expose this to the genuine original first-person game-thread, strictly after original source-stable mission match. New `room_original_guest_host_object_contract.hpp` and `_witness.hpp` contain an exact-scope 64-entry bounded immutable FIFO, single game-thread affinity, replay/gap/tick/overflow poisoning, stage/abort reset. Native unit `test_room_original_guest_host_object_contract.cpp` checks all conditions; no game physics/native memory writes. Safe coarse `ORIGINAL_GUEST_HOST_OBJECT_EVENT_INBOX_NOT_NATIVE_DOOR_APPLIED` event 103.
- Stage tool copies new headers, source preflight checks for no `kernel::W32` and `CallGuest`, Windows APK allowlist extended, native g++ sanitizer test added. `RoomObjectEventTlsTest` tests real FOUR encrypted peers and now `RoomSession` synthetic guest original game-thread frame path, wrong episode, empty, tombstone and original thread ownership. `RoomTls` rejects a later sequence whose `sourceTick` goes backwards.
- CI latest pushed code run IDs: real TLS `45094353077`, Android 29 `45094353079`, native source/preflight `45094353051`, Windows guest game APK `45094353098` (room lobby + original single-player, **NOT COMPLETED CO-OP**). The previous v9 TLS core `45093109148` and Android29 `45093109200` succeeded. **Check latest runs before claiming success; all new runs initially IN_PROGRESS at this session write.**
- **P0 BLOCKERS (strict, do not fake):** A. host original engine actors 2–4 and physics proxies/weapon hit logic missing; B. source-verified native draw of each remote body on phone unproven; C. enemy AI-target registration/attacks on all humans missing; D. source-proven original destructible event hook and per-instance stable object identity missing; E. applying that signed event safely to a matching original guest door is missing; F. original mission/objectives/checkpoint one-world authority unproven. Both phones still have separate SINGLETON original simulation, and door damage is not shared despite network transport.
- Pinned PC MIT reference `portingpete/Black-2006-Recomp@b2512fafd3`: experimental P2 shadow actor + physics proxy + soldier puppet; the reference itself says enemy targeting P2 not yet working. Port only with independently proven Android ABI/pointer/scene lifecycle, not pointer guesses.
- Owner files and branches protected: no `main`, `CL` writes, `biso`, original XBE/ISO/saves. Keep source and build CI tests, avoid making repeated code commits that auto-cancel costly full APK builds.


### Session86 — 2026-11-23 — original BLACK host 20-slot physics proxy reserve source survey (READ ONLY)

- Source commit [`f8edb2fbbe73483f78ed8f3939ba5cb29283305c`](https://github.com/alwansan/BO/commit/f8edb2fbbe73483f78ed8f3939ba5cb29283305c) adds `room_original_host_physics_proxy_contract.hpp`, witness and native unit regression under original source-matched local host game thread.
- ABI evidence: pinned MIT PC `portingpete/Black-2006-Recomp@b2512fafd3/src/recomp_manual.c` `black_proxies_free` reads 20 occupancy bytes at `[[0x2D45F4]+0x2960]`; `black_proxy_motion` obtains the first-person physics motion through `actor+0xB4 -> proxy+0x34 -> wrapper+0xC -> body+0x58`. Owner XBE source link previously verified; Android runtime ABI still requires actual phone diagnostic.
- Native source code checks each original memory page with `OriginalGuestSpanCommitted`, validates owner-world pointer does not change, checks the LOCAL original first-person physics proxy motion, counts available 20 proxy slots, requires one spare for original enemy AI in addition to 1–3 remote humans, emits fixed coarse non-pointer event codes 2100 unreadable / 2101 local physics missing / 2102 insufficient reserves / 2103 source-capacity eligible **NO NEW ACTOR**.
- Correctly triggers only on source-matched original mission plus signed host roster peer mask 0x02/0x06/0x0E on ORIGINAL game thread. **NO physics allocation, no proxy writes, no use of DF400 or DBBE0, and NOT proof that enemies target remote human.**
- CI source staging and Windows strict additional-file allowlist updated. Native C++ purity/capacity regression runs with undefined sanitizer. [Source preflight `45094581462` SUCCESS](https://github.com/alwansan/BO/actions/runs/45094581462), isolated UDP `45094581480` SUCCESS, newest Windows ARM64 APK `45094581459` IN_PROGRESS at last check (still room network + original single-player, NO GAMEPLAY co-op). Previous full v9 core tests `45094353077` TLS SUCCESS, Android29 `45094353079` SUCCESS, native source `45094353051` SUCCESS.
- Session85 native guest object-events inbox remains source-only and TESTED. The following **critical P0 tasks are unimplemented**: safe original player-context/physics actors 2–4, independent weapon/ballistics and AI targeting; proven original soldier puppets visible on actual phone; original door identity/destruction producer; safe native guest object-state application and global mission authority. Two original BLACK levels currently remain independent, even if both load stage1 with one START.
- Next agent: before making original host actors, read source proof above and physics proxy diagnostic; do NOT blindly call DF400/DBBE0/engine loader in owner game. Preserve asset files and all other branches. Test latest GitHub Actions, fix only proven CI errors; only ask owner for one substantial combined phone test after actual shared-gameplay implementation. Keep all handoffs/memory/session notes up to date.


### Session87 — 2026-11-23 — real original remote-human soldier animation and pinned actor ABI survey
- Code commit `7951e38158e8c71c169e48f43bc826c531fce523`: `OriginalRemoteHumanMotionPlan` selects original detached soldier idle/move clips based on authenticated source-sampled remote feet (two-sample jitter guard, replay/stale/scene epoch), `OriginalHumanBotVisual` calls source-linked `sub_0017E130` on original visual model instance with game-memory clip strings, only after successful original model creation and dispatch/pointer checks. C++/Python/Java source preflight `45095381428` SUCCESS, UDP `45095381433` SUCCESS. **NOT verified on actual phone; NOT original second physical actor or shared AI**.
- Pin-verified owner XBE audit commit `22c8a07cda4751d2b547be6558a5c66b514ff637`: exact original executable windows of P2 setup / owner initializer / physics proxy create-release / player tick / animation and pose. GitHub Actions `45095558435` both synthetic and private owner-xbe jobs SUCCESS. Numeric-only artifact. This proves source entry candidates, NOT Android-safe ABI/runtime invocation.
- Windows source-only ORIGINAL BLACK Android candidate APK `45095381425` still building as of last query, DO NOT report it completed, DO NOT give as playable co-op. Changing room native source would cancel this expensive long-running build, so continue analysis/docs until verification.
- Key technical source ref: `portingpete/Black-2006-Recomp@b2512fafd3/src/recomp_manual.c`; PC P2 actor context swap must preserve core/world grid self-links and swap camera owner, banks/first-person animations, weapons, shells and FX, then rebind physical proxy and AI target. `docs/ORIGINAL_HOST_PLAYER_CONTEXT_ABI_AR.md` has precise integration gates and risks.
- P0 remains **UNIMPLEMENTED**: extra original physical actor 2–4 under HOST, original weapons/collision damage and AI target, stable door identity and native guest apply, host-owned objective/checkpoint. Do not ask owner for small phone tests. Remain exclusively `experiment/black-coop`; update restoration/plan/memory/session after changes.


### BO-COOP Session88 — 2026-11-23 — source-verified ORIGINAL forward yaw + signed host extra-actor prerequisite gate

- Owner GOAL unchanged: host presses START, 2–4 players in ONE ORIGINAL BLACK mission/gameplay world with real original physics and independent weapon/AI damage, soldier bodies, synchronized doors/objectives. No minor phone trials. No edit to `main`, `CL`, `biso`, owner ISO/XBE/saves.
- `44bec0b8567a7db2e6c2c7664498c16ab033b60a`: original actor `player+0xF0` forward aiming vector from pinned `RanggaCasper/Black-Android hooks.cpp` is now quantized to heading from `atan2(forwardX,forwardZ)` in centidegrees (with bounds, zero vector and NaN rejects). Native original player eye JNI sends eye/feet/heading (7 ints), not six. Guest authenticated pad source sends actual original facing yaw instead of forced 0; `RoomSession` sends original HOST first-person yaw in avatar 1 of signed TLS world snapshot. Detached soldier BOT draw rotates source original horizontal basis to remote human's yaw. Source-only: neither combat/physics nor actual on-phone visible model proven. Previous animation idle/walk sourced from `7951e381` remains.
- Existing test builds initially failed because Java TLS tests and Python preflight fixtures still expected six fields; fixed by `7f7e3a7a`, `7232fb42`, `ddeb3c79`, `fa48fbe1`. Real TLS test [`45096561917`](https://github.com/alwansan/BO/actions/runs/45096561917) SUCCESS after test updates. Source preflight [`45096661581`](https://github.com/alwansan/BO/actions/runs/45096661581) SUCCESS. The source tests now check original+0xF0 quantization and safe rotation. No protocol bump required: 16-bit yaw was already a v9 field.
- `80e8cdf2d0974dbe3c981dd9a350851c8580e90e`: original HOST game-thread read-only gate combines signed roster 2–4, fresh remote input, original physics proxy available with NPC spare, 32 AI target capacity/bound original local human, and 32 soldier look slot capacity. Codes 2120..2127 diagnose missing stage, bad roster, mismatched input epoch, stale peer input, physics, AI, soldier slots, or source-capacity eligible WITHOUT AN ORIGINAL GAME ACTOR.
- `ebd43706112c7fa8b80810396a61a545ad01eb2c`: corrected a serious identity flaw in 80e8: a source input epoch must NEVER authenticate itself. Added independent JNI `coopHostCommittedOriginalSceneEpochFromGameThread` from actual HOST signed TLS `RoomStoryWire.Ticket`, requiring committed host, original running-ready ticket, fresh matched core/stream stage, correct owner game thread. The native gate requires that signed scene epoch equals `OriginalHostHumanInputs().Epoch()` from a separate TLS input inbox. No untrusted packet can set source `signedStoryEpoch`.
- Source-native C++/Java original game bridge [`45096922154`](https://github.com/alwansan/BO/actions/runs/45096922154) SUCCESS, isolated UDP [`45096922153`](https://github.com/alwansan/BO/actions/runs/45096922153) SUCCESS. Genuine JVM TLS [`45096922229`](https://github.com/alwansan/BO/actions/runs/45096922229), Android29 [`45096922209`](https://github.com/alwansan/BO/actions/runs/45096922209) and Windows game APK [`45096922149`](https://github.com/alwansan/BO/actions/runs/45096922149) **IN_PROGRESS** at last check (confirm actual outcome before claiming success). Windows APK workflow is explicitly `NO GAMEPLAY`, even if all steps succeed. Avoid further native code pushes just to update docs: expensive builds cancel on newer candidate.
- **P0 BLOCKERS OPEN:** original engine-owned shadow host actor2–4, original camera owner and independent weapon/animation asset ownership, physical proxy acquisition/release and rollback, enemy AI targeting these humans, original stable door/object event producer and safe guest object apply, host source checkpoints/objectives and one actual simulation. So the two phones still run independent original BLACK scenes. This task was NOT completed by readiness, yaw transport, or successful CI tests.

