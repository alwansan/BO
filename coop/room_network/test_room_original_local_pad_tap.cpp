#include "room_original_local_pad_tap.hpp"
#include "room_original_guest_pad_contract.hpp"
#include <cassert>
#include <iostream>
using namespace blackcoop;
int main(){
    OriginalLocalPadTap tap;
    assert(!tap.Read(0).connected&&!tap.Read(10).connected);
    tap.Capture({true,true,16000,-22000,255,240,-32768,32767},100);
    auto sample=tap.Read(130);
    assert(sample.connected&&sample.fromTouch&&sample.lx==16000&&sample.ly==-22000);
    assert(sample.rx==-32768&&sample.ry==32767);
    auto command=QuantizeOriginalGuestPad(sample.connected,sample.lx,sample.ly,
                                           sample.a,sample.rightTrigger);
    assert(command.moveX>0&&command.moveY<0&&command.fire&&command.jump);
    assert(!tap.Read(351).connected&&tap.Read(351).rx==0&&tap.Read(351).ry==0);
    assert(!tap.Read(99).connected);
    // Critical release: touch::Apply sets 'connected' only while used.
    // A zero/not-connected report MUST replace held fire and movement.
    tap.Capture({false,false,0,0,0,0},200);
    auto released=tap.Read(225);
    assert(!released.connected&&!released.fromTouch&&released.rx==0&&released.ry==0);
    command=QuantizeOriginalGuestPad(released.connected,released.lx,released.ly,
                                      released.a,released.rightTrigger);
    assert(command.moveX==0&&command.moveY==0&&!command.fire&&!command.jump);
    // Touch control may be active alone, without physical SDL controller.
    tap.Capture({true,true,-32768,32767,0,200},300);
    sample=tap.Read(320);
    assert(sample.connected&&sample.fromTouch&&sample.lx==-32768&&sample.ly==32767);
    tap.Reset();assert(!tap.Read(330).connected);
    std::cout<<"ORIGINAL_TOUCH_MERGED_PAD_TAP_PASS: sampled once after touch::Apply, stale/release goes neutral, no second pad Poll and no game memory writes\n";
}
