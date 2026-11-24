// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Native BLACK original 1E0E40 multi-frame setup for extra human P2/P3/P4.
// Based on pinned MIT PC black_p2_setup(): EDX=core, EAX=0, low EAX byte is done.
// Source ABI contract only. A real ORIGINAL guest context exchange must provide
// EnterOwned, GuestSetup1E0E40 and RestoreOriginalP1 before any call is enabled.
#pragma once
#include <array>
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalSetupScene {
 uint64_t nativeScene=0;uint32_t signedEpoch=0;
 uint32_t world=0,p1Actor=0,p1Owner=0,ownedActor=0,ownedOwner=0;
 uint8_t peer=0;
};
enum class OriginalSetupState:uint8_t {
 Rejected,Waiting,SetupFinished,TimedOut,Quarantined
};
class OriginalHumanSetupTick final {
 const std::thread::id owner_=std::this_thread::get_id();
 OriginalSetupScene scope_{};
 uint32_t lastFrame_=0,attempts_=0;
 bool bound_=false,finished_=false,poisoned_=false;
 bool onOwner()const noexcept{return owner_==std::this_thread::get_id();}
 static bool ptr(uint32_t p)noexcept{return p>=0x10000&&p<0xfffffff0u;}
 static bool same(const OriginalSetupScene& a,const OriginalSetupScene& b)noexcept{
  return a.nativeScene==b.nativeScene&&a.signedEpoch==b.signedEpoch&&
   a.world==b.world&&a.p1Actor==b.p1Actor&&a.p1Owner==b.p1Owner&&
   a.ownedActor==b.ownedActor&&a.ownedOwner==b.ownedOwner&&a.peer==b.peer;
 }
 template<class IO>bool live(IO& io)const{
  uint32_t world=0,owner=0,vt=0,kind=0;
  return io.OriginalScene()==scope_.nativeScene&&
   io.OwnsMaterializedActor(scope_.ownedActor,0x900u)&&
   io.OwnsOriginalCamera(scope_.ownedOwner,0x16C0u)&&
   io.Read32(0x2D45D4u,world)&&world==scope_.world&&
   io.Read32(0x2D12B8u,owner)&&owner==scope_.p1Owner&&
   io.Read32(scope_.p1Actor,vt)&&vt==0x28CFE0u&&
   io.Read32(scope_.p1Actor+0xC4u,kind)&&kind==2u&&
   io.CorePhase(scope_.world)==0x1Cu;
 }
public:
 OriginalHumanSetupTick()=default;
 OriginalHumanSetupTick(const OriginalHumanSetupTick&)=delete;
 OriginalHumanSetupTick& operator=(const OriginalHumanSetupTick&)=delete;
 template<class IO>bool Bind(IO& io,const OriginalSetupScene& s){
  if(!onOwner()||poisoned_||!s.nativeScene||!s.signedEpoch||
     s.peer<2||s.peer>4||!ptr(s.world)||!ptr(s.p1Actor)||
     !ptr(s.p1Owner)||!ptr(s.ownedActor)||!ptr(s.ownedOwner)||
     s.p1Actor==s.ownedActor||s.p1Owner==s.ownedOwner||
     s.ownedActor==s.ownedOwner)return false;
  if(bound_){
   if(!same(scope_,s)){poisoned_=true;return false;}
   return true;
  }
  scope_=s;if(!live(io)){scope_={};return false;}
  bound_=true;return true;
 }
 template<class IO>OriginalSetupState Tick(IO& io,uint32_t originalFrame){
  if(!onOwner()||!bound_||!originalFrame)return OriginalSetupState::Rejected;
  if(poisoned_)return OriginalSetupState::Quarantined;
  if(!live(io)){poisoned_=true;return OriginalSetupState::Quarantined;}
  if(finished_)return OriginalSetupState::SetupFinished;
  if(originalFrame<=lastFrame_)return OriginalSetupState::Rejected;
  if(attempts_>=600u)return OriginalSetupState::TimedOut;
  lastFrame_=originalFrame;
  bool entered=false,called=false,left=false;
  bool enterAttempted=false,restoreAttempted=false;
  uint32_t result=0;
  try{
   enterAttempted=true;
   entered=io.EnterOwnedOriginalContext(scope_);
   if(entered)called=io.GuestSetup1E0E40(scope_.world,0u,result);
   // Enter may fail after modifying P1. The adapter must support recovery
   // after every attempted entry, including a failed or partial entry.
   restoreAttempted=true;
   left=io.RestoreOriginalP1(scope_);
  }catch(...){
   if(enterAttempted&&!restoreAttempted)
    try{(void)io.RestoreOriginalP1(scope_);}catch(...){}
   // Never blindly repeat a throwing restore; quarantine uncertain state.
   poisoned_=true;return OriginalSetupState::Quarantined;
  }
  ++attempts_;
  if(!entered||!called||!left||!live(io)||!io.FullP1Restored(scope_)){
   poisoned_=true;return OriginalSetupState::Quarantined;
  }
  if(result&0xFFu){finished_=true;return OriginalSetupState::SetupFinished;}
  return attempts_==600u?OriginalSetupState::TimedOut:OriginalSetupState::Waiting;
 }
 bool ReadyForPhysics()const noexcept{return onOwner()&&bound_&&finished_&&!poisoned_;}
 template<class IO>bool ReadyForPhysicsNow(IO& io){
  if(!ReadyForPhysics())return false;
  try{
   if(live(io)&&io.FullP1Restored(scope_))return true;
  }catch(...){}
  poisoned_=true;return false;
 }
 unsigned Attempts()const noexcept{return onOwner()?attempts_:0;}
 bool Quarantined()const noexcept{return onOwner()&&poisoned_;}
 // Network leave, epoch change or C++ object destruction is NOT proof of
 // original engine teardown. No guest allocation is freed here.
 bool AfterVerifiedEngineTeardown(bool verified)noexcept{
  if(!onOwner()||!verified)return false;
  scope_={};lastFrame_=attempts_=0;
  bound_=finished_=poisoned_=false;return true;
 }
};
class OriginalHumanSetupLanes final {
 std::array<OriginalHumanSetupTick,3> lanes_{};
public:
 OriginalHumanSetupTick* At(uint8_t p)noexcept{
  return p>=2&&p<=4?&lanes_[p-2]:nullptr;
 }
};
// Runs the independently bound P2/P3/P4 setup lanes on the same original
// post-world frame. Original singleton contexts must be visited serially,
// not on concurrent threads. One failed restoration stops ALL lanes.
class OriginalSetupFrameScheduler final {
 const std::thread::id thread_=std::this_thread::get_id();
 OriginalHumanSetupLanes lanes_;
 std::array<OriginalSetupScene,3> scopes_{};
 uint32_t frame_=0;uint8_t mask_=0;bool poisoned_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 static bool overlaps(uint32_t a,uint32_t n,uint32_t b,uint32_t m)noexcept{
  return uint64_t(a)<uint64_t(b)+m&&uint64_t(b)<uint64_t(a)+n;
 }
public:
 template<class IO>bool Bind(IO& io,const OriginalSetupScene& s,uint8_t mask){
  if(!owner()||poisoned_||frame_||s.peer<2||s.peer>4||
     (mask!=3&&mask!=7&&mask!=15)||!(mask&(1u<<(s.peer-1)))||
     (mask_&&mask_!=mask)||scopes_[s.peer-2].peer||
     uint64_t(s.world)+0x30u!=s.p1Actor||
     overlaps(s.ownedActor,0x900u,s.world,0x5CC0u)||
     overlaps(s.ownedOwner,0x16C0u,s.world,0x5CC0u)||
     overlaps(s.ownedActor,0x900u,s.p1Owner,0x16C0u)||
     overlaps(s.ownedOwner,0x16C0u,s.p1Owner,0x16C0u)||
     overlaps(s.ownedActor,0x900u,s.ownedOwner,0x16C0u))return false;
  for(const auto& old:scopes_)if(old.peer){
   if(old.nativeScene!=s.nativeScene||old.signedEpoch!=s.signedEpoch||
      old.world!=s.world||old.p1Actor!=s.p1Actor||old.p1Owner!=s.p1Owner||
      overlaps(s.ownedActor,0x900u,old.ownedActor,0x900u)||
      overlaps(s.ownedActor,0x900u,old.ownedOwner,0x16C0u)||
      overlaps(s.ownedOwner,0x16C0u,old.ownedActor,0x900u)||
      overlaps(s.ownedOwner,0x16C0u,old.ownedOwner,0x16C0u))return false;
  }
  if(!lanes_.At(s.peer)->Bind(io,s))return false;
  scopes_[s.peer-2]=s;mask_=mask;return true;
 }
 template<class IO>OriginalSetupState Tick(const std::array<IO*,3>& io,uint32_t frame){
  if(!owner()||!mask_||!frame||frame<=frame_)return OriginalSetupState::Rejected;
  if(poisoned_)return OriginalSetupState::Quarantined;
  for(unsigned i=0;i<3;i++)if(mask_&(2u<<i))
   if(!scopes_[i].peer||!io[i])return OriginalSetupState::Rejected;
  frame_=frame;bool all=true;
  for(unsigned i=0;i<3;i++)if(mask_&(2u<<i)){
   auto& lane=*lanes_.At(static_cast<uint8_t>(i+2));
   try{
    const auto result=lane.Tick(*io[i],frame);
    if(result==OriginalSetupState::TimedOut){poisoned_=true;return result;}
    if(result==OriginalSetupState::Quarantined||result==OriginalSetupState::Rejected){
     poisoned_=true;return OriginalSetupState::Quarantined;
    }
    if(result==OriginalSetupState::SetupFinished){
     if(!lane.ReadyForPhysicsNow(*io[i])){poisoned_=true;return OriginalSetupState::Quarantined;}
    }else all=false;
   }catch(...){poisoned_=true;return OriginalSetupState::Quarantined;}
  }
  return all?OriginalSetupState::SetupFinished:OriginalSetupState::Waiting;
 }
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
};
} // namespace blackcoop
