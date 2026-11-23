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
class OriginalGuestHostObjectInbox final {
 static constexpr size_t kCapacity=64u;
 std::array<OriginalHostObjectEvent,kCapacity> queue_{};
 std::thread::id gameThread_{};
 bool bound_=false,scoped_=false,poisoned_=false;
 uint32_t epoch_=0,revision_=0,seq_=0,tick_=0;
 uint64_t token_=0;
 size_t head_=0,count_=0;
 bool owner()const noexcept{
  return !bound_||gameThread_==std::this_thread::get_id();
 }
public:
 void Reset()noexcept{
  if(!owner())return;
  queue_={};scoped_=poisoned_=false;
  epoch_=revision_=seq_=tick_=0;token_=0;head_=count_=0;
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
  bool sameScope=scoped_&&
      epoch_==matched.sceneEpoch&&revision_==matched.rosterRevision&&
      token_==token;
  if(sameScope){
   if(seq_==0x7fffffffu||seq!=seq_+1u||tick<tick_){
    poisoned_=true;return false;
   }
  }else{
   // Different authenticated story: old changes never survive a restart.
   Reset();
   if(seq!=1u)return false;
  }
  if(count_>=kCapacity){
   poisoned_=true;return false; // no silent drops on overflow
  }
  if(!bound_){gameThread_=std::this_thread::get_id();bound_=true;}
  scoped_=true;epoch_=matched.sceneEpoch;revision_=matched.rosterRevision;
  token_=token;seq_=seq;tick_=tick;
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
 size_t Pending()const noexcept{return owner()&&!poisoned_?count_:0u;}
 bool Poisoned()const noexcept{return owner()&&poisoned_;}
};
} // namespace blackcoop
