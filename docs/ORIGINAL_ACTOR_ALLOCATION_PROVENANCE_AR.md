### Session108 verified Android-native outcome

[45206563984](https://github.com/alwansan/BO/actions/runs/45206563984) **SUCCESS** after [242fa976](https://github.com/alwansan/BO/commit/242fa976716961f2c2eacb459de64ca9bba0229c) strict exact header allowlist repair. Actual Windows runner `candidate-probe` built Android original `libmain.so` and isolated APK; the workflow's `Compile isolated Android baseline with bounded parallelism`, `Verify ... JNI, signature, ABI and compile image` and artifact-upload steps each SUCCESS. Therefore the original player storage/proxy headers are now **compiled into the pinned Android game source without breaking the build**. This is NOT actor initialization or playable shared-world co-op: no extra game-engine actor, separate host player weapons, synced enemy AI/doors/objectives or guest original world has been proven. Avoid unnecessary user APK test.

The original Windows private engine [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) has confirmed `dashboard_error_harddrive`; this is virtual-HDD compatibility of the **different PC emulator**, not phone storage. Track work separately. See `docs/ORIGINAL_PC_HARDDRIVE_DASHBOARD_ERROR_AR.md`.

# إثبات ملكية الذاكرة الأصلية للاعب BLACK الإضافي — Session108

2026-11-23. الإصلاح [2910ab90](https://github.com/alwansan/BO/commit/2910ab90004b40749008ca8d82daa7787d3935e4) على الفرع `experiment/black-coop`، واختبار C++ المصدر [45204058892](https://github.com/alwansan/BO/actions/runs/45204058892) **SUCCESS**.

## السبب

قبل التصحيح كان `OriginalHostProxyEngine::actor()` يتحقق من `vtable/kind/serial/collider` وصلاحية صفحة الذاكرة وخروج actor عن نطاق world/NPC/physics، ولكنه لا يثبت أن actor خُصص من original player storage. يمكن لكائن مزيف أن ينسخ كلمات header الصحيحة في نطاق committed مجهول ثم يصل إلى استدعاء `DF400`. هذا ليس مجرد اختلاف بصري: محرك الفيزياء الأصلي يحتفظ بروابط عناوين مرجعية تحتاج lifetime مثبتاً.

## التنفيذ المصدر

- `OriginalHostProxyEngine::actor()` يطلب الآن `io.OwnsActor(actor,0x900)` **قبل** قبول header واستدعاء أي original engine guest call.
- `OriginalHostProxyKernelIO` العام يرفض تلقائياً كل actor (`OwnsActor=false`) لأنه ليس مالكاً لتخصيص independent human.
- `OriginalPlayerStorageKernelIO` يعطي الإثبات فقط إذا كان عنوان actor ومساحة `0x900` مسجلين في ledger التخصيصات لهذا storage adapter، وتثبت `mem::AllocationSize` عدم انتهاء ذلك التخصيص. ليست `mem::IsCommitted` وحدها شهادة ملكية.
- الأصل المستهدف `OriginalHostPlayerStorage::Allocate` يسجل خمسة أجزاء مستقلة، ومنها actor `0x900`. وفي teardown الآمن `Free` يمحو سجل التخصيص.
- أُضيفت اختبارات رفض header مزيف خارج سجلات التخصيص، رفض actor لم يعد مملوكاً قبل Acquire، ورفض Release بعد سحب شهادته مع حجر scene؛ الاختبار تحت ASAN/UBSAN نجح ضمن المصدر.

## ما لا يثبته الاختبار

- لم تنفذ بعد original setup مكتمل أو نشر هذا adapter على خيط Android الأصلي مع lifecycle الجاهز؛ `setupComplete` لا يزال يحتاج شهادة مستقلة فعلية لا يأتي بها عنوان التخصيص وحده.
- `OriginalProxySceneLedger` لا يجوز استبداله عند failure أو انتقال room/network؛ يجب أن يبقى مع owner original scene حتى تحرير مثبت. هذا دليل مصدر fixture لا صورة حقيقية للاعب في مهمة.
- Android APK الحالي room-only وليس `2–4 players original world`. المحرك Windows الخاص لا يزال يخرج عبر Dashboard ERROR قبل القائمة؛ اختبار الفئة الدقيقة [45202073768](https://github.com/alwansan/BO/actions/runs/45202073768) منتظر عند مراجعة Session108.

## الخطوة القادمة

استخراج فئة Dashboard error الدقيقة دون تسريب launch path/titleid/guest stack، ثم إصلاح native boot على أساس الدليل؛ تفعيل actor2–4 وسلاحهم وتحريكهم وفيزيائهم وAI ومزامنة host original NPC/mission + guest original state على خيط اللعبة الحقيقي؛ وبعد نجاح التكامل فقط تجربة كبيرة على هاتفين أو أكثر. لا تعديل `main` أو `CL` أو `biso` أو محتويات المالك الأصلية.

### Session108 — إعداد اختبار الدمج الأصلي داخل Android

- فحص مصدر Android الأصل المثبت `RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806` أظهر أنه يستخدم SDK مختلفاً عن مختبر Windows: `TGP482/XboxXBERecomp` مع `runtime/kernel/memory.h` و`runtime/cpu/x86_context.h` و`runtime/kernel/kernel.h`. تمت مراجعة واجهات `mem::AllocSystem/AllocationSize/FreeSystem/IsCommitted` و`kernel::CallGuest` من المصدر العام، لكن **مطابقة API ليست دليلاً على صحة آلية إنشاء actor داخل Android**.
- Commit [10e546bf](https://github.com/alwansan/BO/commit/10e546bf2b978b00d086ed7554c71081624dc023) يضيف ملفات `room_original_host_proxy_engine.hpp` و`room_original_host_proxy_kernel.hpp` و`room_original_host_player_storage.hpp` و`room_original_host_player_storage_kernel.hpp` إلى مرحلة `prepare_room_network.py`، ويضيف `#include` ضمن `src/game/cheats.cpp` الخاص باللعبة الحقيقية **للتجميع فقط دون إنشاء أي actor أو استعمال CallGuest إضافي**.
- اختبار مصدر Android [45205630356](https://github.com/alwansan/BO/actions/runs/45205630356) **SUCCESS**؛ اختبار بناء APK الأصلي [45205630319](https://github.com/alwansan/BO/actions/runs/45205630319) كان قيد التنفيذ عند آخر قراءة. لا تعلن نجاح native C++ Android قبل اكتمال `candidate-probe`.
- مواصفات الأمان: ليس مسموحاً استخدام base kernel IO الذي يرفض `OwnsActor` أو إنشاء `OriginalProxySceneLedger` جديد للخروج من الحجر، ولا تفعيل الإنشاء بدون actor setup/scene lifetime/physics+AI proven. مصدر Android الأساسي باقٍ دون تعديل.


### Session108 Android candidate validation and exact allowlist recovery

- Original Android full-build [45205630319](https://github.com/alwansan/BO/actions/runs/45205630319) **FAILED before native compilation** at the strict `reference/src/coop` file-name equality check. All Python source transformations/tests passed; error was **four newly staged reviewed headers missing from the workflow's strict allowlist**. This is not an NDK C++ compile error or game runtime result.
- Fixed strictly in [242fa976](https://github.com/alwansan/BO/commit/242fa976716961f2c2eacb459de64ca9bba0229c): add EXACTLY the four reviewed original host proxy/storage header names to `.github/workflows/black-coop-room-network-apk.yml` allowlist; preserve existing native gameplay file allowlist and fail-closed policy. Retest [45206563984](https://github.com/alwansan/BO/actions/runs/45206563984) IN_PROGRESS on initial check.
- Verified **pinned** Android SDK `TGP482/XboxXBERecomp@99078b119b5d5e0d1d6afb1390e29437c0c984c0` contains `mem::AllocSystem,FreeSystem,AllocationSize,IsCommitted` and `kernel::CallGuest` declarations. Actual Android compiler still must pass. No game actors initialized, no shared-world gameplay.
