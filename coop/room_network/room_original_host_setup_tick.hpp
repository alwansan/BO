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
// Original controller manager +A8 switch, from pinned black_p2_update.
// Null scripted input stays null. Actual allocation lifetime is required.
class OriginalHumanControllerSource final {
 const std::thread::id thread_=std::this_thread::get_id();
 uint32_t ctl_=0,first_=0,extra_=0;
 uint64_t cg_=0,fg_=0,eg_=0;
 bool entered_=false,poisoned_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 template<class IO>bool live(IO& io)const{
  return owner()&&cg_&&eg_&&io.ControllerGeneration(ctl_)==cg_&&
   io.InputGeneration(extra_)==eg_&&io.OwnsHumanInput(extra_)&&
   (!first_||(fg_&&io.InputGeneration(first_)==fg_));
 }
public:
 OriginalHumanControllerSource()=default;
 OriginalHumanControllerSource(const OriginalHumanControllerSource&)=delete;
 OriginalHumanControllerSource& operator=(const OriginalHumanControllerSource&)=delete;
 bool Entered()const noexcept{return owner()&&entered_;}
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
 template<class IO>bool Enter(IO& io,uint32_t ctl,uint32_t extra){
  if(!owner()||entered_||poisoned_||ctl<0x10000u||ctl>0xFFFFFF50u||
     extra<0x10000u||extra>0xFFFFFFC0u||(ctl&3u)||(extra&3u))return false;
  uint32_t first=0;
  if(!io.Read32(ctl+0xA8u,first)||first==extra||
     (first&&(first<0x10000u||first>0xFFFFFFC0u||(first&3u)||
     (uint64_t(first)<uint64_t(extra)+0x40u&&uint64_t(extra)<uint64_t(first)+0x40u))))return false;
  ctl_=ctl;first_=first;extra_=extra;
  cg_=io.ControllerGeneration(ctl);fg_=first?io.InputGeneration(first):0;
  eg_=io.InputGeneration(extra);
  if(!live(io))return false;
  auto* word=io.MapControllerSource(ctl);
  uint32_t current=0;
  if(!word||!live(io)||!io.Read32(ctl+0xA8u,current)||current!=first)return false;
  *word=first?extra:0u;entered_=true;
  if(!io.Read32(ctl+0xA8u,current)||current!=(first?extra:0u)){
   poisoned_=true;return false;
  }
  return true;
 }
 template<class IO>bool Restore(IO& io){
  if(!owner()||!entered_||poisoned_)return false;
  uint32_t current=0;
  if(!live(io)||!io.Read32(ctl_+0xA8u,current)||current!=(first_?extra_:0u)){
   poisoned_=true;return false;
  }
  auto* word=io.MapControllerSource(ctl_);
  if(!word||!live(io)||!io.Read32(ctl_+0xA8u,current)||current!=(first_?extra_:0u)){
   poisoned_=true;return false;
  }
  *word=first_;
  if(!io.Read32(ctl_+0xA8u,current)||current!=first_){poisoned_=true;return false;}
  entered_=false;return true;
 }
};
// Original source +C points at its own input component. Pinned PC
// black_p2_update proves manager+98, P2 stride 120. For every candidate,
// including P3/P4, require the actual vtable, port index and shared input
// records from original sub_000C5A80 before any owned-source write.
class OriginalHumanPadBinding final {
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t generation_=0;
 uint32_t input_=0;uint8_t peer_=0;
 bool poisoned_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 template<class IO>static bool topology(IO& io,uint8_t peer,uint32_t& first,uint32_t& pad){
  uint32_t manager=0,v=0,records=0,other=0,port=0,kind=0;
  if(peer<2||peer>4||!io.Read32(0x2D45E8u,manager)||manager<0x10000u||
     uint64_t(manager)+0x98u+0x120u*peer>0x100000000ull)return false;
  first=manager+0x98u;pad=first+0x120u*(peer-1u);
  if(!io.Read32(first,v)||v!=0x28CE00u||
     !io.Read32(first+0x11Cu,port)||port!=0||
     !io.Read32(first+0x118u,records)||records<0x10000u||
     uint64_t(records)+128u>0x100000000ull||
     !io.Read32(pad,v)||v!=0x28CE00u||
     !io.Read32(pad+0x11Cu,port)||port!=peer-1u||
     !io.Read32(pad+0x118u,other)||other!=records||
     !io.Read32(records+32u*port,kind)||kind!=2u)return false;
  return true;
 }
public:
 OriginalHumanPadBinding()=default;
 OriginalHumanPadBinding(const OriginalHumanPadBinding&)=delete;
 OriginalHumanPadBinding& operator=(const OriginalHumanPadBinding&)=delete;
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
 template<class IO>bool Bind(IO& io,uint32_t input,uint8_t peer){
  if(!owner()||poisoned_||peer<2||peer>4||input<0x10000u||
     input>0xFFFFFFC0u||(input&3u)||!io.OwnsHumanInput(input))return false;
  const auto gen=io.InputGeneration(input);
  if(!gen)return false;
  if(generation_&&(input_!=input||peer_!=peer||generation_!=gen)){
   poisoned_=true;return false;
  }
  uint32_t first=0,pad=0,current=0,checkFirst=0,checkPad=0;
  if(!topology(io,peer,first,pad)||!io.Read32(input+0xCu,current)||
     (current!=first&&current!=pad))return false;
  auto* word=io.MapOwnedPadWord(input);
  if(!word||io.InputGeneration(input)!=gen||!io.OwnsHumanInput(input)||
     !topology(io,peer,checkFirst,checkPad)||first!=checkFirst||pad!=checkPad||
     !io.Read32(input+0xCu,current)||(current!=first&&current!=pad))return false;
  input_=input;peer_=peer;generation_=gen;
  *word=pad;
  if(!io.Read32(input+0xCu,current)||current!=pad||
     io.InputGeneration(input)!=gen){poisoned_=true;return false;}
  return true;
 }
 template<class IO>bool Active(IO& io,uint32_t input,uint8_t peer)const{
  uint32_t first=0,pad=0,current=0;
  return owner()&&!poisoned_&&generation_&&input==input_&&peer==peer_&&
   io.OwnsHumanInput(input)&&io.InputGeneration(input)==generation_&&
   topology(io,peer,first,pad)&&io.Read32(input+0xCu,current)&&current==pad;
 }
};
// Original extra-human update after the genuine 1F1080 world tick.
// The original enemy/ally AI is untouched. The shared world is never
// stepped twice. IO must provide the original controller/actor methods,
// independent input, interaction context and verified physical proxy.
enum class OriginalHumanUpdateStatus:uint8_t {Rejected,Updated,Quarantined};
class OriginalHumanUpdateTick final {
 const std::thread::id thread_=std::this_thread::get_id();
 OriginalSetupScene binding_{};
 uint32_t lastFrame_=0;
 bool bound_=false,poisoned_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 static bool same(const OriginalSetupScene& a,const OriginalSetupScene& b)noexcept{
  return a.nativeScene==b.nativeScene&&a.signedEpoch==b.signedEpoch&&
   a.world==b.world&&a.p1Actor==b.p1Actor&&a.p1Owner==b.p1Owner&&
   a.ownedActor==b.ownedActor&&a.ownedOwner==b.ownedOwner&&a.peer==b.peer;
 }
public:
 template<class IO>OriginalHumanUpdateStatus Tick(IO& io,
      const OriginalSetupScene& scope,uint32_t frame){
  if(!owner()||!frame||frame<=lastFrame_||!scope.nativeScene||
     !scope.signedEpoch||scope.peer<2||scope.peer>4)return OriginalHumanUpdateStatus::Rejected;
  if(poisoned_)return OriginalHumanUpdateStatus::Quarantined;
  if(bound_&&!same(binding_,scope)){poisoned_=true;return OriginalHumanUpdateStatus::Quarantined;}
  // No mutation before the original per-human setup, physical proxy,
  // exact frame input and full restored P1 context are all proven.
  try{if(!io.ReadyForOriginalHumanUpdate(scope,frame))return OriginalHumanUpdateStatus::Rejected;}
  catch(...){return OriginalHumanUpdateStatus::Rejected;}
  binding_=scope;bound_=true;lastFrame_=frame;
  bool ok=false,restored=false,attempted=false,restoreAttempted=false;
  try{
   attempted=true;
   if(io.EnterOwnedOriginalContext(scope))
    ok=io.CallOriginalHumanUpdate(scope,frame);
   restoreAttempted=true;
   restored=io.RestoreOriginalP1(scope);
   if(ok&&restored&&io.FullP1Restored(scope))return OriginalHumanUpdateStatus::Updated;
  }catch(...){
   if(attempted&&!restoreAttempted)try{(void)io.RestoreOriginalP1(scope);}catch(...){}
  }
  poisoned_=true;return OriginalHumanUpdateStatus::Quarantined;
 }
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
};
// Runs the independently bound P2/P3/P4 setup lanes on the same original
// post-world frame. Original singleton contexts must be visited serially,
// not on concurrent threads. One failed restoration stops ALL lanes.
class OriginalSetupFrameScheduler final {
 const std::thread::id thread_=std::this_thread::get_id();
 OriginalHumanSetupLanes lanes_;
 std::array<OriginalSetupScene,3> scopes_{};
 std::array<OriginalHumanUpdateTick,3> updates_{};
 uint32_t frame_=0;uint8_t mask_=0;bool poisoned_=false,setupComplete_=false;
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
  setupComplete_=all;
  return all?OriginalSetupState::SetupFinished:OriginalSetupState::Waiting;
 }
 // After ALL setup lanes finish, advance human-only original frames on
 // the same owner thread. A setup frame can never also be an update frame.
 // Preflight every participant before touching any original actor.
 template<class IO>OriginalHumanUpdateStatus TickHumans(const std::array<IO*,3>& io,uint32_t frame){
  if(!owner()||!mask_||!frame)return OriginalHumanUpdateStatus::Rejected;
  if(poisoned_)return OriginalHumanUpdateStatus::Quarantined;
  if(!setupComplete_||frame<=frame_)return OriginalHumanUpdateStatus::Rejected;
  for(unsigned i=0;i<3;i++)if(mask_&(2u<<i))
   if(!io[i]||!scopes_[i].peer)return OriginalHumanUpdateStatus::Rejected;
  frame_=frame;
  try{
   for(unsigned i=0;i<3;i++)if(mask_&(2u<<i)){
    if(!lanes_.At(static_cast<uint8_t>(i+2))->ReadyForPhysicsNow(*io[i])){
     poisoned_=true;return OriginalHumanUpdateStatus::Quarantined;
    }
    if(!io[i]->ReadyForOriginalHumanUpdate(scopes_[i],frame))
     return OriginalHumanUpdateStatus::Rejected;
   }
   // Original singleton contexts are serial. Any failed update stops
   // the entire scene; never rerun an earlier human or the world.
   for(unsigned i=0;i<3;i++)if(mask_&(2u<<i)){
    if(updates_[i].Tick(*io[i],scopes_[i],frame)!=OriginalHumanUpdateStatus::Updated){
     poisoned_=true;return OriginalHumanUpdateStatus::Quarantined;
    }
   }
  }catch(...){poisoned_=true;return OriginalHumanUpdateStatus::Quarantined;}
  return OriginalHumanUpdateStatus::Updated;
 }
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
};
} // namespace blackcoop
