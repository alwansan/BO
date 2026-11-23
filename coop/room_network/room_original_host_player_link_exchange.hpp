// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// A SOURCE-FAITHFUL READ-ONLY preview of BLACK's world-owned P1 grid links.
// NEVER performs a guest write, original game callback or context exchange.
#pragma once
#include "room_original_host_player_storage.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <thread>
namespace blackcoop {
class OriginalPlayerLinkExchangePlan final {
 static constexpr uint32_t kBytes=0x900u,kWords=kBytes/4u;
 const std::thread::id thread_=std::this_thread::get_id();
 std::array<uint8_t,kBytes> first_{},second_{};
 std::array<uint8_t,kWords> protected_{};
 OriginalPlayerStorageScene scene_{};
 uint32_t source_=0,target_=0;
 bool ready_=false;
 unsigned dynamic_=0;
 bool sameThread()const noexcept{return thread_==std::this_thread::get_id();}
 static bool range(uint32_t at,uint32_t n)noexcept{
  return at>=0x10000u&&n&&uint64_t(at)+n<=0x100000000ull;
 }
 static bool overlap(uint32_t a,uint32_t n,uint32_t b,uint32_t m)noexcept{
  return uint64_t(a)<uint64_t(b)+m&&uint64_t(b)<uint64_t(a)+n;
 }
 static uint32_t word(const uint8_t* p)noexcept{
  uint32_t v=0;std::memcpy(&v,p,4u);return v;
 }
 template<class IO>static bool roots(IO& io,const OriginalPlayerStorageScene& b)noexcept{
  uint32_t c=0,o=0,p=0,n=0;
  return b.scene&&io.Scene()==b.scene&&
   range(b.core,0x5CC0u)&&range(b.owner,0x16C0u)&&
   range(b.physics,0x2974u)&&range(b.npcs,0x8540u)&&
   io.Span(b.core,0x5CC0u)&&io.Span(b.owner,0x16C0u)&&
   io.Span(b.physics,0x2974u)&&io.Span(b.npcs,0x8540u)&&
   io.Read32(0x002D45D4u,c)&&c==b.core&&
   io.Read32(0x002D12B8u,o)&&o==b.owner&&
   io.Read32(0x002D45F4u,p)&&p==b.physics&&
   io.Read32(0x002D1980u,n)&&n==b.npcs;
 }
public:
 OriginalPlayerLinkExchangePlan()=default;
 OriginalPlayerLinkExchangePlan(const OriginalPlayerLinkExchangePlan&)=delete;
 OriginalPlayerLinkExchangePlan& operator=(const OriginalPlayerLinkExchangePlan&)=delete;
 void Reset()noexcept{
  if(!sameThread())return;
  first_.fill(0);second_.fill(0);protected_.fill(0);
  scene_={};source_=target_=0;dynamic_=0;ready_=false;
 }
 ~OriginalPlayerLinkExchangePlan(){Reset();}
 bool Ready()const noexcept{return sameThread()&&ready_;}
 unsigned WorldNodeWords()const noexcept{return Ready()?14u:0u;}
 unsigned DynamicLinkWords()const noexcept{return Ready()?dynamic_:0u;}
 bool ProtectedWord(uint32_t offset)const noexcept{
  return Ready()&&!(offset&3u)&&offset<kBytes&&protected_[offset/4u];
 }
 template<class IO>bool Prepare(IO& io,const OriginalPlayerStorageScene& b,
                                 uint32_t independentlyOwnedP2Actor)noexcept{
  if(!sameThread())return false;
  Reset();
  if(!roots(io,b)||b.core>0xfffffff0u-0x30u)return false;
  const uint32_t original=b.core+0x30u,p2=independentlyOwnedP2Actor;
  if(!range(original,kBytes)||!range(p2,kBytes)||
     (p2&15u)||overlap(original,kBytes,p2,kBytes)||
     overlap(p2,kBytes,b.core,0x5CC0u)||
     overlap(p2,kBytes,b.owner,0x16C0u)||
     overlap(p2,kBytes,b.physics,0x2974u)||
     overlap(p2,kBytes,b.npcs,0x8540u)||
     !io.OwnsActor(p2,kBytes)||!io.Span(p2,kBytes))return false;
  if(!io.CopyGuest(original,first_.data(),kBytes)||
     !io.CopyGuest(p2,second_.data(),kBytes)){Reset();return false;}
  // Pinned MIT upstream black_p2_find_links: 0x20..0x57 belong to
  // original world spatial lists, never to the replaceable player state.
  for(uint32_t off=0x20u;off<0x58u;off+=4u)
   protected_[off/4u]=1u;
  for(uint32_t off=0;off<kBytes;off+=4u){
   if(protected_[off/4u])continue;
   const uint32_t v=word(first_.data()+off);
   // Exactly follow the upstream unsigned-address self alias check.
   if(uint32_t(v-original)<kBytes||(v&3u)||!range(v,8u)||
      !io.Span(v,8u))continue;
   uint32_t next=0,prev=0;
   if(!io.Read32(v,next)||!io.Read32(v+4u,prev)){Reset();return false;}
   if(next==original+off||prev==original+off||
      (off>=4u&&(next==original+off-4u||prev==original+off-4u))){
    protected_[off/4u]=1u;++dynamic_;
   }
  }
  // Do not trust a source that moved while link neighbors were inspected.
  std::array<uint8_t,kBytes> check{};
  if(!roots(io,b)||!io.OwnsActor(p2,kBytes)||
     !io.CopyGuest(original,check.data(),kBytes)||
     std::memcmp(check.data(),first_.data(),kBytes)!=0||
     !io.CopyGuest(p2,check.data(),kBytes)||
     std::memcmp(check.data(),second_.data(),kBytes)!=0){
   Reset();return false;
  }
  scene_=b;source_=original;target_=p2;ready_=true;return true;
 }
 template<class IO>bool SourceStillCurrent(IO& io)const noexcept{
  if(!Ready()||!roots(io,scene_)||!io.OwnsActor(target_,kBytes))return false;
  std::array<uint8_t,kBytes> current{};
  return io.CopyGuest(source_,current.data(),kBytes)&&
   std::memcmp(current.data(),first_.data(),kBytes)==0&&
   io.CopyGuest(target_,current.data(),kBytes)&&
   std::memcmp(current.data(),second_.data(),kBytes)==0;
 }
 // Synthetic, offline snapshot of what P1/P2 would contain on ENTER.
 // Never expose raw source words to logs or JNI in production.
 bool PreviewEnterWord(uint32_t offset,uint32_t& core,uint32_t& shadow)const noexcept{
  if(!Ready()||(offset&3u)||offset>=kBytes)return false;
  core=word((protected_[offset/4u]?first_:second_).data()+offset);
  shadow=word((protected_[offset/4u]?second_:first_).data()+offset);
  return true;
 }
 // Full 2-way bit-accurate non-guest simulation, including protected
 // dynamic links, proving only reversibility of this mask, NOT safety of
 // camera/animation/audio/weapons or actual in-game actor switching.
 bool PreviewRoundTrip()const noexcept{
  if(!Ready())return false;
  auto first=first_,second=second_;
  for(unsigned pass=0;pass<2;pass++)
   for(unsigned i=0;i<kWords;i++)if(!protected_[i]){
    uint8_t word4[4];
    std::memcpy(word4,first.data()+4u*i,4);
    std::memcpy(first.data()+4u*i,second.data()+4u*i,4);
    std::memcpy(second.data()+4u*i,word4,4);
   }
  return first==first_&&second==second_;
 }
};
} // namespace blackcoop
