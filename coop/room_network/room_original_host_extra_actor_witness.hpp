// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// ORIGINAL BLACK original-player thread: fail-closed extra-human readiness.
// Coarse diagnostic categories only. NO remote actors, AI, proxies or writes.
#pragma once
#include "coop/room_original_host_extra_actor_contract.hpp"
#include "coop/room_original_trusted_input_witness.hpp"
#include "coop/room_original_ai_target_witness.hpp"
#include "coop/room_original_host_physics_proxy_witness.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include <SDL3/SDL.h>
#include <cstdint>
namespace blackcoop {
inline void ObserveOriginalHostExtraActorGate(uint32_t episode,uint32_t world,
 uint32_t localOriginalPlayer,uint8_t authenticatedMask,
 bool verifiedOriginalSource)noexcept{
 static uint32_t lastEpisode=0,lastWorld=0;
 static uint8_t lastMask=0,reported=0;
 static uint64_t lastAt=0;
 if(!episode||!world||!verifiedOriginalSource||!authenticatedMask){
  lastEpisode=lastWorld=0;lastMask=reported=0;lastAt=0;return;
 }
 if(episode!=lastEpisode||world!=lastWorld||authenticatedMask!=lastMask){
  lastEpisode=episode;lastWorld=world;lastMask=authenticatedMask;
  reported=0;lastAt=0;
 }
 const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
 if(lastAt&&now>=lastAt&&now-lastAt<1000u)return;
 lastAt=now;
 OriginalHostExtraActorPreconditions inputs{};
 inputs.originalEpisode=episode;inputs.originalWorld=world;
 inputs.sourceMatched=verifiedOriginalSource;
 inputs.authenticatedOtherMask=authenticatedMask;
 inputs.signedStoryEpoch=OriginalHostHumanInputs().Epoch();
 inputs.inputEpoch=OriginalHostHumanInputs().Epoch();
 // Peer IDs are assigned by signed TLS server host roster, never by
 // guest-provided packet fields. A live command must match current epoch.
 for(uint8_t peer=2;peer<=4;peer++){
  if(!(authenticatedMask&(1u<<(peer-1))))continue;
  TrustedHumanCommand command{};
  if(OriginalHostHumanInputs().Fresh(peer,command)&&
     command.peer==peer&&command.sceneEpoch==inputs.signedStoryEpoch)
   inputs.freshInputMask=static_cast<uint8_t>(
      inputs.freshInputMask|(1u<<(peer-1)));
 }
 inputs.physicsReadable=ReadOriginalHostPhysicsProxyCapacity(
    localOriginalPlayer,inputs.physical);
 inputs.aiReadable=ReadOriginalAiTargetPool(world,inputs.targets);
 inputs.botReadable=ReadOriginalBotPoolAvailability(inputs.appearances);
 const int code=inputs.coarseStatus();
 const uint8_t bit=static_cast<uint8_t>(1u<<static_cast<unsigned>(code-2120));
 if(!(reported&bit)){
  reported=static_cast<uint8_t>(reported|bit);
  SendSafeWorldCodeToAndroid(code);
 }
}
} // namespace blackcoop
