# استعادة BO-COOP — 2026-11-21، جلسة07

الهدف مود BLACK Android تعاوني حقيقي داخل القصة: هاتف هوست، لاعبان ثم4، LAN/ZeroTier، غرف واكتشاف/اتصال يدوي، late-join، AI/قتال/أهداف مشتركة، نوك/إنعاش، friendly fire، أسماء ومجسمات، وحفظ الفردي. النطاق التفصيلي وبراهين القبول في docs/COOP_REQUIREMENTS_AR.md. لا gameplay مقبول ولا APK جماعية حالياً.

## استعد بالترتيب
1. AGENTS.md؛ الكتابة في experiment/black-coop فقط بالمستودع alwansan/BO. main والمراجع للقراءة فقط؛ لا force push ولا مساس بأصل اللعبة أو الحفظ أو التوقيع.
2. memory/MEMORY.md وSTATE.json وPLAN_AR.md وISSUES_AR.md وprompt/SESSION_PROTOCOL_AR.md.
3. آخر جلسة memory/sessions/2026-11-21_bo-coop-06-manual-abi.md وسابقتها05، ثم docs/SOURCE_MAP_AR.md وV150_PROVENANCE_AR.md.
4. تحقق من HEAD/Actions الحيين قبل التعديل. لا تعاد اختبارات terminal/success بلا تغير ذي صلة.

## الحقائق المثبتة
- Arabic1.5.0-ar1.0 code35 يرتبط reference f3754d6985a8c35204e7df071546e586b10f1806 (tagv1.5.0)؛ المصدر العربي الكامل غير متاح. SDK المثبت99078b119b5d5e0d1d6afb1390e29437c0c984c0. esp.cpp وweapons.cpp غير موجودين في tag، فلا تستعمل مراجع main لإثباته.
- libmain compile image SHA1=82d0588124e70102ee650f8969c0a56374476234، القرص المرفق XBE=c5bfed25449c8212d7a976f15246e87ead57713f. لا تطابق؛ لا تعطل hash gate. المالك أكد أنه لا يملك XBE الأصلي أو مصدر التعديلات؛ لا تكرر طلبهما.
- NET003 مستقل: Linux UDP وAndroid ARM64 crosscompile نجحا44834027711؛ لا Android runtime/ZeroTier. مختبر غير مصادق/مشفر، ليس netcode إنتاجياً. evidence NET-003-cpp-android-44834027711.json ودليل docs/NET_003_NATIVE_LAB_AR.md.
- مسار بديل للتحليل فقط: إعادة ترجمة المرشح الموجود ساكناً نجحت44840960136، فحص أعمق44841534370 على code08fd55b4cb1640b4b56445d43e1801dbc635b1a3. functions15042/instructions798292؛ unimplemented موجودة في بقية الناتج. targets host28/28 موجودة دون unimplemented فيها/عند مواضع hooks؛ ليس إثبات semantics أو actor factory.
- scanner:108/154 function و6/11 data مطابق؛46/5 غير محسوم. ABI11 غير محسوم آلياً، ليس تعارضاً موجباً. مراجعة يدوية x86 لمدخلات المواقع11 تدعم السجلات/stack المحددة فقط؛ ليست إثبات كل الأدوار أو مسارات العودة. evidence SRC-002b-deep-44841534370.json وSRC-002b-manual-abi-inputs.json.
- أول probe44840599544 فشل toolchain (Clang20 مع MSVC يتطلب22) قبل التحليل؛ أُصلح باختيار Clang22.1.3 المرفق بـVisualStudio. لا تغير شروط STL أو ملفات اللعبة.
- توجد logs عامة بنفس hash المرشح في upstream Android issue33؛ هذا لا يثبت تشغيل نسخة المالك ولا أن الاختلاف بسبب تعريب.

## تحديث جلسة07 — العقبة السابقة حُسمت
المالك أكد دخول النسخة الحالية للمهمة وعملها، وقدم ملفات التطبيق محلياً. loaded hash في black.log=c5bfed... وbuild f3754d6، وSHA256 كامل ISO من الهاتف مطابق manifest f6d15f.... raw log والسيفات خاصة لا تنشرها. دليل metadata: memory/evidence/DEVICE-existing-owner-runtime.json. `quit: ConfirmWarning, FEAudio failed` موثق كرسالة exit-menu، وExit0؛ لا نخفيه ولا نفترض عطل قتال. لا تكرر طلب اختبار الأصل أو XBE المفقود.

## الخطوة الحالية
راجع أحدث session07 وSTATE وdocs/superpowers/plans/2026-11-21-candidate-baseline.md. واصل workflow black-coop-baseline.yml لتجربة بناء candidate مستقل من XBE الفعلي، دون تعطيل hash check. package org.bocoop.black.baseline وحفظ مستقل، label NO CO-OP. native/src/config لا تغيير فيها، packaging/updater/build concurrency فقط. لا source-equivalent Arabic claim ولا gameplay/device acceptance قبل اختبار الناتج. افحص CI/HEAD الحاليين؛ لا APK ناجحة حتى build+signature/package/ABI/embedded-hash gates. الأصل لا يمس، ولا تنشر ISO/XBE/generated sources أو log/hdd. بعد baseline يحتاج اختبار الهاتف قبل HOOK/ACTOR.

بعد كل دفعة أو اكتشاف أو CI: حدّث STATE/MEMORY/PLAN/ISSUES حسب الحاجة، جلسة مستقلة وRESTORE، وcommit على الفرع التجريبي ثم تحقق من HEAD. الهدف لم يكتمل؛ لا عمل خلفي خارج الجلسة.

مراجعة baseline قبل CI: الاستيراد داخل assets الخاصة فقط، لا صلاحيات تخزين واسعة، artifacts لا تصعد قبل نجاح تحقق APK. source packaging preview الحقيقي PASS واختبارات4/4 PASS. لا نتيجة بناء Android بعد.
