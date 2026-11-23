// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_soldier_copy.hpp"
#include <array>
#include <cassert>
#include <algorithm>
#include <iostream>
int main(){
 constexpr uint32_t root=0x10000,source=root+0xA0,blank=source+31*0x3C0;
 constexpr uint32_t freeList=root+0x79A0,liveList=root+0x79B0;
 std::array<uint32_t,0x3C0/4> donor{},target{};
 donor.fill(0xDEAD);target.fill(0xBEEF);
 donor[0x250/4]=liveList;target[0x250/4]=freeList;
 donor[0x294/4]=source;target[0x294/4]=blank;
 target[0x374/4]=77;target[0x378/4]=UINT32_MAX;
 donor[0x320/4]=0x44400;
 const auto before=target;
 blackcoop::CopyOriginalSoldierLookWords(
  [&](uint32_t off){return donor[off/4];},
  [&](uint32_t off,uint32_t value){target[off/4]=value;});
 assert(target[0x320/4]==donor[0x320/4]);
 for(auto off:{0x250u,0x294u,0x374u,0x378u,0x328u,0x34Cu,0x350u})assert(target[off/4]==before[off/4]);
 // Original1B6770 removes from actor+250's previous sparse container.
 // A copied donor membership leaves blank in BOTH lists after acquisition.
 std::array<uint32_t,2> free{blank,0},live{source,0};
 unsigned freeCount=1,liveCount=1;
 auto& old=target[0x250/4]==freeList?free:live;
 auto found=std::find(old.begin(),old.end(),blank);
 if(found!=old.end()){*found=0;if(&old==&free)--freeCount;else --liveCount;}
 live[1]=blank;target[0x250/4]=liveList;++liveCount;
 assert(freeCount==0&&liveCount==2&&free[0]==0&&live[1]==blank);
 blackcoop::OriginalSoldierSlotIdentity identity{root,blank,77,0x50000};
 assert(identity.InPool(root));assert(!identity.InPool(root+0x10000));
 assert(identity.Matches(root,77,freeList,0x50000));
 assert(!identity.Matches(root,78,freeList,0x50000));
 assert(!identity.Matches(root,77,liveList,0x50000));
 assert(!identity.Matches(root,77,freeList,0x51000));
 identity.actor=source+1;assert(!identity.InPool(root));
 identity={0xFFFFFF00u,0xFFFFFFA0u,77,0x50000};assert(!identity.InPool(identity.pool));
 std::array<uint32_t,0x20000/4> memory{};
 auto put=[&](uint32_t p,uint32_t value){assert(p/4<memory.size());memory[p/4]=value;};
 auto read=[&](uint32_t p,uint32_t& value){if(p%4||p/4>=memory.size())return false;value=memory[p/4];return true;};
 put(freeList,0);put(freeList+4,32);put(freeList+8,1);put(freeList+12,0x18000);
 put(0x18000,blank);put(blank+0x250,freeList);
 assert(blackcoop::OriginalFreeSoldierSlot(read,root,blank));
 put(blank+0x250,liveList);assert(!blackcoop::OriginalFreeSoldierSlot(read,root,blank));
 put(blank+0x250,freeList);put(0x18004,blank);put(freeList+8,2);
 assert(!blackcoop::OriginalFreeSoldierSlot(read,root,blank));
 put(0x18004,0);assert(!blackcoop::OriginalFreeSoldierSlot(read,root,blank));
 put(freeList+8,1);put(0x18000,source+1);assert(!blackcoop::OriginalFreeSoldierSlot(read,root,blank));
 blackcoop::OriginalSoldierAttemptLifetime lifetime;
 lifetime.Observe(9,root);lifetime.Poison();
 for(unsigned i=0;i<20;i++){
  lifetime.Observe(0,0);lifetime.Observe(9,root);assert(lifetime.Failed());
 }
 lifetime.Observe(10,root);assert(!lifetime.Failed());
 std::cout<<"SOLDIER_COPY_OWNERSHIP_PASS: actual clone policy preserves native sparse-list transfer, serial, AI identity and pool lifetime\n";
}
