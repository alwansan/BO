// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_host_physics_proxy_contract.hpp"
#include <cassert>
#include <iostream>
int main(){
 using namespace blackcoop;
 std::array<uint8_t,20> used{};
 auto s=CountOriginalHostPhysicsProxies(used,true);
 assert(s.free==20&&s.occupied==0&&s.canReserve(1)&&
        s.canReserve(2)&&s.canReserve(3));
 assert(!s.canReserve(0)&&!s.canReserve(4));
 used.fill(1u);
 s=CountOriginalHostPhysicsProxies(used,true);
 assert(s.free==0&&s.occupied==20&&!s.canReserve(1)&&
        s.coarseCode(3)==2102);
 used[0]=used[1]=used[2]=used[3]=0u;
 s=CountOriginalHostPhysicsProxies(used,true);
 assert(s.free==4&&s.canReserve(3)&&s.coarseCode(3)==2103);
 used[3]=1;
 s=CountOriginalHostPhysicsProxies(used,true);
 assert(s.canReserve(2)&&!s.canReserve(3)&&s.coarseCode(3)==2102);
 s=CountOriginalHostPhysicsProxies(used,false);
 assert(s.free==3&&!s.canReserve(1)&&s.coarseCode(1)==2101);
 std::cout<<"ORIGINAL_HOST_PHYSICS_PROXY_POOL_READ_ONLY_PASS: 20 source slots, own proxy must be valid, one reserve for original enemies, 2-4 players eligibility NO ACTOR ALLOCATED"<<std::endl;
}
