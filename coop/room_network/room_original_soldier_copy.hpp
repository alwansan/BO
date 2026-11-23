// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
namespace blackcoop {
// A detached visual keeps the original slot's intrusive-list and identity
// words. Donor art is never permission to copy its lifetime or AI identity.
inline bool OriginalSoldierOwnWord(uint32_t off)noexcept{
 return off==0x10u||off==0x14u||(off>=0x20u&&off<0x58u)||
  off==0xB0u||off==0xB4u||off==0x250u||off==0x294u||
  (off>=0x298u&&off<0x2A4u)||off==0x324u||off==0x328u||
  off==0x34Cu||off==0x350u||off==0x374u||off==0x378u||off==0x37Cu;
}
template<class Read,class Write>void CopyOriginalSoldierLookWords(Read read,Write write){
 for(uint32_t off=0;off<0x3C0u;off+=4u)
  if(!OriginalSoldierOwnWord(off))write(off,read(off));
}
struct OriginalSoldierSlotIdentity{
 uint32_t pool=0,actor=0,serial=0,instance=0;
 bool InPool(uint32_t current)const noexcept{
  const uint64_t first=uint64_t(pool)+0xA0u,end=first+32u*0x3C0u;
  return pool>=0x10000u&&pool==current&&end<=0x100000000ull&&
   actor>=first&&actor<end&&(uint64_t(actor)-first)%0x3C0u==0;
 }
 bool Matches(uint32_t current,uint32_t currentSerial,
              uint32_t container,uint32_t currentInstance)const noexcept{
  return InPool(current)&&uint64_t(pool)+0x79A0u<=UINT32_MAX&&
   currentSerial==serial&&container==pool+0x79A0u&&instance&&
   currentInstance==instance;
 }
};
// Keep a failed irreversible clone poisoned for its ORIGINAL stage, including
// source readiness gaps and replica-evidence invalidation in that same stage.
class OriginalSoldierAttemptLifetime {
 uint32_t episode_=0,world_=0;bool failed_=false;
public:
 void Observe(uint32_t episode,uint32_t world)noexcept{
  if(!episode||!world)return;
  if(episode_!=episode||world_!=world){episode_=episode;world_=world;failed_=false;}
 }
 void Poison()noexcept{failed_=true;}
 bool Failed()const noexcept{return failed_;}
};
// Validate actual sparse free-container membership immediately before clone.
// Merely seeing model=brain=0 does not prove the slot is available.
template<class Read>bool OriginalFreeSoldierSlot(Read read,uint32_t root,uint32_t actor){
 OriginalSoldierSlotIdentity identity{root,actor,0,0};
 if(!identity.InPool(root)||uint64_t(root)+0x8540u>0x100000000ull)return false;
 const uint32_t container=root+0x79A0u;
 uint32_t flags=0,capacity=0,count=0,table=0,member=0;
 if(!read(container,flags)||flags!=0||!read(container+4,capacity)||capacity!=32||
    !read(container+8,count)||count==0||count>32||!read(container+12,table)||
    table<0x10000u||uint64_t(table)+128u>0x100000000ull||
    !read(actor+0x250u,member)||member!=container)return false;
 uint32_t mask=0,seen=0;bool found=false;
 for(uint32_t i=0;i<32;i++){
  uint32_t item=0;if(!read(table+i*4u,item))return false;if(!item)continue;
  identity.actor=item;if(!identity.InPool(root))return false;
  const uint32_t bit=1u<<((item-root-0xA0u)/0x3C0u);
  if(mask&bit)return false;
  mask|=bit;++seen;found|=item==actor;
 }
 return found&&seen==count;
}
}
