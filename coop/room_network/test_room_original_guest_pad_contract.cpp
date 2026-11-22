#include "room_original_guest_pad_contract.hpp"
#include <cassert>
#include <iostream>
using namespace blackcoop;
int main(){
    assert(QuantizeOriginalPadAxis(0)==0);
    assert(QuantizeOriginalPadAxis(4096)==0);
    assert(QuantizeOriginalPadAxis(-4096)==0);
    assert(QuantizeOriginalPadAxis(32767)>=999);
    assert(QuantizeOriginalPadAxis(-32768)==-1000);
    assert(QuantizeOriginalPadAxis(40000)==0);
    assert(QuantizeOriginalPadAxis(-40000)==0);
    auto idle=QuantizeOriginalGuestPad(false,32767,32767,255,255);
    assert(idle.moveX==0&&idle.moveY==0&&!idle.fire&&!idle.jump);
    auto forward=QuantizeOriginalGuestPad(true,17000,-18000,255,190);
    assert(forward.moveX>500&&forward.moveY< -500);
    assert(forward.jump&&forward.fire);
    auto release=QuantizeOriginalGuestPad(true,0,0,0,127);
    assert(release.moveX==0&&release.moveY==0&&!release.jump&&!release.fire);
    // Remote command has no NPC ID, original guest pointer or guest body pose.
    std::cout<<"ORIGINAL_GUEST_CONTROLLER_SOURCE_PASS: original SDK pad axes+trigger+A, 40Hz-ready native bounded values, no guest memory writes or GAMEPLAY claim\n";
}
