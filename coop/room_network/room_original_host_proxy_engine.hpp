// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
#include <array>
#include <thread>
namespace blackcoop {
// Caller must bind this to its independently verified committed HOST scene
// and completed ORIGINAL kind-2 setup. This adapter never initializes actors.
struct OriginalHostProxyBinding {
 uint64_t scene=0;
 uint32_t world=0,physics=0,actor=0,serial=0,collider=0;
 bool setupComplete=false;
};
struct OriginalHostProxyHandle {
 OriginalHostProxyBinding binding{};uint32_t proxy=0;
};
enum class OriginalProxyStatus {Rejected,Owned,Released,RolledBack,RollbackFailed,ScopeLost,OwnershipLost};
// IO contract: safe Read32/Read8/Span; Scene returns the LIVE independent
// committed host scene token; Call executes the real entry synchronously on
// this game thread, returning the C++ CallGuest result, NOT restored ctx.eax.
// No mutex: all operations, including construction/Handle, are game-thread only.

// Host-owned scene-lifetime record, deliberately shared by ALL additional
// original human actor adapters in one committed original engine scene.
// Not a per-adapter guard: dropping an adapter cannot erase an ambiguous
// engine CallGuest side effect. Keep this ledger alive beyond every adapter.
// No arbitrary Clear/Reset method: a new ledger requires PROVEN engine scene
// teardown, never only a network room/epoch change.
class OriginalProxySceneLedger {
 struct Entry {uint32_t actor=0, proxy=0;};
 enum class Pending {None,Acquire,Release};
 const std::thread::id thread_=std::this_thread::get_id();
 std::array<Entry,3> entries_{};
 uint64_t scene_=0;
 uint32_t world_=0,physics_=0,actor_=0,proxy_=0;
 Pending pending_=Pending::None;
 bool poisoned_=false;
 bool owner()const noexcept{return std::this_thread::get_id()==thread_;}
 bool same(const OriginalHostProxyBinding& b)const noexcept{
  return scene_&&scene_==b.scene&&world_==b.world&&physics_==b.physics;
 }
 bool bind(const OriginalHostProxyBinding& b)noexcept{
  if(!owner()||poisoned_||pending_!=Pending::None||
     !b.scene||!b.world||!b.physics||!b.actor||!b.setupComplete)return false;
  if(!scene_){scene_=b.scene;world_=b.world;physics_=b.physics;}
  return same(b);
 }
public:
 OriginalProxySceneLedger()=default;
 OriginalProxySceneLedger(const OriginalProxySceneLedger&)=delete;
 OriginalProxySceneLedger& operator=(const OriginalProxySceneLedger&)=delete;
 bool Quarantined()const noexcept{return owner()&&(poisoned_||pending_!=Pending::None);}
 unsigned ActiveCount()const noexcept{
  if(!owner())return 0;
  unsigned n=0;for(const auto& e:entries_)if(e.actor)++n;return n;
 }
 void Poison()noexcept{poisoned_=true;}
 bool HasOwned(const OriginalHostProxyBinding& b,uint32_t proxy)const noexcept{
  if(!owner()||poisoned_||pending_!=Pending::None||!same(b)||!proxy)return false;
  for(const auto& e:entries_)if(e.actor==b.actor&&e.proxy==proxy)return true;
  return false;
 }
 bool BeginAcquire(const OriginalHostProxyBinding& b)noexcept{
  if(!bind(b))return false;
  bool free=false;
  for(const auto& e:entries_){
   if(e.actor==b.actor)return false;
   if(!e.actor)free=true;
  }
  if(!free)return false;
  actor_=b.actor;proxy_=0;pending_=Pending::Acquire;
  return true;
 }
 bool ConfirmAcquire(const OriginalHostProxyBinding& b,uint32_t proxy)noexcept{
  if(!owner()||poisoned_||pending_!=Pending::Acquire||!same(b)||
     actor_!=b.actor||!proxy)return false;
  for(const auto& e:entries_)
   if(e.actor==b.actor||(e.proxy&&e.proxy==proxy))return false;
  for(auto& e:entries_)if(!e.actor){
   e={b.actor,proxy};pending_=Pending::None;actor_=proxy_=0;return true;
  }
  return false;
 }
 bool ConfirmRolledBack(const OriginalHostProxyBinding& b)noexcept{
  if(!owner()||poisoned_||pending_!=Pending::Acquire||!same(b)||actor_!=b.actor)return false;
  pending_=Pending::None;actor_=proxy_=0;return true;
 }
 bool BeginRelease(const OriginalHostProxyBinding& b,uint32_t proxy)noexcept{
  if(!HasOwned(b,proxy))return false;
  actor_=b.actor;proxy_=proxy;pending_=Pending::Release;
  return true;
 }
 bool ConfirmRelease(const OriginalHostProxyBinding& b,uint32_t proxy)noexcept{
  if(!owner()||poisoned_||pending_!=Pending::Release||!same(b)||
     actor_!=b.actor||proxy_!=proxy)return false;
  for(auto& e:entries_)if(e.actor==b.actor&&e.proxy==proxy){
   e={};pending_=Pending::None;actor_=proxy_=0;return true;
  }
  return false;
 }
};
class OriginalHostProxyEngine {
 OriginalProxySceneLedger& ledger_;
 std::thread::id owner_=std::this_thread::get_id();
 OriginalHostProxyHandle handle_{};
 // A native call can have side effects even if it throws/returns false.
 // Once uncertain, never retry acquire/release without independently proven
 // engine cleanup. A new object instance is NOT proof the scene was cleaned.
 bool uncertain_=false;
 static bool range(uint32_t p,uint32_t n)noexcept{return p>=0x10000u&&uint64_t(p)+n<=0x100000000ull;}
 bool owner()const noexcept{return owner_==std::this_thread::get_id();}
 template<class IO>static bool scope(IO& io,const OriginalHostProxyBinding& b){
  uint32_t world=0,physics=0;
  return b.scene&&io.Scene()==b.scene&&range(b.world,0x5CC0)&&range(b.physics,0x2974)&&
   io.Span(b.world,0x5CC0)&&io.Span(b.physics,0x2974)&&
   io.Read32(0x2D45D4,world)&&world==b.world&&io.Read32(0x2D45F4,physics)&&physics==b.physics;
 }
 template<class IO>static bool actor(IO& io,const OriginalHostProxyBinding& b){
  uint32_t table=0,kind=0,serial=0,collider=0,pool=0;
  if(!b.setupComplete||!io.OwnsActor(b.actor,0x900)||!range(b.actor,0x900)||!io.Span(b.actor,0x900)||b.actor==b.world+0x30||
   !io.Read32(0x2D1980,pool)||!range(pool,0x8540))return false;
  // Never accept any address in the original NPC allocation, not just starts.
  if(uint64_t(b.actor)<uint64_t(pool)+0x8540&&uint64_t(b.actor)+0x900>pool)return false;
  // The original core spans 0x5CC0 (pinned PC original tick); none of its
  // embedded storage may masquerade as an independently owned shadow actor.
  if(uint64_t(b.actor)<uint64_t(b.world)+0x5CC0&&uint64_t(b.actor)+0x900>b.world)return false;
  // A forged kind-2 header inside the live physics allocator is not a
  // separately owned human. Reject every overlap, including interior spans.
  if(uint64_t(b.actor)<uint64_t(b.physics)+0x2974&&
     uint64_t(b.actor)+0x900>b.physics)return false;
  return io.Read32(b.actor,table)&&table==0x28CFE0&&io.Read32(b.actor+0xC4,kind)&&kind==2&&
   io.Read32(b.actor+0x374,serial)&&serial==b.serial&&
   io.Read32(b.actor+0x354,collider)&&collider==b.collider&&range(collider,0x60)&&io.Span(collider,0x60);
 }
 template<class IO>static bool owned(IO& io,const OriginalHostProxyHandle& h){
  const auto& b=h.binding;
  if(!h.proxy||h.proxy<b.physics+0x2320u||
   uint64_t(h.proxy)>=uint64_t(b.physics)+0x2320u+20u*0x50u||
   (h.proxy-b.physics-0x2320u)%0x50u||!io.Span(h.proxy,0x50))return false;
  uint8_t flag=0;uint32_t back=0,proxy=0,local=0;
  const uint32_t slot=(h.proxy-b.physics-0x2320u)/0x50u;
  return io.Read8(b.physics+0x2960u+slot,flag)&&flag!=0&&
   io.Read32(h.proxy+0x30,back)&&back==b.actor&&
   io.Read32(b.actor+0xB4,proxy)&&proxy==h.proxy&&
   io.Read32(b.world+0x30+0xB4,local)&&local!=h.proxy;
 }
 template<class IO>bool release(IO& io){
  uint32_t ignored=0;
  try{if(!io.Call(0xDBBE0,handle_.binding.physics,handle_.proxy,ignored))return false;}
  catch(...){return false;}
  const auto& b=handle_.binding;uint32_t proxy=1,back=1;uint8_t flag=1;
  // Do not dereference a previous world after the original call changes scope.
  if(!scope(io,b)||!actor(io,b))return false;
  const uint32_t slot=(handle_.proxy-b.physics-0x2320u)/0x50u;
  if(!io.Read32(b.actor+0xB4,proxy)||proxy||!io.Read32(handle_.proxy+0x30,back)||back||
   !io.Read8(b.physics+0x2960u+slot,flag)||flag)return false;
  handle_={};uncertain_=false;return true;
 }
public:
 explicit OriginalHostProxyEngine(OriginalProxySceneLedger& ledger):ledger_(ledger){}
 ~OriginalHostProxyEngine(){if(handle_.proxy||uncertain_)ledger_.Poison();}
 OriginalHostProxyEngine()=delete;
 OriginalHostProxyEngine(const OriginalHostProxyEngine&)=delete;
 OriginalHostProxyEngine& operator=(const OriginalHostProxyEngine&)=delete;
 OriginalHostProxyHandle Handle()const noexcept{return owner()?handle_:OriginalHostProxyHandle{};}
 // A positive result requires engine-lifecycle intervention, not a new retry.
 bool RequiresEngineCleanup()const noexcept{return owner()&&uncertain_;}
 template<class IO>OriginalProxyStatus Acquire(IO& io,const OriginalHostProxyBinding& b){
  if(!owner()||uncertain_||handle_.proxy||!scope(io,b)||!actor(io,b))return OriginalProxyStatus::Rejected;
  uint32_t previous=0;if(!io.Read32(b.actor+0xB4,previous)||previous)return OriginalProxyStatus::Rejected;
  unsigned free=0;uint32_t expected=0;
  for(uint32_t i=0;i<20;i++){uint8_t flag=0;if(!io.Read8(b.physics+0x2960+i,flag))return OriginalProxyStatus::Rejected;
   if(!flag){if(!expected)expected=b.physics+0x2320+i*0x50;++free;}}
  if(free<2||!expected||!io.Span(expected,0x50)||!scope(io,b)||!actor(io,b)||
   !io.Read32(b.actor+0xB4,previous)||previous)return OriginalProxyStatus::Rejected;
  // DF400 has no exhaustion guard. The free scan and call must run on the
  // original engine thread without yielding or concurrent engine allocation.
  // The original DF400 may allocate before its return value or failure.
  // Quarantine immediately before touching the engine; later failures must
  // not permit another DF400 against the possibly occupied slot.
  if(!ledger_.BeginAcquire(b))return OriginalProxyStatus::Rejected;
  uncertain_=true;
  uint32_t result=0;bool called=false;
  try{called=io.Call(0xDF400,b.physics,b.actor,result);}
  catch(...){return OriginalProxyStatus::OwnershipLost;}
  if(!scope(io,b))return OriginalProxyStatus::ScopeLost;
  uint32_t proxy=0;
  if(!actor(io,b)||!io.Read32(b.actor+0xB4,proxy))return OriginalProxyStatus::OwnershipLost;
  OriginalHostProxyHandle candidate{b,proxy};
  if(!owned(io,candidate))return OriginalProxyStatus::OwnershipLost;
  handle_=candidate;
  if(called&&(result&255u)==1u&&proxy==expected){
   if(!ledger_.ConfirmAcquire(b,proxy)){ledger_.Poison();return OriginalProxyStatus::OwnershipLost;}
   uncertain_=false;return OriginalProxyStatus::Owned;
  }
  // Even a failed result may have acquired a native object. Release only an
  // exact same-scene slot whose owner/backlink is still exclusively ours.
  if(release(io)&&ledger_.ConfirmRolledBack(b))return OriginalProxyStatus::RolledBack;
  ledger_.Poison();uncertain_=true;return OriginalProxyStatus::RollbackFailed;
 }
 template<class IO>OriginalProxyStatus Release(IO& io){
  if(!owner()||uncertain_||!handle_.proxy)return OriginalProxyStatus::Rejected;
  if(!scope(io,handle_.binding)){uncertain_=true;ledger_.Poison();return OriginalProxyStatus::ScopeLost;}
  if(!actor(io,handle_.binding)||!owned(io,handle_)||
     !ledger_.HasOwned(handle_.binding,handle_.proxy)){
   uncertain_=true;ledger_.Poison();return OriginalProxyStatus::OwnershipLost;
  }
  const auto binding=handle_.binding;const uint32_t proxy=handle_.proxy;
  if(!ledger_.BeginRelease(binding,proxy))return OriginalProxyStatus::Rejected;
   // Even destructor failure/throw can release a proxy before reporting status.
    // Released covers the pool slot and backlinks only. The old solver body
    // and motion row can each retain collider references independently.
  uncertain_=true;
  if(release(io)&&ledger_.ConfirmRelease(binding,proxy))return OriginalProxyStatus::Released;
  ledger_.Poison();uncertain_=true;return OriginalProxyStatus::RollbackFailed;
 }
};
}
