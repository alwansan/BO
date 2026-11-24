// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "room_original_host_player_storage.hpp"
#include "room_original_host_proxy_kernel.hpp"
#include "room_original_host_player_link_exchange.hpp"
#include "room_original_host_setup_tick.hpp"
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
// Original camera singleton mapping, separate from actor IO (which requires
// P1 owner restored). All writable access restricted to camera scope.
class OriginalCameraKernelIO final {
 OriginalPlayerStorageKernelIO& storage_;
 OriginalPlayerStorageScene binding_{};
 uint32_t extra_=0;
 using LifetimeSource=uint64_t(*)(uint32_t,uint32_t)noexcept;
 LifetimeSource lifetime_;
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t coreGen_=0;
 bool live()const noexcept{
  uint32_t core=0,physics=0,npcs=0;
  return thread_==std::this_thread::get_id()&&lifetime_&&coreGen_&&
    binding_.scene&&storage_.Scene()==binding_.scene&&
    storage_.Read32(0x2D45D4u,core)&&core==binding_.core&&
    lifetime_(core,0x5CC0u)==coreGen_&&
    storage_.Read32(0x2D45F4u,physics)&&physics==binding_.physics&&
    storage_.Read32(0x2D1980u,npcs)&&npcs==binding_.npcs;
 }
public:
 OriginalCameraKernelIO(OriginalPlayerStorageKernelIO& storage,
    const OriginalPlayerStorageScene& binding,uint32_t extra,
    LifetimeSource lifetime)noexcept
  :storage_(storage),binding_(binding),extra_(extra),lifetime_(lifetime){
  if(lifetime_&&binding_.core)coreGen_=lifetime_(binding_.core,0x5CC0u);
 }
 uint64_t CameraScene()const noexcept{return live()?binding_.scene:0;}
 uint64_t CameraGeneration(uint32_t p,uint32_t n)const noexcept{
  if(!live()||n!=0x16C0u||(p!=binding_.owner&&p!=extra_)||
     !storage_.Span(p,n)||(p==extra_&&!storage_.Owns(p,n)))return 0;
  return lifetime_(p,n);
 }
 bool OwnsExtraCamera(uint32_t p,uint32_t n)const noexcept{
  return p==extra_&&CameraGeneration(p,n)!=0;
 }
private:
 friend class OriginalCameraOwnerScope;
 uint8_t* MapCameraOwnerRoot()const noexcept{
  if(!live()||!storage_.Span(0x2D12B8u,4u))return nullptr;
  return static_cast<uint8_t*>(kernel::HostPtr(0x2D12B8u));
 }
};

