// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// ORIGINAL BLACK character-pool APPEARANCE readiness, not actor creation.
// Pinned MIT PC prototype b2512fafd: 32 slots, 0x3C0 stride, root global
// 0x002D1980 + 0xA0. Fields model+0x320, controller+0x324,
// visual-instance+0x328 and part buffers+0x34C/+0x350.
// A soldier (enemy OR ally) is acceptable as a LOOK source. Never borrow AI.
#pragma once
#include <array>
#include <cstdint>
namespace blackcoop {
struct OriginalBotPoolSlotEvidence{
    bool model=false,controller=false;
    bool instanceValid=false,partABufferValid=false,partBBufferValid=false;
};
struct OriginalBotPoolAvailability{
    uint8_t sourceLooks=0, emptyVisualSlots=0;
    // Appearance-only: model material/effects, engine scene ownership and
    // function ABI remain unverified; this is NEVER story loaded proof.
    uint8_t coarseCode()const noexcept{
        if(!sourceLooks)return 60;
        if(!emptyVisualSlots)return 61;
        if(emptyVisualSlots==1)return 62;
        if(emptyVisualSlots==2)return 63;
        return 64;
    }
    bool potentialOneToThreeHumans()const noexcept{
        return sourceLooks!=0&&emptyVisualSlots>=3;
    }
};
inline OriginalBotPoolAvailability CountOriginalBotLookSlots(
 const std::array<OriginalBotPoolSlotEvidence,32>& slots)noexcept{
    OriginalBotPoolAvailability counted{};
    for(const auto& s:slots){
        if(s.model&&s.instanceValid&&s.partABufferValid&&s.partBBufferValid)
            ++counted.sourceLooks;
        // If the original game has given a character an NPC controller it
        // must never be borrowed even if its model field is zero.
        if(!s.model&&!s.controller&&s.instanceValid&&
           s.partABufferValid&&s.partBBufferValid)
            ++counted.emptyVisualSlots;
    }
    return counted;
}
} // namespace blackcoop
