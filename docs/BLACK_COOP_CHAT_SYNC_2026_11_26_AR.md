# BO-COOP: استعادة الدفعات المحلية Chat-Batch01..07

التاريخ: 2026-11-26. الفرع الوحيد المسموح: `experiment/black-coop` في `alwansan/BO`.
المصدر السابق الذي جرت عليه الدفعات المحلية: `f7551ada9e7e352f937bd24e7b5ba405b947602d`، Session148. يجب فحص `git rev-parse HEAD` ورأس GitHub قبل الاستئناف؛ لا تعتمد على SHA تاريخي بوصفه الرأس الحي.

## نطاق المصدر المستعاد

- Batch01: سيناريوهات C++ اصطناعية لتقارب NPC/الأجسام مع نطاق المهمة، دون engine apply.
- Batch02: إعادة التحقق من المشهد بعد حل هوية الكائن وقبل تطبيق الحالة.
- Batch03: حفظ `descriptorId` ثمانية بايتات، و`counter`/`state` مستقلين؛ مرجع MIT مثبت، لا checkpoint restore حي.
- Batch04: مطابقة جميع حقول `RoomStoryWire.Ticket`، مع 157 حالة C++/Java.
- Batch05: تجزئة/تجميع أهداف بحد 26 سجلًا في الجزء، 158 جزءًا لـ4096 سجل، مع SHA256 ومنع نشر اللقطة الناقصة.
- Batch06: بروتوكول `RoomWire` version/profile 10 ونوع 17، نقل أجزاء عبر TLS فعلي إلى ثلاثة ضيوف اصطناعيين، إبطال عند ABORT وتغير المشاركين.
- Batch07: دورة `RoomSession` تربط جدول هويات مستقل قبل READY، ولا تقرأ أو تنشر إلا عند COMMIT وشاهد مشهد محلي حديث، وتلغي النقل عند تغير المشهد.

جميع هذه الأعمال مصدر مود/عقود واختبارات اصطناعية، وليست إثبات تشغيل BLACK أو ظهور شخصيات أو مزامنة مهام حية أو XAPK جاهزة. لا تغيّر ملفات ISO/XBE والحفظ والأصول أو AI والأعداء.

## إعادة الاختبار من checkout نظيف

المتطلبات: Python3، JDK بترجمة `javac` وتشغيل `java`، g++ C++20، و`keytool` من JDK. لا مفاتيح ثابتة ولا ملفات أصلية للعبة. يصنع الفحص ملف PKCS12 **اختباريًا مؤقتًا** داخل مجلد نواتج ويزيله عند النجاح؛ لا ترفعه.

```bash
python3 coop/room_network/check_objective_tls.py --source-root . --output /tmp/bo-objective-tls
python3 coop/room_network/check_objective_story_scope_parity.py --source-root . --output /tmp/bo-objective-parity
python3 coop/room_network/check_objective_chunks_interop.py --source-root . --output /tmp/bo-objective-interop --disable-leak-check
```

في بيئة لا تدعم هدف JDK 8، يمكن استعمال الخيار `--java-current-runtime` للفحص المحلي فقط، مع كتابة قيد عدم إثبات Java8. فحص LSAN غير مثبت عندما يُعطل `--disable-leak-check`. تشغيل Java/TLS على JVM لا يثبت Android أو اللعبة.

أدلة سابقة من Chat-Batch07: 10/10 من برامج Java تحت إعدادين محليين، 7/7 اختبارات RoomSession الجديدة، 11 حالة لنقل TLS، و157/157 قواعد Ticket، و12 اختبارًا لتجزئة الأهداف مع تحقق C++ من 4096 سجل لكل واحد من ثلاثة ضيوف. هذه أرقام سجل محلي تاريخي وليست GitHub CI.

فحص إعادة النشر المحلي بتاريخ 2026-11-26: `check_objective_tls.py` على نسخة المصدر المطابقة للرقعة التراكمية، مع `--java-current-runtime`، نجح وظهر `OBJECTIVE_TLS_CHECK_PASS`. نتائج هذه الدورة قابلة للتجديد بالأوامر أعلاه. لا ترفع المجلد `/tmp/bo-objective-*` أو مفاتيحه الاختبارية.

## CI والنشر الآمن

- `black-coop-room-network-apk.yml`: جُعل `workflow_dispatch` فقط عند نشر دفعات Chat؛ كانت `push` السابقة تستعيد ISO مالك اللعبة في runner. لا تشغّل هذا workflow يدويًا قبل مراجعة وتوثيق تصريح الأصول. **هذا ليس إثباتاً بحظر سير قديم يعمل فعلاً؛ افحص Actions.**
- `black-coop-room-network-preflight.yml` و`black-coop-parallel-engine-tracks.yml` و`black-coop-real-lobby-core.yml` و`black-coop-lobby-android-platform.yml` و`black-coop-probe.yml`: مسارات مرتبطة بملفات هذا التحديث؛ المصدرية/الاصطناعية فقط وفق مراجعة تعريفاتها. راقب نتائجها عند push، ولا تفسر مجرد إنشاء run نجاحًا.
- لا تستخدم force push، ولا تعيد إدخال artifacts، أو PKCS12، أو keystore، أو ISO/XBE، أو logs خاصة، أو نسخ أدوات كبيرة يمكن تثبيتها من مراجع مثبتة.

## العوائق والخطوة التالية

لا مصدر موثّق لأهداف BLACK الحية ولا دورة عمر جدولها، ولا واجهة تطبيق أصلية متحققة، ولا إثبات عدم تغيّر المشهد أثناء استهلاك لقطة منسوخة. لا تأكيد استلام كامل/مهلة لحدَاثة اللقطات. الخطوة التالية: حاجز حداثة `read` واستهلاك قيم الأهداف مع تحقق ticket/original scene عند الاستخدام، واختبارات delay/abort/scope، بدون تفعيل تطبيق engine. ثم الربط الحي فقط بعد إثبات المصدر والعمر.

**سياسة صاحب العمل الجديدة:** قبل كل دفعة، قارن الرأس والفرق، احفظ التغييرات المحلية دون فقد؛ انشر فقط العمل المسموح على `experiment/black-coop`، حدّث `prompt/RESTORE_SESSION_AR.md`، راقب CI بعد الدفع، وتحقق من SHA والملفات من GitHub وحاول الاستعادة النظيفة. لا تجارب هاتف صغيرة؛ قبول واحد بهاتفين بعد جاهزية Android والمزامنة والاختبارات.
