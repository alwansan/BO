#include "room_original_bot_pool_contract.hpp"
#include <array>
#include <cassert>
#include <iostream>
using namespace blackcoop;
int main(){
    std::array<OriginalBotPoolSlotEvidence,32> slots{};
    OriginalBotPoolAvailability summary=CountOriginalBotLookSlots(slots);
    assert(summary.coarseCode()==60&&!summary.potentialOneToThreeHumans());
    // Existing hostile original bot can donate APPEARANCE. Its AI remains.
    slots[5]={true,true,true,true,true};
    summary=CountOriginalBotLookSlots(slots);
    assert(summary.sourceLooks==1&&summary.coarseCode()==61);
    // Three unused original character slots have allocated visual buffers,
    // no model and no live controller. They are NOT yet reserved.
    slots[31]={false,false,true,true,true};
    slots[30]={false,false,true,true,true};
    slots[29]={false,false,true,true,true};
    summary=CountOriginalBotLookSlots(slots);
    assert(summary.sourceLooks==1&&summary.emptyVisualSlots==3);
    assert(summary.potentialOneToThreeHumans()&&summary.coarseCode()==64);
    // A live controller wins: never take original NPC-owned slot.
    slots[29].controller=true;
    summary=CountOriginalBotLookSlots(slots);
    assert(summary.emptyVisualSlots==2&&summary.coarseCode()==63);
    slots[30].partBBufferValid=false;
    summary=CountOriginalBotLookSlots(slots);
    assert(summary.emptyVisualSlots==1&&summary.coarseCode()==62);
    slots[31].instanceValid=false;
    assert(CountOriginalBotLookSlots(slots).coarseCode()==61);
    slots[5].model=false;
    assert(CountOriginalBotLookSlots(slots).coarseCode()==60);
    // Any original friendly/hostile model is allowed; appearance-only
    // readiness is not a game actor, collision, AI or stage READY.
    slots[6]={true,false,true,true,true};
    slots[30]={false,false,true,true,true};
    slots[29].controller=false;
    slots[31].instanceValid=true;
    summary=CountOriginalBotLookSlots(slots);
    assert(summary.sourceLooks==1&&summary.emptyVisualSlots==3);
    assert(summary.coarseCode()==64);
    // Authenticated HOST TLS roster uses ordinal mask 0b0010/0110/1110.
    // Appearance from any live enemy/ally but never copy NPC controller.
    OriginalHumanLookPlanner planner;
    std::array<OriginalBotPoolSlotEvidence,32> proposalSlots{};
    assert(planner.Observe(1,0x12000,0,proposalSlots)==
           OriginalHumanLookPlanState::Inactive);
    assert(planner.Observe(1,0x12000,0x02,proposalSlots)==
           OriginalHumanLookPlanState::NoAppearanceSource);
    proposalSlots[5]={true,true,true,true,true};
    assert(planner.Observe(1,0x12000,0x02,proposalSlots)==
           OriginalHumanLookPlanState::InsufficientBlankSlots);
    proposalSlots[31]={false,false,true,true,true};
    proposalSlots[30]={false,false,true,true,true};
    proposalSlots[29]={false,false,true,true,true};
    assert(planner.Observe(1,0x12000,0x02,proposalSlots)==
           OriginalHumanLookPlanState::Proposed);
    assert(planner.Candidate().appearanceSlot==5);
    assert(planner.Candidate().blankSlots[2]==31);
    assert(planner.Candidate().blankSlots[3]==0xFFu);
    assert(planner.Observe(1,0x12000,0x06,proposalSlots)==
           OriginalHumanLookPlanState::Invalidated); // roster changed
    assert(planner.Observe(1,0x12000,0x02,proposalSlots)==
           OriginalHumanLookPlanState::Invalidated); // never silently recover
    assert(planner.Observe(2,0x12000,0x0E,proposalSlots)==
           OriginalHumanLookPlanState::Proposed); // new original launch only
    assert(planner.Candidate().blankSlots[2]==31);
    assert(planner.Candidate().blankSlots[3]==30);
    assert(planner.Candidate().blankSlots[4]==29);
    proposalSlots[30].controller=true; // original NPC reclaimed slot
    assert(planner.Observe(2,0x12000,0x0E,proposalSlots)==
           OriginalHumanLookPlanState::Invalidated);
    proposalSlots[30].controller=false;
    assert(planner.Observe(2,0x12000,0x0E,proposalSlots)==
           OriginalHumanLookPlanState::Invalidated);
    assert(planner.Observe(3,0x12000,0x0E,proposalSlots)==
           OriginalHumanLookPlanState::Proposed);
    proposalSlots[5].instanceValid=false; // source soldier removed
    assert(planner.Observe(3,0x12000,0x0E,proposalSlots)==
           OriginalHumanLookPlanState::Invalidated);
    assert(planner.Observe(4,0x12000,0x01,proposalSlots)==
           OriginalHumanLookPlanState::Inactive); // forged host-only mask
    std::cout<<"ORIGINAL_BOT_POOL_APPEARANCE_COUNTS_PASS: original enemy/ally looks, three independent free rigs, NO actor write / AI / READY\n";
}
