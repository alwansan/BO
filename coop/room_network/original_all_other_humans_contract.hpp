// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Canonical HOST-owner body positions for ALL OTHER humans (2–4 total)
// on one ORIGINAL BLACK game thread. Value-only; not engine actors, collision,
// original NPC AI, independent weapons, or shared objective authority.
#pragma once
#include "original_other_human_body_source.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>
namespace blackcoop {
class OriginalAllOtherHumansSource final {
 std::array<OriginalOtherHumanBody,5> peer_{};
 std::thread::id gameThread_{};
 bool bound_=false;
 uint32_t epoch_=0,revision_=0,tick_=0;
 uint8_t self_=0;
 uint64_t receivedAtMs_=0;
 bool owner()const noexcept{
  return !bound_||gameThread_==std::this_thread::get_id();
 }
public:
 void Reset()noexcept {
  if(!owner())return;
  peer_={};epoch_=revision_=tick_=0;self_=0;receivedAtMs_=0;
 }
 // [epoch, revision, tick, selfSlot, otherCount,
 //  peerSlot, xCm, yCm, zCm, yawCentideg, flags] * 1..3.
 // [0]*5 is the mandatory fail-closed tombstone after disconnect/abort.
 bool Ingest(const int32_t* f,size_t n,uint64_t now)noexcept {
  if(!owner()||!f)return false;
  if(n==5){
   bool zero=true;for(size_t i=0;i<5;i++)zero&=f[i]==0;
   if(zero){Reset();return true;}
   return false;
  }
  if(n<11||n>23||!now||f[0]<=0||f[1]<=0||f[2]<=0||
     f[3]<1||f[3]>4||f[4]<1||f[4]>3||
     n!=5u+static_cast<size_t>(f[4])*6u)
   return false;
  const unsigned total=static_cast<unsigned>(f[4])+1u;
  if(static_cast<unsigned>(f[3])>total)return false;
  std::array<OriginalOtherHumanBody,5> incoming{};
  unsigned mask=1u<<static_cast<unsigned>(f[3]-1);
  int prev=0;
  for(int i=0;i<f[4];i++){
   const size_t at=5u+static_cast<size_t>(i)*6u;
   const int32_t id=f[at];
   if(id<=prev||id<1||static_cast<unsigned>(id)>total||
      id==f[3]||f[at+1]<-20000000||f[at+1]>20000000||
      f[at+2]<-20000000||f[at+2]>20000000||
      f[at+3]<-20000000||f[at+3]>20000000||
      f[at+4]<-18000||f[at+4]>18000||
      f[at+5]<0||f[at+5]>3)return false;
   prev=id;mask|=1u<<static_cast<unsigned>(id-1);
   OriginalOtherHumanBody v{};
   v.epoch=static_cast<uint32_t>(f[0]);
   v.revision=static_cast<uint32_t>(f[1]);
   v.tick=static_cast<uint32_t>(f[2]);
   v.self=static_cast<uint8_t>(f[3]);
   v.peer=static_cast<uint8_t>(id);
   v.x=f[at+1];v.y=f[at+2];v.z=f[at+3];
   v.yaw=static_cast<int16_t>(f[at+4]);
   v.flags=static_cast<uint8_t>(f[at+5]);
   incoming[static_cast<size_t>(id)]=v;
  }
  if(mask!=(1u<<total)-1u)return false;
  if(bound_&&epoch_==static_cast<uint32_t>(f[0])&&
     revision_==static_cast<uint32_t>(f[1])&&
     self_==static_cast<uint8_t>(f[3])&&
     (static_cast<uint32_t>(f[2])<=tick_||now<receivedAtMs_))
    return false;
  if(!bound_){bound_=true;gameThread_=std::this_thread::get_id();}
  peer_=incoming;
  epoch_=static_cast<uint32_t>(f[0]);
  revision_=static_cast<uint32_t>(f[1]);
  tick_=static_cast<uint32_t>(f[2]);
  self_=static_cast<uint8_t>(f[3]);receivedAtMs_=now;
  return true;
 }
 bool Read(uint8_t peer,uint64_t now,OriginalOtherHumanBody& out)const noexcept {
  if(!owner()||!bound_||!epoch_||!tick_||!receivedAtMs_||
     peer<1||peer>4||peer==self_||peer_[peer].peer!=peer||
     now<receivedAtMs_||now-receivedAtMs_>350)return false;
  out=peer_[peer];return true;
 }
 uint8_t Self()const noexcept{return owner()?self_:0;}
};
}
