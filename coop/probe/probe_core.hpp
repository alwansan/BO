// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <cmath>
#include <charconv>
#include <cfenv>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <type_traits>
#include <sys/uio.h>
#include <unistd.h>

namespace bo_probe {
// Sampling must not leak host errno or floating-point flags into original code.
class HostState {
    int error_=errno;
    fenv_t env_{};
    bool valid_=std::fegetenv(&env_)==0;
public:
    ~HostState() noexcept {if(valid_) std::fesetenv(&env_);errno=error_;}
};
// Kernel copy returns a short read/EFAULT for invalid/protected remote pages.
// Never dereference a guest-derived host pointer directly or modify the output
// after a failed/partial read. This reads only our own process.
class GuestReader {
    uintptr_t base_;
public:
    explicit GuestReader(uintptr_t base):base_(base) {}
    template<class T> bool Read(uint32_t va,T& out) const noexcept {
        static_assert(std::is_trivially_copyable_v<T> && sizeof(T)<=256);
        if(va<0x10000 || uint64_t(va)+sizeof(T)>(UINT64_C(1)<<32) ||
           base_>std::numeric_limits<uintptr_t>::max()-va ||
           base_+va>std::numeric_limits<uintptr_t>::max()-(sizeof(T)-1)) return false;
        T temporary{};
        iovec local{&temporary,sizeof(T)};
        iovec remote{reinterpret_cast<void*>(base_+va),sizeof(T)};
        if(process_vm_readv(getpid(),&local,1,&remote,1,0)!=static_cast<ssize_t>(sizeof(T))) return false;
        out=temporary;
        return true;
    }
    template<class T> bool Field(uint32_t object,uint32_t offset,T& out) const noexcept {
        if(uint64_t(object)+offset>UINT32_MAX) return false;
        return Read(static_cast<uint32_t>(object+offset),out);
    }
};

struct Registry {
    struct Slot { uint32_t address=0,vtable=0; };
    std::array<Slot,64> slots{};
    uint32_t world=0,epoch=0;
    void World(uint32_t value) noexcept {
        if(world==value) return;
        world=value; ++epoch; slots={};
    }
    // Preserve one slot for the local player. Render/animation hooks can see
    // many NPCs before the first player update; otherwise the 64-ID cap could
    // prevent every later local-player sample. IDs are correlations, not
    // persistent actor identities or lifetime-proof handles.
    uint32_t Id(uint32_t address,uint32_t vtable,bool local=false) noexcept {
        if(!address || !vtable) return 0;
        for(size_t i=0;i<slots.size();++i)
            if(slots[i].address==address && slots[i].vtable==vtable) return static_cast<uint32_t>(i+1);
        if(local) {
            // A respawn in the same world may re-use ID 1. Consumers must not
            // treat these numbers as stable across character lifetimes.
            slots[0]={address,vtable};
            return 1;
        }
        // Non-local objects cannot consume the player's reserved slot.
        for(size_t i=1;i<slots.size();++i)
            if(!slots[i].address) {slots[i]={address,vtable}; return static_cast<uint32_t>(i+1);}
        return 0;
    }
};

struct Budget {
    bool started=false;
    uint64_t start=0,second=0;
    uint32_t attempts=0,records=0,total=0;
    void Start(uint64_t now) noexcept { if(!started) {started=true;start=now;second=now/1000;} }
    bool Active(uint64_t now) const noexcept {return started && now>=start && now-start<180000 && total<2048;}
    void Roll(uint64_t now) noexcept {
        if(second!=now/1000) {second=now/1000;attempts=0;records=0;}
    }
    bool Attempt(uint64_t now) noexcept {
        if(started && !Active(now)) return false;
        Roll(now);
        if(attempts>=256) return false;
        ++attempts;return true;
    }
    bool Record(uint64_t now) noexcept {
        if(!Active(now)) return false;
        Roll(now);
        if(records>=32) return false;
        ++records;++total;return true;
    }
};
struct ByteBudget {
    size_t used=0;
    bool Reserve(size_t n) noexcept {
        if(!n || n>1024*1024-used) return false;
        used+=n;return true;
    }
};
inline bool ValidPosition(float x,float y,float z) noexcept {
    return std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z)&&
        std::abs(x)<=1e7f && std::abs(y)<=1e7f && std::abs(z)<=1e7f;
}
enum class Kind:uint8_t { Player=0,Lod=1,Animation=2 };
struct Sample {
    Kind kind=Kind::Player;
    uint64_t ms=0;
    uint32_t thread=0,epoch=0,id=0,vtable=0,kind_value=0,dying=0;
    uint32_t resource320=0,render35c=0,clip_hash=0;
    float x=0,y=0,z=0,health=0;
    bool local=false,position_valid=false,health_valid=false,resource_valid=false,render_valid=false;
    std::array<char,14> name{};
};
inline size_t Format(const Sample& s,char* out,size_t cap) noexcept {
    const auto kind=static_cast<unsigned>(s.kind);
    if(kind>2 || !ValidPosition(s.x,s.y,s.z) || !std::isfinite(s.health)) return 0;
    const char* names[]={"player","lod","animation"};
    // to_chars is locale independent; JSON decimal points must stay dots.
    char numbers[4][40]{};
    const float values[]={s.x,s.y,s.z,s.health};
    for(size_t i=0;i<4;++i) {
        auto r=std::to_chars(numbers[i],numbers[i]+39,values[i]);
        if(r.ec!=std::errc{}) return 0;
    }
    char label[14]{};
    for(size_t i=0;i<13 && s.name[i];++i) {
        char c=s.name[i];
        label[i]=((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.')?c:'?';
    }
    int n=std::snprintf(out,cap,
        "{\"event\":\"%s\",\"ms\":%llu,\"thread\":%u,\"epoch\":%u,\"id\":%u,\"local\":%s,"
        "\"vtable\":%u,\"kind_value\":%u,\"position_valid\":%s,\"position\":[%s,%s,%s],"
        "\"health_valid\":%s,\"health\":%s,\"dying\":%u,\"resource_valid\":%s,\"resource320\":%u,"
        "\"render_valid\":%s,\"render35c\":%u,\"clip_hash\":%u,\"anim_name\":\"%s\"}\n",
        names[kind],static_cast<unsigned long long>(s.ms),s.thread,s.epoch,s.id,s.local?"true":"false",
        s.vtable,s.kind_value,s.position_valid?"true":"false",numbers[0],numbers[1],numbers[2],
        s.health_valid?"true":"false",numbers[3],s.dying,s.resource_valid?"true":"false",s.resource320,
        s.render_valid?"true":"false",s.render35c,s.clip_hash,label);
    return n>0 && static_cast<size_t>(n)<cap?static_cast<size_t>(n):0;
}
} // namespace bo_probe
