#include "room_original_guest_launch_match_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using namespace blackcoop;
static OriginalGuestPreparedStory p(uint32_t epoch,uint8_t mission,
                                   uint8_t stage,uint8_t slot=2){
    OriginalGuestPreparedStory s{};
    s.sceneEpoch=epoch;s.revision=7;s.tokenHi=5;s.tokenLo=123;
    s.mission=mission;s.stage=stage;s.selfSlot=slot;
    s.participants=slot==2?3:7;
    return s;
}
int main(){
    OriginalGuestLaunchMatchGate g;
    OriginalGuestLaunchMatchKey result{};
    auto a=p(100,3,0);
    assert(g.ObserveOriginalStreamingEntry(0,3,0,&a)==0);
    assert(g.ObserveOriginalStreamingEntry(1,3,0,nullptr)==0);
    assert(g.ObserveOriginalStreamingEntry(1,3,0,&a)==78);
    assert(g.ObserveOriginalStreamingEntry(1,4,0,&a)==0); // same original episode: not relabeled
    assert(!g.ReadMatchedKey(result));
    for(int i=0;i<29;i++)
        assert(g.ObserveOriginalRunningWorld(1,0x2d0000,3,0,true)==0);
    assert(g.ObserveOriginalRunningWorld(1,0x2d0000,3,0,true)==81);
    assert(g.ReadMatchedKey(result));
    assert(result.originalEpisode==1&&result.sceneEpoch==100&&
           result.level==3&&result.stage==0&&result.tokenLo==123);
    assert(g.ObserveOriginalRunningWorld(1,0x2d0000,3,0,true)==0);
    bool fromAnotherThread=false;
    std::thread t([&]{OriginalGuestLaunchMatchKey other{};
        fromAnotherThread=g.ReadMatchedKey(other)&&
                         other.sceneEpoch==100;});
    t.join();assert(fromAnotherThread); // mutex-safe distinct native threads
    auto second=p(101,4,1,3);
    assert(g.ObserveOriginalStreamingEntry(2,0,1,&second)==79);
    assert(!g.ReadMatchedKey(result));
    for(int i=0;i<40;i++)
        assert(g.ObserveOriginalRunningWorld(2,0x2d0000,4,1,true)==0);
    assert(g.ObserveOriginalStreamingEntry(2,4,1,&second)==0);
    assert(g.ObserveOriginalStreamingEntry(3,4,1,&second)==78);
    for(int i=0;i<20;i++)
        assert(g.ObserveOriginalRunningWorld(3,0x2d0000,4,1,true)==0);
    assert(g.ObserveOriginalRunningWorld(3,0x2d0000,4,1,false)==0);
    for(int i=0;i<29;i++)
        assert(g.ObserveOriginalRunningWorld(3,0x2d0000,4,1,true)==0);
    assert(g.ObserveOriginalRunningWorld(3,0x2d0000,4,1,true)==81);
    assert(g.ReadMatchedKey(result)&&result.sceneEpoch==101&&
           result.selfSlot==3&&result.participants==7);
    g.Reset();assert(!g.ReadMatchedKey(result));
    auto stale=p(102,7,15);
    assert(g.ObserveOriginalStreamingEntry(4,7,15,&stale)==78);
    for(int i=0;i<20;i++)
        assert(g.ObserveOriginalRunningWorld(4,0x2d0000,7,15,true)==0);
    for(int i=0;i<29;i++)
        assert(g.ObserveOriginalRunningWorld(4,0x3d0000,7,15,true)==0);
    assert(g.ObserveOriginalRunningWorld(4,0x3d0000,7,15,true)==81);
    assert(g.ReadMatchedKey(result));
    assert(g.ObserveOriginalStreamingEntry(3,7,15,&stale)==0);
    std::cout<<"ORIGINAL_GUEST_LAUNCH_STAGE_CORRELATION_PASS: source stage, real local original world 30 samples, mismatches and resets; NO READY\n";
}
