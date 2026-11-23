// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Source-authentic ORIGINAL BLACK 0x001F1080 AFTER vanilla world tick.
// Pinned MIT PC b2512fafd3e181e4febfb298f297245b845ef628 uses exactly
// this post-update frame boundary for black_p2_tick. Read-only stage now.
// NOT a loaded-scene event, peer COMMIT, player-2 setup or shared AI proof.
#pragma once
#include <cstdint>
namespace blackcoop {
struct OriginalPostFrameSample {
 uint32_t world=0,owner=0,physics=0,npcPool=0;
 uint32_t corePhase=0,localKind=0;
};
class OriginalPostFrameGate final {
 OriginalPostFrameSample current_{};
 unsigned consecutive_=0;
 bool logged_=false;
 static bool same(const OriginalPostFrameSample& a,const OriginalPostFrameSample& b)noexcept{
  return a.world==b.world&&a.owner==b.owner&&
    a.physics==b.physics&&a.npcPool==b.npcPool;
 }
public:
 void Reset()noexcept{current_={};consecutive_=0;logged_=false;}
 // Returns true only ONCE after 30 consecutive genuine stable frames.
 bool Observe(const OriginalPostFrameSample& sample,bool originalPostTick)noexcept{
  if(!originalPostTick||sample.world<0x10000u||sample.owner<0x10000u||
     sample.physics<0x10000u||sample.npcPool<0x10000u||
     sample.corePhase!=0x1Cu||sample.localKind!=2u){
   Reset();return false;
  }
  if(!same(current_,sample)){
   current_=sample;consecutive_=0;logged_=false;
  }
  if(consecutive_<30u)++consecutive_;
  if(logged_||consecutive_<30u)return false;
  logged_=true;return true;
 }
 unsigned StableOriginalFrames()const noexcept{return consecutive_;}
 // Source candidate only; this never authorizes original native P2 setup.
 bool OriginalPlayer2Ready()const noexcept{return false;}
};
} // namespace blackcoop
