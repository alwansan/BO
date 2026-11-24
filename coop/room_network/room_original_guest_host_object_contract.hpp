// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Strict host-signed object-event inbox on one ORIGINAL BLACK guest game thread.
// Stores opaque, stable source object keys ONLY. No original game memory writes,
// physics, collision, AI/quests or claim that a door was actually destroyed.
#pragma once
#include "room_original_guest_launch_match_contract.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalHostObjectEvent {
 uint32_t sceneEpoch=0,revision=0,sequence=0,tick=0;
 uint64_t token=0,objectKey=0;
 uint8_t kind=0,state=0;
};
enum class OriginalObjectApplyResult:uint8_t { Empty, WaitingForIdentity, Applied, Quarantined, Rejected };
class OriginalGuestHostObjectInbox final {
 static constexpr size_t kCapacity=64u;
 std::array<OriginalHostObjectEvent,kCapacity> queue_{};
 std::thread::id gameThread_{};
 bool bound_=false,scoped_=false,poisoned_=false,revoked_=false;
 OriginalGuestLaunchMatchKey next_{};
 uint32_t epoch_=0,revision_=0,seq_=0,tick_=0,episode_=0;
 uint8_t level_=0,stage_=0,self_=0,participants_=0;
 uint64_t token_=0;
 size_t head_=0,count_=0;
 static bool sameKey(const OriginalGuestLaunchMatchKey& a,const OriginalGuestLaunchMatchKey& b)noexcept{
  return a.originalEpisode==b.originalEpisode&&a.sceneEpoch==b.sceneEpoch&&
   a.rosterRevision==b.rosterRevision&&a.tokenHi==b.tokenHi&&a.tokenLo==b.tokenLo&&
   a.level==b.level&&a.stage==b.stage&&a.selfSlot==b.selfSlot&&a.participants==b.participants;
 }
 static bool newer(uint32_t epoch,uint32_t revision,uint32_t oldEpoch,uint32_t oldRevision)noexcept{
  return epoch>oldEpoch||(epoch==oldEpoch&&revision>oldRevision);
 }
 bool owner()const noexcept{
  return !bound_||gameThread_==std::this_thread::get_id();
 }
public:
 void Reset()noexcept{
  if(!owner())return;
  queue_={};scoped_=poisoned_=revoked_=false;next_={};
  epoch_=revision_=seq_=tick_=episode_=0;token_=0;head_=count_=0;
  level_=stage_=self_=participants_=0;
 }
 // The local original launch can change even with no new object packets.
 // Revoke pending events for any mismatched launch. Same signed scope with
 // a different original identity is poisoned until explicit teardown/reset.
 bool CheckLocalLaunch(const OriginalGuestLaunchMatchKey& key)noexcept{
  if(!owner()||poisoned_)return false;
  if(!scoped_)return true;
  if(revoked_){
   if(sameKey(key,next_))return true;
   if(!newer(key.sceneEpoch,key.rosterRevision,next_.sceneEpoch,next_.rosterRevision))return false;
   next_=key;return true;
  }
  const uint64_t token=(uint64_t(key.tokenHi)<<32)|key.tokenLo;
  if(key.sceneEpoch!=epoch_||key.rosterRevision!=revision_||token!=token_){
   // Discard old pending values, but retain replay watermarks until a
   // strictly newer signed scope is accepted by Ingest.
   queue_={};head_=count_=0;
   if(!newer(key.sceneEpoch,key.rosterRevision,epoch_,revision_)){
    poisoned_=true;return false;
   }
   revoked_=true;next_=key;return true;
  }
  if(key.originalEpisode!=episode_||key.level!=level_||key.stage!=stage_||
     key.selfSlot!=self_||key.participants!=participants_){
   queue_={};head_=count_=0;poisoned_=true;return false;
  }
  return true;
 }
 // [epoch, revision, tokenHi, tokenLo, sequence, sourceTick,
 //  objectKeyHi, objectKeyLo, kind, state], exactly TEN signed JNI integers.
 // Expected key comes only from an independently matched ORIGINAL guest stage
 // and its signed HOST PREPARE. Java TLS has already checked the room session.
 bool Ingest(const int32_t* data,size_t n,
             const OriginalGuestLaunchMatchKey& matched)noexcept{
  if(!owner()||!data||n!=10||poisoned_)return false;
  if(!matched.originalEpisode||matched.sceneEpoch==0||
     matched.rosterRevision==0||matched.selfSlot<2||matched.selfSlot>4||
     (matched.participants!=3&&matched.participants!=7&&matched.participants!=15)||
     !(matched.participants&(1u<<(matched.selfSlot-1)))||
     (matched.tokenHi==0&&matched.tokenLo==0))return false;
  const uint64_t token=(static_cast<uint64_t>(matched.tokenHi)<<32)|
                       static_cast<uint64_t>(matched.tokenLo);
  if(token>0x7fffffffffffffffull)return false;
  const uint64_t fromWire=
       (static_cast<uint64_t>(static_cast<uint32_t>(data[2]))<<32)|
       static_cast<uint32_t>(data[3]);
  const uint64_t key=
       (static_cast<uint64_t>(static_cast<uint32_t>(data[6]))<<32)|
       static_cast<uint32_t>(data[7]);
  if(data[0]<=0||data[1]<=0||data[4]<=0||data[5]<=0||
     data[0]!=static_cast<int32_t>(matched.sceneEpoch)||
     static_cast<uint32_t>(data[1])!=matched.rosterRevision||
     fromWire!=token||!key||key>0x7fffffffffffffffull||
     data[8]<1||data[8]>3||data[9]<0||data[9]>3||
     (data[8]==1&&data[9]!=1))return false;
  const uint32_t seq=static_cast<uint32_t>(data[4]);
  const uint32_t tick=static_cast<uint32_t>(data[5]);
  // Older authenticated stories/revisions must never displace a newer
  // canonical guest object queue; token switches require an explicit reset.
  if(scoped_&&(matched.sceneEpoch<epoch_||
      (matched.sceneEpoch==epoch_&&matched.rosterRevision<revision_)||
      (matched.sceneEpoch==epoch_&&matched.rosterRevision==revision_&&
       token!=token_)))return false;
  if(revoked_&&!sameKey(matched,next_))return false;
  bool sameScope=scoped_&&
      epoch_==matched.sceneEpoch&&revision_==matched.rosterRevision&&
      token_==token;
  if(sameScope){
   if(!CheckLocalLaunch(matched))return false;
   if(seq_==0x7fffffffu||seq!=seq_+1u||tick<tick_){
    poisoned_=true;return false;
   }
  }else{
   // Different authenticated story: old changes never survive a restart.
   if(seq!=1u)return false; // invalid first packet cannot erase watermark
   Reset();
  }
  if(count_>=kCapacity){
   poisoned_=true;return false; // no silent drops on overflow
  }
  if(!bound_){gameThread_=std::this_thread::get_id();bound_=true;}
  scoped_=true;epoch_=matched.sceneEpoch;revision_=matched.rosterRevision;
  token_=token;seq_=seq;tick_=tick;episode_=matched.originalEpisode;
  level_=matched.level;stage_=matched.stage;self_=matched.selfSlot;
  participants_=matched.participants;
  queue_[(head_+count_)%kCapacity]={
    matched.sceneEpoch,matched.rosterRevision,seq,tick,
    token,key,static_cast<uint8_t>(data[8]),static_cast<uint8_t>(data[9])};
  ++count_;
  return true;
 }
 // Future engine object-identity matcher must call Take() on the SAME
 // original guest thread, and then map the stable key to a proven actor.
 bool Take(OriginalHostObjectEvent& output)noexcept{
  if(!owner()||!scoped_||poisoned_||!count_)return false;
  output=queue_[head_];
  queue_[head_]={};
  head_=(head_+1u)%kCapacity;--count_;return true;
 }
 // Transactional engine-side consumer. Do not pop until the exact live
 // original object is resolved and its authoritative state is read back.
 // IO must prove scene, stable identity, generation and real original
 // callback semantics; no production engine adapter exists yet.
 template<class IO>OriginalObjectApplyResult ApplyNext(IO& io,
     const OriginalGuestLaunchMatchKey& matched,uint64_t nativeScene){
  if(!owner()||!nativeScene)return OriginalObjectApplyResult::Rejected;
  if(poisoned_||!CheckLocalLaunch(matched))return OriginalObjectApplyResult::Quarantined;
  if(!count_)return OriginalObjectApplyResult::Empty;
  const auto event=queue_[head_];
  uint32_t actor=0;uint64_t generation=0;
  try{
   if(io.OriginalScene()!=nativeScene)return OriginalObjectApplyResult::Rejected;
   if(!io.ResolveOriginalObject(event.objectKey,event.kind,actor,generation))
    return OriginalObjectApplyResult::WaitingForIdentity;
   if(actor<0x10000u||!generation||
      !io.StillOwnsOriginalObject(event.objectKey,event.kind,actor,generation))
    return OriginalObjectApplyResult::WaitingForIdentity;
   // Once attempted, an uncertain mutation cannot be replayed blindly.
   if(!io.ApplyOriginalHostState(actor,event.kind,event.state)||
      io.OriginalScene()!=nativeScene||
      !io.StillOwnsOriginalObject(event.objectKey,event.kind,actor,generation)||
      !io.VerifyOriginalHostState(actor,event.kind,event.state)){
    poisoned_=true;return OriginalObjectApplyResult::Quarantined;
   }
  }catch(...){
   poisoned_=true;return OriginalObjectApplyResult::Quarantined;
  }
  queue_[head_]={};head_=(head_+1u)%kCapacity;--count_;
  return OriginalObjectApplyResult::Applied;
 }
 size_t Pending()const noexcept{return owner()&&!poisoned_?count_:0u;}
 bool Poisoned()const noexcept{return owner()&&poisoned_;}
};
} // namespace blackcoop
