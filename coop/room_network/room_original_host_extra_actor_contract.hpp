// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Single original BLACK host-world extra-human actor prerequisite contract.
// No actor allocation, XBE function call, AI target insertion, physics proxy
// acquisition, scene loader, or client object mutation occurs here.
#pragma once
#include "room_original_bot_pool_contract.hpp"
#include "room_original_ai_target_contract.hpp"
#include "room_original_host_physics_proxy_contract.hpp"
#include <cstdint>
namespace blackcoop {
struct OriginalHostExtraActorPreconditions {
 uint32_t originalEpisode=0,originalWorld=0;
 uint32_t signedStoryEpoch=0,inputEpoch=0;
 uint8_t authenticatedOtherMask=0,freshInputMask=0;
 bool sourceMatched=false,physicsReadable=false,aiReadable=false,botReadable=false;
 OriginalHostPhysicsProxySummary physical{};
 OriginalAiTargets targets{};
 OriginalBotPoolAvailability appearances{};
 unsigned extraHumans()const noexcept{
  return authenticatedOtherMask==0x02u?1u:
         authenticatedOtherMask==0x06u?2u:
         authenticatedOtherMask==0x0eu?3u:0u;
 }
 int coarseStatus()const noexcept{
  if(!originalEpisode||!originalWorld||!sourceMatched)return 2120;
  const unsigned n=extraHumans();
  if(n==0)return 2121;
  if(!signedStoryEpoch||inputEpoch!=signedStoryEpoch)return 2122;
  if((freshInputMask&authenticatedOtherMask)!=authenticatedOtherMask||
     (freshInputMask&~authenticatedOtherMask)!=0u)return 2123;
  if(!physicsReadable||!physical.canReserve(n))return 2124;
  if(!aiReadable||!targets.localOriginalPlayerBound||
     targets.free<n)return 2125;
  if(!botReadable||appearances.sourceLooks==0||
     appearances.emptyVisualSlots<n)return 2126;
  // Even 2127 only means SOURCE read-only prerequisites look ready.
  // A game-engine player context, owner camera, model buffers, weapon bank,
  // live physics proxy, AI target and collision integration DO NOT EXIST.
  return 2127;
 }
};
} // namespace blackcoop
