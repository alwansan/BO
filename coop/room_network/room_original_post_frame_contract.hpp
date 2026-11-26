// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Source-authentic ORIGINAL BLACK 0x001F1080 AFTER vanilla world tick.
// Pinned MIT PC b2512fafd3e181e4febfb298f297245b845ef628 uses exactly
// this post-update frame boundary for black_p2_tick. Read-only stage now.
// NOT a loaded-scene event, peer COMMIT, player-2 setup or shared AI proof.
#pragma once
#include "room_original_running_scene_source_gate.hpp"
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalPostFrameSample {
 uint32_t world=0,owner=0,physics=0,npcPool=0;
 uint32_t corePhase=0,unloadPhase=0,localKind=0,localVtable=0;
};
class OriginalPostFrameGate final {
 const std::thread::id thread_=std::this_thread::get_id();
 bool onThread()const noexcept{return thread_==std::this_thread::get_id();}
 OriginalPostFrameSample current_{};
 unsigned consecutive_=0;
 bool logged_=false;
 static bool same(const OriginalPostFrameSample& a,const OriginalPostFrameSample& b)noexcept{
  return a.world==b.world&&a.owner==b.owner&&
    a.physics==b.physics&&a.npcPool==b.npcPool;
 }
public:
 void Reset()noexcept{if(!onThread())return;current_={};consecutive_=0;logged_=false;}
 // Returns true only ONCE after 30 consecutive genuine stable frames.
 bool Observe(const OriginalPostFrameSample& sample,bool originalPostTick)noexcept{
  if(!onThread()||!originalPostTick||sample.world<0x10000u||sample.owner<0x10000u||
     sample.physics<0x10000u||sample.npcPool<0x10000u||
     !IsOriginalWorldRunning(sample.corePhase,sample.unloadPhase)||sample.localKind!=2u||
     sample.localVtable!=0x0028CFE0u){
   Reset();return false;
  }
  if(!same(current_,sample)){
   current_=sample;consecutive_=0;logged_=false;
  }
  if(consecutive_<30u)++consecutive_;
  if(logged_||consecutive_<30u)return false;
  logged_=true;return true;
 }
 unsigned StableOriginalFrames()const noexcept{return onThread()?consecutive_:0u;}
 // Source candidate only; this never authorizes original native P2 setup.
 bool OriginalPlayer2Ready()const noexcept{return false;}
};
} // namespace blackcoop
