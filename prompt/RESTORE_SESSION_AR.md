**تحديث النتائج:** core44872596855SUCCESS بكلاjob وقرئتlogs؛APK44872596889لايزالجارياً. بروتوكولاختبارالهاتفdocs/PROBE3_DEVICE_TEST_AR.md؛لايُطلبقبلنجاحAPK.

**Actions قيد المتابعة:** APK44872596889 وcore44872596855،source96b9c8366a80dd321be47857dc67f1000213fe7f. تحقّقمنحالتهمانفسهماأولاً؛لمتثبتنتيجةAPKبعد.

# أحدث متابعة — جلسة21: Probe3 مدمج بالمصدر، بناء APK لم يُثبت بعد
اقرأ memory/sessions/2026-11-21_bo-coop-21-probe3-device-build.md. CoreCI44871689814 SUCCESS بكلاjob. أضيفprepare_probe3.py وpool_observer.inc وworkflowblack-coop-probe3-apk.yml؛محلياًC++/Python/JSONtestsPASS ومراجعةمستقلةدونملاحظاتمهمةمتبقية. **تابعActionsالحيقبلطلباختبارالهاتف**؛لاAPKProbe3موثوقحتىنجاحالبناءوالفحوص. tuplepool=[slot,group,serial,state,resource320,resource35c]؛resourcepointersإشاراتمرصودةفقط،لاmodelownershipproof. resetcounterAL1بعدoriginal. Probe2/Baselineمحفوظان. لاتكررإنشاءcoreمنالصفر.

# أحدث متابعة — جلسة20
اقرأ memory/sessions/2026-11-21_bo-coop-20-pool-reader.md والخطة docs/superpowers/plans/2026-11-21-probe3-pool.md. أضيفقارئpool مستقل واختباراتkernelselfread وworkflow؛المحليPASS معتصحيحمراجعةrawarrayreorder،CIقيدالانتظار،ولاAPK/ربطgameبعد. تحققآخرActions ثمأكملTask2. لا تكررbaseline/Probe2 tests أوتطلبأصولموجودة.

**آخر دليل:** reset1D5E00 يعيدcounter8524 إلى1 ويعيد32 خانة إلىfree-list على مسارAL1 فقط؛راجع ملحق docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md. نطاق المنتج الكامل محفوظ في docs/OWNER_PROJECT_OBJECTIVE_AR.md.

**تصحيح أحدث:** اقرأ docs/ACTOR_POOL_STATIC_CONTRACTS_AR.md: container+4=capacity و+8=occupied، مصفوفات sparse؛ 1DF440 acquisition path إلىinit1DA4F0، و1B6770 نقل عضوية. لا تعتمد تسميات count القديمة.

**تنبيه متابعة جلسة19:** 0x1E6860 لها آثار جانبية وليست capacity predicate نقية؛ لا تستدعها من probe. +79C0/+79D0 قوائم حالات متعددة وactor+384 يكتب1/2. اقرأ ملحق جلسة19 قبل التنفيذ.

# استعادة عاجلة — جلسة19، 2026-11-21
اقرأ أولاً AGENTS.md ثم memory/STATE.json ثم **memory/sessions/2026-11-21_bo-coop-19-cloud-handoff.md**. هذه أحدث معرفة؛ ما أدناه تاريخ جلسة18 للمساندة.
الهدف story co-op حقيقي Android؛ لم يكتمل. المالك يجيز متابعة البحث والتنفيذ وGitHub Actions حتى الحاجة لاختبار هاتف. الكتابة فقط experiment/black-coop.
**الاكتشاف الجديد:** pool32 للشخصيات عند manager global0x2D1980، slots+0xA0 stride0x3C0، وserial مرشح actor+0x374 من manager+0x8524. مثبتة تعليمات التهيئة فقط؛ دوال lifecycle لم تثبت بعد. ابدأ بفحص دوال pool الموثقة في جلسة19 ثم Probe3 read-only، وليس تكرار baseline أو توسيع mock net. لا APK جديدة ولا CI جديد في جلسة19.
المصادر الكبيرة موجودة في BO/biso؛ **لا تطلب إعادة رفعها**. سجل Probe2 موجود في CL، لا تنشر الخام في BO. حافظ على baseline/Probe2 والحفظ. تحقق من HEAD وActions قبل العمل، وحدّث هذه الذاكرة بانتظام.

