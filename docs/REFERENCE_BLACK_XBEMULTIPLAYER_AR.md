# مصدر BO-COOP المرجعي المهم: BLACK-XBEMultiplayer (Windows TDM)
**حفظ دائم داخل المشروع — 2026-11-21**. يجب الرجوع إليه في كل جلسة عند بحث اللاعب الثاني، موارد الجندي والأنيميشن، شبكة LAN/host، الأسلحة، الصوت، القوائم أو الخادم.

- المستودع: <https://github.com/Peterson047/BLACK-XBEMultiplayer>
- فرع القراءة فقط: `main`؛ commit المثبّت **`0ee3ae6411c7f0989c62cbe36b224d3f55043923`**.
- صاحب المشروع: Peterson047، مبني بحسب README على `TGP482/BLACK-XBERecomp` و`XboxXBE-Recomp`.
- يدّعي README أن الشفرة الكاملة للنسخة الأخرى موجودة في <https://github.com/Peterson047/BLACK-Coop/tree/feat/windows-multiplayer>، commit `095488c`؛ **فشل فتح المستودع والفرع والـtree عبر GitHub 404 في 2026-11-21**. لا تدّع فحص الشفرة أو وجود حق الوصول إليها أو تطابقها مع BO. ابحث من جديد في الجلسة الجديدة إذا أتيحت.
- المستودع نفسه **ليس مصدراً كاملاً لـC++**: الملفات المفتوحة `README.md`, `THIRD-PARTY-NOTICES.txt`, `tools/extract_mp_map.py`, `tools/prepare_pvp_audio.py`, `tools/prepare_pvp_weapons.py`, `windows/*.bat`, `assets/README.txt`، وفهرس Git كامل 91 إدخالاً. بقية كبيرة منها `BLACK.exe`, OpenSSL DLL، بيئة Python مضمنة، ملفات animations وأيقونات. **لم يُفك الـEXE ولم تُحلل animation bytes**.
- GitHub repo metadata `license=null`، ولا ملف ترخيص شامل في Git tree. ملفات `THIRD-PARTY-NOTICES.txt` تنسب MIT/SDL/Apache/PSF **لمكونات محددة**، ولا تمنح تلقائياً حق إعادة نسخ كود المشروع نفسه. `assets/anim/body.bca` (351,570B) يحوي أنيميشن محوّل من Kevin Iglesias، والترخيص الموثق **استعمال ضمن ذلك المود فقط، ولا يجوز استخراج/إعادة توزيع الحركات منفردة**. لا تستخدم الأصل أو الـDLL أو body.bca أو badges ضمن APK دون تصريح مناسب. نستفيد من الفكرة ونكتب تنفيذاً مستقلاً فقط.

## ما ينجزه مشروع Peterson فعلياً حسب توثيقه
- **Team Deathmatch** مستقل عن القصة، Black Ops ضد Seventh Wave في خريطة Veblensk، حتى **8 لاعبين**، Windows 10/11 x64 وVulkan 1.2.
- UDP host/join وLAN discovery وdirect-IP، خادم dedicated اختياري، متصفح غرف، وضع TDM/ترسانة أسلحة، hits/kills، أنيميشن وصوت خطوات للخصوم، killmarks وscoreboard، تحكمات إضافية.
- خيط شبكة **منفصل عن الرندر** وفق README. **ليس إثباتاً أن اللعبة الأساسية فيها actor factory قابلة لإعادة الاستخدام للطور القصصي**؛ الأجسام البعيدة قد تُعرض عبر proxies مستقلة.
- `host_port=1508` و`discovery_port=1510` مثال إعدادات، **يجب أن يختلفا**: README يحذّر من معالجة JOIN مرتين عند مشاركة منفذ UDP في Windows. طبقة اكتشاف غرف BO يجب أن ترفض تساوي المنفذين قبل bind، مع ميزانية بث واستجابة وتدقيق مصدر/حجم الرزم.
- خادم الإنترنت قد يحتاج port forwarding؛ VPN هو اختيار المستخدم لتجنب بعض مشاكل NAT، ولا يضمن P2P بلا relay. README يذكر حماية TLS للخوادم التجريبية فقط، وليس إثبات أن هذا البناء آمن لكل اتصال.

## أهم استفادة من نظام الموارد
`tools/extract_mp_map.py` يحدد حزمة خريطة TDM الأولى من `levels/level_00`: 13 ملفاً مطلوباً للمرحلة، بما فيها `Unit_01.bin`, `Collide.awd`, `LevelDat.bin`, `Level.awd`, `speech.slb`, `stg_0001/StLevel.bin`, `StUnit01.bin`, `guns.bin`, `guns_s.bin`, `AIWpns.awd`, `Emitter.awd`, `Destruct.bks`, `Destruct.ssh`. يذكر الحفاظ على جداول speech اللازمة للـloader حتى عند إطفاء حوار القصة؛ هذه ملاحظة تقنية مهمة لأي فصل صوتي آمن.
- تأكدت **من الأسماء والأحجام لكل الملفات الـ13/13** في `alwansan/biso` المفكك المثبت `9ceb6a65fb5a4414c106ca3961295a1f8f7e558b`، ووجود **16 زوجاً DB/WDD** داخل `level_00/fpguns`. هذه **مطابقة فهرس موارد**؛ لا تعني نجاح تشغيل TDM أو كونها actor body جاهزاً.
- `tools/prepare_pvp_weapons.py` يعيد جمع موارد fpguns من الـISO المملوكة، مع فحص bytewise قبل استبدال موجود وتوليد إلى حزمة منفصلة. `tools/prepare_pvp_audio.py` يصمم bank صوتي من موارد السلاح وأصول المستخدم، يتحقق من حدود الذاكرة ودقة الصوت وإعادة قراءة الناتج، ويحتوي فرضيات خاصة بوظائف الصوت `sub_0014AE60` و`sub_0013FC20`. **لا تُسقط هذه الـoffsets على Android ABI أو تقدمها على أنها مثبتة في build BO**.
- نُفذ بشكل **مستقل** في BO: `coop/analysis/tdm_reference_profile.py`, `coop/analysis/test_tdm_reference_profile.py`، `.github/workflows/black-coop-tdm-asset-preflight.yml`. يقرأ CI فهرس biso فقط ويقارن بيانات الأسماء والأحجام دون تنزيل ISO أو نسخ شفرات/أصول Peterson. CI <https://github.com/alwansan/BO/actions/runs/44868801462> **SUCCESS**.

