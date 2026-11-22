#include "room_original_guest_prepared_story_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
using blackcoop::OriginalGuestPreparedStoryInbox;
using blackcoop::OriginalGuestPreparedStory;
int main(){
    OriginalGuestPreparedStoryInbox source;
    OriginalGuestPreparedStory cmd{};
    const int32_t none[8]={};
    const int32_t valid[8]={1,3,0,5,2,345,2,3};
    assert(!source.Fresh(1000,cmd));
    assert(source.Ingest(none,8,1000)==0);
    assert(source.Ingest(valid,8,1000)==76);
    assert(source.Fresh(1001,cmd)&&cmd.sceneEpoch==1&&cmd.mission==3&&
           cmd.selfSlot==2&&cmd.participants==3&&cmd.tokenLo==345);
    assert(source.Ingest(valid,8,1100)==0);
    assert(source.Fresh(2300,cmd)&&!source.Fresh(2301,cmd));
    int32_t changed[8];for(int i=0;i<8;i++)changed[i]=valid[i];
    changed[0]=2;changed[1]=7;changed[2]=15;
    changed[3]=-1;changed[5]=-2;changed[6]=3;changed[7]=7;
    assert(source.Ingest(changed,8,2400)==77);
    assert(source.Fresh(2401,cmd)&&cmd.revision==0xffffffffu&&
           cmd.tokenLo==0xfffffffeu&&cmd.selfSlot==3);
    changed[7]=3; // slot3 not a member
    assert(source.Ingest(changed,8,2500)==0);
    assert(source.Fresh(2401,cmd)); // invalid packet didn't overwrite
    bool rejected=false;
    std::thread foreign([&]{rejected=source.Ingest(valid,8,2500)==0&&
                                   !source.Clear()&&!source.Fresh(2500,cmd);});
    foreign.join();assert(rejected);
    assert(source.Ingest(none,8,2600)==0);
    assert(!source.Fresh(2600,cmd));
    assert(source.Ingest(valid,8,2700)==76); // new room may restart epoch
    assert(!source.Ingest(valid,7,2800));
    assert(source.Ingest(none,8,2800)==0);
    assert(!source.Fresh(2801,cmd));
    std::cout<<"ORIGINAL_GUEST_FRONTEND_AUTH_HOST_STAGE_PASS: 8 ints, token/revision/roster/epoch, TTL, clear, JNI-only, no loader or READY\n";
}
