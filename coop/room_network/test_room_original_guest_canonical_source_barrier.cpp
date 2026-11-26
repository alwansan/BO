#include "room_original_guest_canonical_source_barrier.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <vector>
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

// Integration fixture for the existing mod contracts only. These object keys,
// actor handles, scenes, generations and state values are synthetic. No BLACK
// assets, native callbacks, sockets or original AI execute in this fixture.
namespace synthetic_mission {
constexpr std::array<uint64_t,3> kKeys{101u,202u,303u};
constexpr std::array<uint8_t,3> kKinds{2u,3u,3u}; // activation + two objectives
using MissionState=std::array<uint8_t,3>;
using Change=std::pair<size_t,uint8_t>;
const std::vector<Change> kChanges{{0u,1u},{1u,1u},{2u,1u},{1u,2u},{2u,3u}};

bool SameNpc(const OriginalNpcValue& a,const OriginalNpcValue& b){
 return a.slot==b.slot&&a.flags==b.flags&&a.x==b.x&&a.y==b.y&&
        a.z==b.z&&a.healthCenti==b.healthCenti;
}

struct MissionIO {
 uint8_t self;
 uint64_t scene,generation=1;
 bool npcReady=true,objectReady=true,readback=true;
 unsigned npcCalls=0,objectCalls=0;
 MissionState state{};
 OriginalNpcValue npcState{};
 std::vector<Change> applied;
 std::vector<char> order;
 explicit MissionIO(uint8_t slot):self(slot),scene(0x100000u+slot*0x10000u){}
 uint64_t OriginalScene()const{return scene;}
 uint32_t NpcActor()const{return 0x300000u+self*0x10000u;}
 uint32_t ObjectActor(size_t index)const{
  return 0x400000u+self*0x10000u+static_cast<uint32_t>(index)*0x100u;
 }
 bool ResolveOriginalNpc(uint8_t slot,uint32_t& actor,uint64_t& gen){
  if(!npcReady||slot!=0)return false;
  actor=NpcActor();gen=generation;return true;
 }
 bool StillOwnsOriginalNpc(uint8_t slot,uint32_t actor,uint64_t gen)const{
  return npcReady&&slot==0&&actor==NpcActor()&&gen==generation;
 }
 bool ApplyOriginalNpc(uint32_t actor,const OriginalNpcValue& value){
  if(actor!=NpcActor())return false;
  npcState=value;++npcCalls;order.push_back('N');return true;
 }
 bool VerifyOriginalNpc(uint32_t actor,const OriginalNpcValue& value)const{
  return actor==NpcActor()&&SameNpc(npcState,value);
 }
 bool ResolveOriginalObject(uint64_t key,uint8_t kind,uint32_t& actor,uint64_t& gen){
  if(!objectReady)return false;
  for(size_t i=0;i<kKeys.size();++i){
   if(key==kKeys[i]&&kind==kKinds[i]){
    actor=ObjectActor(i);gen=generation;return true;
   }
  }
  return false;
 }
 bool StillOwnsOriginalObject(uint64_t key,uint8_t kind,uint32_t actor,uint64_t gen)const{
  if(!objectReady||gen!=generation)return false;
  for(size_t i=0;i<kKeys.size();++i)
   if(key==kKeys[i]&&kind==kKinds[i]&&actor==ObjectActor(i))return true;
  return false;
 }
 bool ApplyOriginalHostState(uint32_t actor,uint8_t kind,uint8_t value){
  for(size_t i=0;i<kKeys.size();++i){
   if(actor==ObjectActor(i)&&kind==kKinds[i]){
    state[i]=value;++objectCalls;applied.emplace_back(i,value);
    order.push_back('O');return true;
   }
  }
  return false;
 }
 bool VerifyOriginalHostState(uint32_t actor,uint8_t kind,uint8_t value)const{
  if(!readback)return false;
  for(size_t i=0;i<kKeys.size();++i)
   if(actor==ObjectActor(i)&&kind==kKinds[i])return state[i]==value;
  return false;
 }
 void SimulateEngineTeardown(){
  // This is a fixture action, never evidence of an actual native teardown.
  ++generation;state={};npcState={};npcReady=objectReady=readback=true;
 }
};

struct Guest {
 OriginalGuestHostWorldInbox world;
 OriginalGuestHostNpcInbox npc;
 OriginalGuestHostObjectInbox objects;
 OriginalGuestCanonicalSourceBarrier barrier;
 OriginalGuestCanonicalApplyFrame frame;
 OriginalGuestLaunchMatchKey key{};
 MissionIO io;
 uint8_t humans;
 uint64_t now=1000;
 // The production source has independent counters for these streams.
 // Deliberately use different ranges; do not invent a shared host clock.
 int32_t avatarTick=400,npcTick=20,eventSequence=0,eventTick=9000;
 Guest(uint8_t self,uint8_t count):io(self),humans(count){
  key.originalEpisode=100u+self;key.sceneEpoch=23;key.rosterRevision=6;
  key.tokenHi=2;key.tokenLo=99;key.selfSlot=self;
  key.participants=static_cast<uint8_t>((1u<<count)-1u);
 }
 bool ObserveNext(bool publishNpcs=true){
  now+=10;
  std::array<int32_t,28> avatars{};
  avatars[0]=static_cast<int32_t>(key.sceneEpoch);avatars[1]=++avatarTick;
  avatars[2]=key.selfSlot;avatars[3]=humans;
  for(uint8_t slot=1;slot<=humans;++slot){
   const size_t at=4u+static_cast<size_t>(slot-1u)*6u;
   avatars[at]=slot;avatars[at+1]=100*slot;
   avatars[at+2]=200;avatars[at+3]=300;
  }
  assert(world.Ingest(avatars.data(),4u+static_cast<size_t>(humans)*6u,now));
  if(publishNpcs){
   const int32_t enemies[]={static_cast<int32_t>(key.sceneEpoch),++npcTick,
                          1,0,111,222,333,1000,0};
   assert(npc.Ingest(enemies,9,now));
  }
  return barrier.Observe(key,io.scene,world,npc,now);
 }
 void EstablishSource(bool publishNpcs=true){
  assert(!ObserveNext(publishNpcs));assert(!ObserveNext(publishNpcs));
  assert(ObserveNext(publishNpcs));
 }
 void Enqueue(const Change& change){
  const int32_t event[]={static_cast<int32_t>(key.sceneEpoch),
   static_cast<int32_t>(key.rosterRevision),static_cast<int32_t>(key.tokenHi),
   static_cast<int32_t>(key.tokenLo),++eventSequence,++eventTick,0,
   static_cast<int32_t>(kKeys[change.first]),kKinds[change.first],change.second};
  assert(objects.Ingest(event,10,key));
 }
 OriginalCanonicalApplyStatus TickAt(uint64_t at){
  return frame.Tick(io,barrier,key,io.scene,world,npc,objects,at);
 }
 OriginalCanonicalApplyStatus Tick(){return TickAt(now);}
};

void ConvergesForAllGuests(uint8_t humans){
 std::array<std::unique_ptr<Guest>,3> guests;
 MissionState host{};
 for(uint8_t i=0;i<humans-1u;++i){
  guests[i]=std::make_unique<Guest>(static_cast<uint8_t>(i+2u),humans);
  guests[i]->EstablishSource();
 }
 for(const auto& change:kChanges){
  host[change.first]=change.second;
  for(uint8_t i=0;i<humans-1u;++i)guests[i]->Enqueue(change);
 }
 // Neither unresolved NPC identity nor unresolved mission identity may
 // consume an objective. Successful NPC application is not repeated.
 auto& delayed=*guests[0];
 delayed.io.npcReady=false;
 assert(delayed.Tick()==OriginalCanonicalApplyStatus::Waiting);
 assert(delayed.io.npcCalls==0&&delayed.io.objectCalls==0);
 assert(delayed.objects.Pending()==5&&delayed.io.state==MissionState{});
 delayed.io.npcReady=true;delayed.io.objectReady=false;
 assert(delayed.Tick()==OriginalCanonicalApplyStatus::Waiting);
 assert(delayed.Tick()==OriginalCanonicalApplyStatus::Waiting);
 assert(delayed.io.npcCalls==1&&delayed.io.objectCalls==0);
 assert(delayed.objects.Pending()==5&&delayed.io.state==MissionState{});
 delayed.io.objectReady=true;
 const MissionState afterFour{1u,2u,1u};
 for(uint8_t i=0;i<humans-1u;++i){
  auto& guest=*guests[i];
  assert(guest.io.state==MissionState{}&&guest.io.objectCalls==0);
  assert(guest.Tick()==OriginalCanonicalApplyStatus::Waiting);
  assert(guest.objects.Pending()==1&&guest.io.objectCalls==4);
  assert(guest.io.state==afterFour&&guest.io.order.size()==5);
  assert(guest.io.order.front()=='N');
  for(size_t at=1;at<guest.io.order.size();++at)assert(guest.io.order[at]=='O');
  assert(guest.Tick()==OriginalCanonicalApplyStatus::Applied);
  assert(guest.objects.Pending()==0&&guest.io.state==host);
  assert(guest.io.npcCalls==1&&guest.io.objectCalls==5&&guest.io.applied==kChanges);
  const OriginalNpcValue expectedNpc{0u,0u,111,222,333,1000};
  assert(SameNpc(guest.io.npcState,expectedNpc));
  assert(guest.Tick()==OriginalCanonicalApplyStatus::Applied);
  assert(guest.io.npcCalls==1&&guest.io.objectCalls==5&&guest.io.state==host);
  // Separate local instances/handles: processing one guest cannot update
  // an unprocessed guest. This is not proof of live player input isolation.
  for(uint8_t other=static_cast<uint8_t>(i+1u);other<humans-1u;++other){
   assert(guests[other]->io.state==MissionState{});
   assert(guests[other]->io.objectCalls==0&&guests[other]->objects.Pending()==5);
  }
 }
 std::cout<<"SYNTHETIC_MISSION_CONVERGENCE_PASS humans="<<unsigned(humans)<<'\n';
}

void WaitsForFreshSource(){
 Guest guest(2,2);guest.EstablishSource();guest.Enqueue({1u,2u});
 guest.now+=101;
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Rejected);
 assert(guest.io.npcCalls==0&&guest.io.objectCalls==0);
 assert(guest.io.state==MissionState{}&&guest.objects.Pending()==1);
 guest.EstablishSource();
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Applied);
 assert(guest.io.state[1]==2&&guest.io.objectCalls==1&&guest.objects.Pending()==0);
 std::cout<<"SYNTHETIC_MISSION_FRESHNESS_PASS\n";
}

