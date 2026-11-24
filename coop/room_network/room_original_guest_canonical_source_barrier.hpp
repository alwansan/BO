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
 uint32_t episode_=0,epoch_=0,revision_=0,worldTick_=0,npcTick_=0;
 uint8_t level_=0,stage_=0,self_=0,participants_=0;
 uint64_t token_=0,originalScene_=0,lastObservedAt_=0;
 unsigned stable_=0;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
public:
 void Reset()noexcept{
  if(!owner())return;
  episode_=epoch_=revision_=worldTick_=npcTick_=0;
  level_=stage_=self_=participants_=0;
  token_=originalScene_=lastObservedAt_=0;stable_=0;
 }
 bool Observe(const OriginalGuestLaunchMatchKey& key,uint64_t nativeScene,
              const OriginalGuestHostWorldInbox& players,
              const OriginalGuestHostNpcInbox& npcs,uint64_t now)noexcept{
  // A foreign caller must not mutate game-thread-owned quorum.
  if(!owner())return false;
  if(!nativeScene||!key.originalEpisode||!key.sceneEpoch||
     !key.rosterRevision||(key.tokenHi==0&&key.tokenLo==0)||
     key.selfSlot<2||key.selfSlot>4||
     (key.participants!=3&&key.participants!=7&&key.participants!=15)||
     !(key.participants&(1u<<(key.selfSlot-1)))||
     players.Scene()!=key.sceneEpoch||players.Self()!=key.selfSlot||
     npcs.Epoch()!=key.sceneEpoch||
     !players.Fresh(now)||!npcs.Fresh(now)){
    stable_=0;lastObservedAt_=0;return false;
  }
  const uint64_t token=(uint64_t(key.tokenHi)<<32)|key.tokenLo;
  if(token>0x7fffffffffffffffull){
   stable_=0;lastObservedAt_=0;return false;
  }
  if(epoch_){
   if(key.sceneEpoch<epoch_||(key.sceneEpoch==epoch_&&
      key.rosterRevision<revision_)){
    stable_=0;lastObservedAt_=0;return false;
   }
   if(key.originalEpisode!=episode_||key.level!=level_||
      key.stage!=stage_||key.selfSlot!=self_||key.participants!=participants_||
      key.sceneEpoch!=epoch_||key.rosterRevision!=revision_||
      token!=token_||nativeScene!=originalScene_){
    Reset();return false; // new original engine requires explicit scene rebind
   }
  }
  if(players.PlayerMask(now)!=key.participants){
   stable_=0;lastObservedAt_=0;return false;
  }
  if(players.Tick()<worldTick_||npcs.Tick()<npcTick_){
   stable_=0;lastObservedAt_=0;return false;
  }
  // Expired quorum cannot be revived by one fresh packet after a long gap.
  if(lastObservedAt_ && (now<lastObservedAt_ || now-lastObservedAt_>100u))stable_=0;
  // Three confirmations require three distinct authenticated host frames,
  // not three render callbacks replaying the same packet pair.
  const bool advanced=players.Tick()>worldTick_||npcs.Tick()>npcTick_;
  episode_=key.originalEpisode;epoch_=key.sceneEpoch;revision_=key.rosterRevision;
  level_=key.level;stage_=key.stage;self_=key.selfSlot;participants_=key.participants;
  token_=token;originalScene_=nativeScene;
  worldTick_=players.Tick();npcTick_=npcs.Tick();
  // Replaying the same authenticated frame must not renew freshness.
  if(advanced)lastObservedAt_=now;
  if(advanced&&stable_<3u)++stable_;
  return stable_>=3u;
 }
 // A previous accepted source cannot grant eternal readiness when game
 // thread observation stops or network updates disappear.
 bool SourceCoherent(uint64_t now)const noexcept{
  return owner()&&stable_>=3u&&lastObservedAt_&&now>=lastObservedAt_&&
         now-lastObservedAt_<=100u;
 }
 // Source state is NOT equivalent to applying native NPCs, doors or quests.
 bool OriginalGuestEngineApplied()const noexcept{return false;}
};
} // namespace blackcoop
