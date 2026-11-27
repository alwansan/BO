// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "room_original_animation_context.hpp"
#include "kernel/memory.h"
namespace blackcoop {
// Actual SDK physical allocator, no original guest allocator impersonation.
// Only the canonical physical window is accepted by the generation witness.
struct OriginalWeaponBackingKernelAllocator {
 uint32_t Allocate(uint32_t bytes){
  if(bytes!=OriginalWeaponBankLayout::Total)return 0;
  return mem::AllocContiguous(bytes,0,0xffffffffu,0x1000u);
 }
 uint64_t Generation(uint32_t p,uint32_t n)const noexcept{
  return n==OriginalWeaponBankLayout::Total?
    mem::CoopContiguousAllocationGeneration(p,n):0;
 }
 void Free(uint32_t p){mem::FreeContiguous(p);}
};
using OriginalWeaponBackingKernelOwner=OriginalWeaponBackingOwner<OriginalWeaponBackingKernelAllocator>;
// Pinned PC source identifies P2 slots 16/18 only. No invented P3/P4
// descriptors. Explicit reservation witness must establish exclusive ownership.
class OriginalWeaponHeapKernelIO final {
 using Proof=bool(*)(uint8_t,uint32_t,uint32_t,uint32_t,uint64_t)noexcept;
 uint64_t(*scene_)()noexcept;
 Proof proof_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 friend class OriginalWeaponHeapInstall;
 uint8_t* MapHeapDescriptor(uint32_t at,uint32_t n)const noexcept{
  if(!owner()||!Scene()||n!=0x24u||
     (at!=0x2D12ECu+16u*0x24u&&at!=0x2D12ECu+18u*0x24u)||
     !mem::IsCommitted(at)||!mem::IsCommitted(at+n-1u)||!mem::Base())return nullptr;
  return mem::Base()+at;
 }
public:
 OriginalWeaponHeapKernelIO(uint64_t(*scene)()noexcept,Proof proof):scene_(scene),proof_(proof){}
 uint64_t Scene()const noexcept{return owner()&&scene_?scene_():0;}
 bool VerifiedUnusedOriginalWeaponHeap(uint32_t id)const noexcept{
   if(id!=16u&&id!=18u)return false;
   const auto address=0x2D12ECu+id*0x24u;
   auto* p=MapHeapDescriptor(address,0x24u);
   constexpr uint32_t selectedHeap=0x2D164Cu,selectedIndex=0x2D1654u;
   if(!p||!mem::IsCommitted(selectedHeap)||!mem::IsCommitted(selectedIndex+3u))return false;
   uint32_t enabled=0,active=0,index=0;
   std::memcpy(&enabled,p,4u);
   std::memcpy(&active,mem::Base()+selectedHeap,4u);
   std::memcpy(&index,mem::Base()+selectedIndex,4u);
   return !enabled&&active!=address&&index!=id;
 }
 bool OwnsReservedHeapPair(uint8_t peer,uint32_t first,uint32_t second,
       uint32_t base,uint64_t generation)const noexcept{
  return Scene()&&peer==2&&first==16u&&second==18u&&proof_&&
     proof_(peer,first,second,base,generation)&&
     mem::CoopContiguousAllocationGeneration(base,OriginalWeaponBankLayout::Total)==generation;
 }
};
}
