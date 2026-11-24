// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Read-only guest values from authenticated HOST's REAL original NPC pool.
// Does NOT replace guest engine AI/health/objectives or prove a shared world.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalNpcValue{
 uint8_t slot=255u,flags=0;
 int32_t x=0,y=0,z=0,healthCenti=0;
};
class OriginalGuestHostNpcInbox final {
 std::array<OriginalNpcValue,32> npcs_{};
 uint32_t epoch_=0,tick_=0;
 uint64_t receivedAt_=0;
 std::thread::id sourceThread_{};
 bool bound_=false;
 bool owner()const noexcept{
  return !bound_||sourceThread_==std::this_thread::get_id();
 }
public:
 void Clear()noexcept{
  if(!owner())return;
  npcs_={};epoch_=tick_=0;receivedAt_=0;
 }
 // [epoch,tick,count,(original host slot,xCm,yCm,zCm,hpCenti,dead)].
 // [0,0,0] immediately revokes an old trusted NPC world.
 bool Ingest(const int32_t* f,size_t n,uint64_t now)noexcept{
  if(!owner()||!f||n<3||n>195)return false;
  if(n==3&&f[0]==0&&f[1]==0&&f[2]==0){
   Clear();return true;
  }
  if(f[0]<=0||f[1]<=0||f[2]<0||f[2]>32||
     n!=3u+static_cast<size_t>(f[2])*6u||!now)return false;
  if(epoch_&&static_cast<uint32_t>(f[0])<epoch_)return false;
  if(receivedAt_&&now<receivedAt_)return false;
  std::array<OriginalNpcValue,32> next{};
  for(auto& e:next)e.slot=255u;
  int prev=-1;
  for(int32_t i=0;i<f[2];i++){
   size_t k=3u+static_cast<size_t>(i)*6u;
   int32_t slot=f[k];
   if(slot<=prev||slot<0||slot>=32||
      f[k+1]<-20000000||f[k+1]>20000000||
      f[k+2]<-20000000||f[k+2]>20000000||
      f[k+3]<-20000000||f[k+3]>20000000||
      f[k+4]<0||f[k+4]>1000000||
      f[k+5]<0||f[k+5]>1)return false;
   prev=slot;
   next[static_cast<size_t>(slot)]={
    static_cast<uint8_t>(slot),static_cast<uint8_t>(f[k+5]),
    f[k+1],f[k+2],f[k+3],f[k+4]};
  }
  if(epoch_==static_cast<uint32_t>(f[0])&&
     static_cast<uint32_t>(f[1])<=tick_)return false;
  if(!bound_){sourceThread_=std::this_thread::get_id();bound_=true;}
  npcs_=next;epoch_=static_cast<uint32_t>(f[0]);
  tick_=static_cast<uint32_t>(f[1]);receivedAt_=now;
  return true;
 }
 bool Read(uint8_t slot,uint64_t now,OriginalNpcValue& out)const noexcept{
  if(!owner()||!bound_||!epoch_||!tick_||!receivedAt_||
     slot>=32||npcs_[slot].slot!=slot||
     now<receivedAt_||now-receivedAt_>350)return false;
  out=npcs_[slot];return true;
 }
 uint32_t Tick()const noexcept{return owner()?tick_:0;}
 uint32_t Epoch()const noexcept{return owner()?epoch_:0;}
 bool Fresh(uint64_t now)const noexcept{
  return owner()&&bound_&&epoch_&&tick_&&receivedAt_&&
         now>=receivedAt_&&now-receivedAt_<=350u;
 }
};
// A host slot is not an original object identity. IO must independently
// corroborate the stable original identity and lifetime before mutation.
// No production resolver or original callback is enabled.
enum class OriginalNpcApplyStatus:uint8_t {
 Rejected, AlreadyApplied, WaitingForIdentity, Applied, Quarantined
};
class OriginalGuestNpcApplyBatch final {
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t scene_=0;uint32_t epoch_=0,lastTick_=0;
 bool poisoned_=false;
 struct Target {OriginalNpcValue value{};uint32_t actor=0;uint64_t generation=0;};
public:
 template<class IO>OriginalNpcApplyStatus Apply(IO& io,const OriginalGuestHostNpcInbox& inbox,
    uint64_t nativeScene,uint32_t signedEpoch,uint64_t now){
  if(thread_!=std::this_thread::get_id())return OriginalNpcApplyStatus::Rejected;
  if(poisoned_)return OriginalNpcApplyStatus::Quarantined;
  if(!nativeScene||!signedEpoch||inbox.Epoch()!=signedEpoch||!inbox.Fresh(now))
   return OriginalNpcApplyStatus::Rejected;
  if(scene_&&(scene_!=nativeScene||epoch_!=signedEpoch)){
   poisoned_=true;return OriginalNpcApplyStatus::Quarantined;
  }
  const auto tick=inbox.Tick();
  if(tick<=lastTick_)return OriginalNpcApplyStatus::AlreadyApplied;
  std::array<Target,32> targets{};unsigned count=0;
  bool attempted=false;
  try{
   if(io.OriginalScene()!=nativeScene)return OriginalNpcApplyStatus::Rejected;
   for(uint8_t slot=0;slot<32;++slot){
    OriginalNpcValue value{};
    if(!inbox.Read(slot,now,value))continue;
    Target target{};target.value=value;
    if(!io.ResolveOriginalNpc(slot,target.actor,target.generation)||
       target.actor<0x10000u||!target.generation||
       !io.StillOwnsOriginalNpc(slot,target.actor,target.generation))
     return OriginalNpcApplyStatus::WaitingForIdentity;
    for(unsigned j=0;j<count;++j)
     if(targets[j].actor==target.actor)return OriginalNpcApplyStatus::WaitingForIdentity;
    targets[count++]=target;
   }
   if(io.OriginalScene()!=nativeScene)return OriginalNpcApplyStatus::Rejected;
   for(unsigned i=0;i<count;++i){
    const auto& t=targets[i];
    if(!io.StillOwnsOriginalNpc(t.value.slot,t.actor,t.generation))
     return OriginalNpcApplyStatus::WaitingForIdentity;
   }
   for(unsigned i=0;i<count;++i){
    const auto& t=targets[i];
    if(io.OriginalScene()!=nativeScene||
       !io.StillOwnsOriginalNpc(t.value.slot,t.actor,t.generation)){
     if(!attempted)return OriginalNpcApplyStatus::WaitingForIdentity;
     poisoned_=true;return OriginalNpcApplyStatus::Quarantined;
    }
    attempted=true;
    if(!io.ApplyOriginalNpc(t.actor,t.value)||
       io.OriginalScene()!=nativeScene||
       !io.StillOwnsOriginalNpc(t.value.slot,t.actor,t.generation)||
       !io.VerifyOriginalNpc(t.actor,t.value)){
     poisoned_=true;return OriginalNpcApplyStatus::Quarantined;
    }
   }
  }catch(...){
   if(attempted){poisoned_=true;return OriginalNpcApplyStatus::Quarantined;}
   return OriginalNpcApplyStatus::Rejected;
  }
  scene_=nativeScene;epoch_=signedEpoch;lastTick_=tick;
  return OriginalNpcApplyStatus::Applied;
 }
 bool AfterVerifiedEngineTeardown(bool verified)noexcept{
  if(thread_!=std::this_thread::get_id()||!verified)return false;
  scene_=0;epoch_=lastTick_=0;poisoned_=false;return true;
 }
};
}
