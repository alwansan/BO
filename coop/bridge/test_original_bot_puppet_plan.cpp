#include "original_bot_puppet_plan.hpp"
#include <cassert>
#include <array>
#include <iostream>
#include <thread>
using namespace bo_bridge;
int main(){
    OriginalBotPuppetPlanner host,guest;
    std::array<OriginalBotSlotSource,kOriginalBotPoolSlots> pool{};
    std::array<uint64_t,5> generations{{0,10,20,30,40}};
    OriginalBotLookPlan planned{};
    // Engine callback and COMMIT are independent mandatory checks.
    assert(!host.Bind(4,1,15,false,true));
    assert(!host.Bind(4,1,15,true,false));
    assert(!host.Bind(0,1,15,true,true));
    assert(!host.Bind(4,1,1,true,true));
    assert(!host.Bind(4,2,5,true,true)); // self not in roster
    assert(host.Bind(4,1,15,true,true));
    assert(!host.Plan(pool,generations,planned));
    // A live hostile NPC may donate its LOOK only, never its NPC AI.
    const unsigned enemy=6;
    pool[enemy]={0xA155,0x2999,0x1800,0x2500,0x3500};
    // Distinct preexisting empty engine character/animation slots, not
    // existing combat AI or copies of the first-person player's hands.
    pool[31]={0,0,0x1111,0x2222,0x3333};
    pool[30]={0,0,0x1112,0x2223,0x3334};
    pool[29]={0,0,0x1113,0x2224,0x3335};
    pool[28]={0,999,0x1114,0x2225,0x3336}; // NPC controller: NOT FREE
    assert(host.Plan(pool,generations,planned));
    assert(planned.count==3);
    for(unsigned i=0;i<3;i++){
        const auto& h=planned.remotes[i];
        assert(h.human==i+2&&h.sourceBotSlot==enemy);
        assert(h.emptyOriginalCharacterSlot==31-i);
        assert(h.sceneEpoch==4&&h.humanGeneration==generations[i+2]);
        assert(h.localOpaqueModel==0xA155&&!h.borrowedNpcBrain);
    }
    assert(!planned.actualRendererInstalled&&!planned.hostSimulationIntegrated);
    // Missing one FREE slot must fail all three humans, not silently hide #4.
    pool[29].controller=88;
    OriginalBotLookPlan stale=planned;
    assert(!host.Plan(pool,generations,stale));
    assert(stale.count==3); // no partial output corruption
    pool[29].controller=0;
    // Missing exact authenticated per-peer incarnation invalidates all.
    generations[3]=0;
    assert(!host.Plan(pool,generations,stale));
    generations[3]=3000;
    assert(host.Plan(pool,generations,stale));
    assert(stale.remotes[1].humanGeneration==3000);
    // Original player on guest remains first-person; the remote host is
    // another appearance-only human, not another weapon/hands view.
    assert(guest.Bind(4,2,3,true,true));
    assert(guest.Plan(pool,generations,stale));
    assert(stale.count==1&&stale.remotes[0].human==1);
    // Any friendly bot appearance works too; NO ally-only dependency.
    pool[enemy]={};
    pool[9]={0xB966,0,0x1800,0x2500,0x3500};
    assert(host.Plan(pool,generations,stale));
    assert(stale.remotes[0].sourceBotSlot==9);
    // Never select a nonrenderable model without asset buffers.
    pool[9].partB=0;
    assert(!host.Plan(pool,generations,stale));
    pool[9].partB=0x3500;
    bool foreign=false;
    std::thread worker([&]{foreign=host.Plan(pool,generations,stale);});
    worker.join();
    assert(!foreign);
    // Scene transition requires the engine to authenticate and confirm again.
    assert(!host.Bind(5,1,15,false,true));
    assert(host.Bind(5,1,15,true,true));
    assert(host.Plan(pool,generations,stale));
    assert(stale.remotes[0].sceneEpoch==5);
    std::cout<<"ANY_BOT_REMOTE_HUMAN_PUPPET_PLAN_PASS: source enemy/ally look, 3 separate blank slots, no NPC AI, scene/roster gated, NO live actor or renderer\n";
}
