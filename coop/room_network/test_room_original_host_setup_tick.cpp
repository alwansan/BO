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
  ++entered;return enterOk&&s.ownedActor==actor&&s.ownedOwner==owner;
 }
 bool GuestSetup1E0E40(uint32_t world,uint32_t eax,uint32_t& out){
  assert(world==0x300000u&&eax==0);++calls;
  if(throwing)throw 1;
  out=calls>=completeAt?1u:0u;return callOk;
 }
 bool RestoreOriginalP1(const OriginalSetupScene&){++left;return restoreOk;}
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
  assert(ticker->ReadyForPhysics()&&e.calls==3&&e.left==3);
  assert(ticker->Tick(e,4)==OriginalSetupState::SetupFinished&&e.calls==3);
  auto different=s;different.ownedActor+=16;
  assert(!ticker->Bind(e,different)&&ticker->Quarantined());
  assert(ticker->Tick(e,5)==OriginalSetupState::Quarantined);
  assert(!ticker->AfterVerifiedEngineTeardown(false));
  assert(ticker->AfterVerifiedEngineTeardown(true));
 }
 for(int mode=0;mode<5;mode++){
  MockEngine e;const auto s=scoped(2);e.actor=s.ownedActor;e.owner=s.ownedOwner;
  OriginalHumanSetupTick t;assert(t.Bind(e,s));
  if(mode==0)e.enterOk=false;
  if(mode==1)e.callOk=false;
  if(mode==2)e.restoreOk=false;
  if(mode==3)e.fullOk=false;
  if(mode==4)e.throwing=true;
  assert(t.Tick(e,1)==OriginalSetupState::Quarantined);
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
 std::cout<<"ORIGINAL_BLACK_P2_P3_P4_MULTIFRAME_SETUP_CONTRACT_PASS_NO_LIVE_GUEST_CALL\n";
}
