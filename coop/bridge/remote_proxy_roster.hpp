// BO-COOP ACTOR-001 staging: authoritative, value-only remote proxy roster.
// NO BLACK game objects, actor factories, guest pointers, JNI, network trust,
// physics, animation instances, saves, or in-game multiplayer.
// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "../native/net_core.hpp"
#include <array>
#include <cstdint>
#include <thread>
#include <limits>

namespace bo_bridge {
struct ProxyView {
    uint8_t player=0;
    uint32_t scene=0, tick=0;
    uint64_t generation=0;
    int32_t x=0,y=0; // toy grid only, NOT real BLACK world coordinates.
};
class RemoteProxyRoster {
    struct Slot {
        bool present=false;
        uint64_t generation=0;
        int32_t x=0,y=0;
    };
    std::array<Slot,5> slots_{};
    std::thread::id owner_{};
    bool bound_=false, has_snapshot_=false;
    uint32_t scene_=0, tick_=0;
    uint64_t last_update_=0, generation_=0;
public:
    // Caller must obtain this scene epoch on a verified game-owned thread.
    bool BindScene(uint32_t scene) noexcept {
        if(!scene) return false;
        const auto caller=std::this_thread::get_id();
        if(bound_ && owner_!=caller) return false;
        if(!bound_) {bound_=true;owner_=caller;}
        if(scene_!=scene) {
            scene_=scene; tick_=0; has_snapshot_=false; last_update_=0;
            slots_={};
        }
        return true;
    }
    // Call on authenticated session disconnect/replacement, including when the
    // next room reuses the same original scene identifier. Never carry remote
    // actors or monotonic packet ticks across separate network sessions.
    // Only the bound game thread may clear an active scene.
    bool ResetSession() noexcept {
        if(bound_ && std::this_thread::get_id()!=owner_) return false;
        slots_={}; scene_=0; tick_=0; has_snapshot_=false;
        last_update_=0; owner_={}; bound_=false;
        // Deliberately preserve generation_: stale renderer handles from a
        // previous session must never match a recycled player slot.
        return true;
    }
    // This must be called ONLY after production-grade authenticated
    // session validation at the network boundary. The current toy protocol
    // has NO such authentication: this is not a WAN-safe entry point.
    // Snapshots are FULL rosters, not differential actor-spawn commands.
    bool ApplyTrustedSnapshot(const bo::Packet& packet,
                              uint32_t claimed_scene,uint64_t now) noexcept {
        if(!bound_ || std::this_thread::get_id()!=owner_ ||
           claimed_scene!=scene_ || !scene_ ||
           packet.type!=bo::Type::Snapshot || !packet.tick ||
           packet.count<1 || packet.count>bo::kMaxPlayers ||
           (has_snapshot_ && (packet.tick<=tick_ || now<last_update_)))
            return false;
        std::array<bool,5> present{};
        std::array<int32_t,5> xx{},yy{};
        for(std::size_t i=0;i<packet.count;++i) {
            const auto& actor=packet.actors[i];
            if(actor.id<1 || actor.id>4 || present[actor.id] ||
               actor.x < -10000 || actor.x > 10000 ||
               actor.y < -10000 || actor.y > 10000)
                return false;
            present[actor.id]=true;
            xx[actor.id]=actor.x;yy[actor.id]=actor.y;
        }
        if(!present[1]) return false; // original host must be included
        // No mutation until the entire authoritative roster is valid.
        for(uint8_t id=2;id<=4;++id) {
            auto& slot=slots_[id];
            if(!present[id]) {
                slot.present=false; // no actual guest avatar to delete
                continue;
            }
            if(!slot.present) {
                // Identifiers can be recycled after disconnects. A new
                // generation is required before an eventual renderer binds.
                ++generation_;
                if(!generation_) ++generation_;
                slot.generation=generation_;
            }
            slot.present=true;
            slot.x=xx[id];slot.y=yy[id];
        }
        tick_=packet.tick;
        last_update_=now;
        has_snapshot_=true;
        return true;
    }
    bool Read(uint8_t id,uint64_t now,ProxyView& out) const noexcept {
        if(!bound_ || std::this_thread::get_id()!=owner_ ||
           id<2 || id>4 || !has_snapshot_ || !slots_[id].present ||
           now<last_update_ || now-last_update_>bo::kPeerTimeoutMs)
            return false;
        const auto& s=slots_[id];
        out={id,scene_,tick_,s.generation,s.x,s.y};
        return true;
    }
    uint32_t Scene() const noexcept { return scene_; }
};
} // namespace bo_bridge
