# NET-003 — مختبر C++ وشبكة UDP للهاتف (21 نوفمبر 2026)
الفرع experiment/black-coop فقط. لا تعديل للعبة BLACK ولا إضافة Multiplayer داخل اللعبة بعد.

## النتيجة المثبتة
- كود C++20 أصلي في `coop/native/net_core.hpp`: ترميز صارم لحزم JOIN/ACCEPT/INPUT/SNAPSHOT/REJECT، المضيف صاحب القرار، 3 عملاء + هوست وهمي (4 لاعبين منطقياً)، مهلة 1500ms، رفض المكرر والقديم والمزيف، حد للغرفة وللقيم.
- `coop/native/test_net.cpp`: اختبارات بروتوكول/حالات وعملية UDP فعلية بين مضيف وعميلين على Linux loopback. ASAN/UBSAN compile and run.
- `coop/native/udp_cli.cpp`: برنامج CLI يعمل على IPv4 UDP، وضع host/join، 10 حركات مصطنعة للعميل و10 snapshots تقريباً بحسب الاتصال. **لا ينفّذ لعبة BLACK، لا فيزياء ولا إطلاق نار**.
- [CI 44833884316](https://github.com/alwansan/BO/actions/runs/44833884316) PASS للمنطق والتحقق.
- [CI 44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) PASS: Linux host/client process smoke + Android NDK 28.2.13676358 target API29 ARM64 cross-compile. لم تُختبر الأداة على هاتف فعلي.
- Artifact 14677732935، الاسم `black-coop-net-lab-android-arm64-NO-GAME`، ملف التنفيذ `black-coop-net-lab-aarch64`، SHA256 `0ca758a2ae4ac13a322cfa5106ff49ab58f753cb5758d43e7d9a296a2ccbe100`.
- فشلان سابقان للتجارب 44833697806/44833772092 سببهما **توقعات اختبار غير صحيحة** وتم تصحيح الاختبار بلا تخفيف تحقق الكود. لا حذف سجلات فشل.
- التفاصيل: `memory/evidence/NET-003-cpp-android-44834027711.json`.

## تجربة من هاتفين (اختيارية، عند المالك)
1. افتح [Run 44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) ثم Artifacts وحمّل `black-coop-net-lab-android-arm64-NO-GAME`. استخرج الملف التنفيذي فقط.
2. ثبّت Termux، ثم انقل الملف من Download إلى مجلد داخل HOME لأن التخزين المشترك قد لا يسمح بالتنفيذ:
```sh
pkg install -y coreutils
mkdir -p ~/bo-coop
cp "/storage/emulated/0/Download/black-coop-net-lab-aarch64" ~/bo-coop/
chmod 700 ~/bo-coop/black-coop-net-lab-aarch64
sha256sum ~/bo-coop/black-coop-net-lab-aarch64
```
إذا كان الملف داخل مجلد ZIP مستخرج، غيّر مسار `cp` حسب المكان.
3. الهاتف المضيف، بعد اتصال الجهازين بنفس Wi-Fi أو بنفس شبكة ZeroTier الخاصة المصرح بها:
```sh
termux-wake-lock
~/bo-coop/black-coop-net-lab-aarch64 host 0.0.0.0 24872
```
4. الهاتف الثاني:
```sh
~/bo-coop/black-coop-net-lab-aarch64 join IP_OF_HOST 24872
```
استخدم IPv4 ZeroTier للمضيف إذا كانا على شبكتين مختلفتين، أو عنوان LAN عند نفس Wi-Fi. `IP_OF_HOST` يستبدل بالعنوان الحقيقي؛ لا تنسخ الحروف نفسها. يمكن فتح Termux في هاتفين مستقلين أو تجريب المضيف والعميل في عمليتين داخل الهاتف على `127.0.0.1`.
5. المتوقع نجاح الاتصال: `Connected as simulated player 2` و`snapshot tick=...`، وليس ظهور لاعبين داخل اللعبة. احفظ نص المخرجات إن احتجنا إصلاح مشكلة جهاز.

## التحذيرات/الحدود
- هذا **مختبر غير مشفر وغير موثق الهوية**: token تجريبي ليس مصادقة آمنة، لا تستخدم اتصال IP عام ولا تفتح/توجّه منفذ 24872 على الراوتر إلى الإنترنت العام، ولا تنشر حزم أو كلمات مرور ZeroTier.
- ZeroTier عادةً يعتمد واجهة VPN على Android؛ يجب أن يكون الجهازان مصرحاً بهما على نفس الشبكة مع IP قابل للتوجيه، ولا يلزم تفعيل route-all-traffic.
- لا NAT traversal/ICE/STUN/TURN، ولا loss recovery ولا reliable delivery، ولا host migration أو gameplay. لا إطلاق أي اتصال خارجي من GitHub Actions.
- لا نعرف تشغيل CLI على هاتف POCO بعد، ولم نحصل على XBE المطابق لإعادة بناء BLACK. الـGame source match ما زال مسدوداً.
- التحول إلى لعب فعلي يحتاج مطابقة XBE، hooks آمنة على game thread، multi-actor، التصادم والأسلحة والAI والحفظ، نظام تشفير قوي، ومحرك نقل موثوق؛ لا يجوز استعمال هذا الـCLI كحل إنتاجي.

## الخطوات التالية
- الحصول على default.xbe مرخص مطابق SHA1 82d0588124e70102ee650f8969c0a56374476234 بطريقة خاصة، من دون رفعه في public repo.
- HOOK-001 قراءة read-only للعبة قبل كتابة الحالة؛ ACTOR-001 اختبار شخصية ثانية؛ ثم Net gameplay.
- NET-004: اختبارات Android runtime على جهازين وZeroTier، ثم بناء المصادقة والتعامل مع jitter/retransmissions؛ الربط بالكود الفعلي بعد إثبات هوية الإصدار.