---

# BO-COOP — استعادة جلسة18 بتاريخ 2026-11-21
**الكتابة حصراً:** `alwansan/BO:experiment/black-coop`. اقرأ أولاً `AGENTS.md` ثم `memory/STATE.json`, `memory/MEMORY.md`, `memory/PLAN_AR.md`, `memory/ISSUES_AR.md`, `prompt/SESSION_PROTOCOL_AR.md`، وأحدث جلسة `memory/sessions/2026-11-21_bo-coop-18-thirdparty-black-multiplayer.md`، ثم `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`, `docs/ACTOR001A_SOURCE_HOOK_DECISIONS_AR.md`, `docs/ACTOR_001A_NEXT_GATES_AR.md`, `docs/ROOM_TOP_LEVEL_FEASIBILITY_AR.md`, `docs/OWNER_BISO_SOURCE_AR.md`. تحقق من HEAD وActions الحيين.

## اكتشاف صاحب المشروع الجديد
المالك أرسل <https://github.com/Peterson047/BLACK-XBEMultiplayer>. قرأنا README وكل scripts التقنية المطلوبة وnotices وجردنا كل tree 91 مدخلاً، pin **`0ee3ae6411c7f0989c62cbe36b224d3f55043923`**. إنه **Windows 10/11 x64 Team Deathmatch binary alpha** 8 لاعبين، dedicated/host+LAN browser/direct IP، لا story co-op ولا Android. هذا المستودع **لا يحتوي native C++ multiplayer source**. README يحيل إلى `Peterson047/BLACK-Coop:feat/windows-multiplayer@095488c` لكن GitHub connector 404؛ قد يكون محذوفاً أو خاصاً، لا تفترض إمكانية نسخ C++ منه. لا LICENSE شامل؛ `assets/anim/body.bca` مقيد بترخيص صاحبه «داخل ذلك المود فقط». لا تنسخ source scripts/art/binaries، اعتمد الأفكار وحدها مع كود مستقل. اقرأ الوثيقة `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md` دائماً عند أي عمل actor/network/animation.

