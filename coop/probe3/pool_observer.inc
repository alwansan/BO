// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Included only in Probe3's bo_probe anonymous namespace, after Write().
// No context/guest writes and no original function calls from observations.
std::atomic<uint64_t> pool_reset_sequence{0};
uint64_t pool_bucket=UINT64_MAX;
uint64_t pool_attempts=0,pool_rejected=0;
void ObservePool(const uint8_t* base) noexcept {
    if(!enabled.load(std::memory_order_relaxed)) return;
    HostState preserve;
    try {
        std::unique_lock<std::mutex> held(lock,std::try_to_lock);
        if(!held.owns_lock() || !base || !enabled.load(std::memory_order_relaxed) || output<0) return;
        const uint64_t now=Now();
        if(!budget.Active(now) || pool_bucket==now/1000) return;
        pool_bucket=now/1000; // Limit failures too, not only successful snapshots.
        ++pool_attempts;
        GuestReader read(reinterpret_cast<uintptr_t>(base));
        const uint64_t reset_before=pool_reset_sequence.load(std::memory_order_acquire);
        uint32_t manager=0,manager_after=0,world=0,world_after=0;
        bo_probe3::PoolSnapshot snapshot;
        const bool copied=read.Read(0x2D45D4,world) && world && world==registry.world &&
            read.Read(0x2D1980,manager) && bo_probe3::ReadPool(read,manager,snapshot) &&
            read.Read(0x2D1980,manager_after) && manager==manager_after &&
            read.Read(0x2D45D4,world_after) && world==world_after;
        const uint64_t reset_after=pool_reset_sequence.load(std::memory_order_acquire);
        const bool valid=copied && reset_before==reset_after;
        if(!valid) ++pool_rejected;
        // One additional diagnostic record/sec may use the reserved stream,
        // keeping dense LOD events from starving pool lifetime observations.
        if(!budget.Record(now,true)) return;
        char line[4096];
        int prefix=std::snprintf(line,sizeof(line),
            "{\"event\":\"pool\",\"ms\":%llu,\"world_epoch\":%u,\"reset_sequence\":%llu,"
            "\"valid\":%s,\"attempts\":%llu,\"rejected\":%llu",
            static_cast<unsigned long long>(now-budget.start),registry.epoch,
            static_cast<unsigned long long>(reset_after),valid?"true":"false",
            static_cast<unsigned long long>(pool_attempts),static_cast<unsigned long long>(pool_rejected));
        if(prefix<0 || static_cast<size_t>(prefix)>=sizeof(line)) {End("pool_format");return;}
        size_t used=static_cast<size_t>(prefix);
        const auto append=[&](const char* format,auto... values) {
            const int n=std::snprintf(line+used,sizeof(line)-used,format,values...);
            if(n<0 || static_cast<size_t>(n)>=sizeof(line)-used) return false;
            used+=static_cast<size_t>(n);return true;
        };
        if(valid) {
            if(!append(",\"counter\":%u,\"occupied\":[%u,%u,%u,%u],\"actors\":[",snapshot.counter,
                snapshot.containers[0].occupied,snapshot.containers[1].occupied,
                snapshot.containers[2].occupied,snapshot.containers[3].occupied)) {End("pool_format");return;}
            // Slot is a physical pool index, not a network/player identity.
            // Free slots are listed only by membership: serial/state may be stale.
            for(size_t i=0;i<snapshot.actors.size();++i) {
                const auto& actor=snapshot.actors[i];
                if(!append("%s[%u,%u,%u,%u,%u,%u]",i?",":"",static_cast<unsigned>(i),actor.container,
                           actor.container?actor.serial:0,actor.container?actor.state:0,
                           actor.container?actor.resource320:0,actor.container?actor.render35c:0)) {End("pool_format");return;}
            }
            if(!append("%s","]")) {End("pool_format");return;}
        }
        if(!append("%s","}\n")) {End("pool_format");return;}
        (void)Write(line,used);
    } catch(...) {
        enabled.store(false,std::memory_order_relaxed);
    }
}
