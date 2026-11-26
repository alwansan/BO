// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Independently sustained genuine BLACK first-person ORIGINAL world proof.
// Original source gate = original core running phase0x1C + committed game
// pages + two original stage resource files + streamed/core same level/stage
// + 30 previous original eye samples. Require ANOTHER 30 samples here.
// Means local ORIGINAL level running; does NOT prove an authoritative SHARED
// co-op AI/quest world, renderer or scene loader ABI. No engine writes.
#pragma once
#include <cstdint>
namespace blackcoop {
// Pinned c5 image: 001EDC90 sets both fields on entering gameplay.
// 001EBD60 advances the unload field while the running field remains 0x1C.
constexpr bool IsOriginalWorldRunning(uint32_t loadPhase,uint32_t unloadPhase)noexcept{
    return loadPhase==0x1Cu&&unloadPhase==0x1Du;
}

class OriginalRunningSceneSourceGate final {
    uint32_t episode_=0,world_=0;
    int level_=-1,stage_=-1;
    unsigned stable_=0;
public:
    void Reset()noexcept{
        episode_=world_=stable_=0;level_=stage_=-1;
    }
    bool Observe(uint32_t episode,uint32_t world,int level,int stage,
                 bool independentlySourceMatched)noexcept{
        if(!episode||!world||!independentlySourceMatched||
           level<0||level>7||stage<0||stage>15){
            Reset();return false;
        }
        if(episode_!=episode||world_!=world||level_!=level||stage_!=stage){
            episode_=episode;world_=world;level_=level;stage_=stage;stable_=0;
        }
        if(stable_<30)++stable_;
        return stable_>=30;
    }
};
} // namespace blackcoop
