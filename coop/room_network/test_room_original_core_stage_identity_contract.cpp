// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_core_stage_identity_contract.hpp"
#include <cassert>
#include <cstdio>
using blackcoop::OriginalCoreStageIdentityWitness;
int main(){
    OriginalCoreStageIdentityWitness w;
    for(int i=0;i<29;i++)assert(w.Observe(4,0x123400,true,0,1,true,0,1)==0);
    assert(w.Observe(4,0x123400,true,0,1,true,0,1)==67);
    assert(w.Observe(4,0x123400,true,0,1,true,0,1)==0);
    // Fail closed after original launch transition, even with the same world.
    for(int i=0;i<29;i++)assert(w.Observe(5,0x123400,true,0,1,true,1,1)==0);
    assert(w.Observe(5,0x123400,true,0,1,true,1,1)==68);
    assert(w.Observe(5,0x123400,false,0,1,true,1,1)==0);
    for(int i=0;i<30;i++)
        assert(w.Observe(5,0x123400,true,0,1,false,-1,-1)==(i==29?69:0));
    // Unstable core byte must reset the consecutive-sample streak.
    w.Reset();
    for(int i=0;i<20;i++)assert(w.Observe(8,0x20000,true,3,1,true,3,1)==0);
    assert(w.Observe(8,0x20000,true,3,1,true,3,2)==0);
    for(int i=0;i<29;i++)assert(w.Observe(8,0x20000,true,3,1,true,3,1)==0);
    assert(w.Observe(8,0x20000,true,3,1,true,3,1)==67);
    // No source stream claim or invalid level should ever be a MATCH.
    w.Reset();
    for(int i=0;i<30;i++)
        assert(w.Observe(9,0x20000,true,-1,1,true,0,1)==(i==29?69:0));
    w.Reset();
    for(int i=0;i<30;i++)
        assert(w.Observe(9,0x20000,true,0,1,true,255,1)==(i==29?69:0));
    // UNKNOWN at startup must not hide the later matching source state.
    w.Reset();
    for(int i=0;i<30;i++)
        assert(w.Observe(11,0x30000,true,0,0,false,-1,-1)==(i==29?69:0));
    for(int i=0;i<30;i++)
        assert(w.Observe(11,0x30000,true,0,0,true,0,0)==(i==29?67:0));
    assert(w.Observe(11,0x30000,true,0,0,true,0,0)==0);
    std::puts("ORIGINAL_CORE_STAGE_IDENTITY_SOURCE_ONLY_PASS");
}
