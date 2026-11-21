# NET-LAN-001 — قاعدة اكتشاف الغرف المحمولة المستقلة (غير مربوطة باللعبة)
**مصدر الفكرة:** مخطط LAN discovery في `Peterson047/BLACK-XBEMultiplayer@0ee3ae6411c7f0989c62cbe36b224d3f55043923`، الذي يستخدم UDP منفصلاً للعبة وLAN. **الكود في BO مكتوب من جديد** ويعمل فقط في مختبر C++ مستقل. تفاصيل المصدر وترخيصه في `docs/REFERENCE_BLACK_XBEMULTIPLAYER_AR.md`.

## ما أضفناه فعلياً
- `coop/native/lan_discovery.hpp` — رأس C++20 portable، بلا تبعيات Windows أو Android، يصدر/يتحقق من طلب `BCLN`/إصدار 1 `Query` مع challenge عدد32بت، ورد `Reply` يحمل نفس challenge ومنفذ اللعبة وعدد اللاعبين واسم غرفة ASCII محدوداً إلى32 حرفاً.
- يمنع تساوي منفذ اللعبة والاكتشاف، يفرض المنافذ غير المميزة 1024..65535 ويقترح **1508 للعبة و1510 للاكتشاف** وفق README الخارجي. لا يضمن قابلية الوصول عبر NAT؛ VPN/Port forwarding يحتاجان إعداداً آخر.
- يرفض الرزم القصيرة والامتدادات الإضافية والإصدار/النوع غير الصحيح، ويحد سعة الغرفة إلى4 أشخاص (المضيف +3) حسب خطة BO، ويفحص الأسماء حتى لا تعبر CRLF/control bytes إلى السجلات. `RateGate` يمنع إرسال استعلام/رد في نفس المسار أكثر من مرة/ثانية؛ **لا توجد بعد آلية limiter لكل IP للمستقبل**.
- `coop/native/test_lan_discovery.cpp` يحتوي اختبارات packet/port/malformed/version/reply challenge و5000 رزمة مشوهة اصطناعية. أُضيف إلى `.github/workflows/black-coop-native-net.yml` واختبار cross-compile Linux→Android ARM64.

## نتائج محققة
- GitHub Actions [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957) **SUCCESS**: Linux ASAN+UBSAN، strict C++20، فحوص فك الرزمة وتعارض المنافذ وسلوك limiter، وإعادة تمرير الاختبارات الحالية UDP loopback لاستقبال Client 2/3. Android NDK28 cross-compile متحقق ELF AArch64، **دون تشغيل اختبار على الهاتف**.
- ليس تطبيقاً لغرف LAN حتى الآن: **لا sockets broadcast، لا دعوة Host/Join في التطبيق، لا JNI/واجهة الغرفة، لا session auth/encryption، لا شخصيات أو مهام متزامنة**. لا تنشر رزم LAN على الإنترنت المفتوح باعتبارها مصادقة؛ challenge ليس مفتاحاً. امنع عرض الغرف غير الموثوقة تلقائياً دون تحكم.

## الخطوات التالية
1. اربط `Query/Reply` بفاحص UDP IPv4 في مختبر POSIX أولاً، مع فصل المنفذين، bind/error/fd RAII، source endpoint validation، limits لكل IP، وخيار تعطيل discovery.
2. اختبر packet-loss/duplicate replies/timeouts وتجاهل إعلانات الإصدار المخالف. راجع Android 15 multicast/broadcast والجدار الناري على جهاز حقيقي قبل الادعاء.
3. أبقِ Host authoritative على هاتف اللاعب، وصمم UI CO-OP مباشرة تحت SETTINGS. لا تعمل JOIN فعلياً أو تستدعي BLACK actor قبل ACTOR-001 proof وsession authentication.
4. فقط بعد إثبات موديل NPC والـABI وlifecycle، اربط network worker→SceneInbox→RemoteProxyRoster→render proxy عبر thread اللعبة، وبعدها NPC/mission/checkpoint sync للطور القصصي.

## تنفيذ UDP منفصل مثبت — NET-LAN-002
أُضيف `coop/native/lan_socket.hpp` مع POSIX IPv4 socket RAII على Linux/Android NDK، يربط على loopback افتراضياً (بدلاً من البث العام)، ويدعم bind الصريح لشبكة LAN لاحقاً، ولا يستعمل `SO_REUSEADDR`. يرفض تداخل منافذ الخدمات، يستقبل datagrams حتى64 بايت مع رفض أي طول غير صالح، يتحقق من endpoint IP/port وحقل التحدي، يفرض محدد inbound 16 endpoint بمسافة500ms/طلب ونافذة تنظيف5000ms (ليس مكافحة هجمات شاملة). اختبارات `coop/native/test_lan_socket.cpp` تفتح sockets حقيقة على localhost وترسل Query/Reply، وتختبر bind collision وtimeout وعدم التسريب عبر الرد. **لا يوجد broadcast فعلي ولا اتصال من هاتف إلى هاتف حتى الآن**.

اختبار [44869388876](https://github.com/alwansan/BO/actions/runs/44869388876) **SUCCESS**: Linux ASAN+UBSAN يدعم مرور UDP loopback الحقيقي، وAndroid ARM64 NDK28 cross compile فقط (لا تشغيل على الهاتف). معاً مع اختبارات codec السابقة [44869092957](https://github.com/alwansan/BO/actions/runs/44869092957) تنجح 5000 malformed packet probes.
