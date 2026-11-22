# BLACK Co-op — شاهد تصنيف Vertex Shader الأصلي (بدون بيانات لعبة)
**جلسة 46 — 2026-11-22.** Branch: `alwansan/BO:experiment/black-coop` فقط.

## لماذا أُضيف؟
فحص `stride36` من أصول المالك بيّن أن البيانات عند `+12` كثيراً ما تجمع أربعة bytes إلى 255، وأن `+20` يحتوي بكثافة على bytes صغيرة. وهذه **قرائن لا تثبت الأوزان/العظام**. لنعرف نوع المحتويات كما يفسرها Xbox D3D، نحتاج مراقبة **VertexShader declaration** الأصلية، لا الاكتفاء بقراءة bytes.

في `RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806` ينشئ `src/d3d/d3d8.cpp` الـVertexShader، ويستدعي `gpu::ParseDeclaration(*vs)`. وفي SDK المثبت `TGP482/XboxXBERecomp@99078b119b5d5e0d1d6afb1390e29437c0c984c0` `runtime/gpu/state.h` توجد `VertexShader::inputs[16]` مع `stream/offset/type` و`stream_stride[16]`. هذه بنية مسار المحرك الحقيقي، ولا تحتاج فحص XBE أو أصول اللعبة raw من الهاتف.

## التنفيذ الفعلي
1. **عقد C++ مستقل قابل للاختبار:** `coop/room_network/gpu_decl_contract.hpp` — `Classify` يقبل مصفوفة أنواع/إزاحات streams صناعية، ويعرض **ست فئات ثابتة فقط**. `gpu_probe::Once` يمنع تكرار الرسالة بأمان باستخدام atomic.
2. **Adapter للعنوان الأصلي:** `coop/room_network/gpu_decl_witness.hpp` — ينسخ فقط `stream,offset,type,stride` من `gpu::VertexShader` وقت التشغيل، يصنفه، ثم يرسل كوداً ثابتاً نادراً عبر `SendSafeWorldCodeToAndroid` إلى `RoomDiagnostics.event`. لا يصدر shader instructions أو program IDs أو أسماء مواد أو قيم vertex/bone أو ذاكرة اللعبة.
3. **تجهيز مصدر Android المؤقت فقط:** `coop/prepare_room_network.py` يضيف include/observer بعد النداء الأصلي `gpu::ParseDeclaration(*vs)` في `src/d3d/d3d8.cpp` دون تعديل سلوك الرسم أو return value أو ملكية game actor. يسمح جهاز Android الحالي بكود واحد لكل فئة في Activity `coopRecordWorldTick` (الاسم قديم ويستقبل أيضاً GPU codes)، مع بقية رموز world tick الأصلية.
4. **الحماية:** المصدر الرئيسي للعبة `RanggaCasper/Black-Android` لا يتغير؛ تعديلات BO تبقى داخل `experiment/black-coop`، وWindows CI يرفض أي تعديلات native جديدة عدا `src/d3d/d3d8.cpp` والرؤوس `gpu_decl_contract.hpp`/`gpu_decl_witness.hpp` المحددة بالاسم. لا توجد اتصالات جديدة أو guest writes أو actor writes أو حركة NPC أو Scene READY.
5. **الفحوص الصناعية:** `coop/room_network/test_gpu_decl_contract.cpp` مع `.github/workflows/black-coop-world-tick-contract.yml` باستخدام ASAN+UBSAN. CI `44966575850 SUCCESS`، وفيه تطابق offsets والأصناف، رفض stride آخر، وفحص `Once` في خيطين. مصدر تجهيز room network CI `44966623833 SUCCESS` يراجع code hooks والخصوصية. بناء Android الكامل `44966638815` **يجب التحقق من نتيجته الحية** (كان in_progress/queued عند إنشاء التقرير).

## سجل الفئات الثابتة فقط
| Code | RoomDiagnostics.event | معنى الشاهد |
|---|---|---|
| 4 | `GPU_V36_DECL_CANDIDATE` | رُصد إعلان يتضمن stream مُعرّف بارتفاع 36 بايت |
| 5 | `GPU_V36_SLOT12_PACKED_DECL` | Input عند offset12 من نوع `0x40/0x44/0x34` (byte-packed type) |
| 6 | `GPU_V36_SLOT16_PACKED_DECL` | مثلها عند offset16 |
| 7 | `GPU_V36_SLOT20_PACKED_DECL` | مثلها عند offset20 |
| 8 | `GPU_V36_SLOT24_NORMPACK_DECL` | Input عند offset24 من نوع NORMPACKED3 `0x16` |
| 9 | `GPU_V36_SLOT28_FLOAT2_DECL` | Input عند offset28 من نوع FLOAT2 `0x22` |

> كل إشعار كود آمن يختلف عن إظهار سلاسل byte وعناوين guest. تعرّف `0x40,0x44,0x34,0x16,0x22` عام لمكتبة GPU المثبتة، **ولا يثبت أن نموذج BC1_MAT أو BC1_TOM بالذات مر بهذا الإعلان**. التوقيت الحالي وقت *إنشاء* shader، لا وقت draw أو اختيار actor، لذلك في المراحل اللاحقة يلزم إثبات ارتباط declaration مع draw باستخدام MODEL resource قبل اعتبار semantics حقيقية.

## سياسة الاختبار ودرجة الثقة
- CI يثبت أن Observer **يمكن تجهيزه وترجمته** في APK فقط بعد نجاح بناء Android الجديد؛ نجاح C++ الصناعي لا يثبت صدور الأكواد في هاتف المالك.
- لم يطلب من المالك تثبيت APK للغرف لهذا التفصيل. **فقط عند اكتمال قصة مضيف مشتركة ومجسم حليف بشري عن بعد ونتيجة كبيرة قابلة للرؤية**، سيُضم هذا الشاهد إلى الـlogs الآمنة التلقائية مع بقية مؤشرات mission/scene/roster/actor/input/performance في تجربة واحدة من الهاتفين.
- حظر استنتاج `STORY_READY` أو جاهزية Scene من هذه الشيفرة. لا لعبة شبكة ولا رندر بشري ناتج مباشرة من هذا التحديث.

## نتائج مرتبطة
- `docs/OWNER_VERTEX36_SKIN_RESEARCH_AR.md`: فحص أصول المالك `44965837041 SUCCESS` 38,917 رأساً مشاراً إليه و6 word lanes (إحصائيات فقط).
- `coop/bridge/synthetic_ally_skin_contract.hpp`: حسابات CPU رياضية صناعية بحد أقصى 64 joint، `44966105790 SUCCESS` ASAN/UBSAN، وليس فكّ Skinning أصلياً.
- `memory/sessions/2026-11-22_bo-coop-46-stride36-lanes-synthetic-ally-skin.md`, `prompt/RESTORE_SESSION_AR.md`, `memory/STATE.json`: مصدر الاستعادة المستمر.
