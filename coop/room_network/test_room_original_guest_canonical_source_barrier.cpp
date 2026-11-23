#include "room_original_guest_canonical_source_barrier.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace blackcoop;
int main(){
 OriginalGuestHostWorldInbox world;OriginalGuestHostNpcInbox npc;
 OriginalGuestCanonicalSourceBarrier gate;
 OriginalGuestLaunchMatchKey key{};
 key.originalEpisode=2;key.sceneEpoch=23;key.rosterRevision=6;
 key.selfSlot=3;key.participants=7;key.tokenHi=2;key.tokenLo=99;
 int32_t avatars[]={23,9,3,3,
 1,100,200,300,0,0,2,120,200,300,0,0,3,130,200,300,0,0};
 int32_t enemies[]={23,7,1,0,100,200,300,1000,0};
 assert(world.Ingest(avatars,22,1000)&&npc.Ingest(enemies,9,1000));
 assert(!gate.Observe(key,0,world,npc,1001));
 assert(!gate.Observe(key,0x100000,world,npc,1001));
 assert(!gate.Observe(key,0x100000,world,npc,1002));
 assert(gate.Observe(key,0x100000,world,npc,1003));
 assert(gate.SourceCoherent(1003)&&!gate.OriginalGuestEngineApplied());
 assert(!gate.SourceCoherent(1104)); // no perpetual source readiness
 int32_t unexpected[]={23,10,3,4,
  1,100,200,300,0,0,2,120,200,300,0,0,
  3,130,200,300,0,0,4,150,200,300,0,0};
 assert(world.Ingest(unexpected,28,1010));
 assert(world.PlayerMask(1010)==15u);
 assert(!gate.Observe(key,0x100000,world,npc,1011)); // guest room mask 7
 avatars[1]=11;
 assert(world.Ingest(avatars,22,1020));
 assert(!gate.Observe(key,0x100000,world,npc,1021));
 assert(!gate.Observe(key,0x100000,world,npc,1022));
 assert(gate.Observe(key,0x100000,world,npc,1023));
 assert(gate.SourceCoherent(1023)&&!gate.SourceCoherent(1124));
 bool foreign=true;std::thread t([&]{
  foreign=gate.Observe(key,0x100000,world,npc,1003);
 });t.join();assert(!foreign);
 assert(!gate.Observe(key,0x100000,world,npc,1351));
 assert(!gate.SourceCoherent(1351));
 avatars[0]=24;avatars[1]=1;enemies[0]=24;enemies[1]=1;
 assert(world.Ingest(avatars,22,1400)&&npc.Ingest(enemies,9,1400));
 auto newer=key;newer.sceneEpoch=24;newer.tokenLo=100;
 assert(!gate.Observe(newer,0x200000,world,npc,1401));
 gate.Reset();
 assert(!gate.Observe(newer,0x200000,world,npc,1401));
 assert(!gate.Observe(newer,0x200000,world,npc,1402));
 assert(gate.Observe(newer,0x200000,world,npc,1403));
 const int32_t zero[]={0,0,0};assert(npc.Ingest(zero,3,1404));
 assert(!gate.Observe(newer,0x200000,world,npc,1404));
 std::cout<<"ORIGINAL_GUEST_HOST_NPC_AVATAR_BARRIER_PASS_NO_ENGINE_APPLY\n";
}
