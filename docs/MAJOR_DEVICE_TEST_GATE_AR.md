## Session43 event witness prepared for ONE future substantial phone acceptance (not now)
The staged Android room-only source includes three **automatically collected bounded fixed codes**, using existing original BLACK source wrappers with no guest writes:
`WORLD_MISSION_LAUNCH_ATTEMPT` (max eight), `WORLD_LOCAL_PLAYER_UPDATE_SEEN`, `WORLD_POINTER_CHANGED_UNVERIFIED`. They are emitted from original mission attempt/player update callbacks through JNI to `RoomDiagnostics.event`, which uses a bounded async disk worker. They contain NO mission ID, world pointer, character coordinates, IP, name, PIN, raw XBE or scene. **NONE of these alone proves actual stage loaded or authorizes STORY_READY**. This enables correlating future major user sessions' mission request and first local gameplay update without asking the owner to test another small button or manual log. C++ synthetic guard CI44962215874 and source staging CI44962275672 both PASS; full Android APK build for new native wrappers must be separately confirmed, currently CI44962611024 check live. No install request until actual host world and human remote AllyBody are implemented and CI verified.

---

## CI achievement: safe combined log export verified without owner participation (2026-11-22)
Run `44957104337` Android29 **SUCCESS** with `ROOM_LOG_ROTATION_PASS`: automatically generated 900 synthetic safe events; previous+current rotated room event log exported once through MediaStore; began/ended markers found; tampered previous log refused. TLS/Keystore and rehost also PASS. This validates ROOM transport diagnostics, not the FUTURE real BLACK scene/actor/combat/world instrumentation. Do not ask owner to test until the actual substantial shared-world milestone is ready.

---

# BO-COOP — سياسة تقليل تجارب المالك وتجربة القبول الموحّدة (2026-11-22)

## توجيه المالك الملزم
يفضّل المالك الاعتماد على GitHub Actions والمحاكيات والاختبارات الصناعية وتحليل المصدر إلى أقصى حد. **لا تُطلب منه تجربة هاتف إلا عند إنجاز مرحلة تكامل كبيرة يصعب إثباتها من CI**. كل التفاصيل الصغيرة التي قد نحتاجها تُجمع تلقائياً في logs آمنة ومحدودة أثناء **تجربة قبول واحدة**، بدل الطلب المتكرر لتجربة زر أو شاشة أو إشارة مفردة. لا تُكرر طلب ISO أو التجربة المؤكدة لنموذج اليدين. لا يُطلب تثبيت APK الغرفة وحدها.

## البوابة الإلزامية قبل طلب تجربة هاتف
1. تأكيد تطابق المصدر مع صورة المالك c5 ومعمارية التطبيق، وخلو البناء من أعطال CI، ونجاح اختبارات Java TLS/Android KeyStore والمصدر C++ تحت ASAN/UBSAN.
2. وجود رابط رسمي موحّد لحزمة Android حديثة **على الهاتفين نفس البروتوكول**، مع هوية commit وبصمة APK وتوقيع وسبب تجربة ذي قيمة.
3. مسار قصة مضيف حقيقي في محرك BLACK، اختيار مهمة 0..7 من اللعبة الأصلية، وتحميل scene فعلي للضيف والمضيف، وعدم المزج بين عالمين فرديين. يجب إثبات game-thread scene epoch قبل إرسال READY، لا الاكتفاء باللوبي.
4. على الأقل تجربة مفهوم مفيدة للمشاهد الآخر: مجسم حليف كامل مملوك للاعب البشري البعيد، مع تحكمه بالشبكة، من دون brain NPC أو نسخ ممثل حي. المحافظة على اليدين والكاميرا والسلاح الأصليين على كل هاتف.
5. تشخيصات تلقائية مدمجة تكفي للإجابة عن الأسئلة الصغيرة، ومفحوصة صناعياً دون البيانات الخاصة.

**الحالة الآن: البوابة 3 و4 و5 غير مكتملة. لا حاجة لأي تجربة من المالك.**

