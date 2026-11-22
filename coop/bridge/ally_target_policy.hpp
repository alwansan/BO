// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// HUMAN-ALLY-POLICY-001: value-only target classification, NO game hooks.
// Original ally bot allegiance and human peer identity are DISTINCT evidence.
// No original actor, weapon, HUD, damage, scene, guest memory or NPC AI writes.
#pragma once
#include "remote_ally_roster.hpp"
#include <cstdint>
#include <thread>

namespace bo_bridge {
enum class TargetKind : uint8_t {
    Unknown=0, OriginalAllyBot=1, AuthenticatedRemoteHuman=2,
    OriginalHostileNpc=3
};
enum class ReticleHint : uint8_t { Neutral=0, FriendlyGreen=1 };
enum class DamageHint : uint8_t { Deny=0, OriginalHostileOnly=1 };
struct TargetDecision {
    TargetKind kind=TargetKind::Unknown;
    ReticleHint reticle=ReticleHint::Neutral;
    DamageHint damage=DamageHint::Deny;
    VisualKind remoteVisual=VisualKind::OriginalFirstPerson;
    bool npcBrainOwned=false;
    bool operator==(const TargetDecision&) const = default;
};
enum class EngineRelation : uint8_t {
    Unknown=0, OriginalAlly=1, OriginalEnemy=2
};
// POLICY IS NOT THE GAME'S FRIEND/FOE ALGORITHM. Future verified engine
// integration must prove original allegiance + reticle/damage call sites.
// This is only a fail-closed gate around independently validated evidence.
class AllyTargetPolicy final {
    std::thread::id owner_{};
    uint32_t scene_=0;
    uint8_t self_=0;
    bool bound_=false;
    bool onThread() const noexcept {
        return bound_ && owner_==std::this_thread::get_id();
    }
    static TargetDecision unknown() noexcept { return {}; }
    static TargetDecision friendHuman() noexcept {
        return {TargetKind::AuthenticatedRemoteHuman,ReticleHint::FriendlyGreen,
            DamageHint::Deny,VisualKind::RemoteAllyBody,false};
    }
    static TargetDecision originalAlly() noexcept {
        return {TargetKind::OriginalAllyBot,ReticleHint::FriendlyGreen,
            DamageHint::Deny,VisualKind::OriginalFirstPerson,true};
    }
public:
    // "confirmed" cannot be inferred from world-pointer changes, a first
    // player tick, or GPU draw. Only a real engine scene-loaded callback.
    bool BindVerifiedScene(uint32_t scene,uint8_t localPlayer,
                           bool independentlyConfirmed) noexcept {
        if(!independentlyConfirmed||!scene||localPlayer<1||localPlayer>4)
            return false;
        if(bound_&&!onThread())return false;
        if(!bound_){owner_=std::this_thread::get_id();bound_=true;}
        scene_=scene;self_=localPlayer;
        return true;
    }
    TargetDecision FromOriginalActor(EngineRelation relation,
                                     bool verifiedOriginalGameFaction) const noexcept {
        if(!onThread()||!verifiedOriginalGameFaction)return unknown();
        if(relation==EngineRelation::OriginalAlly)return originalAlly();
        if(relation==EngineRelation::OriginalEnemy)
            return {TargetKind::OriginalHostileNpc,ReticleHint::Neutral,
                DamageHint::OriginalHostileOnly,VisualKind::OriginalFirstPerson,true};
        return unknown();
    }
    // Authentication MUST be attested by the actual TLS room transport and
    // its roster, not supplied by arbitrary UI/UDP packets. Reject stale
    // visible proxy generations to avoid targeting a different rejoining peer.
    TargetDecision FromRemoteHuman(const RemoteAllyRoster& roster,
                                   uint8_t player,uint64_t visibleGeneration,
                                   uint64_t now,bool authenticatedHostSnapshot) const noexcept {
        if(!onThread()||!authenticatedHostSnapshot||!visibleGeneration||
           roster.Scene()!=scene_||roster.Self()!=self_||
           player<1||player>4||player==self_)return unknown();
        AvatarView observed{};
        if(!roster.ReadRemote(player,now,observed)||
           observed.player!=player||observed.scene!=scene_||
           observed.generation!=visibleGeneration||
           observed.appearance!=VisualKind::RemoteAllyBody)return unknown();
        return friendHuman();
    }
    uint32_t Scene() const noexcept { return onThread()?scene_:0; }
    uint8_t Self() const noexcept { return onThread()?self_:0; }
};
} // namespace bo_bridge
