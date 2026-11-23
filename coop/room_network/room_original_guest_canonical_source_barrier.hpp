// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Parallel original GUEST story track: match signed launch and HOST
// human + NPC frames within one original local native scene. Values only.
// NO direct guest AI/object/door writes or fake campaign COMMIT.
#pragma once
#include "room_original_guest_host_world_contract.hpp"
#include "room_original_guest_host_npc_contract.hpp"
#include "room_original_guest_launch_match_contract.hpp"
#include <cstdint>
#include <thread>
namespace blackcoop {
class OriginalGuestCanonicalSourceBarrier final {
 const std::thread::id thread_=std::this_thread::get_id();
 uint32_t epoch_=0,revision_=0,worldTick_=0,npcTick_=0;
 uint64_t token_=0,originalScene_=0;
 unsigned stable_=0;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
public:
 void Reset()noexcept{
  if(!owner())return;
  epoch_=revision_=worldTick_=npcTick_=0;
  token_=originalScene_=0;stable_=0;
 }
 bool Observe(const OriginalGuestLaunchMatchKey& key,uint64_t nativeScene,
              const OriginalGuestHostWorldInbox& players,
              const OriginalGuestHostNpcInbox& npcs,uint64_t now)noexcept{
  if(!owner()||!nativeScene||!key.originalEpisode||!key.sceneEpoch||
     !key.rosterRevision||(key.tokenHi==0&&key.tokenLo==0)||
     key.selfSlot<2||key.selfSlot>4||
     (key.participants!=3&&key.participants!=7&&key.participants!=15)||
     !(key.participants&(1u<<(key.selfSlot-1)))||
     players.Scene()!=key.sceneEpoch||players.Self()!=key.selfSlot||
     npcs.Epoch()!=key.sceneEpoch||
     !players.Fresh(now)||!npcs.Fresh(now)){
    stable_=0;return false;
  }
  const uint64_t token=(uint64_t(key.tokenHi)<<32)|key.tokenLo;
  if(token>0x7fffffffffffffffull)return false;
  if(epoch_){
   if(key.sceneEpoch<epoch_||(key.sceneEpoch==epoch_&&
      key.rosterRevision<revision_))return false;
   if(key.sceneEpoch!=epoch_||key.rosterRevision!=revision_||
      token!=token_||nativeScene!=originalScene_){
    Reset();return false; // new original engine requires explicit scene rebind
   }
  }
  for(uint8_t peer=1;peer<=4;peer++){
   if(!(key.participants&(1u<<(peer-1))))continue;
   if(!players.HasPlayer(peer,now)){stable_=0;return false;}
  }
  if(players.Tick()<worldTick_||npcs.Tick()<npcTick_){
   stable_=0;return false;
  }
  epoch_=key.sceneEpoch;revision_=key.rosterRevision;
  token_=token;originalScene_=nativeScene;
  worldTick_=players.Tick();npcTick_=npcs.Tick();
  if(stable_<3u)++stable_;
  return stable_>=3u;
 }
 bool SourceCoherent()const noexcept{return owner()&&stable_>=3u;}
 // Source state is NOT equivalent to applying native NPCs, doors or quests.
 bool OriginalGuestEngineApplied()const noexcept{return false;}
};
} // namespace blackcoop
