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
    std::cout<<"ORIGINAL_BOT_POOL_APPEARANCE_COUNTS_PASS: original enemy/ally looks, three independent free rigs, NO actor write / AI / READY\n";
}
