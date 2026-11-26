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
// Read-only port of the Windows lab's unit+0x14 membership check. It exposes
// no addresses, ordinals, or lifetime handles to the caller.
template<class Memory>
bool HasOriginalWorldUnitMembership(const Memory& memory, uint32_t world) noexcept {
 constexpr uint32_t kWorldRoot=0x002D45D4u, kRecordStride=0x30u, kObjectStride=0xF0u;
 if ((world&3u)||!memory.Contains(kWorldRoot,4u)||!memory.Contains(world,0x5CB8u))
   return false;
 const auto running=[&] {
   return memory.Read32(kWorldRoot)==world &&
     IsOriginalWorldRunning(memory.Read32(world+0x5AE0u),memory.Read32(world+0x5AE4u));
 };
 if(!running())return false;

 const auto hasMembers=[&](uint32_t unit) {
   if(memory.Read32(unit)!=0x1Cu||memory.Read8(unit+0x38u)!=1u)return false;
   const uint32_t index=memory.Read32(unit+8u);
   if(index==UINT32_MAX||index!=memory.Read32(unit+4u))return false;
   const uint32_t header=memory.Read32(unit+0xCu), objects=memory.Read32(unit+0x14u);
   if((header&3u)||!memory.Contains(header,0x92u))return false;
   if(memory.Read32(header)!=24u)return false;
   const uint32_t count=memory.Read16(header+0x90u), records=memory.Read32(header+0x1Cu);
   if(!count||count>1024u||(objects&3u)||(records&3u)||
      !memory.Contains(objects,count*kObjectStride)||!memory.Contains(records,count*kRecordStride))
     return false;
   for(uint32_t ordinal=0;ordinal<count;++ordinal){
     const uint32_t object=objects+ordinal*kObjectStride;
     if(memory.Read32(object)!=0x28EA60u||
        memory.Read32(object+0xD0u)!=records+ordinal*kRecordStride)return false;
   }
   uint32_t node=memory.Read32(world+0x5CB4u), visited=0, matches=0;
   while(node){
     if(++visited>4096u||(node&3u)||!memory.Contains(node,0xB4u))return false;
     if(node>=objects&&node-objects<count*kObjectStride){
       if((node-objects)%kObjectStride)return false;
       ++matches;
     }
     node=memory.Read32(node+0xB0u);
   }
   return matches==count;
 };
 return (hasMembers(world+0x49D0u)||hasMembers(world+0x5250u))&&running();
}

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
