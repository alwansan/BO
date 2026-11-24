// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Three independent ORIGINAL P2/P3/P4 engine ownership lanes.
// Source-backed progress contract only, NOT an original actor factory.
// No guest writes, CallGuest, fake READY, or FreeSystem.
#pragma once
#include <array>
#include <cstdint>
#include <thread>
namespace blackcoop {
enum class OriginalHumanPhase:uint8_t{
 Absent=0,ConstructorOwned=1,SetupFinished=2,PhysicsAiBound=3,
 PresentationIndependent=4,Gameplay=5
};
struct OriginalHumanResources {
 uint32_t actor=0,owner=0,input=0,weapons=0,model=0,
 proxy=0,ai=0,animation=0,audio=0,body=0;
};
struct OriginalHumanProof {
 uint8_t peer=0;OriginalHumanPhase phase=OriginalHumanPhase::Absent;
 uint32_t frame=0;OriginalHumanResources resources{};
 bool nativeOwned=false,setupDone=false,proxyOwned=false,aiBound=false,
 animationOwned=false,audioOwned=false,weaponOwned=false,bodyVisible=false,
 engineTeardownProven=false,hostWorldAuthoritative=false;
};
struct OriginalHumanHostScene {
 uint64_t originalScene=0;uint32_t signedEpoch=0;
 uint32_t world=0,p1Actor=0,p1Owner=0,physics=0,npcPool=0;
 uint8_t participants=0;
};
class OriginalHostHumanLanes final {
 const std::thread::id thread_=std::this_thread::get_id();
 OriginalHumanHostScene scope_{};
 std::array<OriginalHumanProof,3> slots_{};
 bool bound_=false,quarantined_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 static bool addr(uint32_t v)noexcept{return v>=0x10000u&&v<0xfffffff0u;}
 static std::array<uint32_t,10> values(const OriginalHumanResources& r)noexcept{
  return {r.actor,r.owner,r.input,r.weapons,r.model,r.proxy,r.ai,r.animation,r.audio,r.body};
 }
 bool trustworthy(const OriginalHumanProof& p)const noexcept{
  if(p.peer<2||p.peer>4||!p.frame||!p.nativeOwned||
     p.phase<OriginalHumanPhase::ConstructorOwned||
     p.phase>OriginalHumanPhase::Gameplay)return false;
  const auto v=values(p.resources);
  for(unsigned i=0;i<5;i++)if(!addr(v[i]))return false;
  for(unsigned i=0;i<v.size();i++)if(v[i]){
   if(!addr(v[i])||v[i]==scope_.world||v[i]==scope_.p1Actor||
      v[i]==scope_.p1Owner||v[i]==scope_.physics||v[i]==scope_.npcPool)return false;
   for(unsigned j=0;j<i;j++)if(v[i]==v[j])return false;
   for(const auto& other:slots_)if(other.peer&&other.peer!=p.peer)
    for(uint32_t w:values(other.resources))if(w&&w==v[i])return false;
  }
  if(p.phase>=OriginalHumanPhase::SetupFinished&&!p.setupDone)return false;
  if(p.phase>=OriginalHumanPhase::PhysicsAiBound&&
     (!addr(p.resources.proxy)||!addr(p.resources.ai)||!p.proxyOwned||!p.aiBound))
    return false;
  if(p.phase>=OriginalHumanPhase::PresentationIndependent&&
     (!addr(p.resources.animation)||!addr(p.resources.audio)||!addr(p.resources.body)||
      !p.animationOwned||!p.audioOwned||!p.weaponOwned||!p.bodyVisible))
    return false;
  return p.phase<OriginalHumanPhase::Gameplay||
     (p.engineTeardownProven&&p.hostWorldAuthoritative);
 }
public:
 // Bind only from an independently identified original game scene and
 // verified authenticated host story. Room membership alone is insufficient.
 bool Bind(const OriginalHumanHostScene& s)noexcept{
  if(!owner()||quarantined_||!s.originalScene||!s.signedEpoch||
     !addr(s.world)||!addr(s.p1Actor)||!addr(s.p1Owner)||
     !addr(s.physics)||!addr(s.npcPool)||
     (s.participants!=3&&s.participants!=7&&s.participants!=15))return false;
  if(bound_){
   const auto& b=scope_;
   if(b.originalScene!=s.originalScene||b.signedEpoch!=s.signedEpoch||
      b.world!=s.world||b.p1Actor!=s.p1Actor||b.p1Owner!=s.p1Owner||
      b.physics!=s.physics||b.npcPool!=s.npcPool||
      b.participants!=s.participants){
    quarantined_=true;return false; // no implicit guest heap teardown
   }
   return true;
  }
  scope_=s;bound_=true;return true;
 }
 bool Advance(const OriginalHumanProof& p)noexcept{
  if(!owner()||!bound_||quarantined_||p.peer<2||p.peer>4||
     !(scope_.participants&(1u<<(p.peer-1)))||!trustworthy(p))return false;
  auto& old=slots_[p.peer-2];
  if(old.peer){
   if(p.frame<=old.frame||p.phase<old.phase||
      static_cast<unsigned>(p.phase)>static_cast<unsigned>(old.phase)+1u)return false;
   const auto a=values(p.resources),b=values(old.resources);
   for(unsigned i=0;i<a.size();i++)if(b[i]&&b[i]!=a[i]){
    quarantined_=true;return false; // address reused or aliased in-place
   }
  }else if(p.phase!=OriginalHumanPhase::ConstructorOwned)return false;
  old=p;return true;
 }
 bool PadReady(uint8_t peer,uint32_t epoch)const noexcept{
  if(!owner()||!bound_||quarantined_||peer<2||peer>4||
     epoch!=scope_.signedEpoch)return false;
  const auto& p=slots_[peer-2];
  return p.peer==peer&&p.phase==OriginalHumanPhase::Gameplay&&trustworthy(p);
 }
 // Runtime routing must additionally prove every participating lane was
 // independently observed at this ORIGINAL frame. A historic Gameplay
 // proof alone is not authorization to forward current input.
 bool AllPadsReadyAtFrame(uint32_t epoch,uint32_t frame)const noexcept{
  if(!owner()||!bound_||quarantined_||!frame||epoch!=scope_.signedEpoch)return false;
  for(uint8_t peer=2;peer<=4;peer++)if(scope_.participants&(1u<<(peer-1)))
   if(!PadReady(peer,epoch)||slots_[peer-2].frame!=frame)return false;
  return true;
 }
 unsigned Progress(uint8_t peer)const noexcept{
  return owner()&&peer>=2&&peer<=4?
    static_cast<unsigned>(slots_[peer-2].phase):0;
 }
 bool Quarantined()const noexcept{return owner()&&quarantined_;}
 void Quarantine()noexcept{if(owner())quarantined_=true;}
 // Allowed only after independent proof of full ORIGINAL engine scene
 // destruction (not room leave); no guest frees occur in this object.
 bool ReleaseAfterEngineTeardown(bool verified)noexcept{
  if(!owner()||!verified)return false;
  scope_={};slots_={};bound_=quarantined_=false;return true;
 }
};
} // namespace blackcoop
