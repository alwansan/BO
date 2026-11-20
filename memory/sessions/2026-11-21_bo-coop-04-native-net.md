# جلسة BO-COOP 04 — 21 نوفمبر 2026

استعيدت AGENTS/RESTORE/STATE/PLAN/ISSUES وHEAD 03a05ad6545937ca24d9d770ee6efe5723e10d80 قبل العمل. بُني بروتوكول NET-003 مستقل عن أصول اللعبة.

## التنفيذ
- `coop/native/net_core.hpp`: wire strict, non-allocating decode fixed 512B bound، JOIN/ACCEPT/INPUT/SNAPSHOT/REJECT، limit 3 peers + host، player id 2–4، nonce/token مختبر، sequence monotonic، timeout 1500ms، authoritative simulated grid state؛ لا تشفير/مصادقة.
- `coop/native/test_net.cpp`: malformed/truncated/extra bytes، ID duplicate، idempotent JOIN، max peers، forged endpoint/token، stale sequence، expiry and rejoin؛ socket integration two clients real IPv4 UDP localhost.
- `coop/native/udp_cli.cpp`: host/join processes، bind loopback default via explicit IP، 10 INPUT simulation with snapshots، port IPv4 configurable.
- workflow Linux ASAN/UBSAN+C++ test + process smoke; Android NDK28.2 ARM64/API29 binary artifact only.

## اختبارات
- أول فشل [44833697806](https://github.com/alwansan/BO/actions/runs/44833697806): test duplicate actor used id=2 valid, correction a9b11dc; ثاني فشل [44833772092](https://github.com/alwansan/BO/actions/runs/44833772092): TTL expectation premature at1800, correction 329ec41. **لم تُعطل أي assertions**.
- [44833884316](https://github.com/alwansan/BO/actions/runs/44833884316) SUCCESS Linux sanitizer tests.
- [44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) SUCCESS Linux process smoke and Android cross-build. Artifact 14677732935 `black-coop-net-lab-android-arm64-NO-GAME`, executable SHA256 `0ca758a2ae4ac13a322cfa5106ff49ab58f753cb5758d43e7d9a296a2ccbe100`.
- [44834027684](https://github.com/alwansan/BO/actions/runs/44834027684) Python UDP regression triggered on source update; inspect separately for final verdict if needed.

## حدود القبول
لا Android runtime حتى الآن؛ لا اختبار جهازين، لا ZeroTier اتصال فعلي، لا APK قصة متعددة. لا XBE مطابق؛ compile hash 82d0588124e70102ee650f8969c0a56374476234 ما زال مختلفاً عن disc XBE المرفوع. لا سرّ أو لعبة في artifact. راجع docs/NET_003_NATIVE_LAB_AR.md وmemory/evidence/NET-003-cpp-android-44834027711.json.

## الخطوة القادمة
اختبار CLI على هاتفين إن أراد المالك؛ تجهيز Net transport أكثر موثوقية وأماناً وفصل thread/network عن game thread. لا دمج gameplay قبل baseline. حدّث الذاكرة بعد كل دفعة.

## إغلاق نتائج CI
- [44834027684](https://github.com/alwansan/BO/actions/runs/44834027684) completed SUCCESS لبايثون؛ [44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) completed SUCCESS لـLinux وAndroid cross build. أُعيد التحقق من HEAD والـRESTORE بعد الدفع.
