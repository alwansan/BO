# BLACK Co-op — زر الغرفة مباشرة تحت «الإعدادات»

## حالة التنفيذ — جلسة28
نُفذالمسارفي`coop/prepare_room_ui.py`والخطة`docs/superpowers/plans/2026-11-21-title-coop-entry.md`،source `bf9a5c8199f28983865db20016cd8bb737cb13b5`،Actions44913282838فشلقبلcompileبسببCRLFWindows؛أُصلحبـregressionRED→GREENو13/13preparerPASS. إعادةالبناءمطلوبة. الصوررُئيتفعلياًمنCL:فيblacktitlemenu2الحلقةOptions→CO-OP→Quit؛وفيblacktitlemenuOptions→CO-OP→Profile→Quit. ينقلكCO-OPمباشرةإلىmode6فيOptionsMenuXbox،ويرجعBackإلىtitleالأصلية. لاISOمعدلةلهذهالإضافة. HOST/JOINغيرمتاحينبعد؛لاادعاءnetwork/playableco-op. تفاصيلالأقسامالتاليةهيخلفيةالبحثوالتصميمالسابق؛حالةالتنفيذهناهيالأحدث.

**التاريخ:** 2026-11-21. **المرجع المقروء:** `RanggaCasper/Black-Android@f3754d6985a8c35204e7df071546e586b10f1806`، خصوصاً `src/game/exit_menu.cpp`, `src/game/flash.h`, `src/game/options_menu.cpp`. المرجع للقراءة فقط؛ لا تغيير في main أو binary الأصل.

## الاكتشاف الأساسي
- القائمة الظاهرة في صورة المالك هي `blacktitlemenu2` وفيها `startButton` و`optionsButton`. مصدر `exit_menu.cpp` يعرّف `TitleMenu{"blacktitlemenu2", "options_text", "startButton", "optionsButton"}`.
- الزر `QuitButton` الموجود تحت الإعدادات في نسخة Android **أضيف مسبقاً برمجياً** إلى Flash title menu. تنفذ `CopyButton()` نسخة من مقطع `options_text` باستعمال `duplicateMovieClip`، ثم `Follow()` يضعه على بعد `kPitch=25`، ثم `JoinRing()` يربط `optionsButton -> QuitButton -> startButton` لتعمل الأسهم والتحديد. هذا هو دليل عملي على إمكانية إضافة زر آخر بالطريقة نفسها؛ ليس بعد إثباتاً أن زر Co-op موجود في النسخة الحالية.
- `src/game/options_menu.cpp` يملك صفحة `CO-OP / ROOM` السادسة في حزمة Probe2 الحالية، لكنها لا تظهر تلقائياً في قائمة `blacktitlemenu2` لأنها **داخل** OPTIONS. لهذا صور القائمة الرئيسية الجديدة لا تُظهر CO-OP تحت الإعدادات. صورة صفحة OPTIONS ذاتها لم تصل بعد.
- تشغيل `ExitMenu` يحدث في tick تابع لواجهة اللعبة (ليس من network thread). يجب الحفاظ على `QuitButton` و`ConfirmWarning` والصوت/العودة الأصلية.

## خطة تنفيذ آمنة لاحقاً — Probe3 معزولة
1. Duplicate لـ`CoopButton` من نفس مقطع options المرئي، مع اسم وعمق مستقلين، وتحديد موضعه **بعد** `optionsButton` و**قبل** `QuitButton` في الحلقة. يجب ضبط موضع Quit لأسفل بضعة وحدات حتى لا يتراكب النص، دون تعديل صفحات المهمة أو الحفظ.
2. تسجيل نص `CO-OP` بطريقة مطابقة لربط النص الديناميكي `Show` و`OptionsText` في `options_menu.cpp`؛ **لا يُفترض** أن `$FE_COOP` ترجمة موجودة. لا نستخدم متغير Flash مجهول أو Hook جديد بلا دليل.
3. اختيار CO-OP يجب أن يفتح إعدادات الغرفة الموجودة **بقراءة فقط**، لا إنشاء HOST/JOIN حقيقي. إن اقتضى توجيهاً مباشراً للصفحة السادسة، يكون flag أحادي الاستعمال على thread واجهة Flash مع إعادة ضبط عند back/reload، وليس شبكة أو كتابة actor.
4. فحوص مصدر fail-closed: نسخ clip موجود، عدم تكرار الإنشاء، حفظ ring زر الخروج، عدم تداخل focus/back أو `ConfirmWarning`. اختبار مصدر مثبت v1.5.0 وبناء Android ARM64 مستقل في حزمة جديدة، **لا يحل محل Probe2 التي نجح تصديرها**.
5. بوابة قبول الجهاز: لقطة Main menu فيها `ابدأ المهمة، الإعدادات، CO-OP، خروج`، ثم دخول الغرفة والخروج منها، واختبار بقاء Start mission والطور الفردي يعملان. حتى ذلك الوقت، top-level menu = **لم تُنفّذ**.

## صلة ACTOR-001
واجهة الغرفة تُعرض فقط حالة OFFLINE/NOT AVAILABLE. لا تربط start/join مع UDP قبل إثبات lifecycle/factory وملكية اللاعب الثاني من أثر قراءة حقيقي. لا تستخدم shared renderer pointer كعنوان instance.