## خريطة الاستفادة العملية حسب الأولوية
| الفكرة المثبتة من الوصف | حالة BO | الخطوة الآمنة المطلوبة |
|---|---|---|
| تمثيل لاعب بعيد + أنيميشن شخص كامل | `RemoteProxyRoster` **قيميّ فقط**؛ لا جسم داخل BLACK | ابحث عن rendering/rig ownership وNPC spawn/lifetime بنفس إصدار XBE؛ إذا توفرت شفرة مصدر مرخصة لاحقاً راجع المعمارية ولا تنسخ offsets عشوائياً |
| شبكة UDP وخيط منفصل | NET lab وSceneInbox نجحت اختبارات host | أضف اكتشاف غرف آمن مستقل، port collision guard وtimeouts؛ المصادقة/التشفير لاحقاً قبل WAN |
| بناء map/weapon packs من ISO المستخدم | فهرس `biso` متحقق SHA على جميع ملفات ISO | أداة preflight metadata مكتملة؛ لا تقم بإعادة تجميع/تحوير game assets في CI؛ حافظ على قصة اللاعب |
| واجهة Multiplayer server browser | صفحة CO-OP داخل OPTIONS موجودة تجريبياً | زر أعلى القائمة تحت الإعدادات من source v1.5.0، ثم Host/Join LAN فقط عند backend معتمد |
| remote footsteps/weapon audio | لا مصادر لعبة مشتركة في BO بعد | بعد اللاعب البعيد، اعزل الصوت المكاني ولا تكرر تعديلات صوت القصة؛ يحتاج ownership والتزامن |
| hits, weapons, scoreboard | NET mock فقط | لا تنقل scoreboard أو TDM باعتبارها quest checkpoints؛ مهمتنا campaign co-op وNPC AI/mission/save authority |
| dedicated server | مشروع Peterson يدعمه | ليس أولوية: هاتف المضيف user-hosted وفق شرط المالك، لا تخصص اللاعب المضيف مكاناً للخادم |

## حدود واضحة
- نجاح TDM موثق **حسب README**، ولم نختبر binary على Windows أو Android. **لا مصدر كامل متاح في هذا المستودع**، والمصدر المُحال إليه أعاد 404؛ لا ادعاء «نسخنا كامل Multiplayer».
- TDM خريطة مستقلة ≠ تعاون في حملات BLACK. **يحتاج** ضبط NPC AI، إعادة التحميل والـcheckpoint، الأهداف/الأحداث/الأبواب، انبعاث الأعداء، صحة اللاعبين، الحفظ والجلسات.
- **ممنوع** استعمال `assets/anim/body.bca` أو رسومات HUD/killmarks أو `BLACK.exe` أو OpenSSL DLL أو سكربتاتهم كما هي دون تحقق الترخيص؛ نكتب ملكنا باختبارات مع إعطاء المراجع المناسبة. لا نعدّل `Peterson047` ولا `BO/main` ولا `biso`.
- لا تسأل المالك عن ISO/XBE مفقودة أو إعادة اختبار Baseline؛ سبق إثباتها. المرجع يجب أن يبقى في `AGENTS.md`, `memory/STATE.json`, `memory/MEMORY.md`, `PLAN_AR.md`, `prompt/RESTORE_SESSION_AR.md`.

## جلسة18 — LAN discovery codec مستقل نجح في CI (2026-11-21)
- Developed original `coop/native/lan_discovery.hpp` and `test_lan_discovery.cpp`, source code `33bff07535a85f256c50a6a2842c19714148d5fe`. Inspired by Peterson047's *documented architecture only* (game and discovery ports distinct, LAN browser concept), **no source/assets copied**.
- Wire format `BCLN` version1: short query/reply with 32-bit challenge (correlation, **not auth**), game port, occupied/capacity (1..4), printable room name max32, length/byte/version/duplicate port guards and outgoing 1Hz RateGate. Only **codec/state validation**, **no UDP broadcast socket, actual browser, phone JNI integration, encrypted/authenticated session or playable game network**. Suggested game1508/discovery1510 are defaults referenced by Peterson project, configurable later. Android might require system multicast/broadcast considerations; not yet proven on phone.
- [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957) **SUCCESS BOTH JOBS**: native-linux job136739329989 printed passing strict packet parsing, port/name/rate guard,5000 deterministic malformed packets; legacy UDP loopback 2 clients passes. Android ARM64 job136739329666 cross-compiled LAN test binary and `udp_cli` with NDK28 and confirmed AArch64 ELF; **not executed on Android phone**. Existing unrelated UDP mock [44869092971](https://github.com/alwansan/BO/actions/runs/44869092971) SUCCESS.
- Remote author source `Peterson047/BLACK-Coop` remained 404; do not copy its restricted `body.bca`, binary, badges or unlicensed scripts. Actor lifetime/rig/weapon/quest sync remains primary main blocker. Safe discovery design `docs/NET_LAN_DISCOVERY_AR.md`.
