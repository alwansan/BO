# جلسة28 — تنفيذ CO-OP تحت الإعدادات وتوضيح الاختبار الفردي

## طلب المالك الجديد
الاختبارات السابقة على هاتف واحد فقط: اللاعب المحلي والبوتات الأعداء/الأصدقاء؛لا اختبار انضمام هاتف ثانٍ. المطلوب أجسام مستقلة للاعبين الحقيقيين،يتولى المضيف اختيار المهمة،والجميع ينتقلون معه بين المراحل. المحافظة على البوتات القائمة وتجنب تعديلها دون حاجة؛لا نفترض أن AI سيتعرف تلقائياً على اللاعبين الجدد. هذه متطلبات وليست ميزات مثبتة.

المالك أذن بفرع تجريبي من alwansan/biso وتعديلات محتوى ISO عند الحاجة وإعادة تجميعها عبر Actions. ذلك يوسع الإذن السابق بالقراءة فقط لـbiso،ولا يسمح بتغيير main. لم يُنشأ فرع هناك في هذه الدفعة لأن القائمة يمكن إضافتها من APK دون تغيير ISO؛لا صورة معدلة جديدة.

## مراجع قرئت
رأيت فعلياً صورتي CL black log/img (blob7c6879fb و16cc66d0):قائمة المهمة Start/Options/Quit،وقائمة الملف Continue/Options/Profile/Quit. CO-OP يذهب مباشرة بعدOptions؛في الثانية تتحركProfile/Quit للأسفل. لا تُنسخ الصور الشخصية للمستودع.
رابط chatgpt.com/share/6b01a380-3200-83ed-bb9d-173afec493ae تعذر فتحه (cache miss)؛لا ادعاء قراءة محتواه. مرجعPeterson مازال0ee3ae6411c7f0989c62cbe36b224d3f55043923،binary-onlyWindowsTDM،ولايُنسخbody.bca. bisoمازال9ceb6a65؛قرئ restore_github_upload.py وسكربت إعادة التجميع عبرextract-xiso؛إعادةالتجميع لاتضيف multiplayer بذاتها.

## التنفيذ
prepare_room_ui.py يبني فوقProbe4 في staging مؤقت مع فحص كلanchor قبل الكتابة. يعدلexit_menu/options_menu ويضيفroom_ui.h وroom_route.hpp. نسخclipCO-OP داخلFlashوربطحلقةالتحديد يحافظعلىQuit/confirm؛OptionsMenuXbox يُفتح مباشرة فيmode6 ويعيدBack إلىنفسtitle. route أحادي الاستعمال ويلغي الانتقال الملغى أو الصفحات الأخرى. حزمةorg.bocoop.black.roomui بعلامةBLACK Co-op UI TEST،لاINTERNET،HOST/JOINيبقيانNOT AVAILABLE. لاتسمّهاco-opقابلللعب.

## التحقق
رُصدRED لاختبارrouting قبلوجودheader ثمPASS؛اختبارpreparerقبلوجودmoduleRED ثم12/12اختباراتpreparerPASS علىالمصدرالمثبت،و12/12rootPythonPASS. مراجعةمستقلةللكودالناتجلمتجدfindingمادياً؛بناءNDKوقبولالهاتفمازالامعلقينعندحفظهذهالدفعة.

التالي:انتظاروفحصActionsالخاصبالواجهة،ثممواصلةnativeactor/controlleracquisitionمن1AF880قبلربطgameplaynetwork. لاrequestلاختبارقراءةProbe4مجدداً،ولاإيهامأنقائمةأواكتشافUDPيعنيوجودأجساملاعبين.

## بدء Actions
الكودbf9a5c8199f28983865db20016cd8bb737cb13b5؛run44913282838.نجحLinuxjob136873434926وقرئالسجل(ASAN/UBSAN+route+10poolanalyzertests)،Windowsjob136873477193مازالقيدالتنفيذ. لاAPKجاهزمثبتبعد.

## فشل Windows وإصلاح مثبت
run44913282838فشلjob136873477193فيpreparerقبلcompile:ملفexit_menu.cppمنGitWindowsبصيغةCRLF،بينماالـanchorمتعددالأسطرLF. قرئالسجلواستُعيدالفشلtest_windows_crlf_source_is_supported_without_changing_original_assetsمحلياًREDبنفسValueError. توحيدCRLF→LFلنصالمصدرقبلتطبيقanchorsيعيد13/13preparerPASSوروتينroutePASS. لاgameassetتغيرولاgateضعف. إعادةالبناءمطلوبةعلىالإصلاح.

## تدقيق الجسم أثناء إعادة البناء
localdraw1D25D0يستخدم+420/+460..490خارجNPCstride3C0؛لايمكنتركيبdrawالمحليعلىNPCأوادعاءfullbodyمنProbe4. راجعdocs/ACTOR_INSTANCE_OWNERSHIP_AR.md. الحوض324منفصل16عنصرstride1FE0؛pop1848F0لايفحصnullويحتاجprecheck،والـdescriptorوملكيةمراجعAIمازالاتحتاجتدقيقاً. لامحركspawnمطبقفيهذهالدفعة.

إعادةالبناء44913654405علىالإصلاح96f69b6c8e912760fedaf5d7add52928f3b212f7؛Linux136874536833نجح،Windows136874579612قيدالتنفيذ. مراجعةمستقلةأكدتاختلافdrawوكتابةlocalحتى+49F،وحوض324عدد16؛القفزبرسمlocalإلىNPCغيرآمن.
