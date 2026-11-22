#include "room_original_core_phase_witness.hpp"
#include <cassert>
#include <iostream>
using blackcoop::OriginalCoreRunningPhaseWitness;
int main(){
    OriginalCoreRunningPhaseWitness w;
    // Original owner game context, exact original mission resource pair and
    // original source streaming candidate are ALL mandatory.
    for(int n=0;n<50;n++){
        assert(!w.Observe(0,0x300000,0x1C,true,true,true));
        assert(!w.Observe(1,0,0x1C,true,true,true));
        assert(!w.Observe(1,0x300000,0x1C,false,true,true));
        assert(!w.Observe(1,0x300000,0x1C,true,false,true));
        assert(!w.Observe(1,0x300000,0x1C,true,true,false));
        assert(!w.Observe(1,0x300000,0x1B,true,true,true));
    }
    for(int n=0;n<29;n++)assert(!w.Observe(1,0x300000,0x1C,true,true,true));
    assert(w.Observe(1,0x300000,0x1C,true,true,true));
    for(int n=0;n<80;n++)assert(!w.Observe(1,0x300000,0x1C,true,true,true));
    // A genuine new launch with same world address still needs new evidence.
    for(int n=0;n<29;n++)assert(!w.Observe(2,0x300000,0x1C,true,true,true));
    assert(w.Observe(2,0x300000,0x1C,true,true,true));
    // A changing core phase, missing pair, or world pointer resets evidence.
    assert(!w.Observe(2,0x300000,0,true,true,true));
    for(int n=0;n<29;n++)assert(!w.Observe(2,0x300000,0x1C,true,true,true));
    assert(w.Observe(2,0x300000,0x1C,true,true,true));
    assert(!w.Observe(2,0x350000,0x1C,true,true,true));
    for(int n=0;n<28;n++)assert(!w.Observe(2,0x350000,0x1C,true,true,true));
    assert(w.Observe(2,0x350000,0x1C,true,true,true));
    w.Reset();
    for(int n=0;n<29;n++)assert(!w.Observe(2,0x350000,0x1C,true,true,true));
    assert(w.Observe(2,0x350000,0x1C,true,true,true));
    std::cout<<"ORIGINAL_CORE_PHASE_CANDIDATE_PASS: 30 same-epoch source samples, no READY or guest loader\n";
}
