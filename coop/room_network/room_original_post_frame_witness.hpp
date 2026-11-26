// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// ORIGINAL game-thread hook after 0x001F1080 (same boundary as proven PC
// 2-player runtime). A fixed diagnostic event only; never guest writes.
#pragma once
#include "coop/room_original_post_frame_contract.hpp"
#include "coop/room_original_host_multi_actor_lanes.hpp"
#include "coop/room_original_host_setup_tick.hpp"
#include "coop/room_original_guest_canonical_source_barrier.hpp"
#include "coop/room_original_guest_host_world_witness.hpp"
#include "coop/room_original_guest_host_npc_witness.hpp"
#include "coop/room_original_guest_launch_match_witness.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include "coop/room_world_tick.hpp"
#include "kernel/kernel.h"
#include <atomic>
#include <cstdint>
namespace blackcoop {
struct OriginalUnitMembershipMemory {
 bool Contains(uint32_t addr,uint32_t size)const noexcept{
   return OriginalGuestSpanCommitted(addr,size);
 }
 uint32_t Read32(uint32_t addr)const noexcept{return kernel::R32(addr);}
 uint16_t Read16(uint32_t addr)const noexcept{return kernel::R16(addr);}
 uint8_t Read8(uint32_t addr)const noexcept{
   return *static_cast<const uint8_t*>(kernel::HostPtr(addr));
 }
};

inline OriginalUnitLifecycle* OriginalUnitLifecycleOnFrameThread(bool bind=false)noexcept{
 static std::atomic<OriginalUnitLifecycle*> state{nullptr};
 if(bind){
   static OriginalUnitLifecycle observer;
   state.store(&observer,std::memory_order_release);
 }
 auto* observer=state.load(std::memory_order_acquire);
 return observer&&observer->OnFrameThread()?observer:nullptr;
}

inline void ObserveOriginalPostWorldFrame()noexcept{
 static OriginalPostFrameGate gate;
 // Three native host-player lanes and guest canonical source are kept
 // separately on the genuine original game thread. Never synthesize engine
 // actor readiness or apply host NPC/door events from transport alone.
 static OriginalHostHumanLanes humanLanes;
 static OriginalHumanSetupLanes humanSetup; // native guest-call IO unimplemented, no active actor
 static OriginalGuestCanonicalSourceBarrier guestBarrier;
 constexpr uint32_t worldAt=0x002D45D4u,ownerAt=0x002D12B8u;
 constexpr uint32_t physicsAt=0x002D45F4u,npcAt=0x002D1980u;
 if(!OriginalGuestSpanCommitted(worldAt,4u)||
    !OriginalGuestSpanCommitted(ownerAt,4u)||
    !OriginalGuestSpanCommitted(physicsAt,4u)||
    !OriginalGuestSpanCommitted(npcAt,4u)){
   gate.Reset();return;
 }
 OriginalPostFrameSample s{};
 s.world=kernel::R32(worldAt);s.owner=kernel::R32(ownerAt);
 s.physics=kernel::R32(physicsAt);s.npcPool=kernel::R32(npcAt);
 if(!OriginalGuestSpanCommitted(s.world,0x5CC0u)||
    !OriginalGuestSpanCommitted(s.owner,0x16C0u)||
    !OriginalGuestSpanCommitted(s.physics,0x2974u)||
    !OriginalGuestSpanCommitted(s.npcPool,0x8540u)){
   gate.Reset();return;
 }
 s.corePhase=kernel::R32(s.world+0x5AE0u);
 s.unloadPhase=kernel::R32(s.world+0x5AE4u);
 s.localKind=kernel::R32(s.world+0x30u+0xC4u);
 s.localVtable=kernel::R32(s.world+0x30u); // exact ORIGINAL P1 identity
 const bool stableFrame=gate.Observe(s,true);
 if(stableFrame){
   const bool hasUnitMembership=HasOriginalWorldUnitMembership(OriginalUnitMembershipMemory{},s.world);
   OriginalUnitLifecycleOnFrameThread(true);
   SendSafeWorldCodeToAndroid(2130); // candidate, NOT P2 ready
   if(hasUnitMembership)
     SendSafeWorldCodeToAndroid(2131); // local membership only; no handles retained
 }
 if(auto* lifecycle=OriginalUnitLifecycleOnFrameThread();
    lifecycle&&lifecycle->ObserveRunningFrame(OriginalUnitMembershipMemory{},s.world))
   SendSafeWorldCodeToAndroid(2133);
 (void)humanLanes.Progress(2);(void)humanLanes.Progress(3);
 (void)humanLanes.Progress(4); // never marks a lane ready
 (void)humanSetup.At(2);(void)humanSetup.At(3);(void)humanSetup.At(4);
 OriginalGuestLaunchMatchKey signedMatch{};
 if(gate.StableOriginalFrames()>=30u&&
    GuestOriginalLaunchMatch().ReadMatchedKey(signedMatch)){
  const uint64_t nativeScene=(uint64_t(signedMatch.originalEpisode)<<32)|s.world;
  (void)guestBarrier.Observe(signedMatch,nativeScene,
     OriginalGuestHostWorld(),GuestOriginalHostNpcs(),
     static_cast<uint64_t>(SDL_GetTicks()));
 }else guestBarrier.Reset();
}
} // namespace blackcoop