void RequiresTeardownForNewScope(bool newMission){
 Guest guest(2,2);guest.EstablishSource();guest.Enqueue({1u,2u});
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Applied);
 guest.Enqueue({2u,3u});guest.io.objectReady=false;
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Waiting);
 assert(guest.objects.Pending()==1&&guest.io.objectCalls==1);
 const auto previousNpcTick=guest.npc.Tick();
 ++guest.key.rosterRevision;
 if(newMission){
  ++guest.key.tokenLo;
  ++guest.key.originalEpisode;++guest.key.sceneEpoch;
  ++guest.key.level;++guest.key.stage;guest.io.scene+=0x100000u;
 }
 guest.barrier.Reset();guest.EstablishSource(newMission);
 // Revision-only: same scene, token and NPC frame, with three fresh avatar
 // observations. Successful teardown must clear the prior NPC apply marker.
 if(!newMission)assert(guest.npc.Tick()==previousNpcTick);
 // Fully coherent NEW source must still be refused by the OLD apply frame.
 assert(guest.barrier.MatchesForApplication(guest.key,guest.io.scene,
                                         guest.world,guest.npc,guest.now));
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Quarantined);
 assert(guest.io.npcCalls==1&&guest.io.objectCalls==1&&guest.objects.Pending()==1);
 assert(!guest.frame.AfterVerifiedEngineTeardown(false));
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Quarantined);
 guest.io.SimulateEngineTeardown();
 assert(guest.frame.AfterVerifiedEngineTeardown(true));
 // There is deliberately NO new object packet yet: observing the new scope
 // alone must discard the old pending objective, before applying new NPCs.
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Applied);
 assert(guest.io.npcCalls==2&&guest.io.objectCalls==1);
 assert(guest.io.state==MissionState{}&&guest.objects.Pending()==0);
 guest.eventSequence=0;guest.eventTick=1;
 guest.Enqueue({0u,1u});
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Applied);
 const MissionState expected{1u,0u,0u};
 assert(guest.io.state==expected&&guest.io.objectCalls==2&&guest.objects.Pending()==0);
 assert(guest.io.applied.size()==2&&guest.io.applied.back()==Change(0u,1u));
 std::cout<<"SYNTHETIC_MISSION_SCOPE_TRANSITION_PASS kind="
          <<(newMission?"mission":"revision")<<'\n';
}

