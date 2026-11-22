#include "original_bot_puppet_pose_contract.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <thread>
using namespace bo_bridge;
int main(){
    std::array<OriginalBotSlotSource,kOriginalBotPoolSlots> slots{};
    slots[6]={0xA155,0x9999,0x1000,0x2000,0x3000};
    slots[31]={0,0,101,201,301};
    slots[30]={0,0,102,202,302};
    slots[29]={0,0,103,203,303};
    std::array<uint64_t,5> generations{{0,11,22,33,44}};
    OriginalBotPuppetPlanner p;
    OriginalBotLookPlan plan{};
    assert(p.Bind(17,1,15,true,true)&&p.Plan(slots,generations,plan));
    CanonicalHumanFrame frame{};
    frame.sceneEpoch=17;frame.tick=1;frame.self=1;
    frame.rosterMask=15;frame.receiptMs=1000;
    for(uint8_t id=1;id<=4;++id)
        frame.humans[id]={id,100*id,25,35,0,0};
    OriginalPuppetCanonicalPoseGate g;
    PuppetPoseBatch result{};
    assert(!g.Arm(17,123,1,15,false,true));
    assert(!g.Arm(17,123,1,15,true,false));
    assert(g.Arm(17,123,1,15,true,true));
    assert(g.Accept(plan,slots,generations,frame,1020,result));
    assert(result.count==3&&!result.originalDrawCalled&&
           !result.originalEngineMemoryWritten);
    for(unsigned i=0;i<3;++i){
        assert(result.humans[i].id==i+2);
        assert(result.humans[i].blankSlot==31-i);
        assert(result.humans[i].xCm==100*static_cast<int>(i+2));
    }
    frame.tick=2;
    assert(!g.Accept(plan,slots,generations,frame,1400,result));
    assert(!g.Invalidated()); // expired frame may recover with NEW tick/receipt
    frame.receiptMs=1400;
    assert(g.Accept(plan,slots,generations,frame,1410,result));
    assert(!g.Accept(plan,slots,generations,frame,1420,result)); // replay
    assert(!g.Arm(17,124,1,15,true,true)); // stale ticket relabel poisons
    assert(g.Invalidated());
    assert(g.Reset()&&g.Arm(17,123,1,15,true,true));
    frame.tick=3;
    assert(g.Accept(plan,slots,generations,frame,1421,result));
    slots[30].controller=400;frame.tick=4;
    assert(!g.Accept(plan,slots,generations,frame,1422,result));
    assert(g.Invalidated()); // original enemy took candidate slot
    slots[30].controller=0;
    assert(!g.Accept(plan,slots,generations,frame,1423,result));
    assert(g.Reset()&&g.Arm(17,123,1,15,true,true));
    frame.tick=5;
    assert(g.Accept(plan,slots,generations,frame,1424,result));
    generations[3]=333;
    frame.tick=6;
    assert(!g.Accept(plan,slots,generations,frame,1425,result));
    assert(g.Invalidated()); // rejoin cannot reuse old body
    // Guest second phone sees HOST body, keeping own FPS view.
    OriginalBotPuppetPlanner guest;
    generations[3]=33;
    assert(guest.Bind(18,2,3,true,true));
    OriginalBotLookPlan gp{};
    assert(guest.Plan(slots,generations,gp));
    assert(gp.count==1&&gp.remotes[0].human==1);
    OriginalPuppetCanonicalPoseGate gg;
    CanonicalHumanFrame gf{};
    gf.sceneEpoch=18;gf.tick=1;gf.self=2;gf.rosterMask=3;
    gf.receiptMs=2000;
    gf.humans[1]={1,120,0,0,0,0};
    gf.humans[2]={2,240,0,0,0,0};
    assert(gg.Arm(18,555,2,3,true,true));
    assert(gg.Accept(gp,slots,generations,gf,2010,result));
    assert(result.count==1&&result.humans[0].id==1);
    gf.tick=2;gf.humans[1].xCm=20000001;
    assert(!gg.Accept(gp,slots,generations,gf,2011,result));
    assert(gg.Invalidated());
    bool foreign=false;
    std::thread other([&]{foreign=g.Reset();});other.join();
    assert(!foreign&&g.Invalidated());
    std::cout<<"ORIGINAL_BOT_HOST_POSE_VALUES_PASS; NOT AN ACTOR OR DRAW\n";
}
