#include "room_original_host_player_link_exchange.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>
#include <iostream>
using namespace blackcoop;
struct LinkIO {
 std::vector<uint8_t> bytes=std::vector<uint8_t>(0x700000);
 uint64_t scene=7,sourceGeneration=1,targetGeneration=2;
 bool failCopy=false,failMap=false,aliasMap=false;
 LinkIO(){
  set(0x2D45D4u,0x100000u);set(0x2D12B8u,0x120000u);
  set(0x2D45F4u,0x140000u);set(0x2D1980u,0x150000u);
 }
 void set(uint32_t p,uint32_t v){assert(p+4<=bytes.size());std::memcpy(bytes.data()+p,&v,4);}
 bool Span(uint32_t p,uint32_t n)const{return p>=0x10000u&&n&&uint64_t(p)+n<=bytes.size();}
 bool Read32(uint32_t p,uint32_t& v)const{
  if(!Span(p,4))return false;
  std::memcpy(&v,bytes.data()+p,4);
  return true;
 }
 bool CopyGuest(uint32_t p,void* dst,uint32_t n)const{
  if(failCopy||!dst||!Span(p,n))return false;
  std::memcpy(dst,bytes.data()+p,n);return true;
 }
 bool OwnsActor(uint32_t p,uint32_t n)const{return p==0x300000u&&n==0x900u;}
 uint64_t Scene()const{return scene;}
 uint64_t ActorGeneration(uint32_t p,uint32_t n)const noexcept{
  if(n!=0x900u)return 0;
  if(p==0x100030u)return sourceGeneration;
  if(p==0x300000u)return targetGeneration;
  return 0;
 }
 uint8_t* MapOriginalActor(uint32_t p,uint32_t n)noexcept{
  if(failMap||!ActorGeneration(p,n)||!Span(p,n))return nullptr;
  return bytes.data()+(aliasMap?0x100030u:p);
 }
};
int main(){
 const OriginalPlayerStorageScene b{7,0x100000,0x120000,0x140000,0x150000};
 const uint32_t actor=b.core+0x30u,p2=0x300000u;
 {
  LinkIO io;io.set(actor+0x34u,actor+0x34u);
  io.set(p2+0x34u,p2+0x34u);
  io.set(actor+0x280u,0x350000u);io.set(0x350000u,actor+0x280u);
  io.set(p2+0x280u,0x456789ABu);
  io.set(actor+0x600u,0x11112222u);io.set(p2+0x600u,0x33334444u);
  const auto first=io.bytes;
  OriginalPlayerLinkExchangePlan plan;
  assert(plan.Prepare(io,b,p2)&&plan.Ready());
  assert(plan.WorldNodeWords()==14u&&plan.DynamicLinkWords()==1u);
  assert(plan.ProtectedWord(0x20u)&&plan.ProtectedWord(0x54u));
  assert(plan.ProtectedWord(0x280u)&&!plan.ProtectedWord(0x600u));
  uint32_t c=0,s=0;
  assert(plan.PreviewEnterWord(0x34u,c,s)&&c==actor+0x34u&&s==p2+0x34u);
  assert(plan.PreviewEnterWord(0x280u,c,s)&&c==0x350000u&&s==0x456789ABu);
  assert(plan.PreviewEnterWord(0x600u,c,s)&&c==0x33334444u&&s==0x11112222u);
  assert(!plan.PreviewEnterWord(0x900u,c,s)&&!plan.PreviewEnterWord(0x601u,c,s));
  assert(plan.SourceStillCurrent(io)&&plan.PreviewRoundTrip());
  assert(io.bytes==first); // no guest RAM writes
  io.set(actor+0x22Cu,0x77770000u);
  assert(!plan.SourceStillCurrent(io));
  io.set(actor+0x22Cu,0u);
  assert(plan.SourceStillCurrent(io));
  io.set(0x350000u,0u);
  assert(!plan.SourceStillCurrent(io)); // external linked list drift
  plan.Reset();assert(!plan.Ready()&&!plan.PreviewRoundTrip());
 }
 {LinkIO io;OriginalPlayerLinkExchangePlan plan;auto bad=b;bad.scene++;
  assert(!plan.Prepare(io,bad,p2));}
 {LinkIO io;OriginalPlayerLinkExchangePlan plan;
  assert(!plan.Prepare(io,b,actor));}
 {LinkIO io;OriginalPlayerLinkExchangePlan plan;
  assert(!plan.Prepare(io,b,p2+0x100u));}
 {LinkIO io;OriginalPlayerLinkExchangePlan plan;
  io.failCopy=true;assert(!plan.Prepare(io,b,p2)&&!plan.Ready());}
 {LinkIO io;OriginalPlayerLinkExchangePlan plan;bool fromForeign=true;
  std::thread thread([&]{fromForeign=plan.Prepare(io,b,p2);});thread.join();
  assert(!fromForeign&&!plan.Ready());}
 {
  LinkIO io;OriginalPlayerLinkExchangePlan plan;
  io.set(actor+0x34u,0x12345678u);io.set(p2+0x34u,0x87654321u);
  io.set(actor+0x600u,11u);io.set(p2+0x600u,22u);
  assert(plan.Prepare(io,b,p2));const auto before=io.bytes;
  io.aliasMap=true;assert(!plan.EnterActorOnly(io)&&io.bytes==before);
  io.aliasMap=false;io.failMap=true;
  assert(!plan.EnterActorOnly(io)&&io.bytes==before);
  io.failMap=false;
  assert(plan.EnterActorOnly(io)&&plan.Entered()&&!plan.Ready());
  uint32_t v=0;assert(io.Read32(actor+0x600u,v)&&v==22u);
  assert(io.Read32(p2+0x600u,v)&&v==11u);
  plan.Reset();assert(plan.Entered()&&!plan.Prepare(io,b,p2));
  // Model an original P2 update while entered, including a live grid head.
  io.set(actor+0x600u,33u);io.set(actor+0x34u,0x12345679u);
  assert(plan.RestoreActorOnly(io)&&!plan.Entered());
  assert(io.Read32(actor+0x600u,v)&&v==11u);
  assert(io.Read32(p2+0x600u,v)&&v==33u);
  assert(io.Read32(actor+0x34u,v)&&v==0x12345679u);
  assert(io.Read32(p2+0x34u,v)&&v==0x87654321u);
  assert(!plan.RestoreActorOnly(io)&&plan.Prepare(io,b,p2));
 }
 for(int mode=0;mode<3;mode++){
  LinkIO io;OriginalPlayerLinkExchangePlan plan;
  io.set(actor+0x600u,11u);io.set(p2+0x600u,22u);
  assert(plan.Prepare(io,b,p2)&&plan.EnterActorOnly(io));
  if(mode==0)++io.targetGeneration;
  if(mode==1)io.set(p2+0x600u,99u);
  if(mode==2)io.failMap=true;
  const auto before=io.bytes;
  assert(!plan.RestoreActorOnly(io)&&plan.Quarantined());
  assert(io.bytes==before&&plan.Entered());
  plan.Reset();assert(plan.Quarantined()&&!plan.Prepare(io,b,p2));
 }
 std::cout<<"ORIGINAL_P1_GRID_LINK_ACTOR_EXCHANGE_FIXTURE_PASS_NO_NATIVE_ACTIVATION\n";
}
