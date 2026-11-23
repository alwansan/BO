// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// ORIGINAL game-thread hook after 0x001F1080 (same boundary as proven PC
// 2-player runtime). A fixed diagnostic event only; never guest writes.
#pragma once
#include "coop/room_original_post_frame_contract.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include "coop/room_world_tick.hpp"
#include "kernel/kernel.h"
#include <cstdint>
namespace blackcoop {
inline void ObserveOriginalPostWorldFrame()noexcept{
 static OriginalPostFrameGate gate;
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
 s.localKind=kernel::R32(s.world+0x30u+0xC4u);
 if(gate.Observe(s,true))
  SendSafeWorldCodeToAndroid(2130); // candidate, NOT P2 ready
}
} // namespace blackcoop
