#include "room_original_trusted_input_contract.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <thread>
using blackcoop::OriginalHostHumanInbox;
using blackcoop::TrustedHumanCommand;
int main(){
    OriginalHostHumanInbox inbox;
    const int32_t one[]={42,1,2,10,500,-500,1200,3,-32768,32767};
    const int32_t second[]={42,2,2,11,501,-499,1201,1,0,0,3,7,-900,800,-9000,2,123,-456};
    const int32_t invalid[]={42,2,2,12,800,800,0,0,0,0,2,14,0,0,0,0,0,0};
    const int32_t wrongOrder[]={42,2,3,1,0,0,0,0,0,0,2,1,0,0,0,0,0,0};
    const int32_t invalidFlag[]={42,1,2,13,100,100,0,4,0,0};
    const int32_t invalidLook[]={42,1,2,13,100,100,0,0,32768,-32769};
    const int32_t stale[]={42,1,2,10,100,100,0,0,0,0};
    const int32_t idle[]={42,0};
    const int32_t ended[]={0,0};
    TrustedHumanCommand out{};
    assert(!inbox.Fresh(2,out));
    assert(inbox.Ingest(one,std::size(one))==1);
    assert(inbox.Epoch()==42);
    assert(inbox.Fresh(2,out));
    assert(out.peer==2&&out.sequence==10&&out.x==500&&out.y==-500&&out.flags==3);
    assert(out.lookX==-32768&&out.lookY==32767);
    assert(inbox.Ingest(invalid,std::size(invalid))==0);
    assert(inbox.Ingest(wrongOrder,std::size(wrongOrder))==0);
    assert(inbox.Ingest(invalidFlag,std::size(invalidFlag))==0);
    assert(inbox.Ingest(invalidLook,std::size(invalidLook))==0);
    assert(inbox.Ingest(stale,std::size(stale))==0);
    assert(inbox.Fresh(2,out)&&out.sequence==10);
    assert(inbox.Ingest(second,std::size(second))==2);
    assert(inbox.Fresh(2,out)&&out.sequence==11);
    assert(inbox.Fresh(3,out)&&out.sequence==7&&out.yaw==-9000&&out.flags==2);
    assert(out.lookX==123&&out.lookY==-456);
    assert(!inbox.Fresh(4,out));
    // Neither no-new-input polls nor gameplay pause can retain commands forever.
    for(int i=0;i<4;++i)assert(inbox.Ingest(idle,2)==0);
    assert(!inbox.Fresh(2,out)&&!inbox.Fresh(3,out));
    // New actual committed ticket-scene epoch starts a fresh generation.
    const int32_t newEpoch[]={43,1,2,1,0,0,0,0,0,0};
    assert(inbox.Ingest(newEpoch,std::size(newEpoch))==1);
    assert(inbox.Fresh(2,out)&&out.sceneEpoch==43&&out.sequence==1);
    assert(!inbox.Fresh(3,out));
    const int32_t oldSceneReplay[]={42,1,2,999,1000,-1000,0,3,-700,18000};
    assert(inbox.Ingest(oldSceneReplay,std::size(oldSceneReplay))==0);
    assert(inbox.Epoch()==43);
    assert(inbox.Fresh(2,out)&&out.sceneEpoch==43&&out.sequence==1&&out.flags==0);
    // A foreign worker can NEVER drain or modify original game-thread inbox.
    bool foreign=false,foreignRead=true;
    std::thread worker([&]{
        foreign=inbox.Ingest(one,std::size(one))!=0;
        foreignRead=inbox.Fresh(2,out);
        assert(!inbox.Reset());
    });worker.join();
    assert(!foreign&&!foreignRead&&inbox.Fresh(2,out)&&out.sceneEpoch==43);
    assert(inbox.Ingest(ended,2)==0);
    assert(inbox.Epoch()==0&&!inbox.Fresh(2,out));
    assert(inbox.Ingest(nullptr,8)==0);
    assert(inbox.Ingest(one,7)==0);
    assert(inbox.Ingest(one,OriginalHostHumanInbox::kMaxFrame+1)==0);
    std::cout<<"ORIGINAL_VERIFIED_TLS_INPUT_NATIVE_INBOX_PASS: game-thread only, host authenticated source, 3 humans, stale/epoch reset, no bot AI or gameplay writes\n";
}
