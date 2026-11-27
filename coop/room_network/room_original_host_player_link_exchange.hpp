// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Source-faithful BLACK world-owned P1 grid links and opt-in actor-only exchange.
// Native use requires a separately proven exact actor mapping and generation IO.
// No original game callback, camera/animation swap or production activation.
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
 struct NeighborWitness {uint32_t at=0,next=0,previous=0;};
 std::array<NeighborWitness,kWords> witnesses_{};
 unsigned witnessCount_=0;
 OriginalPlayerStorageScene scene_{};
 uint32_t source_=0,target_=0;
 bool ready_=false,entered_=false,poisoned_=false;
 uint64_t sourceGeneration_=0,targetGeneration_=0;
 unsigned dynamic_=0;
 bool sameThread()const noexcept{return thread_==std::this_thread::get_id();}
 template<class IO>bool neighborLinksCurrent(IO& io)const noexcept{
  for(unsigned i=0;i<witnessCount_;i++){
   const auto& w=witnesses_[i];
   uint32_t next=0,previous=0;
   if(!range(w.at,8u)||!io.Span(w.at,8u)||
      !io.Read32(w.at,next)||!io.Read32(w.at+4u,previous)||
      next!=w.next||previous!=w.previous)return false;
  }
  return true;
 }
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
  if(!sameThread()||entered_||poisoned_)return;
  first_.fill(0);second_.fill(0);protected_.fill(0);
  witnesses_.fill({});witnessCount_=0;
  scene_={};source_=target_=0;dynamic_=0;ready_=false;
  sourceGeneration_=targetGeneration_=0;
 }
 ~OriginalPlayerLinkExchangePlan(){Reset();}
 bool Ready()const noexcept{return sameThread()&&ready_&&!entered_&&!poisoned_;}
 bool Entered()const noexcept{return sameThread()&&entered_;}
 bool Quarantined()const noexcept{return sameThread()&&poisoned_;}
 unsigned WorldNodeWords()const noexcept{return Ready()?14u:0u;}
 unsigned DynamicLinkWords()const noexcept{return Ready()?dynamic_:0u;}
 bool ProtectedWord(uint32_t offset)const noexcept{
   return Ready()&&!(offset&3u)&&offset<kBytes&&protected_[offset/4u];
 }
 bool ExchangedWord(uint32_t address)const noexcept{
   const uint32_t offset=address-source_;
   return Entered()&&!poisoned_&&offset<kBytes&&!(offset&3u)&&!protected_[offset/4u];
 }
 template<class IO>bool Prepare(IO& io,const OriginalPlayerStorageScene& b,
                                 uint32_t independentlyOwnedP2Actor)noexcept{
  if(!sameThread()||entered_||poisoned_)return false;
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
   // Also pin unprotected eligible neighbor contents: if another engine
   // subsystem relinks this list after mask creation, the mask is stale.
   if(witnessCount_>=witnesses_.size()){Reset();return false;}
   witnesses_[witnessCount_++]={v,next,prev};
   if(next==original+off||prev==original+off||
      (off>=4u&&(next==original+off-4u||prev==original+off-4u))){
    protected_[off/4u]=1u;++dynamic_;
   }
  }
  // Do not trust a source that moved while link neighbors were inspected.
  std::array<uint8_t,kBytes> check{};
  if(!roots(io,b)||!io.OwnsActor(p2,kBytes)||
     !neighborLinksCurrent(io)||
     !io.CopyGuest(original,check.data(),kBytes)||
     std::memcmp(check.data(),first_.data(),kBytes)!=0||
     !io.CopyGuest(p2,check.data(),kBytes)||
     std::memcmp(check.data(),second_.data(),kBytes)!=0){
   Reset();return false;
  }
  scene_=b;source_=original;target_=p2;ready_=true;return true;
 }
 template<class IO>bool SourceStillCurrent(IO& io)const noexcept{
  if(!Ready()||!roots(io,scene_)||!io.OwnsActor(target_,kBytes)||
     !neighborLinksCurrent(io))return false;
  std::array<uint8_t,kBytes> current{};
  return io.CopyGuest(source_,current.data(),kBytes)&&
   std::memcmp(current.data(),first_.data(),kBytes)==0&&
   io.CopyGuest(target_,current.data(),kBytes)&&
   std::memcmp(current.data(),second_.data(),kBytes)==0;
 }
 // IO must establish ORIGINAL allocation provenance and generation for the
 // exact embedded P1 and separately owned P2 spans. Committed pages alone
 // are insufficient. All mapping operations are on one original game thread.
 // This primitive swaps actor words ONLY; the caller must coordinate the
 // independently owned camera, animation, input, audio and weapon contexts.
 template<class IO>bool EnterActorOnly(IO& io,uint32_t firstInput=0,
      uint32_t extraInput=0,uint32_t firstPad=0,uint32_t extraPad=0)noexcept{
  // Pinned black_p2_update replaces exact donor input/pad aliases in
  // the extra human actor. Never write original world-owned links.
  const bool rebind=firstInput||extraInput||firstPad||extraPad;
  if(rebind&&(!range(firstInput,0x40u)||!range(extraInput,0x40u)||
     !range(firstPad,0x120u)||!range(extraPad,0x120u)||
     firstInput==extraInput||firstPad==extraPad||
     firstInput==firstPad||firstInput==extraPad||
     extraInput==firstPad||extraInput==extraPad))return false;
  if(!SourceStillCurrent(io))return false;
  const auto sg=io.ActorGeneration(source_,kBytes);
  const auto tg=io.ActorGeneration(target_,kBytes);
  if(!sg||!tg)return false;
  auto* a=io.MapOriginalActor(source_,kBytes);
  auto* b=io.MapOriginalActor(target_,kBytes);
  if(!a||!b||!disjointMappings(a,b)||!SourceStillCurrent(io)||
     io.ActorGeneration(source_,kBytes)!=sg||
     io.ActorGeneration(target_,kBytes)!=tg||
     std::memcmp(a,first_.data(),kBytes)!=0||
     std::memcmp(b,second_.data(),kBytes)!=0)return false;
  if(rebind)for(unsigned i=0;i<kWords;i++)if(protected_[i]){
   const auto v=word(a+4u*i);
   if(v==firstInput||v==firstPad)return false;
  }
  sourceGeneration_=sg;targetGeneration_=tg;
  swapUnprotected(a,b);
  // Only active extra actor words are changed; inactive P1 is untouched.
  if(rebind)for(unsigned i=0;i<kWords;i++)if(!protected_[i]){
   const auto v=word(a+4u*i);
   const auto next=v==firstInput?extraInput:v==firstPad?extraPad:v;
   if(next!=v)std::memcpy(a+4u*i,&next,4u);
  }
  entered_=true;
  return true;
 }
 // The external camera/owner context must already have been restored so
 // roots() sees the original P1 owner. The world-owned grid words are
 // never touched. A changed witness, generation or corrupted inactive P1
 // makes restoration unsafe; quarantine rather than guess or free.
 template<class IO>bool RestoreActorOnly(IO& io)noexcept{
  if(!sameThread()||!entered_||poisoned_)return false;
  if(!roots(io,scene_)||!io.OwnsActor(target_,kBytes)||
     !neighborLinksCurrent(io)||
     io.ActorGeneration(source_,kBytes)!=sourceGeneration_||
     io.ActorGeneration(target_,kBytes)!=targetGeneration_){
   poisoned_=true;return false;
  }
  auto* a=io.MapOriginalActor(source_,kBytes);
  auto* b=io.MapOriginalActor(target_,kBytes);
  if(!a||!b||!disjointMappings(a,b)||
     io.ActorGeneration(source_,kBytes)!=sourceGeneration_||
     io.ActorGeneration(target_,kBytes)!=targetGeneration_){
   poisoned_=true;return false;
  }
  for(unsigned i=0;i<kWords;i++)if(!protected_[i]&&
      std::memcmp(b+4u*i,first_.data()+4u*i,4u)!=0){
   poisoned_=true;return false;
  }
  swapUnprotected(a,b);
  entered_=false;ready_=false;
  return true;
 }
