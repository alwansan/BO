// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Authenticated TLS host-selected STORY_PREPARE -> original BLACK frontend tick.
// This is an INT-ONLY bounded plan. No guest loader, engine write, READY,
// COMMIT, NPC AI or model creation. Stage identity is not loaded scene proof.
#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <thread>

namespace blackcoop {
struct OriginalGuestPreparedStory {
    uint32_t sceneEpoch=0,revision=0,tokenHi=0,tokenLo=0;
    uint8_t mission=0,stage=0,selfSlot=0,participants=0;
};
class OriginalGuestPreparedStoryInbox final {
    static constexpr uint64_t kFreshMs=1200;
    std::array<int32_t,8> previous_{};
    std::thread::id owner_{};
    uint64_t lastMs_=0;
    bool owned_=false;
    bool onOwner()const noexcept{
        return !owned_||owner_==std::this_thread::get_id();
    }
public:
    // [sceneEpoch,mission,stage,rosterRevision,tokenHi,tokenLo,
    //  guestCurrentCompactSlot,participants] from the TLS-authenticated room.
    // All-zeros means no PREPARE or disconnected and clears the candidate.
    // Return 76 for first new ticket, 77 for a changed live ticket, 0 otherwise.
    int Ingest(const int32_t* v,size_t count,uint64_t nowMs)noexcept {
        if(!onOwner()||!v||count!=8)return 0;
        bool clear=true;
        for(size_t i=0;i<8;i++)if(v[i]!=0){clear=false;break;}
        if(clear){Clear();return 0;}
        if(!nowMs||v[0]<=0||v[1]<0||v[1]>7||v[2]<0||v[2]>15||
           v[3]==0||v[4]<0||(v[4]==0&&v[5]==0)||
           v[6]<2||v[6]>4||
           (v[7]!=3&&v[7]!=7&&v[7]!=15)||
           (v[7]&(1<<(v[6]-1)))==0||
           ((v[7]==3&&v[6]!=2)||(v[7]==7&&v[6]>3)))
            return 0;
        if(!owned_){owner_=std::this_thread::get_id();owned_=true;}
        bool hadTicket=previous_[0]!=0;
        bool changed=false;
        for(size_t i=0;i<8;i++)if(v[i]!=previous_[i]){changed=true;break;}
        if(changed)for(size_t i=0;i<8;i++)previous_[i]=v[i];
        lastMs_=nowMs;
        return changed?(hadTicket?77:76):0;
    }
    bool Fresh(uint64_t at,OriginalGuestPreparedStory& out)const noexcept{
        if(!owned_||!onOwner()||!previous_[0]||!lastMs_||
           at<lastMs_||at-lastMs_>kFreshMs)return false;
        out.sceneEpoch=static_cast<uint32_t>(previous_[0]);
        out.mission=static_cast<uint8_t>(previous_[1]);
        out.stage=static_cast<uint8_t>(previous_[2]);
        out.revision=static_cast<uint32_t>(previous_[3]);
        out.tokenHi=static_cast<uint32_t>(previous_[4]);
        out.tokenLo=static_cast<uint32_t>(previous_[5]);
        out.selfSlot=static_cast<uint8_t>(previous_[6]);
        out.participants=static_cast<uint8_t>(previous_[7]);
        return true;
    }
    bool Clear()noexcept{
        if(!onOwner())return false;
        previous_={};lastMs_=0;return true;
    }
};
} // namespace blackcoop
