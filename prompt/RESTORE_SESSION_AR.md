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
- الأولوية: **ACTOR-001A حقيقي** إثبات owner/lifetime/model، ثم رسم proxy، تحكم مستقل/أنيميشن/أسلحة/حفظ أحداث المهمات؛ بالتوازي LAB منفصل لملف LAN discovery وضوابط المنافذ ونسخة بروتوكول، ثم واجهة الغرفة.
- حدّث STATE/MEMORY/PLAN/ISSUES والجلسة وRESTORE بعد CI أو أي كود. اكتب فقط فرع experiment/black-coop؛ لا تعد بعمل مستمر في الخلفية دون automation.
