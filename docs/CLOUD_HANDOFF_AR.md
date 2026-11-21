# بدء العمل من جلسة سحابية — 2026-11-22

المستودع https://github.com/alwansan/BO والفرع experiment/black-coop. ابدأ من prompt/RESTORE_SESSION_AR.md وجلسة37 ثم36 وSTATE والخطة. جميع أدوات المصدر التالية موجودة فيGitHub،ولا تحتاج ملفات الجهاز المحلي أوحزمPythonالمترجمة:

| المهمة | الملفات |
|---|---|
| الشخصية الأصلية فقط | coop/default_player/،coop/prepare_room_player.py،coop/room_player_prepare_checks.py |
| اتصال الغرفة وpresence وTLS | coop/lobby/java/ وcoop/lobby/android/ وcoop/lobby/test/ وcoop/prepare_room_network.py |
| رصدpool/instance السابق | coop/probe/ وcoop/probe3/ وcoop/probe4/ وأدواتprepare/verify الخاصةبها |
| مصنعNPC،بحث محفوظ وليس اختبار المالك التالي | coop/probe5/،coop/prepare_room_factory.py،coop/verify_factory_hook.py |
| قراءة صورة المالك وتحليل السجلات والموارد | coop/analysis/owner_xbe_static_slices.py وmodel_inventory.py واختبارات وتحليلاتcoop/analysis/ |
| بناء ومراجعة تلقائية | .github/workflows/black-coop-room-player-apk.yml وroom-network وlobby-android-platform وreal-lobby-core وmodel-metadata |
| خطط وحالة ومراجع | docs/superpowers/plans/،memory/،docs/SOURCE_MAP_AR.md،docs/REFERENCE_BLACK_PC_COOP_AR.md |

آخرAPKالمطلوبة https://github.com/alwansan/BO/actions/runs/44946597973/artifacts/14724535514 ؛نتيجةالتحقق memory/evidence/DEFAULT-PLAYER-APK-44946597973.json. إذاانتهتصلاحيةartifactلاحقاًيمكنإعادةالبناءبنفسworkflowوالـpins،لا تطلبملفاتاللعبةمجدداً. لا تعِدالبناءلمجردتغييرالجلسة.

مكان رفع المالك للصوروالسجلات: https://github.com/alwansan/CL/tree/main/black%20log ؛صورفيimg وسجلاتالهاتفالثانيفيمجلد«من الهاتف الثاني». افحصشجرةCLالحيةوأحدثالملفاتواقرأالصورفعلياً؛لا تكتفِبأسمائها. آخرصور22:47تثبت2/4بالنسخةالقديمةفقط. رابطجلسةChatGPTالمشتركقدلايفتح؛لاتدّعقراءةمحتواهإنلميفتح،واعتمدالكودوالأدلةالمثبتة.

مصدرالمحتوىالمفكوك https://github.com/alwansan/biso ،والمصدرالمرجعيالجديد https://github.com/portingpete/Black-2006-Recomp ،والقديم https://github.com/Peterson047/BLACK-XBEMultiplayer . اقرأوثائقالترخيصوالـpinsقبلأي نقل.

لمتُرفعالملفاتالمؤقتةالتيتحويdisassemblyكاملاًأوصوراًأوبياناتلعبةأومفاتيحاختبار؛هذهليستأدواتمصدرمطلوبة. أدواتإعادةإنتاجالتحليلواختباراتهامرفوعة. حافظعلىأصولاللعبةوالحفظوخاصةmain. لا تعتبرlocalcacheأو__pycache__ذاكرةللمشروع.
