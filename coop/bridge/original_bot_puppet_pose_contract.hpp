// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Value-only original BLACK bot LOOK pose contract. No guest writes or calls.
// MIT design reference: portingpete/Black-2006-Recomp@b2512fafd.
#pragma once
#include "original_bot_puppet_plan.hpp"
#include <array>
#include <cstdint>
#include <thread>
namespace bo_bridge {
struct CanonicalHumanPose {
    uint8_t id=0;
    int32_t xCm=0,yCm=0,zCm=0,yawCentiDeg=0,flags=0;
};
struct CanonicalHumanFrame {
    uint32_t sceneEpoch=0,tick=0;
    uint8_t self=0,rosterMask=0;
    uint64_t receiptMs=0; // receiver monotonic time, NEVER sender clock
    std::array<CanonicalHumanPose,5> humans{};
};
struct PuppetPoseValue {
    uint8_t id=0,blankSlot=0,lookSlot=0;
    uint32_t sceneEpoch=0,opaqueModel=0;
    uint64_t humanGeneration=0;
    int32_t xCm=0,yCm=0,zCm=0,yawCentiDeg=0,flags=0;
};
struct PuppetPoseBatch {
    std::array<PuppetPoseValue,3> humans{};
    uint8_t count=0;
    bool originalEngineMemoryWritten=false,originalDrawCalled=false;
};
class OriginalPuppetCanonicalPoseGate final {
    bool active_=false,poisoned_=false,fixed_=false;
    std::thread::id owner_{};
    uint32_t scene_=0,lastTick_=0,model_=0;
    uint64_t story_=0;
    uint8_t self_=0,mask_=0,donor_=0;
    std::array<uint8_t,5> fixedSlots_{};
    std::array<uint64_t,5> fixedGenerations_{};
    bool own()const noexcept{
        return active_&&owner_==std::this_thread::get_id();
    }
    bool poison()noexcept{poisoned_=true;return false;}
    static bool validPose(const CanonicalHumanPose& p,uint8_t id)noexcept{
        constexpr int32_t k=20000000;
        return p.id==id&&p.xCm>=-k&&p.xCm<=k&&
          p.yCm>=-k&&p.yCm<=k&&p.zCm>=-k&&p.zCm<=k&&
          p.yawCentiDeg>=-18000&&p.yawCentiDeg<=18000&&
          p.flags>=0&&p.flags<=3;
    }
public:
    // Caller must independently prove original engine loaded scene and
    // authenticated story COMMIT; passive source samples are NOT sufficient.
    bool Arm(uint32_t scene,uint64_t story,uint8_t self,uint8_t mask,
             bool loaded,bool committed)noexcept{
        if(!scene||!story||self<1||self>4||
           !(mask==3||mask==7||mask==15)||
           !(mask&(1u<<(self-1)))||!loaded||!committed)return false;
        if(active_){
            if(!own()||poisoned_)return false;
            if(scene!=scene_||story!=story_||self!=self_||mask!=mask_)
                return poison(); // no silent ticket / rejoin relabel
            return true;
        }
        active_=true;owner_=std::this_thread::get_id();
        scene_=scene;story_=story;self_=self;mask_=mask;
        return true;
    }
    // Pool contents MUST be sampled afresh from original engine on its own
    // game thread. An accepted batch remains VALUES ONLY, not a draw permit.
    bool Accept(const OriginalBotLookPlan& plan,
        const std::array<OriginalBotSlotSource,kOriginalBotPoolSlots>& slots,
        const std::array<uint64_t,5>& generations,
        const CanonicalHumanFrame& world,uint64_t nowMs,
        PuppetPoseBatch& out)noexcept{
        if(!own()||poisoned_||!nowMs||!world.receiptMs||
           nowMs<world.receiptMs||nowMs-world.receiptMs>350||
           world.sceneEpoch!=scene_||world.self!=self_||
           world.rosterMask!=mask_||world.tick<=lastTick_)
            return false; // stale/late frame is rejected, not relabeled
        uint8_t expected=0;
        for(uint8_t id=1;id<=4;++id){
            const bool member=(mask_&(1u<<(id-1)))!=0;
            if(member&&!validPose(world.humans[id],id))return poison();
            if(!member&&world.humans[id].id!=0)return poison();
            if(member&&id!=self_)++expected;
        }
        if(plan.count!=expected||plan.actualRendererInstalled||
           plan.hostSimulationIntegrated)return poison();
        std::array<uint8_t,5> chosen{};
        std::array<uint64_t,5> identities{};
        std::array<bool,kOriginalBotPoolSlots> used{};
        PuppetPoseBatch next{};
        uint8_t sourceSlot=0;
        uint32_t sourceModel=0;
        for(uint8_t i=0;i<plan.count;++i){
            const auto& p=plan.remotes[i];
            const uint8_t id=p.human,freeSlot=p.emptyOriginalCharacterSlot;
            if(id<1||id>4||id==self_||!(mask_&(1u<<(id-1)))||
               freeSlot>=kOriginalBotPoolSlots||
               p.sourceBotSlot>=kOriginalBotPoolSlots||
               freeSlot==p.sourceBotSlot||used[freeSlot]||
               identities[id]!=0||!p.humanGeneration||
               p.humanGeneration!=generations[id]||
               p.sceneEpoch!=scene_||!p.localOpaqueModel||
               p.borrowedNpcBrain)return poison();
            const auto& donor=slots[p.sourceBotSlot];
            const auto& blank=slots[freeSlot];
            if(donor.model!=p.localOpaqueModel||!donor.rig||
               !donor.partA||!donor.partB||
               blank.model||blank.controller||
               !blank.rig||!blank.partA||!blank.partB)
                return poison(); // original NPC has reclaimed a slot
            if(i==0){sourceSlot=p.sourceBotSlot;sourceModel=p.localOpaqueModel;}
            else if(sourceSlot!=p.sourceBotSlot||
                    sourceModel!=p.localOpaqueModel)return poison();
            chosen[id]=freeSlot;identities[id]=p.humanGeneration;
            used[freeSlot]=true;
            const auto& a=world.humans[id];
            next.humans[i]={id,freeSlot,p.sourceBotSlot,scene_,
               p.localOpaqueModel,p.humanGeneration,
               a.xCm,a.yCm,a.zCm,a.yawCentiDeg,a.flags};
            ++next.count;
        }
        for(uint8_t id=1;id<=4;++id)
            if(id!=self_&&(mask_&(1u<<(id-1)))&&!identities[id])
                return poison();
        if(fixed_){
            if(donor_!=sourceSlot||model_!=sourceModel||
               fixedSlots_!=chosen||fixedGenerations_!=identities)
                return poison();
        }else{
            fixed_=true;donor_=sourceSlot;model_=sourceModel;
            fixedSlots_=chosen;fixedGenerations_=identities;
        }
        lastTick_=world.tick;out=next;return true;
    }
    bool Reset()noexcept{
        if(active_&&!own())return false;
        *this=OriginalPuppetCanonicalPoseGate{};
        return true;
    }
    bool Invalidated()const noexcept{return own()&&poisoned_;}
};
} // namespace bo_bridge
