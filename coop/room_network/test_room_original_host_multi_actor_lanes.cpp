#include "room_original_host_multi_actor_lanes.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace blackcoop;
static OriginalHumanProof make(uint8_t peer,uint32_t start){
 OriginalHumanProof p{};
 p.peer=peer;p.frame=1;p.phase=OriginalHumanPhase::ConstructorOwned;
 p.resources={start,start+0x1000,start+0x2000,start+0x3000,
 start+0x4000,start+0x5000,start+0x6000,start+0x7000,
 start+0x8000,start+0x9000};
 p.nativeOwned=p.setupDone=p.proxyOwned=p.aiBound=true;
 p.animationOwned=p.audioOwned=p.weaponOwned=p.bodyVisible=true;
 p.engineTeardownProven=p.hostWorldAuthoritative=true;return p;
}
int main(){
 OriginalHostHumanLanes lanes;
 OriginalHumanHostScene scene{55,42,0x300000,0x300030,0x3a0000,0x400000,0x500000,15};
 assert(lanes.Bind(scene));
 for(uint8_t peer=2;peer<=4;peer++){
  auto p=make(peer,0x100000u+uint32_t(peer)*0x40000u);
  assert(lanes.Advance(p)&&!lanes.PadReady(peer,42));
  for(unsigned phase=2;phase<=5;phase++){
   p.phase=static_cast<OriginalHumanPhase>(phase);++p.frame;
   assert(lanes.Advance(p));
  }
  assert(lanes.PadReady(peer,42)&&!lanes.PadReady(peer,41));
 }
 assert(lanes.Progress(4)==5);
 assert(lanes.AllPadsReadyAtFrame(42,5));
 assert(!lanes.AllPadsReadyAtFrame(42,6)&&!lanes.AllPadsReadyAtFrame(41,5));
 auto refresh=make(4,0x200000u);
 refresh.phase=OriginalHumanPhase::Gameplay;refresh.frame=6;
 assert(lanes.Advance(refresh));
 assert(!lanes.AllPadsReadyAtFrame(42,5)&&!lanes.AllPadsReadyAtFrame(42,6));
 auto repeated=make(2,0x180000u);repeated.frame=4;
 assert(!lanes.Advance(repeated));
 auto changed=make(2,0x190000u);changed.frame=6;
 changed.phase=OriginalHumanPhase::Gameplay;
 assert(!lanes.Advance(changed)&&lanes.Quarantined());
 assert(!lanes.PadReady(2,42));
 assert(!lanes.ReleaseAfterEngineTeardown(false));
 bool foreign=true;std::thread worker([&]{
  foreign=lanes.ReleaseAfterEngineTeardown(true);
  assert(!lanes.PadReady(2,42));
 });worker.join();assert(!foreign);
 assert(lanes.ReleaseAfterEngineTeardown(true));
 scene.originalScene=56;scene.signedEpoch=43;scene.participants=3;
 assert(lanes.Bind(scene)&&!lanes.Advance(make(4,0x180000u)));
 std::cout<<"ORIGINAL_HOST_P2_P3_P4_ENGINE_LANE_STATE_PASS_NO_GUEST_ACTOR_YET\n";
}
