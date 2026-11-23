#include "room_original_host_player_shadow_plan.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
using namespace blackcoop;
struct ShadowIO {
 std::vector<uint8_t> memory=std::vector<uint8_t>(0x700000);
 std::map<uint32_t,uint32_t> owned;
 uint64_t scene=9;unsigned writes=0;bool copyFailure=false,cameraFails=false;
 unsigned allocs=0,frees=0,cameraCalls=0;
 ShadowIO(){
  owned={{0x500000,0x16C0},{0x503000,0x900},{0x504000,0x40},{0x505000,0x40},{0x506000,0x630}};
  set(0x2D45D4,0x100000);set(0x2D12B8,0x120000);
  set(0x2D45F4,0x140000);set(0x2D1980,0x150000);
  set(0x100030,0x28CFE0);set(0x100030+0xC4,2);
  set(0x100030+0x34C,0x200000);set(0x100030+0x350,0x201000);
  set(0x100030+0xB4,0x141111);set(0x100030+0x250,0x120110);
  set(0x120004,0x120800);
 }
 bool Span(uint32_t p,uint32_t n)const{return p>=0x10000&&uint64_t(p)+n<=memory.size();}
 bool Read32(uint32_t p,uint32_t& out)const{if(!Span(p,4))return false;std::memcpy(&out,memory.data()+p,4);return true;}
 bool CopyGuest(uint32_t p,void* dst,uint32_t n)const{if(copyFailure||!dst||!Span(p,n))return false;std::memcpy(dst,memory.data()+p,n);return true;}
 bool Owns(uint32_t p,uint32_t n)const{auto i=owned.find(p);return i!=owned.end()&&i->second==n;}
 uint32_t Alloc(uint32_t size){
  static constexpr uint32_t addresses[]={0x500000,0x503000,0x504000,0x505000,0x506000};
  static constexpr uint32_t sizes[]={0x16C0,0x900,0x40,0x40,0x630};
  if(allocs>=5||sizes[allocs]!=size)return 0;
  return addresses[allocs++];
 }
 bool Free(uint32_t address){
  if(!owned.erase(address))return false;
  ++frees;return true;
 }
 bool Camera(uint32_t address,uint32_t& result){
  ++cameraCalls;
  if(cameraFails||!Owns(address,0x16C0))return false;
  set(address,0x28D12C);set(address+0x790,0x28C3B8);
  result=address;return true;
 }
 void set(uint32_t p,uint32_t w){assert(Span(p,4));std::memcpy(memory.data()+p,&w,4);++writes;}
 uint64_t Scene()const{return scene;}
};
int main(){
 const OriginalPlayerStorageScene b{9,0x100000,0x120000,0x140000,0x150000};
 const OriginalPlayerStorageView t{0x500000,0x503000,0x504000,0x505000,0x506000,0x630};
 {
  ShadowIO io;const auto before=io.memory;const unsigned writes=io.writes;
  OriginalHostPlayerShadowPlan p;uint32_t w=0;
  assert(p.Prepare(io,b,t)&&p.Ready());
  assert(before==io.memory&&writes==io.writes); // no original RAM writes
  assert(p.PreviewActorWord(0x30,w)&&w==0);
  assert(p.PreviewActorWord(0x34,w)&&w==t.actor+0x34);
  assert(p.PreviewActorWord(0x38,w)&&w==t.actor+0x34);
  assert(p.PreviewActorWord(0xB4,w)&&w==0);
  assert(p.PreviewActorWord(0x298,w)&&w==t.weapons);
  assert(p.PreviewActorWord(0x29C,w)&&w==0);
  assert(p.PreviewActorWord(0x34C,w)&&w==t.parts);
  assert(p.PreviewActorWord(0x350,w)&&w==t.parts+0x70);
  assert(p.PreviewActorWord(0x250,w)&&w==t.owner+0x110);
  assert(p.PreviewOwnerWord(4,w)&&w==t.owner+0x800);
  assert(!p.PreviewActorWord(0x8FF,w));
  p.Reset();assert(!p.Ready()&&!p.PreviewActorWord(0,w));
 }
 {ShadowIO io;OriginalHostPlayerShadowPlan p;auto wrong=b;wrong.scene++;assert(!p.Prepare(io,wrong,t));}
 {ShadowIO io;OriginalHostPlayerShadowPlan p;auto wrong=t;wrong.actor=t.weapons;assert(!p.Prepare(io,b,wrong));}
 {ShadowIO io;OriginalHostPlayerShadowPlan p;auto wrong=t;wrong.actor=b.core+0x20;assert(!p.Prepare(io,b,wrong));}
 {ShadowIO io;OriginalHostPlayerShadowPlan p;io.set(b.core+0x30+0xC4,1);assert(!p.Prepare(io,b,t));}
 {ShadowIO io;OriginalHostPlayerShadowPlan p;io.set(b.core+0x30+0x34C,0);assert(!p.Prepare(io,b,t));}
 {ShadowIO io;OriginalHostPlayerShadowPlan p;io.set(b.core+0x30+0x34C,t.parts);assert(!p.Prepare(io,b,t));}
 {ShadowIO io;OriginalHostPlayerShadowPlan p;io.copyFailure=true;assert(!p.Prepare(io,b,t));assert(!p.Ready());}
 {ShadowIO io;OriginalHostPlayerShadowPlan p;bool returned=true;
  std::thread t2([&]{returned=p.Prepare(io,b,t);});t2.join();assert(!returned&&!p.Ready());}
 {ShadowIO io;OriginalHostPlayerStage stage;const auto source=io.memory;
  const unsigned before=io.writes;
  assert(stage.Prepare(io,b)==OriginalHostPlayerStageStatus::Prepared);
  assert(stage.Ready()&&stage.View().actor==t.actor&&io.cameraCalls==1);
  uint32_t w=0;
  assert(stage.PreviewActorWord(0x34C,w)&&w==t.parts);
  assert(stage.PreviewActorWord(0x298,w)&&w==t.weapons);
  assert(io.writes==before+2);
  assert(std::memcmp(source.data()+b.core+0x30,io.memory.data()+b.core+0x30,0x900)==0);
  assert(stage.Prepare(io,b)==OriginalHostPlayerStageStatus::AlreadyOwned);
  assert(stage.ReleaseConstructorOnly(io)==OriginalStorageStatus::Released);
  assert(io.frees==5&&io.owned.empty()&&!stage.Ready());
 }
 {ShadowIO io;io.copyFailure=true;OriginalHostPlayerStage stage;
  assert(stage.Prepare(io,b)==OriginalHostPlayerStageStatus::ShadowRejected);
  assert(!stage.Ready()&&io.cameraCalls==1&&io.frees==5&&io.owned.empty());
 }
 {ShadowIO io;io.cameraFails=true;OriginalHostPlayerStage stage;
  assert(stage.Prepare(io,b)==OriginalHostPlayerStageStatus::StorageRejected);
  assert(io.cameraCalls==1&&io.frees==5&&io.owned.empty());
 }
 {ShadowIO io;OriginalHostPlayerStage stage;OriginalHostPlayerStageStatus result{};
  std::thread foreign([&]{result=stage.Prepare(io,b);});foreign.join();
  assert(result==OriginalHostPlayerStageStatus::WrongThread&&io.cameraCalls==0);
 }
 std::cout<<"ORIGINAL_HOST_PLAYER_SHADOW_PLAN_PASS: no original writes, isolated camera/actor/weapon/parts plan, safe scope/thread\n";
}
