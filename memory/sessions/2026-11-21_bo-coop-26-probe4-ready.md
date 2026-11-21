# جلسة26 — Probe4 جاهز بعد استئناف المقاطعة

المقاطعة حدثت بعد كتابة اختبارات تحليل instance graph في مرحلة RED؛استؤنف التنفيذ وأُكمل collect_instances فيpool_capture.py. يتحقق منschema1 وحدود4NPC+1local وتسعةuint لكلtuple وارتباطNPCslot/serial بقائمةlive والأصفار عندفشل الرابط. لا يخرجrawidentities أوpointers. 10pooltests و19analysis test_*.py و11legacyprivacychecks PASS؛مراجعةpool_core_review لم تجد عيباً مهماً. actualProbe4observer harness نُفذ مجدداً بعد فقد/tmpأثناء المقاطعة،وأُضيفheaderمصطنع واختبرنا مخرجاته بالمحلل:2frames graphs و3validlinks(1NPC+2local)كماالمتوقع. هذهfixturesمصطنعة وليست اختبار الهاتف.

## APK حقيقي مثبت
run44876183662علىsource1b27ecc26e5417b91eb84208479e039e9b792521؛job136761271051LinuxASAN/UBSAN و136761328017Windows/NDK/APK كلاهماcompleted/success. قرئتlogs؛فحوصgeneratedoriginal/reset،JNI/package/debugsignature/imagec5/no-network/assets-exclusion نجحت. artifact14696342069 وmetadata14697085027. APK21749755bytes SHA256bfd7968b04851d8c814bdb88563b1efb8e8b9b88695d2c161c077111bd3c9897. ZIP SHA2569e4cf607925d35c35055e705ec7739c03d3b135d1abcee752fa2b34e2116015a. نُزلZIP بالموصل وفُحصsha ثمAPKsha/size ومطابقةmetadata بشكل مستقل. الملفات فقطAPK/metadata/NOTICES. محلياً/home/a/Desktop/black/Probe4-44876183662/black-probe4-candidate-READ-ONLY-NO-COOP.apk.
رابطartifact: https://github.com/alwansan/BO/actions/runs/44876183662/artifacts/14696342069 . ينتهي2026-12-05؛لا تحفظsignedURLالمؤقتة فيالمستودع.

## التالي يحتاج الهاتف
اختبارواحد وفقdocs/PROBE4_DEVICE_TEST_AR.md:Startcapture،حركةوإطلاقناربقربأكثرمنNPC،قتلعدووcheckpoint/reloadخلال90ثانيةإنأمكن،ثمbackground/exportوأحدثcoop_probe.txt. المقصود التحقق منcontroller328/owner/runtime54/binding50/scratch34C/350 علىالهاتف،وليسإعادةProbe3المقبولأوbaseline. قارن NPC/local وتفرّدالقيمبينالعيناتدونادعاءreadableallocations أوatomicownership. بعدهذاالدليل انتقللتجربةbody/proxyحقيقيةبخيطاللعبةوحمايةslot+serial+scope؛لا مزيدmocknetwork كبديل.

سجلProbe3الذيأرسلهالمالك محفوظكملخصآمن،والمرجعPetersonمراجعومحفوظبـhead0ee3ae6نفسه. لاco-opفعلي/remoteplayer/COMBATمقبولبعدولاISOمعدلجديد؛كلالعملالحاليAPKقراءةفقط. محاولاتاختبارالهاتفلمتصللـProbe4بعد،ولايُستنتجنجاحهعلىالجهازمنCI.


فحص CL الأخير: tree590149cd7328ede669337b79937b8a8195182947 يحتوي سجلَي Probe2 وProbe3 فقط؛لاProbe4captureمتاحعندطلبالاختبارالجديد.