## حزمة الأدلة الموحدة المطلوبة قبل تجربة القبول
الحقول الآمنة العامة (كلها رموز أحداث وأرقام صغيرة دون أسماء/هوية/عناوين):
- build/profile/protocol/schema ونسخة البرنامج ونتيجة فحص الأصول محلياً (bool دون معلومات الأصل).
- حالات host/scan/auth/roster/rehost/leave ومقاييس الأوقات والمدد الإجمالية وقطع الاتصال.
- مرحلة القصة: HOST_CHOICE, STORY_PREPARE, SCENE_LOAD_BEGIN, SCENE_LOADED_VERIFIED, READY, COMMIT, ABORT وسبب الرفض *فقط كرمز*، مع رقم المهمة 0..7، هوية المشهد المحلية غير الحساسة/عداد epoch، وعدد البشر 1..4.
- مكان اللاعب البعيد المرئي: حالة إنشاء/حذف renderer والتبعية للإنسان ووجود rig/bones/material/animation بحقول منطقية/عدادات؛ دون أسماء مفاصل أو بيانات مجسم.
- مقاييس حركة/إدخال/سلاح/AI/ضرر/تقدم هدف المرحلة مع عدادات وعدد الرسائل المرفوضة، لا مواقع أو لقطات خام.
- مستوى الأداء والتحمل: حدود frames, ping/jitter/loss، انقطاعات، أخطاء JNI/Crash/ANR على شكل تصنيفات، وليس ملفات مستخدمين.
- الالتزام بالحجم: حد/تدوير السجل، تجميع معدل الأحداث المتكررة، عدم الكتابة كل frame أو على خيط اللعبة؛ مستخرج واحد منظّم عند نهاية الاختبار، من كل هاتف، بنفس الجلسة.

لا تسجّل PIN أو IP أو fingerprint أو TLS keys أو أسماء أو رمز غرفة سري أو raw save أو ISO/XBE أو بنية خاصة باللاعب. لا تنشر سجلات خام قد تتضمن هذه البيانات في Actions/artifacts.

## تجربة القبول الموحدة المستقبلية (ليست تعليمات للمستخدم الآن)
عندما يصبح تكامل القصة وRemoteAlly صالحاً للاختبار، تُقدّم نسخة واحدة للهاتفين مع خطة واحدة: المضيف يفتح الغرفة، الضيف ينضم، المضيف يختار مهمة من القصة ويبدأها، الطرفان يرون العالم نفسه والشخصية البعيدة ويختبران الحركة/السلاح/التفاعل وتقدم الهدف، ثم يختبران خروج/إعادة اتصال واحدة آمنة عند الحاجة. بعد انتهاء التجربة يصدران **حزمة أدلة واحدة لكل هاتف** من داخل التطبيق، تُجمع في CL مع الصور المهمة عند وجود فشل بصري. لا تُطلب إعادة التجربة من أجل تفاصيل صغيرة؛ يحلل العميل سجلات التجربة السابقة ويعالجها أولاً.

## دليل التقدم الحالي
- ما ثبت: واجهات اللوبي واتصال 2/4 على هاتفين بإصدار سابق، بروتوكول TLS v3 ومراحل PREPARE/READY/COMMIT واختبارات إعادة الاستضافة الصناعية، واختبار RoomSession facade.
- ما لم يثبت: hook تحميل المشهد الحقيقي على c5، عالم مضيف واحد، جسم حليف مستقل للاعب البعيد، مزامنة القتال والقصة، اختبار v3 على الهاتفين.
- أعمال Actions القادمة: فحص ثابت لمرشحي الدخول إلى المهمة من الصورة المملوكة للمالك داخل Runner من دون تصدير أصولها، مع Tests وتقارير آمنة فقط.

اقرأ دائماً AGENTS.md وprompt/RESTORE_SESSION_AR.md وmemory/STATE.json والخطة الحديثة قبل اقتراح أي تجربة هاتف.