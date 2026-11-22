// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Experimental original BLACK soldier LOOK (NOT NPC brain) for the OTHER
// authenticated human on 2-phone original level_00/stg_0001.
// Pinned MIT PC source: portingpete/Black-2006-Recomp@b2512fafd,
// src/recomp_manual.c black_puppet_make/follow/sub_000D1500.
// Owner XBE SHA1 c5bf... matches SAME PC source supported XBE SHA256,
// verified privately BO CI #45067068552. Runtime ABI/visual remains UNTESTED.
// FAIL-CLOSED: source-matched original scene, COMMITTED TLS owner, exact
// two-human frame, native same thread, valid unused original slot;
// no NPC controller, AI, quest, collision list, enemy/weapon ownership.
#pragma once
#include "coop/room_other_human_body_source_witness.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
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
    uint64_t sourceAtMs_=0;
    std::thread::id gameThread_{};
    bool owner_=false;
    bool attempted_=false;
    bool logged_=false;
    static uint32_t fp(float v)noexcept{
        uint32_t bits=0;std::memcpy(&bits,&v,sizeof(bits));return bits;
    }
    void purgeEvidence()noexcept{
        sourceWorld_=sourceEpisode_=replica_=replicaTemplate_=replicaInstance_=replicaEpoch_=0;
        sourceAtMs_=0;attempted_=logged_=false;
    }
    static bool read(uint32_t at,uint32_t& out)noexcept{
        if(!OriginalGuestSpanCommitted(at,4))return false;
        out=kernel::R32(at);return true;
    }
    static uint32_t character(uint32_t root,uint32_t slot)noexcept{
        return root+kFirst+kStride*slot;
    }
    bool valid(uint32_t root)const noexcept{
        if(!replica_||!replicaTemplate_||!replicaInstance_||
           !OriginalGuestSpanCommitted(root+kFirst,kStride*32u))return false;
        uint32_t model=0,ctrl=0,inst=0;
        return read(replica_+0x320,model)&&model==replicaTemplate_&&
               read(replica_+0x324,ctrl)&&!ctrl&&
               read(replica_+0x328,inst)&&inst==replicaInstance_&&
               OriginalGuestSpanCommitted(inst,0x240u);
    }
    // Makes exactly ONE appearance-only character, in a vacant original slot,
    // and never inserts that slot into original world AI/collision lists.
    bool create(X86Context& ctx,uint32_t root,uint32_t epoch)noexcept{
        if(attempted_)return false;
        attempted_=true;
        OriginalBotPoolAvailability result{};
        std::array<OriginalBotPoolSlotEvidence,32> slots{};
        if(!ReadOriginalBotPoolAvailability(result,&slots)||
           !result.sourceLooks||!result.emptyVisualSlots)return false;
        uint32_t donor=0,blank=0;
        for(uint32_t i=0;i<32u;i++)if(OriginalLookSource(slots[i])){
            donor=character(root,i);break;
        }
        for(uint32_t i=32u;i-->0;)if(OriginalLookEmpty(slots[i])){
            blank=character(root,i);break;
        }
        if(!donor||!blank||blank==donor||
           !dispatch::Find(0x0017E420u)||
           !dispatch::Find(0x001801B0u))return false;
        uint32_t model=0,modelType=0,ptr=0,dstInst=0;
        uint32_t donorA=0,donorB=0,dstA=0,dstB=0,origin=0;
        if(!read(donor+0x320,model)||!model||
           !read(model+8u,modelType)||modelType>255u||
           !read(donor+0x34Cu,donorA)||!read(donor+0x350u,donorB)||
           !read(blank+0x328u,dstInst)||!read(blank+0x34Cu,dstA)||
           !read(blank+0x350u,dstB)||
           !read(0x002D12C8u,origin)||!origin)
            return false;
        const uint64_t argsAddr=static_cast<uint64_t>(origin)+0xF8u+
                                static_cast<uint64_t>(modelType)*0x60u;
        if(argsAddr>0xffffffffull||
           !OriginalGuestSpanCommitted(static_cast<uint32_t>(argsAddr),0x60u)||
           !OriginalGuestSpanCommitted(dstInst,0x240u)||
           !OriginalGuestSpanCommitted(donorA,0x70u)||
           !OriginalGuestSpanCommitted(donorB,0x3C0u)||
           !OriginalGuestSpanCommitted(dstA,0x70u)||
           !OriginalGuestSpanCommitted(dstB,0x3C0u))return false;
        // One last read-only vacancy check immediately before any writes.
        uint32_t currentModel=0,controller=0,currentInst=0;
        if(!read(blank+0x320,currentModel)||currentModel||
           !read(blank+0x324,controller)||controller||
           !read(blank+0x328,currentInst)||currentInst!=dstInst)return false;
        // Do not copy list/proxy/weapon/buffers that the blank slot owns.
        for(uint32_t off=0;off<kStride;off+=4u){
            const bool own=(off>=0x20u&&off<0x58u)||
                  off==0xB0u||off==0xB4u||
                  (off>=0x298u&&off<0x2A4u)||
                  off==0x328u||off==0x34Cu||off==0x350u||
                  off==0x37Cu;
            if(!own)kernel::W32(blank+off,kernel::R32(donor+off));
        }
        kernel::W32(blank+0x30u,0);
        kernel::W32(blank+0x34u,blank+0x34u);
        kernel::W32(blank+0x38u,blank+0x34u);
        kernel::W32(blank+0xB0u,0);kernel::W32(blank+0xB4u,0);
        kernel::W32(blank+0x324u,0);
        kernel::W32(blank+0x29Cu,0);kernel::W32(blank+0x2A0u,0);
        kernel::G<uint8_t>(blank+0x13Bu)=0;
        std::memcpy(kernel::HostPtr(dstA),kernel::HostPtr(donorA),0x70u);
        std::memcpy(kernel::HostPtr(dstB),kernel::HostPtr(donorB),0x3C0u);
        X86Context saved=ctx;
        ctx.eax=blank;ctx.esi=dstInst;
        kernel::CallGuest(ctx,0x0017E420u,{static_cast<uint32_t>(argsAddr)});
        ctx=saved;
        replica_=blank;replicaTemplate_=model;replicaInstance_=dstInst;
        replicaEpoch_=epoch;
        return valid(root);
    }
