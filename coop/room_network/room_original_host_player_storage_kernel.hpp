// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "room_original_host_player_storage.hpp"
#include "room_original_host_proxy_kernel.hpp"
#include <cstring>
namespace blackcoop {
// One adapter per storage owner. Keep it alive until storage is safely
// released; its fixed ledger never frees allocations implicitly. Quarantined
// initialized resources require a separately proven engine cleanup lifecycle.
class OriginalPlayerStorageKernelIO : public OriginalHostProxyKernelIO {
 struct Allocation {uint32_t address=0,size=0;};
 std::array<Allocation,5> allocations_{};
 X86Context& context_;
public:
 OriginalPlayerStorageKernelIO(X86Context& context,uint64_t (*scene)()noexcept)
  :OriginalHostProxyKernelIO(context,scene),context_(context){}
 OriginalPlayerStorageKernelIO(const OriginalPlayerStorageKernelIO&)=delete;
 OriginalPlayerStorageKernelIO& operator=(const OriginalPlayerStorageKernelIO&)=delete;
 uint32_t Alloc(uint32_t size){
  // Only the reviewed storage sizes, all aligned without arithmetic overflow.
  if(size!=0x16C0u&&size!=0x900u&&size!=0x40u&&size!=0x630u)return 0;
  for(auto& record:allocations_)if(!record.address){
   const uint32_t address=mem::AllocSystem(size,16);
   if(address)record={address,size};return address;
  }
  return 0;
 }
 bool Owns(uint32_t address,uint32_t size)const noexcept{
  for(const auto& record:allocations_)if(record.address==address&&record.size==size)
   return address&&mem::AllocationSize(address)>=size;
  return false;
 }
 // Reconcile with real original storage's tracked owner, not committed pages.
 bool OwnsActor(uint32_t address,uint32_t size)const noexcept{
  return size==0x900u&&Owns(address,size);
 }
 // Read-only, verified original guest page copy for detached shadow planning.
 // Staging must NEVER write P1, host NPC/physics or a borrowed source buffer.
 bool CopyGuest(uint32_t address,void* destination,uint32_t length)const noexcept{
  if(!destination||!Span(address,length))return false;
  std::memcpy(destination,kernel::HostPtr(address),length);
  return true;
 }
 // Only tracked exact allocation bases, never P1/core/world/NPC or any
 // arbitrary committed guest page. Used by opt-in P2 materialization only.
 bool WriteOwned(uint32_t address,const void* source,uint32_t bytes)noexcept{
  if(!source||!bytes||!Owns(address,bytes)||!Span(address,bytes))return false;
  std::memcpy(kernel::HostPtr(address),source,bytes);
  return true;
 }
 bool Free(uint32_t address){
  for(auto& record:allocations_)if(address&&record.address==address){
   if(!Owns(address,record.size))return false;
   mem::FreeSystem(address);record={};return true;
  }
  return false;
 }
 bool Camera(uint32_t owner,uint32_t& result){
  if(!Owns(owner,0x16C0)||!Span(owner,0x16C0)||!dispatch::Find(0x14380))return false;
  X86Context call=context_;
  call.eax=call.ecx=call.edx=call.ebx=call.edi=0;call.esi=owner;
  result=kernel::CallGuest(call,0x14380,{});
  return true;
 }
};
}
