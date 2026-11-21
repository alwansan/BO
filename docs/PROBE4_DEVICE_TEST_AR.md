**البناء جاهز ومثبت:** [artifact14696342069](https://github.com/alwansan/BO/actions/runs/44876183662/artifacts/14696342069)،run44876183662SUCCESS؛APKsha256bfd7968b04851d8c814bdb88563b1efb8e8b9b88695d2c161c077111bd3c9897. فكZIP وثبتAPKProbe4؛لا حاجة لإعادة رفعgame.iso.

# اختبار روابط التحريك في Probe4 — بعد نجاح البناء فقط

هذه نسخة رصد منفصلة `org.bocoop.black.probe4`، لا تضيف لاعباً ثانياً بعد. سبب التجربة الجديدة هو التحقق من متحكم التحريك الخاص بالشخصية؛ سجل Probe3 السابق قُرئ وقُبل لدليل الجدول وإعادة التهيئة، فلا نطلب إعادته.

بعد تأكيد رابط APK: ثبّت Probe4 واترك Probe3 والأصل والحفظ كما هي. اختر Start capture، وادخل مهمة فيها أعداء. تحرك وأطلق النار قرب شخصيتين ظاهرتين على الأقل، ثم اقتل عدواً وأعد checkpoint أو المهمة خلال أول دقيقة ونصف إن أمكن. ضع التطبيق في الخلفية للتصدير وأرسل أحدث coop_probe.txt مع وصف أي تعطل. لا حاجة لملفات ISO أو Android/data جديدة.

المخرجات الإضافية: instance_schema=1 وinstances. tuple هو [local,slot,serial,valid,controller,runtime,binding,scratchA,scratchB]. NPC يربط slot/serial من لقطة pool الصالحة؛local=1 يشير إلى world+30. عند valid=0 تبقى قيم المؤشرات صفراً. validity تعني تطابق backlink واستقرار الحقول المرصودة فقط، ولا تثبت قابلية قراءة أهداف runtime/binding أو حيازة تخصيصها أو تفردها. لا تنشر السجل الخام في BO.
