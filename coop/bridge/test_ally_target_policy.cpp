// Synthetic-only targets, snapshots and player allegiance claims.
// NOT the original BLACK green HUD renderer or a playable gameplay system.
#include "ally_target_policy.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace bo_bridge;
static bo::Packet Snap(uint32_t tick,std::initializer_list<bo::Actor> actors) {
    std::array<bo::Actor,bo::kMaxPlayers> a{};
    unsigned i=0;
    for(const auto& v:actors){ assert(i<bo::kMaxPlayers);a[i++]=v; }
    bo::Packet p{};
    assert(bo::Decode(bo::Snapshot(tick,a,static_cast<uint8_t>(i)),p));
    return p;
}
static void FriendlyHuman(const TargetDecision& d) {
    assert(d.kind==TargetKind::AuthenticatedRemoteHuman);
    assert(d.reticle==ReticleHint::FriendlyGreen);
    assert(d.damage==DamageHint::Deny);
    assert(d.remoteVisual==VisualKind::RemoteAllyBody);
    assert(!d.npcBrainOwned);
}
static void Unknown(const TargetDecision& d) {
    assert(d.kind==TargetKind::Unknown);
    assert(d.reticle==ReticleHint::Neutral);
    assert(d.damage==DamageHint::Deny);
}
int main() {
    RemoteAllyRoster host,guest;
    AllyTargetPolicy hostPolicy,guestPolicy;
    assert(!hostPolicy.BindVerifiedScene(44,1,false));
    assert(!hostPolicy.BindVerifiedScene(0,1,true));
    assert(!hostPolicy.BindVerifiedScene(44,0,true));
    assert(hostPolicy.BindVerifiedScene(44,1,true));
    assert(guestPolicy.BindVerifiedScene(44,2,true));
    assert(host.BindScene(44,1)&&guest.BindScene(44,2));
    Unknown(hostPolicy.FromRemoteHuman(host,2,7,100,true));
    auto two=Snap(1,{{1,0,0},{2,3,5}});
    assert(host.ApplyTrustedSnapshot(two,44,100));
    assert(guest.ApplyTrustedSnapshot(two,44,100));
    AvatarView guestOnHost{},hostOnGuest{},self{};
    assert(host.ReadRemote(2,100,guestOnHost));
    assert(guest.ReadRemote(1,100,hostOnGuest));
    assert(guest.ReadLocal(100,self)&&self.appearance==VisualKind::OriginalFirstPerson);
    FriendlyHuman(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,100,true));
    FriendlyHuman(guestPolicy.FromRemoteHuman(guest,1,hostOnGuest.generation,100,true));
    Unknown(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,100,false));
    Unknown(hostPolicy.FromRemoteHuman(host,1,guestOnHost.generation,100,true));
    Unknown(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation+1,100,true));
    Unknown(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,
                                            101+bo::kPeerTimeoutMs,true));
    // A real original ally is a verified NPC friendly; it is NOT a human.
    Unknown(hostPolicy.FromOriginalActor(EngineRelation::OriginalAlly,false));
    Unknown(hostPolicy.FromOriginalActor(EngineRelation::Unknown,true));
    auto original=hostPolicy.FromOriginalActor(EngineRelation::OriginalAlly,true);
    assert(original.kind==TargetKind::OriginalAllyBot);
    assert(original.reticle==ReticleHint::FriendlyGreen);
    assert(original.damage==DamageHint::Deny&&original.npcBrainOwned);
    // No client-provided "enemy" or color allows damage without engine proof.
    Unknown(hostPolicy.FromOriginalActor(EngineRelation::OriginalEnemy,false));
    auto enemy=hostPolicy.FromOriginalActor(EngineRelation::OriginalEnemy,true);
    assert(enemy.kind==TargetKind::OriginalHostileNpc);
    assert(enemy.reticle==ReticleHint::Neutral);
    assert(enemy.damage==DamageHint::OriginalHostileOnly);
    // Rejoin reuses player slot but NEVER the visible avatar generation.
    assert(host.ApplyTrustedSnapshot(Snap(2,{{1,0,0},{3,7,8}}),44,200));
    Unknown(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,200,true));
    assert(host.ApplyTrustedSnapshot(Snap(3,{{1,0,0},{2,7,8}}),44,300));
    assert(host.ReadRemote(2,300,guestOnHost));
    assert(guestOnHost.generation!=0);
    Unknown(hostPolicy.FromRemoteHuman(host,2,1,300,true));
    FriendlyHuman(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,300,true));
    // A new actual scene cannot inherit the old green/immune target handle.
    assert(hostPolicy.BindVerifiedScene(45,1,true));
    Unknown(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,300,true));
    assert(host.BindScene(45,1));
    assert(host.ApplyTrustedSnapshot(two,45,400));
    assert(host.ReadRemote(2,400,guestOnHost));
    FriendlyHuman(hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,400,true));
    // No use of the game-thread-only policy from UI/transport workers.
    bool accepted=false;TargetDecision outsider{};
    std::thread worker([&] {
        accepted=hostPolicy.BindVerifiedScene(46,1,true);
        outsider=hostPolicy.FromRemoteHuman(host,2,guestOnHost.generation,401,true);
    });
    worker.join();
    assert(!accepted);Unknown(outsider);
    assert(hostPolicy.Scene()==45&&hostPolicy.Self()==1);
    std::cout<<"ALLY_TARGET_POLICY_PASS: green friendly/deny fire for trusted"
                " remote human and verified original friendly only; no NPC-AI human reuse\n";
    std::cout<<"NO original BLACK HUD, target raycast, damage path, gameplay TLS"
                " or host scene callback installed\n";
}
