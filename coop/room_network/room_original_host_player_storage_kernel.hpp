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
 struct Allocation {uint32_t address=0,size=0;uint64_t generation=0;};
 std::array<Allocation,5> allocations_{};
 X86Context& context_;
 using LifetimeSource=uint64_t(*)(uint32_t,uint32_t)noexcept;
 LifetimeSource lifetime_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool onOwner()const noexcept{return thread_==std::this_thread::get_id();}
public:
 OriginalPlayerStorageKernelIO(X86Context& context,uint64_t (*scene)()noexcept,
     LifetimeSource lifetime=nullptr)
  :OriginalHostProxyKernelIO(context,scene),context_(context),lifetime_(lifetime){}
 OriginalPlayerStorageKernelIO(const OriginalPlayerStorageKernelIO&)=delete;
 OriginalPlayerStorageKernelIO& operator=(const OriginalPlayerStorageKernelIO&)=delete;
 uint32_t Alloc(uint32_t size){
  // Only the reviewed storage sizes, all aligned without arithmetic overflow.
  if(!onOwner()||!lifetime_||
     (size!=0x16C0u&&size!=0x900u&&size!=0x40u&&size!=0x630u))return 0;
  for(auto& record:allocations_)if(!record.address){
   const uint32_t address=mem::AllocSystem(size,16);
   if(!address)return 0;
   const auto generation=lifetime_(address,size);
   // A newly allocated block is not published without actual lifetime proof.
   if(!generation){mem::FreeSystem(address);return 0;}
   record={address,size,generation};return address;
  }
  return 0;
 }
 bool Owns(uint32_t address,uint32_t size)const noexcept{
  if(!onOwner()||!lifetime_)return false;
  for(const auto& record:allocations_)if(record.address==address&&record.size==size)
   return address&&record.generation&&mem::AllocationSize(address)==size&&
     lifetime_(address,size)==record.generation;
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
private:
 // The ShadowPlan is the only guest writer: other callers must enter through
 // OriginalHostPlayerStage::MaterializeOwned, which quarantines first.
 friend class OriginalHostPlayerShadowPlan;
 // Only tracked exact allocation bases, never P1/core/world/NPC or any
 // arbitrary committed guest page. Used by opt-in P2 materialization only.
 bool WriteOwned(uint32_t address,const void* source,uint32_t bytes)noexcept{
  if(!source||!bytes||!Owns(address,bytes)||!Span(address,bytes))return false;
  std::memcpy(kernel::HostPtr(address),source,bytes);
  return true;
 }
public:
 bool Free(uint32_t address){
  if(!onOwner())return false;
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
// Narrow real Android guest-memory adapter for the actor-only exchange.
// The lifetime source MUST come from actual original allocator/teardown
// hooks; a scene counter, committed-page check, or size is NOT a generation.
// No default lifetime source and no production caller.
class OriginalActorExchangeKernelIO final {
 OriginalPlayerStorageKernelIO& storage_;
 OriginalPlayerStorageScene binding_{};
 uint32_t target_=0;
 const std::thread::id owner_=std::this_thread::get_id();
 using LifetimeSource=uint64_t(*)(uint32_t,uint32_t)noexcept;
 LifetimeSource lifetime_=nullptr;
 uint64_t coreGeneration_=0,targetGeneration_=0;
 bool onOwner()const noexcept{return owner_==std::this_thread::get_id();}
 bool live()const noexcept{
  uint32_t core=0,owner=0,physics=0,npcs=0;
  return onOwner()&&lifetime_&&binding_.scene&&
   storage_.Scene()==binding_.scene&&coreGeneration_&&targetGeneration_&&
   storage_.Read32(0x2D45D4u,core)&&core==binding_.core&&
   storage_.Read32(0x2D12B8u,owner)&&owner==binding_.owner&&
   storage_.Read32(0x2D45F4u,physics)&&physics==binding_.physics&&
   storage_.Read32(0x2D1980u,npcs)&&npcs==binding_.npcs&&
   storage_.Span(core,0x5CC0u)&&storage_.OwnsActor(target_,0x900u)&&
   storage_.Span(target_,0x900u)&&
   lifetime_(core,0x5CC0u)==coreGeneration_&&
   lifetime_(target_,0x900u)==targetGeneration_;
 }
public:
 OriginalActorExchangeKernelIO(OriginalPlayerStorageKernelIO& storage,
   const OriginalPlayerStorageScene& binding,uint32_t ownedActor,
   LifetimeSource generation)noexcept
  :storage_(storage),binding_(binding),target_(ownedActor),lifetime_(generation){
  if(!generation||!binding.core||binding.core>0xfffffff0u-0x30u||
     !storage_.OwnsActor(ownedActor,0x900u))return;
  coreGeneration_=generation(binding.core,0x5CC0u);
  targetGeneration_=generation(ownedActor,0x900u);
 }
 OriginalActorExchangeKernelIO(const OriginalActorExchangeKernelIO&)=delete;
 OriginalActorExchangeKernelIO& operator=(const OriginalActorExchangeKernelIO&)=delete;
 bool Live()const noexcept{return live();}
 uint64_t Scene()const noexcept{return live()?binding_.scene:0;}
 bool Span(uint32_t p,uint32_t n)const noexcept{
  return onOwner()&&storage_.Span(p,n);
 }
 bool Read32(uint32_t p,uint32_t& out)const noexcept{
  return onOwner()&&storage_.Read32(p,out);
 }
 bool CopyGuest(uint32_t p,void* out,uint32_t n)const noexcept{
  return onOwner()&&storage_.CopyGuest(p,out,n);
 }
 bool OwnsActor(uint32_t p,uint32_t n)const noexcept{
  return live()&&p==target_&&n==0x900u;
 }
 uint64_t ActorGeneration(uint32_t p,uint32_t n)const noexcept{
  if(n!=0x900u||!live())return 0;
  if(p==binding_.core+0x30u)return coreGeneration_;
  if(p==target_)return targetGeneration_;
  return 0;
 }
private:
 // Only the actor exchange plan may get writable mappings; not JNI,
 // transport, or a general caller. The plan checks overlap and protected
 // world-link words before exchanging anything.
 friend class OriginalPlayerLinkExchangePlan;
 uint8_t* MapOriginalActor(uint32_t p,uint32_t n)const noexcept{
  if(!ActorGeneration(p,n)||!storage_.Span(p,n))return nullptr;
  return static_cast<uint8_t*>(kernel::HostPtr(p));
 }
};
}
