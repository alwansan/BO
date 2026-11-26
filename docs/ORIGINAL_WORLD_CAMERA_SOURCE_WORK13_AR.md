# Work13 — تتبع مصدر كاميرا العالم: نتائج محددة، لا تفعيل حي

2026-11-26؛ الأساس a20b777611fc3e52bd35df8f58a10ace811b417c. هذا تحقيق مباشر لإغلاق عائق كاميرا Work10–12، وليس إضافة نقل أو TTL. لم تتغير اللعبة أو AI أو الأصول أو الحفظ، ولم يُشغل workflow ISO/APK يدوي.

## حقائق المصدر وما لم يُثبت

المرجع MIT المثبت `portingpete/Black-2006-Recomp@b2512fafd3e181e4febfb298f297245b845ef628/src/recomp_manual.c` يحدد:

- كاميرا العالم الأساسية: `[2D67F4]+D450`، وRwCamera من camera+58، وframe من RwCamera+4. مصفوفتا modelling وLTM عند frame+10/+50، صفوف right/up/at/position.
- المنتج الأصلي D2F40 ينتج half-frustum عند camera+48/+4C؛ يميّز المرجع الكاميرا الأساسية عن UI/offscreen، ويحافظ على العدسة الأصلية والـzoom والمشهد السينمائي.
- الرفع عند16F610 يحتوي **MVP transposed** إلى c10..c13. هذا ليس view/projection مستقلًا تلقائيًا؛ المرجع نفسه يميز موارد السلاح الأول ومعاملات مادته عن العالم. لا ننسخ constants عشوائية إلى رسم الصندوق.

قراءة محلية فقط لصورة المالك C5 الموجودة، دون نسخ bytes أو تعليمات إلى المشروع، ضيّقت المسار:

- نافذة المنتج D2F40..D30E9، ومرشحا setters عند2DB60 و2DC20: الأول يكتب حقل80، والثاني84، وكلاهما يعيد إنتاج8C/90. هذه الحقول مرشحة لمجال القص/العمق، وليست ABI مقبولة من مجرد فهرس وصول.
- عُثر على13 مرشح direct-call للمرشحين. مواضع renderer15A312/15A330 قريبة من native-camera58/84/88، ومنها نقطة فحص محددة بدل تخمين offset من محرك آخر.
- callback المرشح2D9F0 يستدعي2D1A0؛ نافذة2D1A0 تصل إلى20 و70/74 و78/7C. هذه صلة حسابية مع العدسة/offset تحتاج تتبع بيانات كاملًا قبل استخدام viewMatrix كـview عادية. لا نضاعف إسقاط العدسة فوق مصفوفة قد تتضمنه.

الأداة `coop/analysis/original_camera_source_survey.py` تحفظ offsets ومرشحات calls/returns فقط، مع SHA1 مطابق قبل القراءة. ناتجها في `memory/evidence/WORK_BATCH13_CAMERA_SOURCE_SURVEY_2026-11-26.json`. لا تحفظ التعليمات أو ELF أو ذاكرة runtime. النوافذ قد تشمل تعليمات/دوال لاحقة أو بيانات مفككة، والحقول القريبة من call ليست بالضرورة وسائطه. لا تحتوي النتيجة علامة قبول كاميرا.

```sh
python3 coop/analysis/original_camera_source_survey.py \
  --owner-xbe "$BLACK_OWNER_XBE" \
  --output /tmp/black-camera-metadata.json
```

هذه قراءة للصورة المحلية المأذونة الموجودة، لا طلب baseline جديد ولا استعادة أصول في Actions. الأداة تحتاج helper الخاص القائم وllvm-objdump؛ لا تنشر stdout الخاص به.

## أُغلقت وصلة المنتج إلى SDK من المصدر نفسه

تتبع لاحق للجسم الأصلي والمتصل أعطى نقطة أدق من generic RW layout:

