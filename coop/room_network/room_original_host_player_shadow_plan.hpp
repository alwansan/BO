// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original BLACK P2 actor/owner source snapshot and independent shadow plan.
// READ ONLY game RAM; never creates/sets up actor, calls guest, swaps P1,
// edits original AI/physics, or certifies playable co-op.
#pragma once
#include "room_original_host_player_storage.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <thread>
namespace blackcoop {
class OriginalHostPlayerShadowPlan final {
 std::thread::id thread_=std::this_thread::get_id();
 std::array<uint8_t,0x900> actor_{};
 std::array<uint8_t,0x16C0> owner_{};
 std::array<uint8_t,0x430> parts_{};
 bool ready_=false;
 static bool range(uint32_t p,uint32_t n)noexcept{
  return p>=0x10000u&&n&&uint64_t(p)+n<=0x100000000ull;
 }
 static bool overlap(uint32_t p,uint32_t n,uint32_t q,uint32_t m)noexcept{
  return uint64_t(p)<uint64_t(q)+m&&uint64_t(q)<uint64_t(p)+n;
 }
 static uint32_t get(const uint8_t* p)noexcept{
  uint32_t v=0;std::memcpy(&v,p,4);return v;
 }
 static void put(uint8_t* p,uint32_t v)noexcept{std::memcpy(p,&v,4);}
 bool onThread()const noexcept{return thread_==std::this_thread::get_id();}
public:
 OriginalHostPlayerShadowPlan()=default;
 OriginalHostPlayerShadowPlan(const OriginalHostPlayerShadowPlan&)=delete;
 OriginalHostPlayerShadowPlan& operator=(const OriginalHostPlayerShadowPlan&)=delete;
 ~OriginalHostPlayerShadowPlan(){if(onThread())Reset();}
 void Reset()noexcept{
  if(!onThread())return;
  actor_.fill(0);owner_.fill(0);parts_.fill(0);ready_=false;
 }
 bool Ready()const noexcept{return onThread()&&ready_;}
 // Preview only already-sanitized stage data; never output it to JNI/logs.
 bool PreviewActorWord(uint32_t off,uint32_t& v)const noexcept{
  if(!Ready()||off>0x8FCu||(off&3u))return false;
  v=get(actor_.data()+off);return true;
 }
 bool PreviewOwnerWord(uint32_t off,uint32_t& v)const noexcept{
  if(!Ready()||off>0x16BCu||(off&3u))return false;
  v=get(owner_.data()+off);return true;
 }
 template<class IO>bool Prepare(IO& io,const OriginalPlayerStorageScene& b,
                                const OriginalPlayerStorageView& t)noexcept{
  if(!onThread())return false;
  Reset();
  if(!b.scene||io.Scene()!=b.scene||!range(b.core,0x5CC0)||!range(b.owner,0x16C0)||
     !range(b.physics,0x2974)||!range(b.npcs,0x8540)||
     !range(t.owner,0x16C0)||!range(t.actor,0x900)||
     !range(t.input,0x40)||!range(t.weapons,0x40)||
     !range(t.parts,0x630)||t.partsBytes!=0x630)return false;
  const uint32_t targets[]={t.owner,t.actor,t.input,t.weapons,t.parts};
  const uint32_t sizes[]={0x16C0,0x900,0x40,0x40,0x630};
  const uint32_t roots[]={b.core,b.owner,b.physics,b.npcs};
  const uint32_t rootSizes[]={0x5CC0,0x16C0,0x2974,0x8540};
  for(unsigned i=0;i<5;i++){
   if(!io.Owns(targets[i],sizes[i])||!io.Span(targets[i],sizes[i]))return false;
   for(unsigned j=0;j<i;j++)
    if(overlap(targets[i],sizes[i],targets[j],sizes[j]))return false;
   for(unsigned j=0;j<4;j++)
    if(overlap(targets[i],sizes[i],roots[j],rootSizes[j]))return false;
  }
  uint32_t core=0,owner=0,physics=0,npcs=0;
  if(!io.Read32(0x2D45D4,core)||core!=b.core||
     !io.Read32(0x2D12B8,owner)||owner!=b.owner||
     !io.Read32(0x2D45F4,physics)||physics!=b.physics||
     !io.Read32(0x2D1980,npcs)||npcs!=b.npcs||
     !io.Span(b.core,0x5CC0)||!io.Span(b.owner,0x16C0)||
     !io.Span(b.physics,0x2974)||!io.Span(b.npcs,0x8540))return false;
  const uint32_t local=b.core+0x30u;
  uint32_t table=0,kind=0,partA=0,partB=0;
  if(!io.Read32(local,table)||table!=0x28CFE0u||
     !io.Read32(local+0xC4,kind)||kind!=2u||
     !io.Read32(local+0x34C,partA)||!io.Read32(local+0x350,partB)||
     !range(partA,0x70)||!range(partB,0x3C0)||
     !io.Span(partA,0x70)||!io.Span(partB,0x3C0))return false;
  for(unsigned i=0;i<5;i++)
   if(overlap(partA,0x70,targets[i],sizes[i])||
      overlap(partB,0x3C0,targets[i],sizes[i]))return false;
  if(!io.CopyGuest(local,actor_.data(),0x900)||
     !io.CopyGuest(b.owner,owner_.data(),0x16C0)||
     !io.CopyGuest(partA,parts_.data(),0x70)||
     !io.CopyGuest(partB,parts_.data()+0x70,0x3C0)){
   Reset();return false;
  }
  // Pinned PC black_p2_start fields, isolated in HOST-only bytes.
  put(actor_.data()+0x30,0);
  put(actor_.data()+0x34,t.actor+0x34);
  put(actor_.data()+0x38,t.actor+0x34);
  put(actor_.data()+0xB4,0); // P1's collision proxy is never inherited
  put(actor_.data()+0x298,t.weapons);
  put(actor_.data()+0x29C,0);put(actor_.data()+0x2A0,0);
  actor_[0x2BB]=0xFF;
  put(actor_.data()+0x34C,t.parts);
  put(actor_.data()+0x350,t.parts+0x70);
  put(actor_.data()+0x8E4,1);put(actor_.data()+0x8E8,0);
  const uint64_t lo=b.owner,hi=lo+0x16C0;
  for(uint32_t off=0;off<0x16C0;off+=4){
   const uint32_t v=get(owner_.data()+off);
   if(uint64_t(v)>=lo&&uint64_t(v)<hi)
    put(owner_.data()+off,t.owner+(v-b.owner));
  }
  for(uint32_t off=0;off<0x900;off+=4){
   const uint32_t v=get(actor_.data()+off);
   if(uint64_t(v)>=lo&&uint64_t(v)<hi)
    put(actor_.data()+off,t.owner+(v-b.owner));
  }
  uint32_t sameWorld=0,sameOwner=0;
  if(io.Scene()!=b.scene||!io.Read32(0x2D45D4,sameWorld)||sameWorld!=b.core||
     !io.Read32(0x2D12B8,sameOwner)||sameOwner!=b.owner){
   Reset();return false;
  }
  ready_=true;return true;
 }
};
} // namespace blackcoop
