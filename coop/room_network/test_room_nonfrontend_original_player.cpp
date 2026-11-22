#include "room_nonfrontend_original_player.hpp"
#include <cassert>
#include <iostream>
int main(){
    blackcoop::NonFrontendOriginalPlayerWitness w{};
    constexpr unsigned game=0x100000,world=0x200000,offset=0x20220;
    for(int i=0;i<100;i++)
        assert(!w.Observe(game,game+offset,offset,world,true));
    for(int i=0;i<29;i++)
        assert(!w.Observe(game,game+0x2000,offset,world,true));
    assert(w.Observe(game,game+0x2000,offset,world,true));
    for(int i=0;i<200;i++)
        assert(!w.Observe(game,game+0x2000,offset,world,true));
    for(int i=0;i<29;i++)
        assert(!w.Observe(game,game+0x2000,offset,world+0x100,true));
    assert(w.Observe(game,game+0x2000,offset,world+0x100,true));
    assert(!w.Observe(game,game+0x2000,offset,world+0x100,false));
    for(int i=0;i<29;i++)
        assert(!w.Observe(game,game+0x3000,offset,world+0x100,true));
    assert(w.Observe(game,game+0x3000,offset,world+0x100,true));
    assert(!w.Observe(0,0,offset,world,true));
    assert(!w.Observe(game,0,offset,world,true));
    assert(!w.Observe(game,game+0x2000,offset,0,true));
    std::cout<<"ORIGINAL_NONFRONTEND_STABLE_WITNESS_PASS: 30 original-player samples with identical world/state, reset on transition, no mission/scene proof\n";
}
