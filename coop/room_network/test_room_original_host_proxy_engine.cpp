// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_host_proxy_engine.hpp"
#include <cassert>
#include <map>
#include <vector>
#include <thread>
#include <iostream>
#include <stdexcept>
#include <tuple>
struct Engine {
 uint64_t scene=7;std::map<uint32_t,uint32_t> words;std::map<uint32_t,uint8_t> flags;
 struct Op{uint32_t fn,ebx,arg;};std::vector<Op> calls;
 bool badReturn=false,changeWorld=false,failRelease=false,reuse=false;
 bool throwAcquire=false,throwRelease=false,revokeActorOwnership=false;
 static constexpr uint32_t world=0x10000,physics=0x20000,actor=0x40000,pool=0x60000,collider=0x80000;
 static constexpr uint32_t actor2=0x50000,actor3=0x90000,actor4=0xA0000;
 Engine(){words[0x2D45D4]=world;words[0x2D45F4]=physics;words[0x2D1980]=pool;
  words[actor]=0x28CFE0;words[actor+0xC4]=2;words[actor+0x354]=collider;words[actor+0x374]=9;words[actor+0xB4]=0;
  words[world+0x30+0xB4]=physics+0x2320+19*0x50;
  for(unsigned i=0;i<20;i++)flags[physics+0x2960+i]=0;
  for(const auto& [a,serial,col]:{std::tuple<uint32_t,uint32_t,uint32_t>{actor2,10,0x81000},
                                {actor3,11,0x82000},{actor4,12,0x83000}}){
   words[a]=0x28CFE0;words[a+0xC4]=2;words[a+0x354]=col;
   words[a+0x374]=serial;words[a+0xB4]=0;
  }
 }
 uint64_t Scene()const{return scene;}
 uint64_t AiTableGeneration(uint32_t p,uint32_t n)const{return p==0x70000u+0xFACu&&n==384u?1u:0u;}
 uint64_t ActorGeneration(uint32_t p,uint32_t n)const{return OwnsActor(p,n)?2u:0u;}
 bool ReplaceOriginalAiTarget(uint32_t p,uint32_t old,uint32_t next,uint64_t gen){
  if(gen!=1||words[p]!=old)return false;
  words[p]=next;return true;
 }
 bool OwnsActor(uint32_t p,uint32_t n)const noexcept{
  return !revokeActorOwnership&&n==0x900u&&
    (p==actor||p==actor2||p==actor3||p==actor4);
 }
 bool Span(uint32_t p,uint32_t n)const{return p>=0x10000&&uint64_t(p)+n<0x400000;}
 bool Read32(uint32_t p,uint32_t& v){if(!Span(p,4))return false;auto i=words.find(p);if(i==words.end())return false;v=i->second;return true;}
 bool Read8(uint32_t p,uint8_t& v){auto i=flags.find(p);if(i==flags.end())return false;v=i->second;return true;}
 bool Call(uint32_t fn,uint32_t ebx,uint32_t arg,uint32_t& result){
  calls.push_back({fn,ebx,arg});assert(ebx==physics);
  if(fn==0xDF400){assert(arg==actor||arg==actor2||arg==actor3||arg==actor4);
   unsigned i=0;while(i<20&&flags[physics+0x2960+i])++i;assert(i<20);
   auto proxy=physics+0x2320+i*0x50;flags[physics+0x2960+i]=1;words[arg+0xB4]=proxy;words[proxy+0x30]=reuse?arg+0x1000:arg;
   if(changeWorld)words[0x2D45F4]=physics+0x10000;
   if(throwAcquire)throw std::runtime_error("partial original allocation");
   result=badReturn?0:0xABCD0001;return true;
  }
  assert(fn==0xDBBE0);uint32_t originalActor=words[arg+0x30];assert(originalActor!=0);
  assert(arg==words[originalActor+0xB4]);
  if(throwRelease)throw std::runtime_error("original release may have partially completed");
  if(!failRelease){flags[physics+0x2960+(arg-physics-0x2320)/0x50]=0;words[arg+0x30]=0;words[originalActor+0xB4]=0;}
  result=0xBAD;return true;
 }
};
int main(){
 using namespace blackcoop;OriginalHostProxyBinding b{7,Engine::world,Engine::physics,Engine::actor,9,Engine::collider,true};
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine adapter(ledger);assert(adapter.Acquire(e,b)==OriginalProxyStatus::Owned);
  auto h=adapter.Handle();assert(h.proxy==Engine::physics+0x2320);
  assert(e.calls.size()==1&&e.calls[0].fn==0xDF400&&e.calls[0].arg==Engine::actor);
  assert(adapter.Release(e)==OriginalProxyStatus::Released);assert(e.calls.size()==2&&e.calls[1].arg==h.proxy);
  assert(!adapter.Handle().proxy);assert(!adapter.RequiresEngineCleanup());}
 for(unsigned k=0;k<8;k++){Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);auto bad=b;
  if(k==0) bad.setupComplete=false;
  if(k==1) e.scene++;
  if(k==2) bad.actor=Engine::world+0x30;
  if(k==3) bad.actor=Engine::pool+0xA0;
  if(k==4) e.words[Engine::actor+0x354]=0;
  if(k==5) e.words[Engine::actor+0x374]++;
  if(k==6)e.words[Engine::actor+0xB4]=Engine::physics+0x2320;
  if(k==7)for(unsigned i=0;i<19;i++)e.flags[Engine::physics+0x2960+i]=1;
  assert(a.Acquire(e,bad)==OriginalProxyStatus::Rejected);assert(e.calls.empty());}
 // Forging the entire original actor header in unrelated committed memory
 // is not proof of original-human allocation ownership.
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);
  auto fake=b;fake.actor=0xB0000;
  for(auto off:{0u,0xC4u,0x354u,0x374u,0xB4u})
   e.words[fake.actor+off]=e.words[Engine::actor+off];
  assert(a.Acquire(e,fake)==OriginalProxyStatus::Rejected);
  assert(e.calls.empty()&&!ledger.Quarantined());
 }
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);
  e.revokeActorOwnership=true;
  assert(a.Acquire(e,b)==OriginalProxyStatus::Rejected);
  assert(e.calls.empty()&&!ledger.Quarantined());
 }
 // A valid-looking kind-2 object elsewhere INSIDE the live original core is
 // not an independently allocated shadow. PC original tick spans 0x5CC0.
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);auto bad=b;bad.actor=Engine::world+0x1000;
  for(auto off:{0u,0xC4u,0x354u,0x374u,0xB4u})e.words[bad.actor+off]=e.words[Engine::actor+off];
  assert(a.Acquire(e,bad)==OriginalProxyStatus::Rejected);assert(e.calls.empty());}
 // A forged actor in the original physics allocation must never reach DF400.
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);auto bad=b;bad.actor=Engine::physics+0x1000;
  for(auto off:{0u,0xC4u,0x354u,0x374u,0xB4u})e.words[bad.actor+off]=e.words[Engine::actor+off];
  assert(a.Acquire(e,bad)==OriginalProxyStatus::Rejected);assert(e.calls.empty());}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);e.badReturn=true;assert(a.Acquire(e,b)==OriginalProxyStatus::RolledBack);assert(e.calls.size()==2&&!a.Handle().proxy);assert(!a.RequiresEngineCleanup());}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);e.badReturn=true;e.failRelease=true;assert(a.Acquire(e,b)==OriginalProxyStatus::RollbackFailed);assert(a.Handle().proxy);assert(a.RequiresEngineCleanup());
  e.failRelease=false;assert(a.Release(e)==OriginalProxyStatus::Rejected);assert(e.calls.size()==2);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);e.changeWorld=true;assert(a.Acquire(e,b)==OriginalProxyStatus::ScopeLost);assert(e.calls.size()==1);
  e.words[0x2D45F4]=Engine::physics;e.words[Engine::actor+0xB4]=0;
  assert(a.RequiresEngineCleanup());assert(a.Acquire(e,b)==OriginalProxyStatus::Rejected);assert(e.calls.size()==1);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);e.reuse=true;assert(a.Acquire(e,b)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);
  e.words[Engine::actor+0xB4]=0;assert(a.RequiresEngineCleanup());
  assert(a.Acquire(e,b)==OriginalProxyStatus::Rejected);assert(e.calls.size()==1);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);e.words[Engine::actor+0x374]++;assert(a.Release(e)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);e.scene++;assert(a.Release(e)==OriginalProxyStatus::ScopeLost);assert(e.calls.size()==1);
  assert(a.RequiresEngineCleanup());e.scene--;assert(a.Release(e)==OriginalProxyStatus::Rejected);assert(e.calls.size()==1);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);auto h=a.Handle();e.words[h.proxy+0x30]=Engine::actor+0x1000;assert(a.Release(e)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);auto h=a.Handle();e.flags[Engine::physics+0x2960+(h.proxy-Engine::physics-0x2320)/0x50]=0;assert(a.Release(e)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);OriginalProxyStatus status{};std::thread t([&]{status=a.Release(e);});t.join();assert(status==OriginalProxyStatus::Rejected);assert(e.calls.size()==1);assert(a.Release(e)==OriginalProxyStatus::Released);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);e.throwAcquire=true;
  assert(a.Acquire(e,b)==OriginalProxyStatus::OwnershipLost);assert(a.RequiresEngineCleanup());
  e.throwAcquire=false;e.words[Engine::actor+0xB4]=0;
  assert(a.Acquire(e,b)==OriginalProxyStatus::Rejected);assert(e.calls.size()==1);}
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);
  e.throwRelease=true;assert(a.Release(e)==OriginalProxyStatus::RollbackFailed);
  assert(a.RequiresEngineCleanup());e.throwRelease=false;
  assert(a.Release(e)==OriginalProxyStatus::Rejected);assert(e.calls.size()==2);}
 // Cross-adapter ledger persists one real original physics scene and owns
 // independent human proxies, but cannot be silently replaced on a failure.
 {Engine e;OriginalProxySceneLedger ledger;
  OriginalHostProxyBinding b2=b,b3=b,b4=b;
  b2.actor=Engine::actor2;b2.serial=10;b2.collider=0x81000;
  b3.actor=Engine::actor3;b3.serial=11;b3.collider=0x82000;
  b4.actor=Engine::actor4;b4.serial=12;b4.collider=0x83000;
  OriginalHostProxyEngine a(ledger),p2(ledger),p3(ledger),fourth(ledger);
  assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);
  assert(p2.Acquire(e,b2)==OriginalProxyStatus::Owned);
  assert(p3.Acquire(e,b3)==OriginalProxyStatus::Owned);
  assert(ledger.ActiveCount()==3&&!ledger.Quarantined());
  assert(fourth.Acquire(e,b4)==OriginalProxyStatus::Rejected);
  assert(e.calls.size()==3);
  assert(p2.Release(e)==OriginalProxyStatus::Released);
  assert(ledger.ActiveCount()==2);
  assert(fourth.Acquire(e,b4)==OriginalProxyStatus::Owned);
  assert(ledger.ActiveCount()==3);
  assert(fourth.Release(e)==OriginalProxyStatus::Released);
  assert(p3.Release(e)==OriginalProxyStatus::Released);
  assert(a.Release(e)==OriginalProxyStatus::Released);
  assert(ledger.ActiveCount()==0&&!ledger.Quarantined());
 }
 {Engine e;OriginalProxySceneLedger ledger;
  {
   OriginalHostProxyEngine old(ledger);assert(old.Acquire(e,b)==OriginalProxyStatus::Owned);
   assert(ledger.ActiveCount()==1);
  }
  // A reconstructed adapter must not revive a leaked native proxy even if
  // another callback accidentally cleared the old actor backlink.
  assert(ledger.Quarantined());
  e.words[Engine::actor+0xB4]=0;e.flags[Engine::physics+0x2960]=0;
  OriginalHostProxyEngine fresh(ledger);
  assert(fresh.Acquire(e,b)==OriginalProxyStatus::Rejected&&e.calls.size()==1);
 }
 {Engine e;OriginalProxySceneLedger ledger;
  {
   OriginalHostProxyEngine old(ledger);e.throwAcquire=true;
   assert(old.Acquire(e,b)==OriginalProxyStatus::OwnershipLost);
   assert(old.RequiresEngineCleanup());
  }
  e.throwAcquire=false;e.words[Engine::actor+0xB4]=0;
  e.flags[Engine::physics+0x2960]=0;
  OriginalHostProxyEngine fresh(ledger);
  assert(ledger.Quarantined()&&fresh.Acquire(e,b)==OriginalProxyStatus::Rejected);
  assert(e.calls.size()==1);
 }
 {Engine e;OriginalProxySceneLedger ledger;
  {OriginalHostProxyEngine old(ledger);assert(old.Acquire(e,b)==OriginalProxyStatus::Owned);
   assert(old.Release(e)==OriginalProxyStatus::Released);}
  e.scene++;auto next=b;next.scene++;
  OriginalHostProxyEngine fresh(ledger);
  assert(fresh.Acquire(e,next)==OriginalProxyStatus::Rejected);
  assert(e.calls.size()==2);
 }
 {Engine e;OriginalProxySceneLedger ledger;OriginalHostProxyEngine a(ledger);
  assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);
  e.revokeActorOwnership=true;
  assert(a.Release(e)==OriginalProxyStatus::OwnershipLost);
  assert(ledger.Quarantined()&&a.RequiresEngineCleanup());
  assert(e.calls.size()==1);
 }
 {
  Engine e;OriginalAiTargetLedger targets;
  constexpr uint32_t table=0x70000u+0xFACu;
  e.words[0x2D1988u]=0x70000u;
  e.words[Engine::world+0x30u+0x378u]=0;
  e.words[Engine::actor+0x378u]=1;
  e.words[table+12u+4u]=0;e.words[table+12u+8u]=Engine::world+0x30u;
  assert(targets.BindAfterSetup(e,b,2));
  assert(e.words[table+12u+8u]==Engine::actor);
  assert(targets.BindAfterSetup(e,b,2));
  auto other=b;other.actor=Engine::actor2;
  e.words[Engine::actor2+0x378u]=1;
  assert(!targets.BindAfterSetup(e,other,3));
  e.words[Engine::actor2+0x378u]=0;
  assert(!targets.BindAfterSetup(e,other,3));
  e.words[Engine::actor2+0x378u]=2;
  e.words[table+24u+4u]=0;e.words[table+24u+8u]=Engine::world+0x30u;
  assert(targets.BindAfterSetup(e,other,3));
  assert(e.words[table+24u+8u]==Engine::actor2);
 }
 std::cout<<"ORIGINAL_PROXY_ENGINE_PASS: original acquire/release operations, ownership, cancellation scopes and rollback\n";
}
