// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// RAPID-TRIAL-BODY: ANY validated original BLACK bot mesh may be a placeholder.
// Not restricted to ally/friendly model, rig, original animation, texture or skin.
// Local player remains real BLACK first-person; remote human = separate identity.
// NO original actor, render object, NPC brain, or game memory is created here.
#pragma once
#include "remote_ally_roster.hpp"
#include <cstdint>
#include <thread>

namespace bo_bridge {
struct TemporaryBotVisual {
    uint8_t remoteHuman=0;
    uint32_t scene=0;
    uint64_t generation=0;
    // OPAQUE local game model identity. Do not log/serialize/export this key.
    uint64_t originalBotModel=0;
    bool temporaryLook=true;
    bool ownsAiBrain=false;
    bool independentlyControlledHuman=true;
};
class TemporaryBotVisualPolicy final {
    std::thread::id owner_{};
    uint32_t scene_=0;
    uint8_t self_=0;
    bool bound_=false;
    bool thread()const noexcept{
        return bound_&&owner_==std::this_thread::get_id();
    }
public:
    // The engine must eventually prove the ORIGINAL story stage loaded.
    bool BindVerifiedScene(uint32_t scene,uint8_t self,bool stageLoaded)noexcept{
        if(!stageLoaded||!scene||self<1||self>4)return false;
        if(bound_&&!thread())return false;
        if(!bound_){owner_=std::this_thread::get_id();bound_=true;}
        scene_=scene;self_=self;return true;
    }
    // The future engine adapter can choose *ANY* available bot model, whether
    // enemy or ally. Only an independently validated renderable stage resource
    // can pass, never an untrusted network-supplied model ID or actor pointer.
    // No game NPC is duplicated or repurposed as a human actor.
    bool Select(const RemoteAllyRoster& roster,uint8_t remote,
                uint64_t expectedGeneration,uint64_t now,
                uint64_t localBotModel,bool validatedLocalBotRenderable,
                bool authenticatedHostSnapshot,TemporaryBotVisual& output)const noexcept{
        if(!thread()||!scene_||!authenticatedHostSnapshot||
           !validatedLocalBotRenderable||!localBotModel||
           remote<1||remote>4||remote==self_||!expectedGeneration||
           roster.Scene()!=scene_||roster.Self()!=self_)
            return false;
        AvatarView view{};
        if(!roster.ReadRemote(remote,now,view)||view.player!=remote||
           view.scene!=scene_||view.generation!=expectedGeneration||
           view.appearance!=VisualKind::RemoteAllyBody)return false;
        output={remote,scene_,view.generation,localBotModel,true,false,true};
        return true;
    }
};
} // namespace bo_bridge
