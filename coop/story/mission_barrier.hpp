// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Host-owned value-only mission barrier; NOT connected to BLACK loading/scene
// hooks or TLS yet. Never starts independent missions on the two phones.
#pragma once
#include <cstdint>
#include <thread>
#include <type_traits>

namespace bo_story {
struct MissionTicket {
    uint64_t session=0, nonce=0; // from one authenticated room and one prepare
    uint32_t mission=0, stage=0, scene_epoch=0;
    uint8_t participants=0;      // bit0=host, bits1..3=guests
    bool operator==(const MissionTicket&) const = default;
};
static_assert(std::is_trivially_copyable_v<MissionTicket>);

// Each expected device must prove it has loaded the host's exact mission,
// stage AND scene epoch before we can issue one authoritative COMMIT.
// This protocol's caller must authenticate the sender and validate room IDs.
class HostMissionBarrier {
    std::thread::id owner_{};
    bool owned_=false,active_=false,committed_=false;
    MissionTicket ticket_{};
    uint8_t ready_=0;
    bool ownerThread()const noexcept{
        return !owned_||std::this_thread::get_id()==owner_;
    }
    static bool valid(const MissionTicket& value)noexcept{
        // BLACK's pinned frontend has eight zero-based missions (0..7),
        // and stage 0 may be valid. Never reject first mission as unset.
        return value.session&&value.nonce&&value.mission<8&&
               value.scene_epoch&&(value.participants&1u)&&
               (value.participants&0x0Eu)&&!(value.participants&0xF0u);
    }
public:
    bool Prepare(const MissionTicket& value,bool isHost)noexcept{
        if(!isHost||!valid(value)||!ownerThread()||active_)return false;
        if(!owned_){owner_=std::this_thread::get_id();owned_=true;}
        ticket_=value;ready_=0;committed_=false;active_=true;
        return true;
    }
    bool Ready(const MissionTicket& key,uint8_t player,
               uint32_t confirmed_loaded_scene,bool trustedPeer)noexcept{
        if(!ownerThread()||!active_||committed_||!trustedPeer||
           !(key==ticket_)||player<1||player>4||
           confirmed_loaded_scene!=ticket_.scene_epoch)return false;
        uint8_t bit=static_cast<uint8_t>(1u<<(player-1));
        if(!(ticket_.participants&bit)||(ready_&bit))return false;
        ready_=static_cast<uint8_t>(ready_|bit);return true;
    }
    // Outgoing COMMIT is value only. Apply to actual game/world exclusively
    // on verified game threads after an independently implemented scene hook.
    bool Commit(MissionTicket& out)noexcept{
        if(!ownerThread()||!active_||committed_||
           ready_!=ticket_.participants)return false;
        out=ticket_;committed_=true;return true;
    }
    bool RemovePeer(uint8_t player)noexcept{
        if(!ownerThread()||!active_||player<2||player>4||
           !(ticket_.participants&(1u<<(player-1))))return false;
        // Invalidate rather than silently committing fewer humans.
        active_=false;ready_=0;committed_=false;ticket_={};return true;
    }
    bool Reset()noexcept{
        if(!ownerThread())return false;
        active_=false;committed_=false;ready_=0;ticket_={};return true;
    }
    bool Prepared()const noexcept{return active_;}
    bool Committed()const noexcept{return committed_;}
    uint8_t ReadyMask()const noexcept{return ready_;}
    MissionTicket Ticket()const noexcept{return ticket_;}
};

class GuestMissionBarrier {
    std::thread::id owner_{};
    bool owned_=false,active_=false,ready_=false,committed_=false;
    MissionTicket ticket_{};
    uint8_t self_=0;
    bool ownerThread()const noexcept{return !owned_||owner_==std::this_thread::get_id();}
public:
    bool AcceptPrepare(const MissionTicket& key,uint8_t local_id,
                       bool fromAuthenticatedHost)noexcept{
        if(!ownerThread()||active_||!fromAuthenticatedHost||
           local_id<2||local_id>4||!key.session||!key.nonce||
           key.mission>7||!key.scene_epoch||
           !(key.participants&1u)||!(key.participants&(1u<<(local_id-1)))||
           (key.participants&0xF0u))return false;
        if(!owned_){owned_=true;owner_=std::this_thread::get_id();}
        ticket_=key;self_=local_id;ready_=false;committed_=false;active_=true;
        return true;
    }
    bool MarkActuallyLoaded(uint32_t mission,uint32_t stage,
                            uint32_t scene_epoch)noexcept{
        if(!ownerThread()||!active_||ready_||mission!=ticket_.mission||
           stage!=ticket_.stage||scene_epoch!=ticket_.scene_epoch)return false;
        ready_=true;return true;
    }
    bool AcceptCommit(const MissionTicket& hostCommit,bool fromAuthenticatedHost)noexcept{
        if(!ownerThread()||!active_||!ready_||committed_||
           !fromAuthenticatedHost||!(hostCommit==ticket_))return false;
        committed_=true;return true;
    }
    bool Reset()noexcept{
        if(!ownerThread())return false;
        ticket_={};self_=0;ready_=committed_=active_=false;return true;
    }
    bool Committed()const noexcept{return committed_;}
    bool Ready()const noexcept{return ready_;}
};
} // namespace bo_story
