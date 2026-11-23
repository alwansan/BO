#include "remote_ally_roster.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
using namespace bo_bridge;
static bo::Packet Snap(uint32_t tick,std::initializer_list<bo::Actor> actors){
    std::array<bo::Actor,bo::kMaxPlayers> array{};
    uint8_t n=0;
    for(const auto& actor:actors){assert(n<bo::kMaxPlayers);array[n++]=actor;}
    bo::Packet packet{};
    assert(bo::Decode(bo::Snapshot(tick,array,n),packet));
    return packet;
}
int main(){
    auto two=Snap(1,{{1,2,3},{2,5,6}});
    RemoteAllyRoster host,guest;
    AvatarView view{};
    assert(!host.BindScene(0,1));
    assert(!host.BindScene(10,0));
    assert(host.BindScene(10,1)&&guest.BindScene(10,2));
    assert(!host.ReadRemote(2,100,view));
    assert(!guest.ApplyTrustedSnapshot(two,11,100));
    assert(host.ApplyTrustedSnapshot(two,10,100));
    assert(guest.ApplyTrustedSnapshot(two,10,100));
    assert(host.ReadLocal(100,view)&&view.player==1&&view.appearance==VisualKind::OriginalFirstPerson);
    assert(!host.ReadRemote(1,100,view));
    assert(host.ReadRemote(2,100,view)&&view.appearance==VisualKind::RemoteAllyBody);
    const auto hostRemoteGeneration=view.generation;
    assert(guest.ReadLocal(100,view)&&view.player==2&&view.appearance==VisualKind::OriginalFirstPerson);
    assert(!guest.ReadRemote(2,100,view));
    assert(guest.ReadRemote(1,100,view)&&view.player==1&&view.appearance==VisualKind::RemoteAllyBody);
    assert(!guest.ReadRemote(5,100,view));
    assert(!guest.ApplyTrustedSnapshot(two,10,101)); // duplicate
    auto wrong=Snap(2,{{1,0,0},{3,4,5}});
    assert(!guest.ApplyTrustedSnapshot(wrong,10,102)); // guest's self ID missing
    auto dup=two;dup.tick=2;dup.actors[1].id=1;
    assert(!host.ApplyTrustedSnapshot(dup,10,102));
    assert(host.ReadRemote(2,100,view)&&view.generation==hostRemoteGeneration);
    assert(!host.ReadRemote(2,100+bo::kPeerTimeoutMs+1,view));
    bool foreign=false;
    std::thread worker([&]{foreign=guest.ReadRemote(1,101,view);});
    worker.join();assert(!foreign);
    // The guest who leaves and later rejoins must get a new appearance generation.
    assert(host.ApplyTrustedSnapshot(Snap(2,{{1,1,1},{3,3,3}}),10,200));
    assert(!host.ReadRemote(2,200,view));
    assert(host.ApplyTrustedSnapshot(Snap(3,{{1,2,2},{2,7,7}}),10,300));
    assert(host.ReadRemote(2,300,view)&&view.generation!=hostRemoteGeneration);
    // Scene epoch and the local-self selection invalidate all remote body state.
    assert(guest.BindScene(11,2));
    assert(!guest.ReadRemote(1,300,view));
    assert(!guest.ApplyTrustedSnapshot(two,10,301));
    assert(guest.ApplyTrustedSnapshot(two,11,301));
    assert(guest.BindScene(12,1)); // this phone later becomes the host
    assert(!guest.ReadRemote(2,302,view));
    assert(guest.ApplyTrustedSnapshot(two,12,302));
    assert(guest.ReadRemote(2,302,view));
    assert(guest.ReadLocal(302,view)&&view.player==1);
    // A different authenticated room in the SAME scene starts at tick=1.
    // Foreign threads must not erase the live guest or host view.
    const uint64_t oldGeneration=view.generation;
    bool foreignReset=true;
    std::thread resetWorker([&]{foreignReset=guest.ResetSession();});
    resetWorker.join();
    assert(!foreignReset);
    assert(guest.ReadRemote(2,302,view));
    assert(guest.ResetSession());
    assert(!guest.ReadRemote(2,302,view));
    assert(!guest.ApplyTrustedSnapshot(two,12,400));
    assert(guest.BindScene(12,1));
    assert(guest.ApplyTrustedSnapshot(two,12,400));
    assert(guest.ReadRemote(2,400,view));
    assert(view.tick==1 && view.generation!=oldGeneration);
    std::cout<<"ALLY_VIEW_CONTRACT_PASS: host sees guest body, guest sees host body,"
                " self only first-person, stale/scene/ownership guarded\\n";
    std::cout<<"No actual BLACK render, actor creation, AI ownership or mission start\\n";
}
