// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Value-only, scene-bound camera/visibility contract. No actor construction,
// AI brain, NPC slot, guest memory, real renderer or online gameplay here.
#pragma once
#include "../native/net_core.hpp"
#include <array>
#include <cstdint>
#include <thread>
#include <limits>

namespace bo_bridge {
enum class VisualKind : uint8_t {
    OriginalFirstPerson=1,  // only on the owner's own phone
    RemoteAllyBody=2        // full-body ALLY LOOK only; NOT NPC AI ownership
};
struct AvatarView {
    uint8_t player=0;
    uint32_t scene=0, tick=0;
    uint64_t generation=0;
    int32_t x=0,y=0; // Existing NET-001 toy coordinates, NOT BLACK world pose
    VisualKind appearance=VisualKind::OriginalFirstPerson;
};
class RemoteAllyRoster {
    struct Slot { bool present=false; uint64_t generation=0; int32_t x=0,y=0; };
    std::array<Slot,5> slots_{};
    std::thread::id owner_{};
    bool bound_=false,has_snapshot_=false;
    uint8_t self_=0;
    uint32_t scene_=0,tick_=0;
    uint64_t last_update_=0,generation_=0;
public:
    // Call from the verified game thread after local room self-ID is assigned.
    // Both host (self=1) and guest (self=2..4) have a first-person LOCAL player.
    bool BindScene(uint32_t scene,uint8_t local_player) noexcept {
        if(!scene || local_player<1 || local_player>4)return false;
        const auto caller=std::this_thread::get_id();
        if(bound_&&owner_!=caller)return false;
        if(!bound_){bound_=true;owner_=caller;}
        if(scene_!=scene||self_!=local_player) {
            scene_=scene;self_=local_player;tick_=0;has_snapshot_=false;
            last_update_=0;slots_={};
        }
        return true;
    }
    // Authenticated room teardown/replacement must invalidate remote views even
    // when the next room loads the same original stage. Only the game owner
    // thread can reset an active roster; preserve generations across sessions.
    bool ResetSession() noexcept {
        if(bound_ && std::this_thread::get_id()!=owner_) return false;
        slots_={}; owner_={}; bound_=false; has_snapshot_=false;
        self_=0; scene_=0; tick_=0; last_update_=0;
        return true;
    }
    // MUST be invoked only after authenticating and verifying an authoritative
    // host snapshot at the network boundary. No raw LAN discovery packets.
    // Complete roster snapshot: omission logically drops remote display state.
    bool ApplyTrustedSnapshot(const bo::Packet& packet,uint32_t claimed_scene,
                              uint64_t now) noexcept {
        if(!bound_||std::this_thread::get_id()!=owner_||claimed_scene!=scene_||
           packet.type!=bo::Type::Snapshot||!packet.tick||
           packet.count<2||packet.count>bo::kMaxPlayers||
           (has_snapshot_&&(packet.tick<=tick_||now<last_update_)))
            return false;
        std::array<bool,5> present{};
        std::array<int32_t,5> xx{},yy{};
        for(std::size_t i=0;i<packet.count;++i) {
            const auto& actor=packet.actors[i];
            if(actor.id<1||actor.id>4||present[actor.id]||
               actor.x < -10000||actor.x > 10000||
               actor.y < -10000||actor.y > 10000)return false;
            present[actor.id]=true;xx[actor.id]=actor.x;yy[actor.id]=actor.y;
        }
        // A guest must see a real authenticated host, and the host must not
        // accept a roster that deleted the local player. NEVER clone self.
        if(!present[1]||!present[self_])return false;
        for(uint8_t id=1;id<=4;++id) {
            Slot& slot=slots_[id];
            if(id==self_||!present[id]){slot.present=false;continue;}
            if(!slot.present){
                ++generation_;if(!generation_)++generation_;
                slot.generation=generation_;
            }
            slot.present=true;slot.x=xx[id];slot.y=yy[id];
        }
        tick_=packet.tick;last_update_=now;has_snapshot_=true;
        return true;
    }
    bool ReadLocal(uint64_t now,AvatarView& out) const noexcept {
        if(!bound_||std::this_thread::get_id()!=owner_||!has_snapshot_||
           now<last_update_||now-last_update_>bo::kPeerTimeoutMs)return false;
        out={self_,scene_,tick_,0,0,0,VisualKind::OriginalFirstPerson};
        return true;
    }
    bool ReadRemote(uint8_t player,uint64_t now,AvatarView& out)const noexcept {
        if(!bound_||std::this_thread::get_id()!=owner_||
           player<1||player>4||player==self_||!has_snapshot_||
           !slots_[player].present||now<last_update_||
           now-last_update_>bo::kPeerTimeoutMs)return false;
        const auto& slot=slots_[player];
        out={player,scene_,tick_,slot.generation,slot.x,slot.y,VisualKind::RemoteAllyBody};
        return true;
    }
    uint8_t Self()const noexcept{return self_;}
    uint32_t Scene()const noexcept{return scene_;}
};
} // namespace bo_bridge