public:
    void ObserveScene(uint32_t episode,uint32_t world,bool independentlyRunning)noexcept{
        if(!owner_){gameThread_=std::this_thread::get_id();owner_=true;}
        if(gameThread_!=std::this_thread::get_id())return;
        if(!independentlyRunning||!episode||!world){
            sourceWorld_=sourceEpisode_=0;sourceAtMs_=0;
            return;
        }
        if(sourceWorld_!=world||sourceEpisode_!=episode)purgeEvidence();
        sourceWorld_=world;sourceEpisode_=episode;
        sourceAtMs_=static_cast<uint64_t>(SDL_GetTicks());
    }
    void Draw(X86Context& ctx,uint32_t grid)noexcept{
        if(!owner_||gameThread_!=std::this_thread::get_id()||!sourceWorld_||
           !sourceEpisode_)return;
        const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
        if(now<sourceAtMs_||now-sourceAtMs_>350)return;
        const uint32_t world=kernel::R32(layout::kWorld);
        if(world!=sourceWorld_||
           static_cast<uint64_t>(world)+0x4960u>0xffffffffull||
           grid!=world+0x4960u)return;
        OriginalOtherHumanBody body{};
        if(!OtherHumanBodySource().Read(now,body)||body.epoch==0||
           !body.revision||body.tick==0)return;
        uint32_t root=0;
        if(!read(kPool,root)||!root||
           static_cast<uint64_t>(root)+kFirst+kStride*32u>0xffffffffull||
           !OriginalGuestSpanCommitted(root+kFirst,kStride*32u))
            return;
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
        X86Context saved=ctx;
        ctx.eax=replicaInstance_;
        kernel::CallGuest(ctx,0x001801B0u,{fp(1.0f/30.0f),0});
        ctx=saved;
        if(!valid(root))return;
        uint32_t vtable=0,method=0;
        if(!read(replica_,vtable)||!vtable||
           !read(vtable+0x14u,method)||!method||
           !dispatch::Find(method))return;
        ctx.ecx=replica_;ctx.esi=0;
        kernel::CallGuest(ctx,method,{0});
        ctx=saved;
        if(!logged_){
            logged_=true;SendSafeWorldCodeToAndroid(91);
        }
    }
};
inline OriginalHumanBotVisual& OriginalRemoteHumanBotVisual()noexcept{
    static OriginalHumanBotVisual body;return body;
}
} // namespace blackcoop
