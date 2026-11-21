## Session38 — THIS TEST HAS ALREADY ARRIVED; do not ask again
Read CL black log/logs/BLACK-Probe-20261121-235407-267/coop_probe.txt. Eight complete default_player_model records: primary loaded model has 44 bones, 2 buffers, mask32 (draw-list5), arms but no head/torso/legs; this is not an exhaustive asset inventory. Xiaomi owner run confirms matching XBE; Huawei Android10 also ran org.bocoop.black.defaultplayer. New directive: original local first person plus full-body ally appearance for OTHER human players. Next meaningful phone gate requires new implemented story/remote body, not repeating this observation.

# اختبار شخصية اللعبة الأصلية — جلسة36

هذه الحزمة **BLACK ORIGINAL PLAYER TEST**، `org.bocoop.black.defaultplayer`، تختبر النموذج المرتبط بشخصية اللعب الأساسية فعلياً، دون إنشاء جسم بوت أوحليف ودون تغييرشخصيتك أوالحفظ. ليستنسخةلعبجماعيجاهزة؛Startالمشتركوالجسمالبعيدلمُينفذابعد.

المطلوب علىهاتفواحد:
1. أغلقنسخBLACKالتجريبيةالأخرى،وافتحالحزمةالجديدةواستوردنفسISOالمملوكعندالحاجة.
2. اختر **Start capture** منطلبالتسجيل،ثمحمّلمهمةوالعببشخصيتكالأساسية20–30ثانية. لايلزماستضافةغرفةلهذاالقياس،ولاتستعملfactoryroomبدلاًمنهذهالحزمة.
3. ارفع `coop_probe.txt` و`black_log.txt` الجديدين من `Downloads/Games/cl/black log/logs/BLACK-Probe-[date]/` إلىCL/black log،وحددأنهtestdefaultplayer. يحتويالرصدالجديدevent=default_player_model؛بحد8سجلاتفقط. لاترسلProbe4القديمبدلاًمنه،ولاISOأوحفظأومفاتيح.

يمكنكأيضاًتثبيتنفسالحزمةعلىالهاتفينوفحصالواجهةالمبسطةفيتجربةمنفصلة:«الأجهزةوالغرف»قبلالاستضافة،ثمHostبالاسموالغرفة،ثمJoinمنالقائمةحتى2/4. لاPINأوبصمةأومنفذ؛IPيدوياحتياطي. استخدمSAVE ROOM LOG TO DOWNLOADSعلىالطرفينإذااختبرته.

سجلاتالنموذجتقيسbone-countوتصنيفمفاصلوdraw-listفقط،ولا تصدرgeometryأوأسماءمفاصلأومؤشرات. body_candidateليستإثباترسمجسمكاملأووجودهعنالبعيد. نحتاجالنتيجةالحيةلنعرفمايمكنإعادةاستخدامهمنالشخصيةالأصليةبملكيةمستقلة،ثمنربطالمهمةوالحركةوالعالم. لا يستبدلالاتصالبهذاالقياس،ولا2/4باللعبالمشترك.

نطاقفحصdefault_player_modelهوprimarydrawmodel:actor+320ثمwrapper[0]،وهوالمسارالأولفـ1D25D0. completeيصفقراءةmetadataلهذاالنموذجفقط. body_candidate=falseلايثبتغيابجسمآخرفيكلالمواردأومجموعةالنماذج؛لا تعمممنهغيابprotagonistfullbody. لاافتراضأنكلwrapperهوbuffer200بايتلمجردPCcopy.
