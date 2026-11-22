// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// BODY-P0: real original BLACK bot model -> 1..3 independent human LOOK slots.
// Derived layout references ONLY from the MIT PC recompiler's puppet path:
// portingpete/Black-2006-Recomp@b2512fafd (char pool +0xA0, stride 0x3C0,
// 32 entries, +0x320 model, +0x324 controller, +0x328 animation parts,
// +0x34C/+0x350 dedicated part buffers).
// This code ONLY plans and validates. It NEVER writes to original game memory,
// creates an NPC/actor, copies a bot brain, calls guest functions, or renders.
#pragma once
#include <array>
#include <cstdint>
#include <thread>

namespace bo_bridge {
inline constexpr unsigned kOriginalBotPoolSlots=32;
inline constexpr uint32_t kOriginalBotPoolOffset=0xA0u;
inline constexpr uint32_t kOriginalBotPoolStride=0x3C0u;
struct OriginalBotSlotSource {
    uint32_t model=0;       // Opaque original guest model reference, NEVER log
    uint32_t controller=0;  // Source bot may have AI; NEVER copy this to human
    uint32_t rig=0;         // Original animation-part data
    uint32_t partA=0,partB=0; // Existing per-slot model part buffers
};
struct RemoteHumanBotLook {
    uint8_t human=0;
    uint8_t emptyOriginalCharacterSlot=0;  // source-free slot, NOT reserved yet
    uint8_t sourceBotSlot=0;               // appearance only; never borrowed AI
    uint32_t sceneEpoch=0;
    uint64_t humanGeneration=0;
    uint32_t localOpaqueModel=0;          // never put on TLS or logs
    bool borrowedNpcBrain=false;
};
struct OriginalBotLookPlan {
    std::array<RemoteHumanBotLook,3> remotes{};
    uint8_t count=0;
    // No real engine actor, no collisions, no weapon authority, no AI.
    bool actualRendererInstalled=false;
    bool hostSimulationIntegrated=false;
};
class OriginalBotPuppetPlanner final {
    bool bound_=false;
    std::thread::id owner_{};
    uint32_t scene_=0;
    uint8_t self_=0,participants_=0;
    bool gameThread()const noexcept{
        return bound_&&std::this_thread::get_id()==owner_;
    }
public:
    // MUST be an independently verified ORIGINAL engine stage-loaded
    // callback and committed authenticated host roster. Never substitute
    // 0x5AE0, paired open files, or camera eye evidence for sceneLoaded=true.
    bool Bind(uint32_t originalSceneEpoch,uint8_t localHuman,
              uint8_t participantMask,bool engineSceneLoaded,
              bool authenticatedStoryCommitted)noexcept{
        if(!originalSceneEpoch||localHuman<1||localHuman>4||
           !(participantMask&1u)||(participantMask&0xF0u)||
           (participantMask&(1u<<(localHuman-1)))==0||
           (participantMask&0x0Fu)==1||!engineSceneLoaded||
           !authenticatedStoryCommitted)return false;
        if(bound_&&!gameThread())return false;
        if(!bound_){bound_=true;owner_=std::this_thread::get_id();}
        scene_=originalSceneEpoch;self_=localHuman;participants_=participantMask;
        return true;
    }
    // The caller must sample the 32 original BLACK character records from the
    // SAME original game-thread/scene. This does NOT allocate original chars:
    // a future adapter must reserve/revalidate every empty slot atomically
    // before any animation initialization/renderer call.
    bool Plan(const std::array<OriginalBotSlotSource,kOriginalBotPoolSlots>& slots,
              const std::array<uint64_t,5>& authenticatedGeneration,
              OriginalBotLookPlan& out)const noexcept{
        if(!gameThread()||!scene_)return false;
        // First viable ORIGINAL bot look (enemy OR ally): the appearance
        // source may have live AI; it is never changed, cloned as NPC, or
        // installed as the remote HUMAN's behavior/controller.
        int source=-1;
        for(unsigned i=0;i<slots.size();++i){
            const auto& item=slots[i];
            if(item.model&&item.rig&&item.partA&&item.partB){
                source=static_cast<int>(i);break;
            }
        }
        if(source<0)return false;
        std::array<uint8_t,3> spare{};
        unsigned free=0;
        for(unsigned i=kOriginalBotPoolSlots;i-->0 && free<3;){
            if(static_cast<int>(i)==source)continue;
            const auto& item=slots[i];
            if(!item.model&&!item.controller&&item.rig&&item.partA&&item.partB)
                spare[free++]=static_cast<uint8_t>(i);
        }
        OriginalBotLookPlan result{};
        for(unsigned id=1;id<=4;++id){
            if(id==self_||!(participants_&(1u<<(id-1))))continue;
            if(result.count>=3||result.count>=free||!authenticatedGeneration[id])
                return false; // fail ALL peers atomically, never partial
            RemoteHumanBotLook& human=result.remotes[result.count++];
            human.human=static_cast<uint8_t>(id);
            human.sourceBotSlot=static_cast<uint8_t>(source);
            human.emptyOriginalCharacterSlot=spare[result.count-1];
            human.sceneEpoch=scene_;
            human.humanGeneration=authenticatedGeneration[id];
            human.localOpaqueModel=slots[static_cast<unsigned>(source)].model;
            human.borrowedNpcBrain=false;
        }
        if(!result.count)return false;
        out=result;
        return true;
    }
};
} // namespace bo_bridge