void StopsAfterUnconfirmedApplication(){
 Guest guest(2,2);guest.EstablishSource();
 guest.Enqueue({1u,1u});guest.Enqueue({2u,2u});guest.io.readback=false;
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Quarantined);
 // The first callback ran, but its result was not confirmed. Never issue
 // the second callback or blindly repeat the first one on the next frame.
 assert(guest.io.objectCalls==1&&guest.io.state[1]==1&&guest.io.state[2]==0);
 assert(guest.Tick()==OriginalCanonicalApplyStatus::Quarantined);
 assert(guest.io.objectCalls==1&&guest.io.applied.size()==1);
 std::cout<<"SYNTHETIC_MISSION_UNCONFIRMED_APPLY_PASS\n";
}

void Run(){
 for(uint8_t humans=2;humans<=4;++humans)ConvergesForAllGuests(humans);
 WaitsForFreshSource();
 RequiresTeardownForNewScope(false);RequiresTeardownForNewScope(true);
 StopsAfterUnconfirmedApplication();
 std::cout<<"SYNTHETIC_MISSION_SYNC_PASS scenarios=7 NO_ANDROID_ENGINE_OR_TLS\n";
}
} // namespace synthetic_mission

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
  // A new authenticated revision with the same native scene must not
  // reuse the apply batch's old per-tick success.
  auto revised=key;++revised.rosterRevision;
  OriginalGuestCanonicalSourceBarrier other;
  assert(!other.Observe(revised,0x100000,world,npc,1008));
  assert(!frame.AfterVerifiedEngineTeardown(false));
  assert(frame.AfterVerifiedEngineTeardown(true));
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
 synthetic_mission::Run();
 std::cout<<"ORIGINAL_GUEST_HOST_NPC_AVATAR_BARRIER_PASS_NO_ENGINE_APPLY\n";
}
