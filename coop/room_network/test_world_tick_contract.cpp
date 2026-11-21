// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "world_tick_contract.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
using blackcoop::WorldTickObserver;
using blackcoop::WorldTickSignal;

int main(){
    WorldTickObserver w;
    assert(w.Observe(0,false)==WorldTickSignal::None);
    assert(w.Observe(0x11110000,false)==WorldTickSignal::None);
    assert(w.Observe(0,true)==WorldTickSignal::None);
    assert(w.Observe(0x11110000,true)==WorldTickSignal::FirstLocalPlayerUpdate);
    assert(w.Observe(0x11110000,true)==WorldTickSignal::None);
    assert(w.Observe(0x22220000,false)==WorldTickSignal::None);
    assert(w.Observe(0x11110000,true)==WorldTickSignal::None);
    assert(w.Observe(0x22220000,true)==WorldTickSignal::WorldPointerChanged);
    assert(w.Observe(0x22220000,true)==WorldTickSignal::None);
    bool produced=false;
    std::thread thread([&] {
        produced=w.Observe(0x22220000,true)==WorldTickSignal::None;
    });
    thread.join();
    assert(produced);
    std::cout<<"WORLD_LOCAL_PLAYER_TICK_CONTRACT_PASS: no pointer export, no first-person mutation, no fake scene ready\n";
}
