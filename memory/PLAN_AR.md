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

SRC-002a PASS: v1.5.0 metadata and compile image identity matched official ELF. SRC-002 baseline BLOCKED on exact XBE. See memory/evidence/SRC-002a-release-comparison-44817699656.json. NET-002 isolated native lab may proceed independently.

## مرحلة NET-003 (21 نوفمبر)
| Gate | النتيجة | الدليل |
|---|---|---|
| NET-003a codec/game-neutral Host | PASS Linux ASAN/UBSAN UDP loopback | [44833884316](https://github.com/alwansan/BO/actions/runs/44833884316) |
| NET-003b CLI two processes | PASS Linux loopback | [44834027711](https://github.com/alwansan/BO/actions/runs/44834027711) |
| NET-003c Android ARM64 compile | PASS cross compile, **device untested** | artifact 14677732935 |
| NET-004 device LAN/ZeroTier | OPEN | no Android runtime test |
| NET-005 authenticated reliable transport | OPEN | no production-grade crypto/reliability |
| HOOK-001/ACTOR-001 | BLOCKED by exact game baseline | original XBE missing |
