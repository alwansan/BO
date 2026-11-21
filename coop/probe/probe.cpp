// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Optional local observation only. No guest writes, network, actor creation or
// unknown guest calls. Original functions always continue after observation.
#include "coop/probe.h"
#include "coop/probe_core.hpp"
#include "app/app.h"
#include "core/log.h"
#include "xdk/hle.h"
#include <atomic>
#include <chrono>
#include <fcntl.h>
#include <jni.h>
#include <mutex>
#include <sys/syscall.h>

namespace bo_probe {
namespace {
std::atomic<bool> enabled{false};
std::mutex lock;
Registry registry;
Budget budget;
ByteBudget bytes;
std::array<std::array<uint64_t,3>,64> last{};
int output=-1;
bool initialized=false;
uint64_t opened_at=0,failures=0,attempts=0;

uint64_t Now() noexcept {
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
void End(const char* why) noexcept {
    enabled.store(false,std::memory_order_relaxed);
    if(output>=0) {
        char line[256];
        int n=std::snprintf(line,sizeof(line),"{\"event\":\"end\",\"reason\":\"%s\",\"records\":%u,\"attempts\":%llu,\"read_failures\":%llu}\n",
            why,budget.total,static_cast<unsigned long long>(attempts),static_cast<unsigned long long>(failures));
        if(n>0 && static_cast<size_t>(n)<sizeof(line) && bytes.Reserve(static_cast<size_t>(n)))
            (void)::write(output,line,static_cast<size_t>(n));
        ::close(output);output=-1;
    }
    LOGR("HOOK001: capture ended (%s), records=%u read_failures=%llu",why,budget.total,static_cast<unsigned long long>(failures));
}
bool Write(const char* data,size_t n) noexcept {
    if(output<0 || !bytes.Reserve(n)) {End("output_limit");return false;}
    if(::write(output,data,n)!=static_cast<ssize_t>(n)) {End("write_failed");return false;}
    return true;
}
bool Init(uint64_t now) {
    if(initialized) return output>=0;
    initialized=true;opened_at=now;
    auto path=app::ExeDir()/"logs"/"coop_probe.jsonl";
    output=::open(path.string().c_str(),O_CREAT|O_TRUNC|O_WRONLY|O_CLOEXEC,0600);
    if(output<0) {End("open_failed");return false;}
    constexpr char header[]="{\"event\":\"start\",\"probe\":\"HOOK001\",\"scope\":\"read_only_no_multiplayer\",\"reader\":\"self_process_vm_readv\",\"max_records\":2048,\"max_bytes\":1048576,\"max_world_seconds\":180,\"id_limitations\":\"world_epoch_pointer_vtable_correlations_not_proven_lifetimes\"}\n";
    LOGR("HOOK001: optional read-only capture started; local logs/coop_probe.jsonl");
    return Write(header,sizeof(header)-1);
}
void Observe(Kind kind,uint32_t argument,const uint8_t* base) noexcept {
    if(!enabled.load(std::memory_order_relaxed)) return;
    HostState preserve;
    try {
        std::unique_lock<std::mutex> held(lock,std::try_to_lock);
        if(!held.owns_lock() || !enabled.load(std::memory_order_relaxed) || !base) return;
        uint64_t now=Now();
        if(!Init(now)) return;
        if((budget.started && !budget.Active(now)) || now-opened_at>=300000) {End("time_or_record_limit");return;}
        if(!budget.Attempt(now)) return;
        ++attempts;
        GuestReader read(reinterpret_cast<uintptr_t>(base));
        uint32_t world=0;
        if(!read.Read(0x2D45D4,world)) {
            ++failures;
            if(errno==EPERM || errno==ENOSYS || failures>=64) End("world_read_unavailable");
            return;
        }
        if(world!=registry.world) {registry.World(world);last={};}
        if(!world || uint64_t(world)+0x30>UINT32_MAX) return;
        budget.Start(now);
        uint32_t object=argument,controller=0;
        if(kind!=Kind::Player && !read.Field(argument,4,object)) {++failures;return;}
        if(kind==Kind::Animation) {
            controller=object;
            if(!read.Read(controller,object)) {++failures;return;}
        }
        uint32_t vtable=0;
        if(!read.Read(object,vtable)) {++failures;return;}
        // Exact image .rdata vicinity; this rejects non-object/scalar guesses.
        // It is a conservative observation filter, not a class/lifetime proof.
        if(vtable<0x282FE0 || vtable>=0x2964E0) return;
        uint32_t id=registry.Id(object,vtable);
        if(!id) return;
        auto index=static_cast<size_t>(kind);
        // Rotate eligible IDs rather than sampling only the first rendered NPC.
        if(kind!=Kind::Player && id%8!=(now/1000)%8) return;
        if(last[id-1][index] && now-last[id-1][index]<1000) return;
        Sample sample;
        sample.kind=kind;sample.ms=now-budget.start;
        sample.thread=static_cast<uint32_t>(::syscall(SYS_gettid));
        sample.epoch=registry.epoch;sample.id=id;sample.vtable=vtable;
        sample.local=object==world+0x30;
        if(kind==Kind::Animation) {
            std::array<char,13> name{};
            if(read.Field(controller,0x5C,name)) std::copy(name.begin(),name.end(),sample.name.begin());
            uint32_t clip=0;
            if(read.Field(controller,0x54,clip) && clip) (void)read.Field(clip,0x18,sample.clip_hash);
        } else {
            struct Position {float x,y,z;} position{};
            if(!read.Field(object,0xA0,position) || !ValidPosition(position.x,position.y,position.z)) {++failures;return;}
            sample.x=position.x;sample.y=position.y;sample.z=position.z;sample.position_valid=true;
            (void)read.Field(object,0xC4,sample.kind_value);
            if(kind==Kind::Player) {
                // Entry can be called on another object; only local identity
                // enables the source-confirmed player health fields.
                if(sample.local) {
                    float health=0;
                    if(read.Field(object,0x2F0,health) && std::isfinite(health)) {sample.health=health;sample.health_valid=true;}
                    (void)read.Field(object,0x384,sample.dying);
                }
            } else {
                sample.resource_valid=read.Field(object,0x320,sample.resource320);
                sample.render_valid=read.Field(object,0x35C,sample.render35c);
            }
        }
        char line[1024];
        size_t n=Format(sample,line,sizeof(line));
        if(!n || !budget.Record(now)) return;
        if(Write(line,n)) last[id-1][index]=now;
    } catch(...) {
        // Do not unwind through guest wrappers or skip original game functions.
        enabled.store(false,std::memory_order_relaxed);
    }
}
} // namespace
void ObservePlayer(uint32_t object,const uint8_t* base) noexcept {Observe(Kind::Player,object,base);}
void ObserveAnimation(uint32_t stack,const uint8_t* base) noexcept {Observe(Kind::Animation,stack,base);}
void ObserveLod(uint32_t stack,const uint8_t* base) noexcept {Observe(Kind::Lod,stack,base);}
void Enable() noexcept {enabled.store(true,std::memory_order_relaxed);}
} // namespace bo_probe

extern "C" JNIEXPORT void JNICALL Java_com_racas_black_BlackActivity_nativeCoopProbeEnable(JNIEnv*,jclass) {
    bo_probe::Enable();
}
// Independently checked entry: first stack argument is the object, ret 4.
// CI must verify this generated original definition before compiling/linking.
X86_EXTERN_FUNC(__imp__sub_001C55A0);
X86_FUNC(sub_001C55A0) {
    bo_probe::ObserveLod(ctx.esp,base);
    __imp__sub_001C55A0(ctx,base);
}