- callback كاميرا الجهاز **2B830** يأخذ ثلاثة cdecl arguments `(0, RwCamera*, 0)`؛ الكاميرا عند **ESP+8 الحقيقي عند الدخول**. الاستعمال EBP+C بعد prologue حيث EBP=entryESP−4، وليس قراءة ESP بعد pushes. المتصل2DA8A ينظف12 بايت. يعيد callback EAX=1 ثم ret عادي2BA5E؛ wrapper2DA70 له convention آخر (camera أو0).
- يتبع RW+4 عبر2F760 إلى LTM=frame+50، ويستعمل inversion2/3D عند32310، ويعكس عناصر عمود view0/4/8/12. يرسل view فعليًا إلى D3DDevice_SetTransform type0 عند2B8BD، ثم projection type1 عند2B9C7. لا إعادة بناء math ولا نسخ RW+20.
- projection الأصلي يستخدم reciprocal window70/74 وoffset78/7C وnear80/far84. في الفرع المنظوري: P10=f/(f−n)، P11=1، P14=−nf/(f−n)، P15=0؛ near→0 وfar→1، مع offset أصلي لا ينبغي إسقاطه.
- ربط config/game.toml:142 في Androidbaseline يثبت D3DDevice_SetTransform=26AAE0؛ التحويل يرفض اختلافه ذريًا قبل أي كتابة (regression مستقل). Androidbaseline `src/d3d/d3d8.cpp:725–730` ينسخ مصفوفتَي XDK0/1 إلى State.transforms2/3. الدالتان **d3d::State()/Lock() عامتان** في SDK `runtime/xdk/d3d8.h`، وليستا private؛ backend وحده يحتاج bridge داخليًا. Vulkan FillTransformConstants يحافظ على ترتيب World×View×Projection ثم transpose لـHLSL؛ لا نضيف قلب Y/depth بالتخمين.

### التنفيذ المصدري

أضيف wrapper2B830 إلى التحويل source staging في `prepare_room_network.py`: يحفظ RW من entryESP+8 بعد span-check، ينفذ original import مرة واحدة، ثم يلتقط المصدر. `room_original_world_camera_witness.hpp` يرفض مرحلة غير1C وRwCamera لا تطابق الكاميرا الأساسية؛ يقرأ State تحت Lock، دون كتابة guest أو State أو JNI أو GPU submit. توجد إشارة atomic لإبطال اللقطة عند منتج فاشل/ثانوي. يُفحص active-cameraD590 عند الاستهلاك، لا نفترض ترتيبه عند الدخول إلى begin.

`room_original_world_camera_frame.hpp` يملك نسخ view/projection/targets/viewport/scissor/depthscale مع serial من أحداث begin الأصلية. يقبل الاستهلاك فقط على الخيط المالك مع نفس الهوية والقيم الحالية؛ يتجنب حتى FP comparison على NaN عبر فحص البتات. لا يصنع serial تذكرة قصة ولا يثبت generation لتخصيصات الأصل. **المستهلك المرئي ما زال يحتاج signed story scope وoriginal scene/frame عند الاستخدام**، وDraw fenced ثم rasterization قبل تفعيل العرض. لا مستهلك أو رسم جديد في هذا التحديث.

اختبار RED عند غياب helper ثم ASAN/UBSAN GREEN محليًا؛ المراجعة المستقلة وجدت lost-revocation race حين ينتظر callback مرفوض خلف نشر ناجح، وأُعيد إنتاجه باختبار NativeWitness نفسه مع SDK mutex وذاكرة اصطناعية (RED)، ثم إصلاحه بإعادة atomic=false بعد أخذ القفل (GREEN) (LSAN معطل في البيئة المحلية)، وSDK syntax-only للـwitness، وخمسة source-stage checks ناجحة. CI بعد النشر غير متحقق بعد. لا أصل BLACK شُغل في هذه الاختبارات ولا camera/runtime أو rasterization أو قبول لعب مشترك.

## لماذا لا نأخذ layout من مشروع RenderWare آخر

[معمارية librw الأصلية للمشروع](https://github.com/aap/librw/blob/master/ARCHITECTURE.MD) تصرح بوجود اختيارات خاصة بها، و[Camera في مصدره](https://github.com/aap/librw/blob/master/src/rwobjects.h) ليس إثباتًا لتخطيط BLACK Xbox. استُعمل المرجع للمقارنة المفاهيمية فقط؛ لم تُنسخ ترويساته ولا اعتمدت offsets منه.

## الخطوة الحاسمة التالية

فُصل مصدر مصفوفتَي SDK من الكاميرا الأساسية. الخطوة الآن ربط المستهلك بنفس تذكرة القصة والمشهد/الإطار وأهداف اللون/العمق، ثم اختبار rasterization فعلي بلا أصول، قبل أي عرض حي. مصفوفتا SDK تُنسخان كما أنتجهما الأصل، مع استبعاد كاميرات السلاح/UI واللقطات التي تغيرت بعد begin. بعد ذلك rasterization بلا أصول على backend المصرح به، ثم hook حي. لا override لبوابات actor، ولا projection مصطنعة، ولا طلب هاتف جزئي. لم يثبت ظهور الجسم أو الفعل المشترك أو انتقال القصة في هذه الدفعة.
