// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_host_proxy_engine.hpp"
#include <cassert>
#include <map>
#include <vector>
#include <thread>
#include <iostream>
struct Engine {
 uint64_t scene=7;std::map<uint32_t,uint32_t> words;std::map<uint32_t,uint8_t> flags;
 struct Op{uint32_t fn,ebx,arg;};std::vector<Op> calls;
 bool badReturn=false,changeWorld=false,failRelease=false,reuse=false;
 static constexpr uint32_t world=0x10000,physics=0x20000,actor=0x40000,pool=0x60000,collider=0x80000;
 Engine(){words[0x2D45D4]=world;words[0x2D45F4]=physics;words[0x2D1980]=pool;
  words[actor]=0x28CFE0;words[actor+0xC4]=2;words[actor+0x354]=collider;words[actor+0x374]=9;words[actor+0xB4]=0;
  words[world+0x30+0xB4]=physics+0x2320+19*0x50;
  for(unsigned i=0;i<20;i++)flags[physics+0x2960+i]=0;
 }
 uint64_t Scene()const{return scene;}
 bool Span(uint32_t p,uint32_t n)const{return p>=0x10000&&uint64_t(p)+n<0x400000;}
 bool Read32(uint32_t p,uint32_t& v){if(!Span(p,4))return false;auto i=words.find(p);if(i==words.end())return false;v=i->second;return true;}
 bool Read8(uint32_t p,uint8_t& v){auto i=flags.find(p);if(i==flags.end())return false;v=i->second;return true;}
 bool Call(uint32_t fn,uint32_t ebx,uint32_t arg,uint32_t& result){
  calls.push_back({fn,ebx,arg});assert(ebx==physics);
  if(fn==0xDF400){assert(arg==actor);unsigned i=0;while(i<20&&flags[physics+0x2960+i])++i;assert(i<20);
   auto proxy=physics+0x2320+i*0x50;flags[physics+0x2960+i]=1;words[actor+0xB4]=proxy;words[proxy+0x30]=reuse?actor+0x1000:actor;
   if(changeWorld)words[0x2D45F4]=physics+0x10000;
   result=badReturn?0:0xABCD0001;return true;
  }
  assert(fn==0xDBBE0);assert(arg==words[actor+0xB4]);
  if(!failRelease){flags[physics+0x2960+(arg-physics-0x2320)/0x50]=0;words[arg+0x30]=0;words[actor+0xB4]=0;}
  result=0xBAD;return true;
 }
};
int main(){
 using namespace blackcoop;OriginalHostProxyBinding b{7,Engine::world,Engine::physics,Engine::actor,9,Engine::collider,true};
 {Engine e;OriginalHostProxyEngine adapter;assert(adapter.Acquire(e,b)==OriginalProxyStatus::Owned);
  auto h=adapter.Handle();assert(h.proxy==Engine::physics+0x2320);
  assert(e.calls.size()==1&&e.calls[0].fn==0xDF400&&e.calls[0].arg==Engine::actor);
  assert(adapter.Release(e)==OriginalProxyStatus::Released);assert(e.calls.size()==2&&e.calls[1].arg==h.proxy);
  assert(!adapter.Handle().proxy);}
 for(unsigned k=0;k<8;k++){Engine e;OriginalHostProxyEngine a;auto bad=b;
  if(k==0)bad.setupComplete=false;if(k==1)e.scene++;
  if(k==2)bad.actor=Engine::world+0x30;if(k==3)bad.actor=Engine::pool+0xA0;
  if(k==4)e.words[Engine::actor+0x354]=0;if(k==5)e.words[Engine::actor+0x374]++;
  if(k==6)e.words[Engine::actor+0xB4]=Engine::physics+0x2320;
  if(k==7)for(unsigned i=0;i<19;i++)e.flags[Engine::physics+0x2960+i]=1;
  assert(a.Acquire(e,bad)==OriginalProxyStatus::Rejected);assert(e.calls.empty());}
 // A valid-looking kind-2 object elsewhere INSIDE the live original core is
 // not an independently allocated shadow. PC original tick spans 0x5CC0.
 {Engine e;OriginalHostProxyEngine a;auto bad=b;bad.actor=Engine::world+0x1000;
  for(auto off:{0u,0xC4u,0x354u,0x374u,0xB4u})e.words[bad.actor+off]=e.words[Engine::actor+off];
  assert(a.Acquire(e,bad)==OriginalProxyStatus::Rejected);assert(e.calls.empty());}
 {Engine e;OriginalHostProxyEngine a;e.badReturn=true;assert(a.Acquire(e,b)==OriginalProxyStatus::RolledBack);assert(e.calls.size()==2&&!a.Handle().proxy);}
 {Engine e;OriginalHostProxyEngine a;e.badReturn=true;e.failRelease=true;assert(a.Acquire(e,b)==OriginalProxyStatus::RollbackFailed);assert(a.Handle().proxy);}
 {Engine e;OriginalHostProxyEngine a;e.changeWorld=true;assert(a.Acquire(e,b)==OriginalProxyStatus::ScopeLost);assert(e.calls.size()==1);}
 {Engine e;OriginalHostProxyEngine a;e.reuse=true;assert(a.Acquire(e,b)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);}
 {Engine e;OriginalHostProxyEngine a;assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);e.words[Engine::actor+0x374]++;assert(a.Release(e)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);}
 {Engine e;OriginalHostProxyEngine a;assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);e.scene++;assert(a.Release(e)==OriginalProxyStatus::ScopeLost);assert(e.calls.size()==1);}
 {Engine e;OriginalHostProxyEngine a;assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);auto h=a.Handle();e.words[h.proxy+0x30]=Engine::actor+0x1000;assert(a.Release(e)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);}
 {Engine e;OriginalHostProxyEngine a;assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);auto h=a.Handle();e.flags[Engine::physics+0x2960+(h.proxy-Engine::physics-0x2320)/0x50]=0;assert(a.Release(e)==OriginalProxyStatus::OwnershipLost);assert(e.calls.size()==1);}
 {Engine e;OriginalHostProxyEngine a;assert(a.Acquire(e,b)==OriginalProxyStatus::Owned);OriginalProxyStatus status{};std::thread t([&]{status=a.Release(e);});t.join();assert(status==OriginalProxyStatus::Rejected);assert(e.calls.size()==1);assert(a.Release(e)==OriginalProxyStatus::Released);}
 std::cout<<"ORIGINAL_PROXY_ENGINE_PASS: original acquire/release operations, ownership, cancellation scopes and rollback\n";
}
