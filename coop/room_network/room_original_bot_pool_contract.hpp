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
// A deterministic read-only assignment proposal for genuine human peers.
// No slot is RESERVED or created here, and NONE of these indices may be
// dereferenced for writes without independent source/ABI proof.
enum class OriginalHumanLookPlanState : uint8_t{
    Inactive,NoAppearanceSource,InsufficientBlankSlots,Proposed,Invalidated
};
struct OriginalHumanLookProposal{
    uint8_t appearanceSlot=0xFFu;
    uint8_t peerMask=0u;
    std::array<uint8_t,5> blankSlots{};
    OriginalHumanLookProposal()noexcept {blankSlots.fill(0xFFu);}
};
inline bool OriginalLookSource(const OriginalBotPoolSlotEvidence& s)noexcept{
    return s.model&&s.instanceValid&&s.partABufferValid&&s.partBBufferValid;
}
inline bool OriginalLookEmpty(const OriginalBotPoolSlotEvidence& s)noexcept{
    return !s.model&&!s.controller&&s.instanceValid&&
           s.partABufferValid&&s.partBBufferValid;
}
class OriginalHumanLookPlanner final{
    uint32_t originalEpisode_=0,world_=0;
    uint8_t peerMask_=0;
    bool proposed_=false,invalidated_=false;
    OriginalHumanLookProposal proposal_{};
public:
    void Reset()noexcept{
        originalEpisode_=world_=0;peerMask_=0;
        proposed_=invalidated_=false;proposal_=OriginalHumanLookProposal{};
    }
    OriginalHumanLookPlanState Observe(uint32_t originalEpisode,uint32_t world,
        uint8_t peerMask,
        const std::array<OriginalBotPoolSlotEvidence,32>& slots)noexcept{
        if(!originalEpisode||!world){Reset();return OriginalHumanLookPlanState::Inactive;}
        if(originalEpisode_!=originalEpisode||world_!=world){
            Reset();originalEpisode_=originalEpisode;world_=world;
        }
        // Current TLS host roster slot convention: 2 then 3 then 4.
        // Never mistake untrusted RoomWire.Player.id for this compact mask.
        const bool maskValid=peerMask==0x02u||peerMask==0x06u||peerMask==0x0Eu;
        if(!maskValid){
            if(proposed_)invalidated_=true;
            return invalidated_?OriginalHumanLookPlanState::Invalidated:
                                OriginalHumanLookPlanState::Inactive;
        }
        if(peerMask_!=0&&peerMask_!=peerMask)invalidated_=true;
        if(invalidated_)return OriginalHumanLookPlanState::Invalidated;
        peerMask_=peerMask;
        if(proposed_){
            if(proposal_.peerMask!=peerMask||
               !OriginalLookSource(slots[proposal_.appearanceSlot]))
                invalidated_=true;
            for(uint8_t peer=2;peer<=4;peer++){
                const uint8_t slot=proposal_.blankSlots[peer];
                if((peerMask&(1u<<(peer-1)))&&
                   (slot>=32||!OriginalLookEmpty(slots[slot])))
                    invalidated_=true;
            }
            return invalidated_?OriginalHumanLookPlanState::Invalidated:
                                OriginalHumanLookPlanState::Proposed;
        }
        uint8_t source=0xFFu;
        for(uint8_t i=0;i<32;i++)if(OriginalLookSource(slots[i])){source=i;break;}
        if(source==0xFFu)return OriginalHumanLookPlanState::NoAppearanceSource;
        OriginalHumanLookProposal next{};
        next.appearanceSlot=source;next.peerMask=peerMask;
        int available=31;
        for(uint8_t peer=2;peer<=4;peer++){
            if(!(peerMask&(1u<<(peer-1))))continue;
            while(available>=0&&
                 !OriginalLookEmpty(slots[static_cast<size_t>(available)]))--available;
            if(available<0)return OriginalHumanLookPlanState::InsufficientBlankSlots;
            next.blankSlots[peer]=static_cast<uint8_t>(available--);
        }
        proposal_=next;proposed_=true;
        return OriginalHumanLookPlanState::Proposed;
    }
    // Candidate slots are immutable for one original launch+roster; no
    // silent reassignment if original NPCs reclaim one of those slots.
    const OriginalHumanLookProposal& Candidate()const noexcept{return proposal_;}
    bool Invalidated()const noexcept{return invalidated_;}
};
} // namespace blackcoop
