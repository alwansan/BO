#include "room_original_guest_canonical_source_barrier.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace blackcoop;
struct ApplyIO {
 unsigned npcCalls=0,objectCalls=0;bool ready=false;
 uint64_t OriginalScene()const{return 0x100000u;}
 bool ResolveOriginalNpc(uint8_t slot,uint32_t& actor,uint64_t& gen){
  if(!ready)return false;
  actor=0x300000u+slot*0x1000u;gen=1;return true;
 }
 bool StillOwnsOriginalNpc(uint8_t,uint32_t,uint64_t){return true;}
 bool ApplyOriginalNpc(uint32_t,const OriginalNpcValue&){++npcCalls;return true;}
 bool VerifyOriginalNpc(uint32_t,const OriginalNpcValue&){return true;}
 bool ResolveOriginalObject(uint64_t,uint8_t,uint32_t& actor,uint64_t& gen){
  actor=0x400000u;gen=2;return true;
 }
 bool StillOwnsOriginalObject(uint64_t,uint8_t,uint32_t,uint64_t){return true;}
 bool ApplyOriginalHostState(uint32_t,uint8_t,uint8_t){++objectCalls;return true;}
 bool VerifyOriginalHostState(uint32_t,uint8_t,uint8_t){return true;}
};
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
 assert(!gate.Observe(key,0x100000,world,npc,1003)); // same packets cannot earn quorum
 avatars[1]=10;assert(world.Ingest(avatars,22,1004));
 assert(!gate.Observe(key,0x100000,world,npc,1005));
 avatars[1]=11;assert(world.Ingest(avatars,22,1006));
 assert(gate.Observe(key,0x100000,world,npc,1007));
 assert(gate.SourceCoherent(1007)&&!gate.OriginalGuestEngineApplied());
 assert(gate.MatchesForApplication(key,0x100000,world,npc,1007));
 {
  OriginalGuestCanonicalApplyFrame frame;OriginalGuestHostObjectInbox objects;ApplyIO io;
  int32_t event[]={23,6,2,99,1,7,0,123,1,1};
  assert(objects.Ingest(event,10,key));
  assert(frame.Tick(io,gate,key,0x200000,world,npc,objects,1007)==OriginalCanonicalApplyStatus::Rejected);
  assert(frame.Tick(io,gate,key,0x100000,world,npc,objects,1007)==OriginalCanonicalApplyStatus::Waiting);
  assert(io.npcCalls==0&&io.objectCalls==0&&objects.Pending()==1);
  io.ready=true;
  assert(frame.Tick(io,gate,key,0x100000,world,npc,objects,1007)==OriginalCanonicalApplyStatus::Applied);
  assert(io.npcCalls==1&&io.objectCalls==1&&objects.Pending()==0);
  assert(frame.Tick(io,gate,key,0x100000,world,npc,objects,1008)==OriginalCanonicalApplyStatus::Applied);
  assert(io.npcCalls==1&&io.objectCalls==1);
 }
 assert(!gate.MatchesForApplication(key,0x200000,world,npc,1007));
 auto badKey=key;++badKey.tokenLo;
 assert(!gate.MatchesForApplication(badKey,0x100000,world,npc,1007));
 assert(gate.Observe(key,0x100000,world,npc,1050)); // replay does not refresh proof
 assert(!gate.SourceCoherent(1108)); // last distinct host frame was at 1007
 int32_t unexpected[]={23,17,3,4,
  1,100,200,300,0,0,2,120,200,300,0,0,
  3,130,200,300,0,0,4,150,200,300,0,0};
 assert(world.Ingest(unexpected,28,1010));
 assert(world.PlayerMask(1010)==15u);
 assert(!gate.MatchesForApplication(key,0x100000,world,npc,1010));
 assert(!gate.Observe(key,0x100000,world,npc,1011)); // guest room mask 7
 avatars[1]=18;
 assert(world.Ingest(avatars,22,1020));
 assert(!gate.Observe(key,0x100000,world,npc,1021));
 assert(!gate.Observe(key,0x100000,world,npc,1022));
 assert(!gate.Observe(key,0x100000,world,npc,1023));
 avatars[1]=19;assert(world.Ingest(avatars,22,1024));
 assert(!gate.Observe(key,0x100000,world,npc,1025));
 avatars[1]=20;assert(world.Ingest(avatars,22,1026));
 assert(gate.Observe(key,0x100000,world,npc,1027));
 assert(gate.SourceCoherent(1027)&&!gate.SourceCoherent(1128));
 bool foreign=true;std::thread t([&]{
  foreign=gate.Observe(key,0x100000,world,npc,1003);
 });t.join();assert(!foreign);
 assert(gate.SourceCoherent(1028)); // foreign callers do not revoke quorum
 assert(!gate.Observe(key,0x100000,world,npc,1351));
 assert(!gate.SourceCoherent(1351));
 avatars[0]=24;avatars[1]=1;enemies[0]=24;enemies[1]=1;
 assert(world.Ingest(avatars,22,1400)&&npc.Ingest(enemies,9,1400));
 auto newer=key;newer.sceneEpoch=24;newer.tokenLo=100;
 assert(!gate.Observe(newer,0x200000,world,npc,1401));
 gate.Reset();
 assert(!gate.Observe(newer,0x200000,world,npc,1401));
 assert(!gate.Observe(newer,0x200000,world,npc,1402));
 assert(!gate.Observe(newer,0x200000,world,npc,1403));
 avatars[1]=2;assert(world.Ingest(avatars,22,1405));
 assert(!gate.Observe(newer,0x200000,world,npc,1406));
 avatars[1]=3;assert(world.Ingest(avatars,22,1407));
 assert(gate.Observe(newer,0x200000,world,npc,1408));
 const int32_t zero[]={0,0,0};assert(npc.Ingest(zero,3,1409));
 assert(!gate.Observe(newer,0x200000,world,npc,1409));
 // After expiry require three NEW host updates, not a single fresh packet.
 OriginalGuestHostWorldInbox lateWorld;OriginalGuestHostNpcInbox lateNpc;
 OriginalGuestCanonicalSourceBarrier lateGate;
 int32_t lateAvatars[]={23,30,3,3,1,100,200,300,0,0,2,120,200,300,0,0,3,130,200,300,0,0};
 int32_t lateEnemies[]={23,30,1,0,100,200,300,1000,0};
 assert(lateWorld.Ingest(lateAvatars,22,2000)&&lateNpc.Ingest(lateEnemies,9,2000));
 assert(!lateGate.Observe(key,0x100000,lateWorld,lateNpc,2001));
 lateAvatars[1]=31;assert(lateWorld.Ingest(lateAvatars,22,2002));
 assert(!lateGate.Observe(key,0x100000,lateWorld,lateNpc,2003));
 lateAvatars[1]=32;assert(lateWorld.Ingest(lateAvatars,22,2004));
 assert(lateGate.Observe(key,0x100000,lateWorld,lateNpc,2005));
 assert(!lateGate.SourceCoherent(2106));
 lateAvatars[1]=33;assert(lateWorld.Ingest(lateAvatars,22,2107));
 assert(!lateGate.Observe(key,0x100000,lateWorld,lateNpc,2108));
 lateAvatars[1]=34;assert(lateWorld.Ingest(lateAvatars,22,2109));
 assert(!lateGate.Observe(key,0x100000,lateWorld,lateNpc,2110));
 lateAvatars[1]=35;assert(lateWorld.Ingest(lateAvatars,22,2111));
 assert(lateGate.Observe(key,0x100000,lateWorld,lateNpc,2112));
 assert(lateGate.SourceCoherent(2112));
 // Changing the original episode/level/stage while retaining the signed
 // epoch and token must never inherit a previous local launch's quorum.
 for(int variant=0;variant<3;variant++){
  OriginalGuestCanonicalSourceBarrier exact;
  const uint64_t t=2120u+static_cast<uint64_t>(variant)*10u;
  assert(!exact.Observe(key,0x100000,lateWorld,lateNpc,t));
  ++lateAvatars[1];assert(lateWorld.Ingest(lateAvatars,22,t+1u));
  assert(!exact.Observe(key,0x100000,lateWorld,lateNpc,t+2u));
  ++lateAvatars[1];assert(lateWorld.Ingest(lateAvatars,22,t+3u));
  assert(exact.Observe(key,0x100000,lateWorld,lateNpc,t+4u));
  auto altered=key;
  if(variant==0)++altered.originalEpisode;
  if(variant==1)++altered.level;
  if(variant==2)++altered.stage;
  assert(!exact.Observe(altered,0x100000,lateWorld,lateNpc,t+5u));
  assert(!exact.SourceCoherent(t+5u));
 }
 std::cout<<"ORIGINAL_GUEST_HOST_NPC_AVATAR_BARRIER_PASS_NO_ENGINE_APPLY\n";
}
