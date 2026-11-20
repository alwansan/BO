# خطة BO-COOP
| Gate | القبول | الحالة |
|---|---|---|
| NET-001 | عميلان loopback وحالة authoritative مستقلة | PASS 44808411128 |
| SRC-001 | تحقق SHA256/XBE/ELF؛ مطابقة هوية compile image | audit PASS 44813529439، identity BLOCKED (SHA1 mismatch) |
| SRC-002 | إعادة بناء upstream pinned على Windows دون أصول منشورة أو تغييرات gameplay | مفتوح |
| HOOK-001 | تسجيل محلي read-only للحالة دون crash أو تعديل الحفظ | مفتوح |
| ACTOR-001 | spawn + render + move شخصية ثانية دون singleton conflict | مفتوح |
| ACTOR-002 | لاعبان مستقلان وcamera/weapon/collision + cleanup | مفتوح |
| NET-002 | اتصال هاتفين LAN/ZeroTier؛ خسارة/ترتيب/timeout وإعادة اتصال واتصال مصادق | مفتوح |
| COMBAT-001 | authoritative AI/damage/ammo/projectile/props/objectives | مفتوح |
| COOP-001 | مهمة أولى كاملة على هاتفين مع checkpoint | مفتوح |
| COOP-002 | 4 لاعبين وبقية المهام وواجهة غرف وWAN | مفتوح |
قبل كل gate: اختبارات سالبة، نتائج صريحة، لا نسبة اكتمال وهمية. لا بناء APK منشور قبل إثبات الهوية وbaseline.

SRC-002a: provenance/image match قبل SRC-002 baseline. راجع evidence SRC-001.
