# NET-004-Prep — حاجز الشبكة وخيط اللعبة، معزول عن BLACK
التاريخ 21 نوفمبر 2026. **لا يضيف multiplayer أو شخصية ثانية إلى اللعبة**. يستخدم `coop/bridge/scene_inbox.hpp` + اختبار C++20 مستقل، دون game headers أو JNI أو hooks أو network sockets.

## الأساس
- خيط الشبكة قد يقرأ بيانات UDP فقط ولا يحق له تعديل ذاكرة الضيف (guest) أو استدعاء محرك اللعبة.
- قبل دخول أي حدث للمحرك: validate النوع وحجم الحقول وplayer[2..4] وscene epoch وsequence غير صفر. بيانات الحركة محدودة إلى [-1000,+1000]؛ لا guest pointers.
- خيط اللعبة وحده يربط inbox بـscene epoch، ثم يستنزف على الأكثر16 رسالة في النداء عبر try-lock وcallback **خارج mutex**. أية thread أخرى لا تستطيع drain أو إعادة bind.
- السعة 64 رسالة فقط؛ عند الامتلاء تُرفض الرسالة الجديدة. عند تغيّر scene تُسقط الرسائل القديمة وتُصفّر sequence tracking. هذا نموذج bounded queue، لا reliable transport أو حل congestion.
- duplicate/out-of-order seq داخل نفس peer+scene لا تُطبّق مرتين؛ يوجد خطر wrap-around للرقم32، لذلك يبقى نموذجاً اختباريّاً لا بروتوكول إنتاج.
- `PeerLeave` حدث بيانات، لا يعني أن اللاعب أو مجسمه حُذف من عالم BLACK. لا يجوز استدعاء factory/destructor أو تعديل actor قبل إثبات دورة الحياة على الهاتف من الرصد.

## شروط عدم الادعاء
وجود هذا الكود أو نجاح ASAN/UBSAN لا يثبت أن هناك خيط لعبة واحداً أو إمكانية إنشاء/تحريك actor، ولا أن الرسائل مصادقة أو مشفرة أو متصلة بالهاتف. ربطه بالمحرك مؤجل لحين نتائج Probe runtime وACTOR-001. اختبارات IPv4 السابقة مستقلة.

## CI
`.github/workflows/black-coop-game-thread-bridge.yml` يتحقق من bounds ومالك الخيط و64 رسالة و16 لكل batch وتغيير scene، من دون ISO أو APK. عند وصول نتيجة GitHub Actions أضف رقم run إلى الذاكرة ورسالة الاستعادة.

## NET004-PREP composition test (2026-11-21)
- Code commit a5d164ca6f5f19bb807851429c7ba294a5d572b4 wires two previously isolated *test components* under coop/bridge/test_transport_pipeline.cpp. A deterministic host validates Join/Accept and Input token/endpoint/sequence; a transport worker stages value-only commands into SceneInbox; the owning simulated game thread drains them into a mock scene. Negative tests cover forged endpoint/token, duplicate replay, wrong drain thread, and scene reload dropping stale input. No actual network socket is opened by this composition test (separate Linux UDP socket tests exist), no BLACK hooks/actor memory access/gameplay, no authentication or phone test.
- [44856542339](https://github.com/alwansan/BO/actions/runs/44856542339) completed SUCCESS with ASAN/UBSAN and strict C++20; input path tested synthetically only. Keep wording «mock host→scene pipeline», not co-op, and do not treat a pseudorandom lab token as secure authentication.
