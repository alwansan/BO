// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original AI targets: source-linked read-only 32-record survey once per
// independently running original level. Emits only coarse fixed categories;
// NEVER adds a player target, changes NPC AI/health, or sends guest pointers.
#pragma once
#include "coop/room_original_ai_target_contract.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include "game/layout.h"
#include <array>
#include <cstdint>
namespace blackcoop {
inline bool ReadOriginalAiTargetPool(
 uint32_t world,OriginalAiTargets& result)noexcept{
 constexpr uint32_t kAiGlobal=0x002D1988u;
 constexpr uint32_t kTargets=0xFACu;
 constexpr uint32_t kCount=32u,kStride=12u;
 uint32_t ai=0;
 if(!OriginalBotSlotRead(kAiGlobal,ai)||!ai||!world)return false;
 const uint64_t end=static_cast<uint64_t>(ai)+kTargets+kCount*kStride;
 const uint64_t player=static_cast<uint64_t>(world)+layout::kWorldPlayer;
 if(end>0x100000000ull||player+0x37Cu>0x100000000ull||
    !OriginalGuestSpanCommitted(ai+kTargets,kCount*kStride)||
    !OriginalGuestSpanCommitted(static_cast<uint32_t>(player+0x378u),4u))
    return false;
 std::array<OriginalAiTargetRecord,kCount> records{};
 for(uint32_t i=0;i<kCount;i++){
  const uint32_t at=ai+kTargets+i*kStride;
  records[i]={kernel::R32(at+4u),kernel::R32(at+8u)};
 }
 uint32_t again=0;
 if(!OriginalBotSlotRead(kAiGlobal,again)||again!=ai)return false;
 const uint32_t target=kernel::R32(static_cast<uint32_t>(player+0x378u));
 result=CountOriginalAiTargets(records,static_cast<uint32_t>(player),target);
 return true;
}
inline void ObserveOriginalAiTargetPoolSourceHint(
 uint32_t episode,uint32_t world,bool genuineOriginalRunning)noexcept{
 static uint32_t lastEpisode=0,lastWorld=0;
 static uint8_t emitted=0;
 if(!episode||!world||!genuineOriginalRunning){
  lastEpisode=lastWorld=0;emitted=0;return;
 }
 if(lastEpisode!=episode||lastWorld!=world){
  lastEpisode=episode;lastWorld=world;emitted=0;
 }
 OriginalAiTargets result{};
 if(!ReadOriginalAiTargetPool(world,result)){
  if(!(emitted&1u)){emitted|=1u;SendSafeWorldCodeToAndroid(95);}
  return;
 }
 if(result.localOriginalPlayerBound&&!(emitted&2u)){
  emitted|=2u;SendSafeWorldCodeToAndroid(89);
 }
 if(!(emitted&4u)){
  emitted|=4u;SendSafeWorldCodeToAndroid(result.freeCode());
 }
}
} // namespace blackcoop
