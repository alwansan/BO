// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Source-known frontend test: original Black-Android exit_menu.cpp
// InFrontEnd(): game && R32(game+kGameState)==game+kFrontEnd.
// Differing game state is merely a NON-FRONTEND CANDIDATE, NOT mission
// identity, fully loaded stage, safely spawnable actor or shared simulation.
#pragma once
#include <cstdint>

namespace blackcoop {
class NonFrontendOriginalPlayerWitness final {
    uint32_t world_=0,state_=0;
    uint8_t consecutive_=0;
    bool reported_=false;
public:
    bool Observe(uint32_t game,uint32_t gameState,uint32_t frontEndOffset,
                 uint32_t world,bool knownOriginalLocalPlayer)noexcept{
        const bool candidate=knownOriginalLocalPlayer&&game!=0&&world!=0&&
                             gameState!=0&&gameState!=game+frontEndOffset;
        if(!candidate){
            world_=0;state_=0;consecutive_=0;reported_=false;
            return false;
        }
        if(world_!=world||state_!=gameState){
            world_=world;state_=gameState;consecutive_=0;reported_=false;
        }
        if(consecutive_<30)consecutive_++;
        if(consecutive_<30||reported_)return false;
        reported_=true;return true;
    }
};
} // namespace blackcoop
