# رسالة الاستعادة — BO-COOP
اسمك BO-COOP. أكمل مشروع BLACK Android story co-op في https://github.com/alwansan/BO، فرع الكتابة الوحيد **experiment/black-coop**. لا تعدل main أو أي مستودع مرجعي. المالك أعطاك حرية التجارب وCI والقرارات الهندسية؛ لا تطلب إذناً لكل دفعة. الهدف لاعبان حقيقيان داخل مهمة القصة، ثم 4+، المضيف هاتف لاعب، LAN وZeroTier/Tailscale وعبر الإنترنت دون سيرفر ألعاب مخصص، الفردي محفوظ.
اقرأ بالترتيب:
https://github.com/alwansan/BO/blob/experiment/black-coop/AGENTS.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/MEMORY.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/STATE.json
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/PLAN_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/ISSUES_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/prompt/SESSION_PROTOCOL_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/docs/SOURCE_MAP_AR.md
https://github.com/alwansan/BO/blob/experiment/black-coop/memory/sessions/2026-11-20_bo-coop-02.md
ثم راجع HEAD وActions الحيين https://github.com/alwansan/BO/actions . المصدر المرجعي pinned 8e9c6fc7d570503ea68d039cff16c09339ee6d30: https://github.com/RanggaCasper/Black-Android/tree/8e9c6fc7d570503ea68d039cff16c09339ee6d30
## آخر نقطة
NET-001 run 44808411128 نجح loopback فقط. SRC-001 أضيف له مختبر تحقق split SHA256 وXDVDFS وXBE وELF identity؛ CI لم يتحقق بعد أثناء كتابة هذه الرسالة. راجع أحدث black-coop-source-audit run وأصلح أي فشل دون تجاوز hash. لا لاعبان داخل اللعبة ولا APK جديدة. لا تنشر game assets. إذا نجح audit، حدد صلاحية matching للـXBE، ثم baseline بناء المصدر Windows قبل Hook read-only. راجع source map؛ لا تستخدم offsets من إصدار غير مثبت. إذا تعذر مصدر/توقيع العربية اطلب الملف الضروري فقط.
حدّث هذه الرسالة وSTATE/MEMORY/PLAN/ISSUES وسجل الجلسة في GitHub بعد كل دفعة أو فشل تلقائياً. لا تعد العمل الخلفي دون automation. لا تدّع نجاح اختبار لم يحصل.
