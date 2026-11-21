// Synthetic remote-proxy state, not rendered actors or BLACK hooks.
#include "remote_proxy_roster.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
using namespace bo_bridge;
bo::Packet Parse(const bo::Bytes& wire) {
    bo::Packet p;assert(bo::Decode(wire,p));return p;
}
bo::Packet Snap(uint32_t tick,std::initializer_list<bo::Actor> values) {
    std::array<bo::Actor,bo::kMaxPlayers> a{};
    unsigned i=0;
    for(const auto& v: values){ assert(i<bo::kMaxPlayers);a[i++]=v; }
    return Parse(bo::Snapshot(tick,a,static_cast<uint8_t>(i)));
}
int main() {
    RemoteProxyRoster roster;
    ProxyView view{};
    auto first=Snap(1,{{1,0,0},{2,5,-7},{3,3,4}});
    assert(!roster.ApplyTrustedSnapshot(first,17,200));
    assert(!roster.BindScene(0));
    assert(roster.BindScene(17));
    assert(!roster.ApplyTrustedSnapshot(first,18,200));
    assert(roster.ApplyTrustedSnapshot(first,17,200));
    assert(roster.Read(2,200,view) && view.x==5 && view.y==-7);
    const uint64_t firstGeneration=view.generation;
    assert(firstGeneration!=0);
    assert(roster.Read(3,200,view) && view.player==3);
    assert(!roster.Read(1,200,view)); // local host never becomes a proxy.
    assert(!roster.Read(4,200,view));
    assert(!roster.ApplyTrustedSnapshot(first,17,201)); // duplicate tick
    assert(!roster.ApplyTrustedSnapshot(Snap(0,{{1,0,0}}),17,201));
    assert(!roster.ApplyTrustedSnapshot(Snap(2,{{1,0,0},{2,90000,1}}),17,201));
    // Validate a direct Packet too; callers cannot bypass Decode safety.
    bo::Packet malformed=first;malformed.tick=2;malformed.count=3;
    malformed.actors[2].id=2; // repeated remote identity
    assert(!roster.ApplyTrustedSnapshot(malformed,17,201));
    assert(roster.Read(2,202,view) && view.tick==1); // rollback-free
    assert(!roster.Read(2,201+bo::kPeerTimeoutMs+1,view)); // stale fail closed
    assert(!roster.ApplyTrustedSnapshot(Snap(2,{{1,0,0},{2,10,10}}),17,199));
    // Full-roster removal is logical only: there is no engine object to free.
    assert(roster.ApplyTrustedSnapshot(Snap(2,{{1,0,0},{3,12,13}}),17,300));
    assert(!roster.Read(2,300,view));
    assert(roster.Read(3,300,view));
    assert(roster.ApplyTrustedSnapshot(Snap(3,{{1,0,0},{2,20,20}}),17,400));
    assert(roster.Read(2,400,view));
    assert(view.generation!=firstGeneration);
    const uint64_t secondGeneration=view.generation;
    assert(!roster.Read(3,400,view));
    // Any other thread may not apply, bind, or read proxy snapshots.
    bool foreignBind=true,foreignApply=true,foreignRead=true;
    std::thread worker([&] {
        foreignBind=roster.BindScene(18);
        foreignApply=roster.ApplyTrustedSnapshot(Snap(4,{{1,0,0},{2,8,8}}),17,500);
        ProxyView foreign{};
        foreignRead=roster.Read(2,500,foreign);
    });worker.join();
    assert(!foreignBind && !foreignApply && !foreignRead);
    assert(roster.Read(2,400,view) && view.generation==secondGeneration);
    assert(roster.BindScene(18));
    assert(!roster.Read(2,401,view));
    assert(!roster.ApplyTrustedSnapshot(Snap(4,{{1,0,0},{2,2,2}}),17,500));
    assert(roster.ApplyTrustedSnapshot(Snap(1,{{1,0,0},{2,1,1}}),18,500));
    assert(roster.Read(2,500,view) && view.scene==18);
    assert(view.generation!=secondGeneration);
    std::cout<<"PASS ACTOR value-only remote roster: owner, epoch, generation, stale, validation\n";
    std::cout<<"No guest model, player2 actor, or secure network session exists\n";
}
