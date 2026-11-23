#include "room_original_host_inbox.hpp"
#include "room_original_host_pad_port_bridge.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
using namespace blackcoop;
struct TestPad {
 bool connected=false;uint16_t buttons=0;
 int16_t lx=0,ly=0,rx=0,ry=0;uint8_t analog[8]{};
};
static TestPad physical(){
 TestPad p{};p.connected=true;p.buttons=0x3Fu;
 p.lx=900;p.analog[0]=180;p.analog[7]=255;return p;
}
static bool neutral(const TestPad& p){
 if(p.connected||p.buttons||p.lx||p.ly||p.rx||p.ry)return false;
 for(uint8_t v:p.analog)if(v)return false;
 return true;
}
int main(){
 OriginalHostHumanInbox& inbox=OriginalHostHumanInputs();
 const int32_t frame[]={52,3,2,11,1000,-1000,1500,3,
                              3,12,-250,750,-1000,1,
                              4,13,0,0,0,2};
 assert(inbox.Ingest(frame,20)==3);
 auto pad=physical();
 assert(OriginalHostPadPortBridge::Route(pad,inbox,52,0,true,true)
   ==OriginalHostPadRoute::Unowned&&pad.analog[7]==255);
 assert(OriginalHostPadPortBridge::Route(pad,inbox,52,1,false,true)
   ==OriginalHostPadRoute::Unowned&&pad.analog[7]==255);
 assert(OriginalHostPadPortBridge::Route(pad,inbox,52,1,true,false)
   ==OriginalHostPadRoute::Unowned&&pad.analog[7]==255);
 assert(OriginalHostPadPortBridge::Route(pad,inbox,52,1,true,true)
   ==OriginalHostPadRoute::Applied);
 assert(pad.connected&&pad.buttons==0&&pad.lx==32767&&pad.ly==-32768);
 assert(pad.analog[0]==255&&pad.analog[7]==255&&pad.rx==0&&pad.ry==0);
 pad=physical();
 assert(OriginalHostPadPortBridge::Route(pad,inbox,52,2,true,true)
   ==OriginalHostPadRoute::Applied);
 assert(pad.lx<0&&pad.ly>0&&pad.analog[7]==255&&pad.analog[0]==0);
 pad=physical();
 assert(OriginalHostPadPortBridge::Route(pad,inbox,52,3,true,true)
   ==OriginalHostPadRoute::Applied);
 assert(pad.analog[0]==255&&pad.analog[7]==0);
 pad=physical();
 assert(OriginalHostPadPortBridge::Route(pad,inbox,53,1,true,true)
   ==OriginalHostPadRoute::NeutralReserved&&neutral(pad));
 const int32_t idle[]={52,0};
 for(int i=0;i<4;++i)assert(inbox.Ingest(idle,2)==0);
 pad=physical();
 assert(OriginalHostPadPortBridge::Route(pad,inbox,52,1,true,true)
   ==OriginalHostPadRoute::NeutralReserved&&neutral(pad));
 bool foreignApplied=true;
 std::thread other([&]{
  auto otherPad=physical();
  foreignApplied=OriginalHostPadPortBridge::Route(
       otherPad,inbox,52,1,true,true)==OriginalHostPadRoute::Applied;
  assert(neutral(otherPad));
 });other.join();
 assert(!foreignApplied);
 const int32_t newer[]={53,1,2,1,-100,100,0,1};
 assert(inbox.Ingest(newer,8)==1);
 pad=physical();
 assert(OriginalHostPadPortBridge::Route(pad,inbox,53,1,true,true)
   ==OriginalHostPadRoute::Applied&&pad.analog[7]==255);
 const int32_t expired[]={0,0};assert(inbox.Ingest(expired,2)==0);
 pad=physical();
 assert(OriginalHostPadPortBridge::Route(pad,inbox,53,1,true,true)
   ==OriginalHostPadRoute::NeutralReserved&&neutral(pad));
 std::cout<<"ORIGINAL_NATIVE_PORT1_2_3_TLS_PAD_BRIDGE_SAFE_NO_ACTOR_PASS\n";
}
