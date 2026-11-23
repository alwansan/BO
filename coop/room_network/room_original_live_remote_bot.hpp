// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Experimental original BLACK soldier LOOK (NOT NPC brain) for EACH OTHER
// authenticated human in a 2–4 person original story level.
// Pinned MIT PC source: portingpete/Black-2006-Recomp@b2512fafd,
// src/recomp_manual.c black_puppet_make/follow/sub_000D1500.
// Owner XBE SHA1 c5bf... matches SAME PC source supported XBE SHA256,
// verified privately BO CI #45067068552. Runtime ABI/visual remains UNTESTED.
// FAIL-CLOSED: source-matched original scene, COMMITTED TLS owner, exact
// 2–4 human source frame, native same thread, valid unused original slot;
// no NPC controller, AI, quest, collision list, enemy/weapon ownership.
#pragma once
#include "coop/room_other_human_body_source_witness.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include "coop/room_original_remote_human_motion_contract.hpp"
#include "coop/room_original_eye_contract.hpp"
#include "coop/room_original_soldier_copy.hpp"
#include "game/layout.h"
#include "kernel/kernel.h"
#include "kernel/memory.h"
#include "xdk/hle.h"
#include <SDL3/SDL.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <thread>
namespace dispatch{X86Func* Find(uint32_t);}
namespace blackcoop {
class OriginalHumanBotVisual {
    static constexpr uint32_t kPool=0x002D1980u;
    static constexpr uint32_t kStride=0x3C0u,kFirst=0xA0u;
    uint32_t sourceWorld_=0,sourceEpisode_=0;
    uint32_t replica_=0,replicaTemplate_=0,replicaInstance_=0;
    uint32_t replicaEpoch_=0;
    OriginalSoldierSlotIdentity slotIdentity_{};
    OriginalSoldierAttemptLifetime cloneLifetime_{};
    uint64_t sourceAtMs_=0;
    std::thread::id gameThread_{};
    bool owner_=false;
    bool attempted_=false;
    uint64_t nextCreateTryMs_=0;
    bool logged_=false;
    uint8_t assignedPeer_=0;
    OriginalRemoteHumanMotionPlan clipPlan_{};
    uint32_t clipNames_=0;
    unsigned activeClip_=0;
    uint32_t visualCodesReported_=0;
    // One safe, coarse category per original stage/peer; NO guest pointers,
    // remote coordinates, network IDs, model bytes or private assets in logs.
    void reportVisualGate(unsigned code)noexcept{
        if(code<2200u||code>2210u)return;
        const uint32_t bit=1u<<(code-2200u);
        if((visualCodesReported_&bit)!=0u)return;
        visualCodesReported_|=bit;
        SendSafeWorldCodeToAndroid(static_cast<jint>(code));
    }
    static void ClearOriginalSoldierCallRegisters(X86Context& call)noexcept{
        // Pinned MIT PC black_guest_call creates a ZERO BlackRegs record
        // (eax/ecx/edx/ebx/esi/edi), preserving the actual stack/ebp.
        // Avoid accidentally passing registers from grid draw to an original
        // game model/animation entry point with undocumented side effects.
        call.eax=call.ecx=call.edx=call.ebx=call.esi=call.edi=0;
    }
    void applyOriginalSoldierAnimation(X86Context& ctx,
        const OriginalOtherHumanBody& body,uint64_t now)noexcept{
        clipPlan_.Observe(body.epoch,body.revision,body.tick,
                          body.x,body.z,now);
        const unsigned desired=clipPlan_.DesiredClip();
        // The PC original BLACK soldier's animation-entry ABI is pinned:
        // eax=character model instance; stack word=clip-name guest pointer.
        // Never try another source function or play an unverified clip.
        if(!desired||desired==activeClip_||
           !dispatch::Find(0x0017E130u)||!replicaInstance_)return;
        static constexpr char kSoldierClips[32]=
            "TM_AR_S_001\0\0\0\0\0TM_AR_S_MOV\0";
        if(!clipNames_||!OriginalGuestSpanCommitted(clipNames_,32u)){
            clipNames_=mem::AllocSystem(sizeof(kSoldierClips),16u);
            if(!clipNames_||!OriginalGuestSpanCommitted(clipNames_,32u)){
                clipNames_=0;return;
            }
            std::memcpy(kernel::HostPtr(clipNames_),
                        kSoldierClips,sizeof(kSoldierClips));
        }
        X86Context call=ctx;
        ClearOriginalSoldierCallRegisters(call);
        call.eax=replicaInstance_;
        kernel::CallGuest(call,0x0017E130u,
                          {clipNames_+(desired==2u?16u:0u)});
        activeClip_=desired; // changes only for genuine source motion
    }
    static uint32_t fp(float v)noexcept{
        uint32_t bits=0;std::memcpy(&bits,&v,sizeof(bits));return bits;
    }
    void purgeEvidence()noexcept{
        sourceWorld_=sourceEpisode_=replica_=replicaTemplate_=replicaInstance_=replicaEpoch_=0;
        sourceAtMs_=nextCreateTryMs_=0;attempted_=logged_=false;
        slotIdentity_={};
        clipPlan_.Reset();activeClip_=0;
    }
    static bool read(uint32_t at,uint32_t& out)noexcept{
        if(!OriginalGuestSpanCommitted(at,4))return false;
        out=kernel::R32(at);return true;
    }
    static uint32_t character(uint32_t root,uint32_t slot)noexcept{
        return root+kFirst+kStride*slot;
    }
    bool valid(uint32_t root)const noexcept{
        if(!slotIdentity_.InPool(root)||!replica_||!replicaTemplate_||!replicaInstance_||
           !OriginalGuestSpanCommitted(root+kFirst,kStride*32u))return false;
        uint32_t model=0,ctrl=0,inst=0,serial=0,member=0,owner=0;
        return read(replica_+0x320,model)&&model==replicaTemplate_&&
               read(replica_+0x324,ctrl)&&!ctrl&&
               read(replica_+0x328,inst)&&inst==replicaInstance_&&
               read(replica_+0x374u,serial)&&read(replica_+0x250u,member)&&
               slotIdentity_.Matches(root,serial,member,inst)&&
               OriginalGuestSpanCommitted(inst,0x240u)&&
               read(inst,owner)&&owner==replica_;
    }
    // Makes exactly ONE appearance-only character, in a vacant original slot,
    // and never inserts that slot into original world AI/collision lists.
    bool create(X86Context& ctx,uint32_t root,uint32_t epoch)noexcept{
        // Pool/model/part buffers may load after the first stable scene
        // sample. Retry read-only eligibility at MOST once per 2s. Once any
        // engine character memory is touched, NEVER silently retry a failed
        // partial clone into a second slot.
        if(attempted_)return false;
        const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
        if(nextCreateTryMs_&&now<nextCreateTryMs_)return false;
        nextCreateTryMs_=now+2000u;
        OriginalBotPoolAvailability result{};
        std::array<OriginalBotPoolSlotEvidence,32> slots{};
        if(!ReadOriginalBotPoolAvailability(result,&slots)){
            reportVisualGate(2205);return false;
        }
        if(!result.sourceLooks||!result.emptyVisualSlots){
            reportVisualGate(2206);return false;
        }
        uint32_t donor=0,blank=0;
        for(uint32_t i=0;i<32u;i++)if(OriginalLookSource(slots[i])){
            donor=character(root,i);break;
        }
        for(uint32_t i=32u;i-->0;)if(OriginalLookEmpty(slots[i])&&
           OriginalFreeSoldierSlot(read,root,character(root,i))){
            blank=character(root,i);break;
        }
        if(!donor||!blank||blank==donor){
            reportVisualGate(2206);return false;
        }
        if(!dispatch::Find(0x0017E420u)||
           !dispatch::Find(0x001801B0u)){
            reportVisualGate(2207);return false;
        }
        uint32_t model=0,modelType=0,ptr=0,dstInst=0;
        uint32_t donorA=0,donorB=0,dstA=0,dstB=0,origin=0;
        // The pinned MIT PC original puppet does not cap modelType at 255:
        // only the computed, 64-bit checked original descriptor address and
        // committed page span may authorize a source template. The former
        // arbitrary 255 restriction could silently reject valid soldiers.
        if(!read(donor+0x320,model)||!model||
           !read(model+8u,modelType)||
           !read(donor+0x34Cu,donorA)||!read(donor+0x350u,donorB)||
           !read(blank+0x328u,dstInst)||!read(blank+0x34Cu,dstA)||
           !read(blank+0x350u,dstB)||
           !read(0x002D12C8u,origin)||!origin){
            reportVisualGate(2208);return false;
        }
        const uint64_t argsAddr=static_cast<uint64_t>(origin)+0xF8u+
                                static_cast<uint64_t>(modelType)*0x60u;
        if(argsAddr>0xffffffffull||
           !OriginalGuestSpanCommitted(static_cast<uint32_t>(argsAddr),0x60u)||
           !OriginalGuestSpanCommitted(dstInst,0x240u)||
           !OriginalGuestSpanCommitted(donorA,0x70u)||
           !OriginalGuestSpanCommitted(donorB,0x3C0u)||
           !OriginalGuestSpanCommitted(dstA,0x70u)||
           !OriginalGuestSpanCommitted(dstB,0x3C0u)){
            reportVisualGate(2208);return false;
        }
        // One last read-only vacancy check immediately before any writes.
        uint32_t currentModel=0,controller=0,currentInst=0,slotSerial=0,stillRoot=0;
        if(!read(kPool,stillRoot)||stillRoot!=root||
           !OriginalFreeSoldierSlot(read,root,blank)||
           !read(blank+0x374u,slotSerial)||
           !read(blank+0x320,currentModel)||currentModel||
           !read(blank+0x324,controller)||controller||
           !read(blank+0x328,currentInst)||currentInst!=dstInst){
            reportVisualGate(2206);return false;
        }
        attempted_=true; // irreversible original guest-memory mutation below
        // Do not copy list/proxy/weapon/buffers that the blank slot owns.
        CopyOriginalSoldierLookWords(
            [donor](uint32_t off){return kernel::R32(donor+off);},
            [blank](uint32_t off,uint32_t value){kernel::W32(blank+off,value);});
        kernel::W32(blank+0x30u,0);
        kernel::W32(blank+0x34u,blank+0x34u);
        kernel::W32(blank+0x38u,blank+0x34u);
        kernel::W32(blank+0xB0u,0);kernel::W32(blank+0xB4u,0);
        kernel::W32(blank+0x324u,0);
        kernel::W32(blank+0x29Cu,0);kernel::W32(blank+0x2A0u,0);
        kernel::G<uint8_t>(blank+0x13Bu)=0;
        std::memcpy(kernel::HostPtr(dstA),kernel::HostPtr(donorA),0x70u);
        std::memcpy(kernel::HostPtr(dstB),kernel::HostPtr(donorB),0x3C0u);
        X86Context call=ctx;
        ClearOriginalSoldierCallRegisters(call);
        call.eax=blank;call.esi=dstInst;
        kernel::CallGuest(call,0x0017E420u,{static_cast<uint32_t>(argsAddr)});
        replica_=blank;replicaTemplate_=model;replicaInstance_=dstInst;
        replicaEpoch_=epoch;
        slotIdentity_={root,blank,slotSerial,dstInst};
        if(!valid(root)){
            // The original guest function may have changed an object after
            // our irreversible clone; do not retry a partially owned slot.
            cloneLifetime_.Poison();
            reportVisualGate(2209);return false;
        }
        return true;
    }
public:
    void BindPeer(uint8_t peer)noexcept{
        if(!assignedPeer_&&peer>=1&&peer<=4)assignedPeer_=peer;
    }
    void ObserveScene(uint32_t episode,uint32_t world,bool independentlyRunning)noexcept{
        if(!owner_){gameThread_=std::this_thread::get_id();owner_=true;}
        if(gameThread_!=std::this_thread::get_id())return;
        if(!independentlyRunning||!episode||!world){
            sourceWorld_=sourceEpisode_=0;sourceAtMs_=0;
            return;
        }
        cloneLifetime_.Observe(episode,world);
        if(sourceWorld_!=world||sourceEpisode_!=episode){
            purgeEvidence();visualCodesReported_=0;
        }
        sourceWorld_=world;sourceEpisode_=episode;
        sourceAtMs_=static_cast<uint64_t>(SDL_GetTicks());
    }
    void Draw(X86Context& ctx,uint32_t grid)noexcept{
        if(cloneLifetime_.Failed())return;
        if(!assignedPeer_||!owner_||!sourceWorld_||!sourceEpisode_)
            return;
        if(gameThread_!=std::this_thread::get_id()){
            reportVisualGate(2201);return;
        }
        const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
        if(now<sourceAtMs_||now-sourceAtMs_>350)return;
        const uint32_t world=kernel::R32(layout::kWorld);
        if(world!=sourceWorld_||
           static_cast<uint64_t>(world)+0x4960u>0xffffffffull||
           grid!=world+0x4960u){
            reportVisualGate(2202);return;
        }
        // A source-matched original draw has fired; this does not prove
        // that the other human is actually visible to any player.
        const uint8_t self=AllOtherHumanBodies().Self();
        if(self==0u){reportVisualGate(2203);return;}
        if(assignedPeer_==self)return;
        reportVisualGate(2200);
        OriginalOtherHumanBody body{};
        if(!AllOtherHumanBodies().Read(assignedPeer_,now,body)||body.epoch==0||
           !body.revision||body.tick==0){
            reportVisualGate(2204);return;
        }
        uint32_t root=0;
        if(!read(kPool,root)||!root||
           static_cast<uint64_t>(root)+kFirst+kStride*32u>0xffffffffull||
           !OriginalGuestSpanCommitted(root+kFirst,kStride*32u)){
            reportVisualGate(2205);return;
        }
        if(replica_&&(!valid(root)||replicaEpoch_!=body.epoch)){
            // Never overwrite or rescue a slot taken back by NPC engine.
            // No undocumented force-release: only stop drawing it.
            // The drawing hook is independent from the resource file
            // witness translation unit. Preserve only this already-verified
            // original launch episode; never infer an episode from a draw.
            const uint32_t existingEpisode=sourceEpisode_;
            purgeEvidence();
            sourceWorld_=world;sourceEpisode_=existingEpisode;
            sourceAtMs_=now;
            reportVisualGate(2209);
            return;
        }
        if(!replica_){
            if(!create(ctx,root,body.epoch))return;
            SendSafeWorldCodeToAndroid(90); // constructed, NOT on-screen proof
        }
        const uint32_t player=world+layout::kWorldPlayer;
        if(!OriginalGuestSpanCommitted(player+0x70u,0x40u)||
           !valid(root))return;
        for(uint32_t off=0x70u;off<0xB0u;off+=4u){
            uint32_t v=kernel::R32(player+off);
            if((off&0xFu)==0xCu&&off<0xA0u)v=0;
            if(off==0xA0u)v=fp(body.x/100.0f);
            if(off==0xA4u)v=fp(body.y/100.0f);
            if(off==0xA8u)v=fp(body.z/100.0f);
            kernel::W32(replica_+off,v);
            kernel::W32(replica_+off+0x60u,v);
        }
        // The original forward +0xF0 also drives first-person ballistics.
        // Rotate the detached soldier's source matrix horizontal basis into
        // the other human's signed source yaw, instead of showing everybody
        // looking in the local player's direction. No model/AI ownership.
        int32_t localYaw=0;
        if(OriginalGuestSpanCommitted(player+0xF0u,12u)&&
           QuantizeOriginalForwardYaw(
             kernel::G<float>(player+0xF0u),kernel::G<float>(player+0xF8u),localYaw)){
            for(uint32_t off=0x70u;off<=0x90u;off+=0x10u){
                float nx=0,nz=0;
                if(!RotateOriginalSoldierHorizontalBasis(
                     kernel::G<float>(player+off),
                     kernel::G<float>(player+off+8u),
                     localYaw,body.yaw,nx,nz))break;
                kernel::W32(replica_+off,fp(nx));
                kernel::W32(replica_+off+8u,fp(nz));
                kernel::W32(replica_+off+0x60u,fp(nx));
                kernel::W32(replica_+off+0x68u,fp(nz));
            }
        }
        // One genuine source-sampled idle/walk transition controls only
        // this detached original soldier's animations. It still has no AI
        // brain, physics proxy, weapons, collision or mission authority.
        applyOriginalSoldierAnimation(ctx,body,now);
        X86Context call=ctx;
        ClearOriginalSoldierCallRegisters(call);
        call.eax=replicaInstance_;
        kernel::CallGuest(call,0x001801B0u,{fp(1.0f/30.0f),0});
        if(!valid(root))return;
        uint32_t vtable=0,method=0;
        if(!read(replica_,vtable)||!vtable||
           !read(vtable+0x14u,method)||!method||
           !dispatch::Find(method)){
            reportVisualGate(2210);return;
        }
        call=ctx;
        ClearOriginalSoldierCallRegisters(call);
        call.ecx=replica_;
        kernel::CallGuest(call,method,{0});
        if(!logged_){
            logged_=true;SendSafeWorldCodeToAndroid(91);
        }
    }
};
inline OriginalHumanBotVisual& OriginalRemoteHumanBotVisual(uint8_t peer)noexcept{
    static std::array<OriginalHumanBotVisual,5> bodies{};
    if(peer<1||peer>4)peer=0;
    bodies[peer].BindPeer(peer);
    return bodies[peer];
}
} // namespace blackcoop
