#include "room_original_post_frame_contract.hpp"
#include <cassert>
#include <iostream>
using namespace blackcoop;
int main(){
 OriginalPostFrameGate gate;
 const OriginalPostFrameSample valid{0x100000,0x120000,0x140000,0x150000,0x1C,2};
 assert(!gate.Observe(valid,false));
 for(unsigned n=0;n<29;n++)assert(!gate.Observe(valid,true));
 assert(gate.StableOriginalFrames()==29u);
 assert(gate.Observe(valid,true));
 for(unsigned n=0;n<500;n++)assert(!gate.Observe(valid,true));
 assert(!gate.OriginalPlayer2Ready());
 auto bad=valid;bad.localKind=1;
 assert(!gate.Observe(bad,true)&&gate.StableOriginalFrames()==0);
 for(unsigned n=0;n<29;n++)assert(!gate.Observe(valid,true));
 assert(gate.Observe(valid,true));
 bad=valid;bad.world++;
 assert(!gate.Observe(bad,true));
 assert(gate.StableOriginalFrames()==1);
 for(unsigned n=0;n<28;n++)assert(!gate.Observe(bad,true));
 assert(gate.Observe(bad,true));
 bad=valid;bad.corePhase=0x1B;
 assert(!gate.Observe(bad,true)&&gate.StableOriginalFrames()==0);
 bad=valid;bad.physics=0;
 assert(!gate.Observe(bad,true));
 bad=valid;bad.owner=0;
 assert(!gate.Observe(bad,true));
 bad=valid;bad.npcPool=0;
 assert(!gate.Observe(bad,true));
 gate.Reset();assert(gate.StableOriginalFrames()==0);
 std::cout<<"ORIGINAL_POST_1F1080_FRAME_SOURCE_GATE_PASS\n";
}
