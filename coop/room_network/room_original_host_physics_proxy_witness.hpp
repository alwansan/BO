// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Owner original BLACK native game-thread source-verified READ-ONLY physics
// proxy capacity witness. Based on pinned MIT PC source recomp_manual.c
// black_proxies_free + black_proxy_motion. NO DF400/DBBE0, actor allocation,
// game memory writes or claim that guest humans are physical players yet.
#pragma once
#include "coop/room_original_host_physics_proxy_contract.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include "coop/room_world_tick.hpp"
#include "game/layout.h"
#include "kernel/kernel.h"
#include <array>
#include <cstdint>
namespace blackcoop {
inline bool ReadOriginalHostPhysicsProxyCapacity(
 uint32_t originalActor,OriginalHostPhysicsProxySummary& out)noexcept{
 constexpr uint32_t kWorldGlobal=0x002D45F4u,kFlagStart=0x2960u;
 uint32_t physicsWorld=0;
 if(!OriginalBotSlotRead(kWorldGlobal,physicsWorld)||!physicsWorld||
    static_cast<uint64_t>(physicsWorld)+kFlagStart+20u>0x100000000ull||
    !OriginalGuestSpanCommitted(physicsWorld+kFlagStart,20u)||
    static_cast<uint64_t>(originalActor)+0xB8u>0x100000000ull||
    !OriginalGuestSpanCommitted(originalActor+0xB4u,4u))
   return false;
 std::array<uint8_t,20> occupied{};
 for(uint32_t i=0;i<20u;i++)
  occupied[i]=kernel::G<uint8_t>(physicsWorld+kFlagStart+i);
 uint32_t proxy=kernel::R32(originalActor+0xB4u);
 uint32_t wrapper=0,body=0,motion=0;
 bool motionValid=false;
 if(proxy&&static_cast<uint64_t>(proxy)+0x38u<=0x100000000ull&&
    OriginalGuestSpanCommitted(proxy+0x34u,4u)){
   wrapper=kernel::R32(proxy+0x34u);
   if(wrapper&&static_cast<uint64_t>(wrapper)+0x10u<=0x100000000ull&&
      OriginalGuestSpanCommitted(wrapper+0xCu,4u)){
    body=kernel::R32(wrapper+0xCu);
    if(body&&static_cast<uint64_t>(body)+0x5Cu<=0x100000000ull&&
       OriginalGuestSpanCommitted(body+0x58u,4u)){
     motion=kernel::R32(body+0x58u);
     motionValid=motion&&static_cast<uint64_t>(motion)+0xB0u<=0x100000000ull&&
         OriginalGuestSpanCommitted(motion,0xB0u);
    }
   }
 }
 // The pool global must still point to the SAME original owner world.
 uint32_t again=0;
 if(!OriginalBotSlotRead(kWorldGlobal,again)||again!=physicsWorld)return false;
 out=CountOriginalHostPhysicsProxies(occupied,motionValid);
 return true;
}
inline void ObserveOriginalHostPhysicsProxyReadiness(
 uint32_t originalEpisode,uint32_t originalActor,
 uint8_t authenticatedOtherHumansMask,bool originalSceneSourceMatched)noexcept{
 static uint32_t lastEpisode=0;
 static uint8_t reported=0;
 if(!originalEpisode||!originalSceneSourceMatched||
    !authenticatedOtherHumansMask){
  lastEpisode=0;reported=0;return;
 }
 if(originalEpisode!=lastEpisode){
  lastEpisode=originalEpisode;reported=0;
 }
 if((authenticatedOtherHumansMask&0xf1u)!=0u||
    (authenticatedOtherHumansMask!=2u&&
     authenticatedOtherHumansMask!=6u&&
     authenticatedOtherHumansMask!=14u))return;
 const unsigned count=(authenticatedOtherHumansMask==2u?1u:
                       authenticatedOtherHumansMask==6u?2u:3u);
 OriginalHostPhysicsProxySummary summary{};
 const int code=ReadOriginalHostPhysicsProxyCapacity(originalActor,summary)?
                summary.coarseCode(count):2100;
 const uint8_t bit=static_cast<uint8_t>(1u<<static_cast<unsigned>(code-2100));
 if(reported&bit)return;
 reported=static_cast<uint8_t>(reported|bit);
 SendSafeWorldCodeToAndroid(code);
}
} // namespace blackcoop
