#include "room_original_host_setup_tick.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <thread>
using namespace blackcoop;
struct MockEngine {
 uint64_t scene=99;uint32_t corePhase=0x1Cu;
 std::map<uint32_t,uint32_t> words{{0x2D45D4u,0x300000u},
   {0x2D12B8u,0x400000u},{0x300030u,0x28CFE0u},
   {0x3000F4u,2u}};
 uint32_t actor=0,owner=0,completeAt=3u;unsigned calls=0,entered=0,left=0;
 bool enterOk=true,callOk=true,restoreOk=true,fullOk=true,throwing=false;
 bool enterThrows=false,restoreThrows=false;
 uint64_t OriginalScene()const{return scene;}
 uint32_t CorePhase(uint32_t)const{return corePhase;}
 bool OwnsMaterializedActor(uint32_t a,uint32_t n)const{
  return a==actor&&n==0x900u;
 }
 bool OwnsOriginalCamera(uint32_t a,uint32_t n)const{
  return a==owner&&n==0x16C0u;
 }
 bool Read32(uint32_t a,uint32_t& v)const{
  auto i=words.find(a);if(i==words.end())return false;
  v=i->second;return true;
 }
 bool EnterOwnedOriginalContext(const OriginalSetupScene& s){
  ++entered;if(enterThrows)throw 2;
  return enterOk&&s.ownedActor==actor&&s.ownedOwner==owner;
 }
 bool GuestSetup1E0E40(uint32_t world,uint32_t eax,uint32_t& out){
  assert(world==0x300000u&&eax==0);++calls;
  if(throwing)throw 1;
  out=calls>=completeAt?1u:0u;return callOk;
 }
 bool RestoreOriginalP1(const OriginalSetupScene&){
  ++left;if(restoreThrows)throw 3;return restoreOk;
 }
 bool FullP1Restored(const OriginalSetupScene&){return fullOk;}
};
static OriginalSetupScene scoped(uint8_t peer){
 const uint32_t a=0x500000u+peer*0x40000u;
 return {99,17,0x300000,0x300030,0x400000,a,a+0x10000u,peer};
}
int main(){
 OriginalHumanSetupLanes all;
 assert(!all.At(1)&&!all.At(5));
 for(uint8_t peer=2;peer<=4;peer++){
  auto *ticker=all.At(peer);MockEngine e;const auto s=scoped(peer);
  e.actor=s.ownedActor;e.owner=s.ownedOwner;
  assert(ticker->Bind(e,s));
  assert(ticker->Tick(e,1)==OriginalSetupState::Waiting);
  assert(ticker->Tick(e,1)==OriginalSetupState::Rejected);
  assert(ticker->Tick(e,2)==OriginalSetupState::Waiting);
  assert(ticker->Tick(e,3)==OriginalSetupState::SetupFinished);
  assert(ticker->ReadyForPhysics()&&ticker->ReadyForPhysicsNow(e)&&e.calls==3&&e.left==3);
  assert(ticker->Tick(e,4)==OriginalSetupState::SetupFinished&&e.calls==3);
  auto different=s;different.ownedActor+=16;
  assert(!ticker->Bind(e,different)&&ticker->Quarantined());
  assert(ticker->Tick(e,5)==OriginalSetupState::Quarantined);
  assert(!ticker->AfterVerifiedEngineTeardown(false));
  assert(ticker->AfterVerifiedEngineTeardown(true));
 }
 for(int mode=0;mode<7;mode++){
  MockEngine e;const auto s=scoped(2);e.actor=s.ownedActor;e.owner=s.ownedOwner;
  OriginalHumanSetupTick t;assert(t.Bind(e,s));
  if(mode==0)e.enterOk=false;
  if(mode==1)e.callOk=false;
  if(mode==2)e.restoreOk=false;
  if(mode==3)e.fullOk=false;
  if(mode==4)e.throwing=true;
  if(mode==5)e.enterThrows=true;
  if(mode==6)e.restoreThrows=true;
  assert(t.Tick(e,1)==OriginalSetupState::Quarantined);
  assert(e.left==1); // even after partial/throwing entry; no blind restore retry
  assert(t.Quarantined()&&!t.ReadyForPhysics());
 }
 {MockEngine e;const auto s=scoped(2);e.actor=s.ownedActor;e.owner=s.ownedOwner;
  e.completeAt=601;OriginalHumanSetupTick t;assert(t.Bind(e,s));
  for(uint32_t i=1;i<600;i++)assert(t.Tick(e,i)==OriginalSetupState::Waiting);
  assert(t.Tick(e,600)==OriginalSetupState::TimedOut);
  assert(t.Attempts()==600&&e.calls==600);
  assert(t.Tick(e,601)==OriginalSetupState::TimedOut&&e.calls==600);
 }
 {MockEngine e;const auto s=scoped(2);e.actor=s.ownedActor;e.owner=s.ownedOwner;
  OriginalHumanSetupTick t;assert(t.Bind(e,s));
  std::thread outsider([&]{assert(t.Tick(e,1)==OriginalSetupState::Rejected);});
  outsider.join();
  e.words[0x2D12B8]=0xA00000u;
  assert(t.Tick(e,1)==OriginalSetupState::Quarantined&&e.calls==0);
 }
 {MockEngine e;const auto s=scoped(2);e.actor=s.ownedActor;e.owner=s.ownedOwner;
  e.completeAt=1;OriginalHumanSetupTick t;assert(t.Bind(e,s));
  assert(t.Tick(e,1)==OriginalSetupState::SetupFinished&&t.ReadyForPhysicsNow(e));
  e.fullOk=false;assert(!t.ReadyForPhysicsNow(e)&&t.Quarantined());
 }
 {
  OriginalSetupFrameScheduler scheduler;
  std::array<MockEngine,3> engines{};
  std::array<MockEngine*,3> io{&engines[0],&engines[1],&engines[2]};
  for(uint8_t p=2;p<=4;p++){
   auto s=scoped(p);auto& e=engines[p-2];e.actor=s.ownedActor;e.owner=s.ownedOwner;
   e.completeAt=p;
   assert(scheduler.Bind(e,s,15));
  }
  for(uint32_t frame=1;frame<=4;frame++){
   auto result=scheduler.Tick(io,frame);
   assert(result==(frame==4?OriginalSetupState::SetupFinished:OriginalSetupState::Waiting));
  }
  assert(engines[0].calls==2&&engines[1].calls==3&&engines[2].calls==4);
  assert(scheduler.Tick(io,4)==OriginalSetupState::Rejected);
  engines[1].fullOk=false;
  assert(scheduler.Tick(io,5)==OriginalSetupState::Quarantined&&scheduler.Quarantined());
 }
 {
  OriginalSetupFrameScheduler scheduler;std::array<MockEngine,3> engines{};
  std::array<MockEngine*,3> io{&engines[0],&engines[1],&engines[2]};
  for(uint8_t p=2;p<=4;p++){
   auto s=scoped(p);auto& e=engines[p-2];e.actor=s.ownedActor;e.owner=s.ownedOwner;
   assert(scheduler.Bind(e,s,15));
  }
  engines[0].restoreOk=false;
  assert(scheduler.Tick(io,1)==OriginalSetupState::Quarantined);
  assert(engines[1].calls==0&&engines[2].calls==0);
 }
 {
  struct UpdateIO:MockEngine {
   bool ready=false;unsigned updates=0;
   bool ReadyForOriginalHumanUpdate(const OriginalSetupScene&,uint32_t){return ready;}
   bool CallOriginalHumanUpdate(const OriginalSetupScene&,uint32_t){++updates;return callOk;}
  } e;
  const auto s=scoped(2);e.actor=s.ownedActor;e.owner=s.ownedOwner;
  OriginalHumanUpdateTick tick;
  assert(tick.Tick(e,s,1)==OriginalHumanUpdateStatus::Rejected&&e.entered==0);
  e.ready=true;
  assert(tick.Tick(e,s,1)==OriginalHumanUpdateStatus::Updated&&e.updates==1&&e.left==1);
  assert(tick.Tick(e,s,1)==OriginalHumanUpdateStatus::Rejected);
  e.callOk=false;
  assert(tick.Tick(e,s,2)==OriginalHumanUpdateStatus::Quarantined&&e.left==2);
  assert(tick.Tick(e,s,3)==OriginalHumanUpdateStatus::Quarantined&&e.updates==2);
 }
 {
  struct UpdateIO:MockEngine {
   bool ready=true;unsigned updates=0;
   bool ReadyForOriginalHumanUpdate(const OriginalSetupScene&,uint32_t){return ready;}
   bool CallOriginalHumanUpdate(const OriginalSetupScene&,uint32_t){++updates;return callOk;}
  };
  OriginalSetupFrameScheduler scheduler;
  std::array<UpdateIO,3> engines{};
  std::array<UpdateIO*,3> io{&engines[0],&engines[1],&engines[2]};
  for(uint8_t p=2;p<=4;p++){
   const auto s=scoped(p);auto& e=engines[p-2];
   e.actor=s.ownedActor;e.owner=s.ownedOwner;e.completeAt=1;
   assert(scheduler.Bind(e,s,15));
  }
  assert(scheduler.TickHumans(io,1)==OriginalHumanUpdateStatus::Rejected);
  assert(scheduler.Tick(io,1)==OriginalSetupState::SetupFinished);
  assert(scheduler.TickHumans(io,1)==OriginalHumanUpdateStatus::Rejected);
  engines[2].ready=false;
  assert(scheduler.TickHumans(io,2)==OriginalHumanUpdateStatus::Rejected);
  for(const auto& e:engines)assert(e.updates==0);
  engines[2].ready=true;
  assert(scheduler.TickHumans(io,2)==OriginalHumanUpdateStatus::Rejected);
  assert(scheduler.TickHumans(io,3)==OriginalHumanUpdateStatus::Updated);
  for(const auto& e:engines)assert(e.updates==1&&e.left==2);
  assert(scheduler.TickHumans(io,3)==OriginalHumanUpdateStatus::Rejected);
  engines[1].callOk=false;
  assert(scheduler.TickHumans(io,4)==OriginalHumanUpdateStatus::Quarantined);
  assert(engines[0].updates==2&&engines[1].updates==2&&engines[2].updates==1);
  assert(engines[1].left==3);
  assert(scheduler.TickHumans(io,5)==OriginalHumanUpdateStatus::Quarantined);
 }
 {
  struct SourceIO {
   uint32_t source=0x600000u;
   uint64_t ctlGen=1,firstGen=2,extraGen=3;
   bool owned=true;
   bool Read32(uint32_t p,uint32_t& v){if(p!=0x7000A8u)return false;v=source;return true;}
   uint64_t ControllerGeneration(uint32_t p){return p==0x700000u?ctlGen:0;}
   uint64_t InputGeneration(uint32_t p){return p==0x600000u?firstGen:p==0x800000u?extraGen:0;}
   bool OwnsHumanInput(uint32_t p){return owned&&p==0x800000u;}
   uint32_t* MapControllerSource(uint32_t p){return p==0x700000u?&source:nullptr;}
  } io;
  OriginalHumanControllerSource scope;
  io.owned=false;
  assert(!scope.Enter(io,0x700000u,0x800000u)&&io.source==0x600000u);
  io.owned=true;
  assert(scope.Enter(io,0x700000u,0x800000u)&&io.source==0x800000u);
  assert(!scope.Enter(io,0x700000u,0x800000u));
  assert(scope.Restore(io)&&io.source==0x600000u);
  io.source=0;
  assert(scope.Enter(io,0x700000u,0x800000u)&&io.source==0);
  assert(scope.Restore(io)&&io.source==0);
  io.source=0x600000u;
  assert(scope.Enter(io,0x700000u,0x800000u));
  ++io.firstGen;
  assert(!scope.Restore(io)&&scope.Quarantined()&&io.source==0x800000u);
 }
 std::cout<<"ORIGINAL_BLACK_P2_P3_P4_MULTIFRAME_SETUP_CONTRACT_PASS_NO_LIVE_GUEST_CALL\n";
}