## ما استُفيد منه فعلاً
- مقارنة metadata من `biso` المالك، pin `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`: **13/13** ملفات مرحلة `level_00` المطلوبة ومقاسات مطابقة، **16 زوج DB/WDD** أسلحة. أداة أصلية `coop/analysis/tdm_reference_profile.py` + synthetic negative tests + workflow `.github/workflows/black-coop-tdm-asset-preflight.yml`، CI [44868801462](https://github.com/alwansan/BO/actions/runs/44868801462) **SUCCESS**، commit `49d987bc5b8afcff0b796dfda087a80e4422d9cf`. تختبر manifest فقط، لا تنزيل ISO/المحتوى أو إثبات تشغيل TDM.
- Peterson docs: منافذ UDP HOST1508 وLAN discovery1510 **يجب فصلها** لتجنب duplicate JOIN، خيط net مستقل عن render، تحضير خريطة من ISO المستخدم، remote-body animation/footsteps، رفض اختلاف الإصدار. طبق بنية مستقلة مناسبة لـAndroid وتحقق أمن الجلسة قبل الربط بالشبكة.
- المصدر الأصلي المملوك متوفر ISO مجزأة و`biso` مفككة؛ لا تطلب إعادة رفع/اختبار Baseline أو XBE مفقود.

## حالة BO العملية
- Probe2 على جهاز المالك Android15: export success، 400 read-only events (player131/lod172/animation97)، 13 LOD+animation correlations من epoch واحدة. لا NPC birth/death proof، والفجوات لا تثبت موتاً.
- `coop/bridge/remote_proxy_roster.hpp` وSceneInbox وhost toy integration **CI SUCCESS** [44867924228](https://github.com/alwansan/BO/actions/runs/44867924228)، **قيم تجريبية فقط** ولا توجد شخصية حقيقية/animation/quest/combat sync أو authentication.
- صفحة CO-OP داخل OPTIONS تجريبية، زر أعلى القائمة تحت الإعدادات غير منفذ. Probe2 وbaseline لا تمس؛ game saves أصلي محفوظ.
- الأولوية: **ACTOR-001A حقيقي** إثبات owner/lifetime/model، ثم proxy والتحكم والمهام. بالتوازي: `lan_discovery.hpp`/codec + `lan_socket.hpp` UDP على loopback صار ناجحاً **فعلياً** على Linux ASAN/UBSAN، مع NDK28 cross compile AArch64 [44869388876](https://github.com/alwansan/BO/actions/runs/44869388876) SUCCESS code d97c3ffe. 5000 packet fuzz، ports distinct، rate gate، inbound limiter16، source+nonce، bounded timeouts. **لا broadcast حقيقي LAN أو Android phone runtime، ولا JNI/الغرفة/مصادقة أو multiplayer فعلي**. اقرأ `docs/NET_LAN_DISCOVERY_AR.md`، القادم LAN bind/device tests ثم game menu بعد ACTOR proof.
- حدّث STATE/MEMORY/PLAN/ISSUES والجلسة وRESTORE بعد CI أو أي كود. اكتب فقط فرع experiment/black-coop؛ لا تعد بعمل مستمر في الخلفية دون automation.

## جلسة18 — LAN discovery codec مستقل نجح في CI (2026-11-21)
- Developed original `coop/native/lan_discovery.hpp` and `test_lan_discovery.cpp`, source code `33bff07535a85f256c50a6a2842c19714148d5fe`. Inspired by Peterson047's *documented architecture only* (game and discovery ports distinct, LAN browser concept), **no source/assets copied**.
- Wire format `BCLN` version1: short query/reply with 32-bit challenge (correlation, **not auth**), game port, occupied/capacity (1..4), printable room name max32, length/byte/version/duplicate port guards and outgoing 1Hz RateGate. Only **codec/state validation**, **no UDP broadcast socket, actual browser, phone JNI integration, encrypted/authenticated session or playable game network**. Suggested game1508/discovery1510 are defaults referenced by Peterson project, configurable later. Android might require system multicast/broadcast considerations; not yet proven on phone.
- [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957) **SUCCESS BOTH JOBS**: native-linux job136739329989 printed passing strict packet parsing, port/name/rate guard,5000 deterministic malformed packets; legacy UDP loopback 2 clients passes. Android ARM64 job136739329666 cross-compiled LAN test binary and `udp_cli` with NDK28 and confirmed AArch64 ELF; **not executed on Android phone**. Existing unrelated UDP mock [44869092971](https://github.com/alwansan/BO/actions/runs/44869092971) SUCCESS.
- Remote author source `Peterson047/BLACK-Coop` remained 404; do not copy its restricted `body.bca`, binary, badges or unlicensed scripts. Actor lifetime/rig/weapon/quest sync remains primary main blocker. Safe discovery design `docs/NET_LAN_DISCOVERY_AR.md`.

## جلسة18 — POSIX UDP LAN discovery loopback فعلي (2026-11-21)
- Added original `coop/native/lan_socket.hpp` and `test_lan_socket.cpp`, source commit `d97c3ffe3bb19c2950acca7db7db3943fee16d2d`. Portable Linux/Android-NDK POSIX socket RAII, default loopback bind, no SO_REUSEADDR to prevent duplicate port consumption, bounded MSG_TRUNC recv, source IP+port checks, 16-slot inbound per-endpoint 500ms PeerRateGate/5s slot expiry, outgoing1Hz budget from BCLN-v1, strict version/nonce/room/port validation. **No broadcast, app/JNI integration, WAN auth/encryption or actual BLACK player**.
- [44869388876](https://github.com/alwansan/BO/actions/runs/44869388876) **SUCCESS** BOTH JOBS on source `d97c3ffe`: Linux job136740257311 used ASAN/UBSAN and validated **actual UDP IPv4 loopback** Query/Reply, source+challenge, duplicate binding rejected, bounded timeout, incoming/outgoing rate caps and synthetic 5000 malformed datagram decodes. Android ARM64 job136740257470 successfully NDK28 cross-compiled test and checked ELF AArch64; **not executed on actual phone**. Separate toy UDP CI [44869388854](https://github.com/alwansan/BO/actions/runs/44869388854) SUCCESS.
- Peterson047 reference remains read-only and unlicensed external binaries/media unavailable for copying. Their Windows Team Deathmatch source `Peterson047/BLACK-Coop` 404; the independent BO room discovery now has a bounded real UDP loopback transport, but it **cannot be marketed as LAN game hosting** until Android sockets interface, LAN network discovery, authenticated join and a real NPC-player render hook exist.
