// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
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
class OriginalHostProxyEngine {
 std::thread::id owner_=std::this_thread::get_id();
 OriginalHostProxyHandle handle_{};
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
  if(!b.setupComplete||!range(b.actor,0x900)||!io.Span(b.actor,0x900)||b.actor==b.world+0x30||
   !io.Read32(0x2D1980,pool)||!range(pool,0x8540))return false;
  // Never accept any address in the original NPC allocation, not just starts.
  if(uint64_t(b.actor)<uint64_t(pool)+0x8540&&uint64_t(b.actor)+0x900>pool)return false;
  // The original core spans 0x5CC0 (pinned PC original tick); none of its
  // embedded storage may masquerade as an independently owned shadow actor.
  if(uint64_t(b.actor)<uint64_t(b.world)+0x5CC0&&uint64_t(b.actor)+0x900>b.world)return false;
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
  if(!io.Call(0xDBBE0,handle_.binding.physics,handle_.proxy,ignored))return false;
  const auto& b=handle_.binding;uint32_t proxy=1,back=1;uint8_t flag=1;
  // Do not dereference a previous world after the original call changes scope.
  if(!scope(io,b)||!actor(io,b))return false;
  const uint32_t slot=(handle_.proxy-b.physics-0x2320u)/0x50u;
  if(!io.Read32(b.actor+0xB4,proxy)||proxy||!io.Read32(handle_.proxy+0x30,back)||back||
   !io.Read8(b.physics+0x2960u+slot,flag)||flag)return false;
  handle_={};return true;
 }
public:
 OriginalHostProxyEngine()=default;
 OriginalHostProxyEngine(const OriginalHostProxyEngine&)=delete;
 OriginalHostProxyEngine& operator=(const OriginalHostProxyEngine&)=delete;
 OriginalHostProxyHandle Handle()const noexcept{return owner()?handle_:OriginalHostProxyHandle{};}
 template<class IO>OriginalProxyStatus Acquire(IO& io,const OriginalHostProxyBinding& b){
  if(!owner()||handle_.proxy||!scope(io,b)||!actor(io,b))return OriginalProxyStatus::Rejected;
  uint32_t previous=0;if(!io.Read32(b.actor+0xB4,previous)||previous)return OriginalProxyStatus::Rejected;
  unsigned free=0;uint32_t expected=0;
  for(uint32_t i=0;i<20;i++){uint8_t flag=0;if(!io.Read8(b.physics+0x2960+i,flag))return OriginalProxyStatus::Rejected;
   if(!flag){if(!expected)expected=b.physics+0x2320+i*0x50;++free;}}
  if(free<2||!expected||!io.Span(expected,0x50)||!scope(io,b)||!actor(io,b)||
   !io.Read32(b.actor+0xB4,previous)||previous)return OriginalProxyStatus::Rejected;
  // DF400 has no exhaustion guard. The free scan and call must run on the
  // original engine thread without yielding or concurrent engine allocation.
  uint32_t result=0;const bool called=io.Call(0xDF400,b.physics,b.actor,result);
  if(!scope(io,b))return OriginalProxyStatus::ScopeLost;
  uint32_t proxy=0;
  if(!actor(io,b)||!io.Read32(b.actor+0xB4,proxy))return OriginalProxyStatus::OwnershipLost;
  OriginalHostProxyHandle candidate{b,proxy};
  if(!owned(io,candidate))return OriginalProxyStatus::OwnershipLost;
  handle_=candidate;
  if(called&&(result&255u)==1u&&proxy==expected)return OriginalProxyStatus::Owned;
  // Even a failed result may have acquired a native object. Release only an
  // exact same-scene slot whose owner/backlink is still exclusively ours.
  return release(io)?OriginalProxyStatus::RolledBack:OriginalProxyStatus::RollbackFailed;
 }
 template<class IO>OriginalProxyStatus Release(IO& io){
  if(!owner()||!handle_.proxy)return OriginalProxyStatus::Rejected;
  if(!scope(io,handle_.binding))return OriginalProxyStatus::ScopeLost;
  if(!actor(io,handle_.binding)||!owned(io,handle_))return OriginalProxyStatus::OwnershipLost;
  return release(io)?OriginalProxyStatus::Released:OriginalProxyStatus::RollbackFailed;
 }
};
}
