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
}