private:
 static bool disjointMappings(const uint8_t* a,const uint8_t* b)noexcept{
  const auto x=reinterpret_cast<uintptr_t>(a),y=reinterpret_cast<uintptr_t>(b);
  return x<y?y-x>=kBytes:x-y>=kBytes;
 }
 void swapUnprotected(uint8_t* a,uint8_t* b)const noexcept{
  for(unsigned i=0;i<kWords;i++)if(!protected_[i]){
   uint8_t w[4];std::memcpy(w,a+4u*i,4u);
   std::memcpy(a+4u*i,b+4u*i,4u);
   std::memcpy(b+4u*i,w,4u);
  }
 }
public:
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
// Pinned PC black_p2_swap also exchanges the camera-owner singleton
// 0x2D12B8. This scope models that separate step, after actor entry and
// before actor restoration. No game callback or animation/audio activation.
// IO must prove both actual original camera allocation lifetimes and the
// fixed singleton word, not merely committed pages.
class OriginalCameraOwnerScope final {
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t scene_=0,p1Gen_=0,extraGen_=0;
 uint32_t first_=0,extra_=0;
 bool entered_=false,poisoned_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 template<class IO>bool live(IO& io)const noexcept{
  return owner()&&scene_&&io.CameraScene()==scene_&&
    io.CameraGeneration(first_,0x16C0u)==p1Gen_&&p1Gen_&&
    io.CameraGeneration(extra_,0x16C0u)==extraGen_&&extraGen_;
 }
public:
 OriginalCameraOwnerScope()=default;
 OriginalCameraOwnerScope(const OriginalCameraOwnerScope&)=delete;
 OriginalCameraOwnerScope& operator=(const OriginalCameraOwnerScope&)=delete;
 bool Entered()const noexcept{return owner()&&entered_;}
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
 template<class IO>bool Enter(IO& io,uint64_t scene,uint32_t p1,uint32_t extra)noexcept{
  if(!owner()||entered_||poisoned_||!scene||p1<0x10000u||extra<0x10000u||
    uint64_t(p1)+0x16C0u>0x100000000ull||
    uint64_t(extra)+0x16C0u>0x100000000ull||
    (uint64_t(p1)<uint64_t(extra)+0x16C0u&&uint64_t(extra)<uint64_t(p1)+0x16C0u))return false;
  scene_=scene;first_=p1;extra_=extra;
  p1Gen_=io.CameraGeneration(p1,0x16C0u);
  extraGen_=io.CameraGeneration(extra,0x16C0u);
  if(!live(io)||!io.OwnsExtraCamera(extra,0x16C0u))return false;
  auto* root=io.MapCameraOwnerRoot();
  uint32_t current=0;
  if(!root||!live(io))return false;
  std::memcpy(&current,root,4u);
  if(current!=first_)return false;
  std::memcpy(root,&extra_,4u);entered_=true;return true;
 }
 template<class IO>bool Restore(IO& io)noexcept{
  if(!owner()||!entered_||poisoned_)return false;
  if(!live(io)||!io.OwnsExtraCamera(extra_,0x16C0u)){
   poisoned_=true;return false;
  }
  auto* root=io.MapCameraOwnerRoot();uint32_t current=0;
  if(!root||!live(io)){poisoned_=true;return false;}
  std::memcpy(&current,root,4u);
  if(current!=extra_){poisoned_=true;return false;}
  std::memcpy(root,&first_,4u);
  entered_=false;return true;
 }
};
} // namespace blackcoop
