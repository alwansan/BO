// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Passive original engine character pool survey, original BLACK game-thread
// ONLY after 30 original-core phase samples + matching source resource pair.
// This NEVER reserves a slot, copies an NPC, moves actors, renders or READY.
#pragma once
#include "coop/room_original_bot_pool_contract.hpp"
#include "coop/room_world_tick.hpp"
#include "kernel/memory.h"
#include "kernel/kernel.h"
#include <SDL3/SDL.h>
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
namespace blackcoop {
// Guest 32-bit arithmetic is checked, never wrap to a second memory map.
// The pinned native Xbox SDK's mem::IsCommitted checks each guest page.
inline bool OriginalGuestSpanCommitted(uint32_t start,uint32_t bytes)noexcept{
    if(start<0x10000u||!bytes)return false;
    const uint64_t limit=static_cast<uint64_t>(start)+bytes;
    if(limit>0x100000000ull)return false;
    const uint64_t first=static_cast<uint64_t>(start)&~uint64_t{0xFFF};
    const uint64_t last=(limit-1u)&~uint64_t{0xFFF};
    for(uint64_t page=first;page<=last;page+=0x1000u){
        if(!mem::IsCommitted(static_cast<uint32_t>(page)))return false;
    }
    return true;
}
inline bool OriginalBotSlotRead(uint32_t addr,uint32_t& value)noexcept{
    if(!OriginalGuestSpanCommitted(addr,4u))return false;
    value=kernel::R32(addr);return true;
}
inline bool ReadOriginalBotPoolAvailability(
 OriginalBotPoolAvailability& availability)noexcept{
    constexpr uint32_t kPoolGlobal=0x002D1980u;
    constexpr uint32_t kFirst=0xA0u,kStride=0x3C0u,kCount=32u;
    uint32_t root=0;
    if(!OriginalBotSlotRead(kPoolGlobal,root)||!root)return false;
    const uint64_t stop=static_cast<uint64_t>(root)+kFirst+kStride*kCount;
    if(stop>0x100000000ull||
       !OriginalGuestSpanCommitted(root+kFirst,kStride*kCount))return false;
    std::array<OriginalBotPoolSlotEvidence,kCount> slots{};
    for(uint32_t i=0;i<kCount;i++){
        const uint32_t ch=root+kFirst+i*kStride;
        uint32_t model=0,controller=0,instance=0,partA=0,partB=0;
        if(!OriginalBotSlotRead(ch+0x320,model)||
           !OriginalBotSlotRead(ch+0x324,controller)||
           !OriginalBotSlotRead(ch+0x328,instance)||
           !OriginalBotSlotRead(ch+0x34C,partA)||
           !OriginalBotSlotRead(ch+0x350,partB))return false;
        slots[i]={
            model!=0u,controller!=0u,
            instance!=0u&&OriginalGuestSpanCommitted(instance,0x240u),
            partA!=0u&&OriginalGuestSpanCommitted(partA,0x70u),
            partB!=0u&&OriginalGuestSpanCommitted(partB,0x3C0u)};
        // If the source looks eligible, confirm the original model template
        // itself is committed before even COUNTING it.
        // Model ownership stays true even if the pointer has gone stale:
        // invalid original NPC records must never be called FREE.
        if(slots[i].model&&!OriginalGuestSpanCommitted(model,0x60u))
            slots[i].instanceValid=false;
    }
    // Do not report pointers or model names to network or Android logs.
    uint32_t stillRoot=0;
    if(!OriginalBotSlotRead(kPoolGlobal,stillRoot)||stillRoot!=root)return false;
    availability=CountOriginalBotLookSlots(slots);
    return true;
}
inline void ObserveOriginalBotPoolSourceHint(uint32_t episode,
                                             bool verifiedSourceGate)noexcept{
    static uint32_t observedEpisode=0,emitted=0;
    static uint64_t lastMs=0;
    if(!verifiedSourceGate||!episode){
        observedEpisode=0;emitted=0;lastMs=0;return;
    }
    if(observedEpisode!=episode){
        observedEpisode=episode;emitted=0;lastMs=0;
    }
    const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
    if(lastMs&&now>=lastMs&&now-lastMs<2000)return;
    lastMs=now;
    OriginalBotPoolAvailability result{};
    if(!ReadOriginalBotPoolAvailability(result))return;
    const int code=result.coarseCode();
    const unsigned bit=1u<<static_cast<unsigned>(code-60);
    if((emitted&bit)!=0u)return;
    emitted|=bit;
    SendSafeWorldCodeToAndroid(code); // 60..64 coarse counts only, not gameplay
}
} // namespace blackcoop