// Exact original 1E0E40 setup ABI. A full presentation context provider
// must isolate/restore original animation, banks, weapons, FX and audio,
// including partial entry recovery. It is not implemented in production.
// Do not call this adapter from current Android gameplay.
template<class Presentation>class OriginalSetupKernelIO final {
 OriginalPlayerStorageKernelIO& storage_;
 OriginalPlayerStorageScene binding_;
 OriginalPlayerStorageView view_;
 OriginalActorExchangeKernelIO actorIo_;
 OriginalCameraKernelIO cameraIo_;
 OriginalPlayerLinkExchangePlan actor_;
 OriginalCameraOwnerScope camera_;
 Presentation& presentation_;
 X86Context& ctx_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,entered_=false,presentationAttempted_=false,
      restored_=false,poisoned_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 bool match(const OriginalSetupScene& s)const noexcept{
  return owner()&&s.nativeScene==binding_.scene&&s.world==binding_.core&&
   s.p1Actor==binding_.core+0x30u&&s.p1Owner==binding_.owner&&
   s.ownedActor==view_.actor&&s.ownedOwner==view_.owner&&s.peer>=2&&s.peer<=4;
 }
public:
 OriginalSetupKernelIO(X86Context& ctx,OriginalPlayerStorageKernelIO& storage,
     const OriginalPlayerStorageScene& binding,const OriginalPlayerStorageView& materialized,
     uint64_t(*lifetime)(uint32_t,uint32_t)noexcept,Presentation& presentation)
  :storage_(storage),binding_(binding),view_(materialized),
   actorIo_(storage,binding,materialized.actor,lifetime),
   cameraIo_(storage,binding,materialized.owner,lifetime),
   presentation_(presentation),ctx_(ctx){}
 uint64_t OriginalScene()const noexcept{return owner()?storage_.Scene():0;}
 bool Read32(uint32_t p,uint32_t& v)const noexcept{return owner()&&storage_.Read32(p,v);}
 uint32_t CorePhase(uint32_t core)const noexcept{
  uint32_t phase=0;return core==binding_.core&&Read32(core+0x5AE0u,phase)?phase:0;
 }
 bool OwnsMaterializedActor(uint32_t p,uint32_t n)const noexcept{
  return owner()&&p==view_.actor&&storage_.OwnsActor(p,n);
 }
 bool OwnsOriginalCamera(uint32_t p,uint32_t n)const noexcept{
  return owner()&&p==view_.owner&&n==0x16C0u&&storage_.Owns(p,n);
 }
 bool EnterOwnedOriginalContext(const OriginalSetupScene& s){
  if(!match(s)||attempted_||poisoned_)return false;
  attempted_=true;restored_=false;
  if(!presentation_.CanEnterOriginal(s)||!actor_.Prepare(actorIo_,binding_,view_.actor)||
     !actor_.EnterActorOnly(actorIo_)||
     !camera_.Enter(cameraIo_,binding_.scene,binding_.owner,view_.owner))return false;
  presentationAttempted_=true;
  if(!presentation_.EnterOriginal(s)||!presentation_.OriginalContextActive(s))return false;
  entered_=true;return true;
 }
 bool GuestSetup1E0E40(uint32_t world,uint32_t eax,uint32_t& result){
  uint32_t current=0;
  if(!owner()||poisoned_||!entered_||!actor_.Entered()||!camera_.Entered()||
     world!=binding_.core||eax||!dispatch::Find(0x1E0E40u)||
     !Read32(0x2D12B8u,current)||current!=view_.owner)return false;
  X86Context call=ctx_;
  call.eax=0;call.edx=world;call.ecx=call.ebx=call.esi=call.edi=0;
  result=kernel::CallGuest(call,0x1E0E40u,{});
  return true;
 }
 // Original human-only update ABI from pinned PC black_p2_update.
 // No world/AI step. A real provider must prove input and interaction
 // ownership, physics proxy and complete scene context before calls.
 bool ReadyForOriginalHumanUpdate(const OriginalSetupScene& s,uint32_t frame){
  return match(s)&&frame&&FullP1Restored(s)&&
    presentation_.ReadyForHumanFrame(s,frame);
 }
 bool CallOriginalHumanUpdate(const OriginalSetupScene& s,uint32_t frame){
  uint32_t ctl=0,source=0,dt=0,cv=0,av=0,cf=0,af=0,post=0;
  if(!match(s)||!frame||poisoned_||!entered_||!actor_.Entered()||
     !camera_.Entered()||!presentation_.OriginalContextActive(s)||
     !Read32(binding_.core+0x354u,ctl)||!storage_.Span(ctl,0xB0u)||
     !Read32(ctl+0xA8u,source)||
     (source&&(source!=view_.input||!storage_.Owns(source,0x40u)))||
     !presentation_.OriginalHumanInputActive(s,frame,ctl,source)||
     !Read32(binding_.core+0x1Cu,dt)||!Read32(ctl,cv)||
     !Read32(s.p1Actor,av)||av!=0x28CFE0u||
     !storage_.Span(cv,4u)||!storage_.Span(av,0x14u)||
     !Read32(cv,cf)||!Read32(av,af)||!Read32(av+0x10u,post)||
     post!=0x1E5FA0u||!dispatch::Find(cf)||!dispatch::Find(af)||
     !dispatch::Find(post))return false;
  // Exact thiscall ABI: controller receives ESI=actor; actor methods
  // receive dt bits on the stack. All calls isolate original CPU context.
  X86Context c=ctx_;
  c.eax=c.edx=c.ebx=c.edi=0;c.ecx=ctl;c.esi=s.p1Actor;
  (void)kernel::CallGuest(c,cf,{});
  if(!presentation_.OriginalContextActive(s))return false;
  c=ctx_;c.eax=c.edx=c.ebx=c.esi=c.edi=0;c.ecx=s.p1Actor;
  (void)kernel::CallGuest(c,af,{dt});
  if(!presentation_.OriginalContextActive(s))return false;
  c=ctx_;c.eax=c.edx=c.ebx=c.esi=c.edi=0;c.ecx=s.p1Actor;
  (void)kernel::CallGuest(c,post,{dt});
  // Original FX aging and use/pickup context are part of the provider,
  // not new bot AI logic. Do not mark a human frame complete without them.
  return presentation_.OriginalContextActive(s)&&
    presentation_.FinishOriginalHumanFrame(s,frame);
 }
 bool RestoreOriginalP1(const OriginalSetupScene& s){
  if(!match(s)||!attempted_||poisoned_)return false;
  bool ok=true;
  // Try each applicable restoration exactly once, even after a failed or
  // throwing presentation restore. Never attempt the setup call again.
  if(presentationAttempted_){
   try{ok=presentation_.RestoreOriginal(s);}catch(...){ok=false;}
  }
  if(camera_.Entered()&&!camera_.Restore(cameraIo_))ok=false;
  if(actor_.Entered()&&!actor_.RestoreActorOnly(actorIo_))ok=false;
  attempted_=entered_=presentationAttempted_=false;
  restored_=ok&&actorIo_.Live()&&!actor_.Entered()&&!camera_.Entered();
  poisoned_=!restored_;return restored_;
 }
 bool FullP1Restored(const OriginalSetupScene& s){
  return match(s)&&restored_&&!poisoned_&&actorIo_.Live()&&
    presentation_.OriginalP1Restored(s);
 }
};
}
