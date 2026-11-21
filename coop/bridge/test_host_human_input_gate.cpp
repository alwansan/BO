// Synthetic-only host ingress checks. Not BLACK engine or TLS integration.
// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "host_human_input_gate.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>
using namespace bo_bridge;

static HumanInput Frame(uint64_t session,uint32_t scene,uint32_t revision,
                        uint8_t player,uint64_t generation,uint32_t seq,
                        uint64_t ms) {
    HumanInput f{};
    f.scope={session,scene,revision};
    f.player=player;
    f.peer_generation=generation;
    f.sequence=seq;f.received_ms=ms;
    f.move_x=100;f.move_y=-100;f.yaw=1200;f.fire=1;f.jump=0;
    return f;
}

int main(){
    HostHumanInputGate gate;
    const HumanWorldScope first{19,8,21},next{19,8,22},otherScene{19,9,23};
    assert(!gate.Bind({0,8,1})&&!gate.Bind({19,0,1})&&!gate.Bind({19,8,0}));
    assert(gate.Bind(first));
    assert(gate.SetPeer(2,90)&&gate.SetPeer(3,81));
    assert(!gate.SetPeer(1,33)&&!gate.SetPeer(5,33));
    auto f=Frame(19,8,21,2,90,1,1000);
    assert(!gate.OfferTrusted(f,false));
    assert(!gate.OfferTrusted(Frame(19,8,21,1,90,1,1000),true));
    assert(!gate.OfferTrusted(Frame(19,8,21,4,33,1,1000),true));
    assert(!gate.OfferTrusted(Frame(19,8,20,2,90,1,1000),true));
    assert(!gate.OfferTrusted(Frame(19,9,21,2,90,1,1000),true));
    assert(!gate.OfferTrusted(Frame(20,8,21,2,90,1,1000),true));
    assert(!gate.OfferTrusted(Frame(19,8,21,2,91,1,1000),true));
    auto invalid=f;invalid.move_x=1001;
    assert(!gate.OfferTrusted(invalid,true));
    invalid=f;invalid.yaw=18001;
    assert(!gate.OfferTrusted(invalid,true));
    invalid=f;invalid.jump=2;
    assert(!gate.OfferTrusted(invalid,true));
    invalid=f;invalid.fire=2;
    assert(!gate.OfferTrusted(invalid,true));
    assert(gate.OfferTrusted(f,true));
    bool outsiderBound=true,outsiderSet=true;
    std::size_t outsiderDrained=7;
    std::thread outsider([&]{
        outsiderBound=gate.Bind(next);
        outsiderSet=gate.SetPeer(2,91);
        outsiderDrained=gate.Drain(1001,[](const HumanInput&){assert(false);});
    });
    outsider.join();
    assert(!outsiderBound&&!outsiderSet&&outsiderDrained==0);
    std::vector<uint32_t> seen;
    assert(gate.Drain(1001,[&](const HumanInput& value){
        seen.push_back(value.sequence);
        // Reentrant producer: callback must never run while holding mutex.
        assert(gate.OfferTrusted(Frame(19,8,21,2,90,2,1001),true));
    })==1);
    assert(gate.Drain(1002,[&](const HumanInput& value){seen.push_back(value.sequence);})==1);
    assert((seen==std::vector<uint32_t>{1,2}));
    // Duplicate, replay and older sequence all filtered on owning game thread.
    assert(gate.OfferTrusted(Frame(19,8,21,2,90,2,1002),true));
    assert(gate.OfferTrusted(Frame(19,8,21,2,90,4,1002),true));
    assert(gate.OfferTrusted(Frame(19,8,21,2,90,3,1002),true));
    assert(gate.Drain(1004,[&](const HumanInput& x){seen.push_back(x.sequence);})==1);
    assert(seen.back()==4);
    // Stale clock packets and future timestamps are dropped.
    assert(gate.OfferTrusted(Frame(19,8,21,2,90,5,100),true));
    assert(gate.OfferTrusted(Frame(19,8,21,2,90,6,1800),true));
    assert(gate.Drain(1500,[](const HumanInput&){assert(false);})==0);
    // A peer leaves; identity is NOT a reused slot index.
    assert(gate.OfferTrusted(Frame(19,8,21,2,90,7,1500),true));
    assert(gate.SetPeer(2,0));
    assert(gate.Pending()==0);
    assert(!gate.OfferTrusted(Frame(19,8,21,2,90,8,1500),true));
    // New join same id must take a DIFFERENT authoritative generation.
    assert(gate.SetPeer(2,91));
    assert(!gate.OfferTrusted(Frame(19,8,21,2,90,8,1500),true));
    assert(gate.OfferTrusted(Frame(19,8,21,2,91,1,1501),true));
    assert(gate.Drain(1502,[&](const HumanInput& value){
        assert(value.peer_generation==91&&value.sequence==1);
    })==1);
    // Changing roster revision or actual game-scene epoch resets all identities.
    assert(gate.OfferTrusted(Frame(19,8,21,2,91,2,1503),true));
    assert(gate.Bind(next)&&gate.Pending()==0);
    assert(!gate.OfferTrusted(Frame(19,8,21,2,91,3,1503),true));
    assert(!gate.OfferTrusted(Frame(19,8,22,2,91,3,1503),true));
    assert(gate.SetPeer(2,92));
    assert(gate.OfferTrusted(Frame(19,8,22,2,92,1,1504),true));
    assert(gate.Bind(otherScene)&&gate.Pending()==0);
    assert(gate.SetPeer(2,93));
    assert(!gate.OfferTrusted(Frame(19,8,22,2,92,2,1505),true));
    // Ring exhaustion must reject new work, never grow or block a game frame.
    for(uint32_t seq=1;seq<=gate.Capacity();++seq)
        assert(gate.OfferTrusted(Frame(19,9,23,2,93,seq,1600),true));
    assert(!gate.OfferTrusted(Frame(19,9,23,2,93,49,1600),true));
    uint32_t last=0,total=0;
    while(gate.Pending()) {
        assert(gate.Drain(1610,[&](const HumanInput& value){
            assert(value.sequence==++last);++total;
        })<=gate.MaxDrain());
    }
    assert(total==gate.Capacity()&&last==48);
    std::cout<<"HOST_HUMAN_INPUT_GATE_PASS: authenticated source required, host scene/roster and reconnect identity fenced, replay/overflow/stale/thread guarded\n";
    std::cout<<"NO BLACK gameplay, remote body, actual authenticated transport integration, or original world mutation\n";
}
