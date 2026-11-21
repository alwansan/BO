#include "scene_inbox.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>
using namespace bo_bridge;

Command Make(uint8_t player,uint32_t scene,uint32_t seq,int16_t x=0,int16_t y=0) {
    return {Kind::RemoteInput,player,scene,seq,x,y};
}
int main() {
    SceneInbox inbox;
    auto first=Make(2,11,1,100,0);
    assert(!inbox.Offer(first)); // no game-thread owner until bound
    assert(!inbox.BindScene(0));
    assert(inbox.BindScene(11));
    assert(!inbox.Offer(Make(1,11,1)));
    assert(!inbox.Offer(Make(5,11,1)));
    assert(!inbox.Offer(Make(2,11,0)));
    assert(!inbox.Offer(Make(2,0,1)));
    assert(!inbox.Offer(Make(2,12,1)));
    assert(!inbox.Offer(Make(2,11,1,1001,0)));
    assert(!inbox.Offer(Make(2,11,1,0,-1001)));
    auto forged=Make(2,11,1);
    forged.kind=static_cast<Kind>(99);
    assert(!inbox.Offer(forged));
    forged.kind=Kind::PeerLeave; forged.move_x=1;
    assert(!inbox.Offer(forged));

    // Worker may enqueue but cannot bind a different game thread or drain.
    bool foreign_bound=true;
    std::size_t foreign_drained=99;
    std::thread worker([&]{
        foreign_bound=inbox.BindScene(12);
        assert(inbox.Offer(first));
        foreign_drained=inbox.Drain([](const Command&){assert(false);});
    });
    worker.join();
    assert(!foreign_bound && foreign_drained==0);
    assert(inbox.Pending()==1);
    std::vector<uint32_t> observed;
    assert(inbox.Drain([&](const Command& c){
        observed.push_back(c.sequence);
        // Callback executes outside the queue mutex: reentrant Offer is safe.
        assert(inbox.Offer(Make(2,11,2,0,-1000)));
    })==1);
    assert((observed==std::vector<uint32_t>{1}));
    assert(inbox.Drain([&](const Command& c){observed.push_back(c.sequence);})==1);
    assert((observed==std::vector<uint32_t>{1,2}));
    // Old and duplicate sequence values cannot be applied twice.
    assert(inbox.Offer(Make(2,11,2)));
    assert(inbox.Offer(Make(2,11,1)));
    assert(inbox.Offer(Make(2,11,4)));
    assert(inbox.Offer(Make(2,11,3)));
    assert(inbox.Drain([&](const Command& c){observed.push_back(c.sequence);})==1);
    assert(observed.back()==4);

    // Fixed-capacity ingress; no unbounded allocations from incoming packets.
    assert(inbox.BindScene(12));
    for(uint32_t seq=1;seq<=64;++seq) assert(inbox.Offer(Make(3,12,seq)));
    assert(!inbox.Offer(Make(3,12,65)));
    assert(inbox.Pending()==64);
    uint32_t prev=0;
    for(int step=0;step<4;++step) {
        assert(inbox.Drain([&](const Command& c){
            assert(c.player==3 && c.sequence==prev+1);
            prev=c.sequence;
        })==16);
    }
    assert(prev==64 && inbox.Pending()==0);
    assert(inbox.Offer(Make(3,12,65)));
    assert(inbox.Offer(Make(2,12,1)));
    // Re-enter scene from same game thread must keep queued commands.
    assert(inbox.BindScene(12));
    assert(inbox.Pending()==2);
    assert(inbox.Drain([&](const Command& c){assert(c.player==3 || c.player==2);})==2);

    // Change scene: invalidate pending events and reset per-peer sequence.
    assert(inbox.Offer(Make(2,12,900)));
    assert(inbox.BindScene(13));
    assert(inbox.Pending()==0);
    assert(!inbox.Offer(Make(2,12,901)));
    assert(inbox.Offer(Make(2,13,1)));
    assert(inbox.Drain([&](const Command& c){
        assert(c.scene==13 && c.sequence==1);
    })==1);
    assert(inbox.Pending()==0);

    // Peer leave is a value-only event, not a game object deletion.
    Command left{Kind::PeerLeave,3,13,1,0,0};
    assert(inbox.Offer(left));
    assert(inbox.Drain([&](const Command& c){assert(c.kind==Kind::PeerLeave);})==1);
    std::cout<<"PASS bounded cross-thread game-thread inbox, scene epochs, dedup, validation\n";
    std::cout<<"No BLACK hooks, guest writes, device test or networking performed\n";
}
