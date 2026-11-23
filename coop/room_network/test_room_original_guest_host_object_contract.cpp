// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_guest_host_object_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
int main(){
 using namespace blackcoop;
 OriginalGuestHostObjectInbox inbox;
 OriginalGuestLaunchMatchKey key{};
 key.originalEpisode=77;key.sceneEpoch=7;key.rosterRevision=5;
 key.tokenHi=3;key.tokenLo=9;key.selfSlot=2;key.participants=3;
 // Exact host-signed change of a stable, non-pointer object identity.
 int32_t msg[]={7,5,3,9,1,6,0,123456,1,1};
 assert(inbox.Ingest(msg,10,key));
 assert(inbox.Pending()==1);
 OriginalHostObjectEvent first{};
 assert(inbox.Take(first)&&first.sequence==1&&first.tick==6&&
        first.objectKey==123456u&&first.kind==1&&first.state==1);
 assert(inbox.Pending()==0);
 assert(!inbox.Ingest(msg,10,key)); // replay/gap fail closed
 assert(inbox.Poisoned());
 inbox.Reset();
 assert(inbox.Ingest(msg,10,key));
 msg[4]=2;msg[5]=7;msg[6]=1;msg[7]=-1;msg[8]=2;msg[9]=3;
 assert(inbox.Ingest(msg,10,key));
 OriginalHostObjectEvent event{};
 assert(inbox.Take(event)&&event.sequence==1);
 assert(inbox.Take(event)&&event.sequence==2&&
        event.objectKey==0x00000001ffffffffull);
 assert(!inbox.Take(event));
 bool remoteAccepted=true;
 std::thread outsider([&]{remoteAccepted=inbox.Ingest(msg,10,key);
                           OriginalHostObjectEvent tmp{};
                           assert(!inbox.Take(tmp));});
 outsider.join();assert(!remoteAccepted);
 inbox.Reset();
 key.sceneEpoch=8;key.tokenLo=10;
 msg[0]=8;msg[3]=10;msg[4]=1;msg[5]=1;msg[6]=0;msg[7]=555;msg[8]=1;msg[9]=1;
 assert(inbox.Ingest(msg,10,key));
 auto old=key;old.sceneEpoch=7;assert(!inbox.Ingest(msg,10,old));
 msg[9]=0;assert(!inbox.Ingest(msg,10,key));
 msg[9]=1;msg[7]=0;assert(!inbox.Ingest(msg,10,key));
 msg[7]=555;msg[0]=7;assert(!inbox.Ingest(msg,10,key));
 msg[0]=8;
 inbox.Reset();
 assert(inbox.Pending()==0);
 assert(inbox.Ingest(msg,10,key));
 // Long sequence is bounded to 64; never silently drop on overflow.
 for(int i=2;i<=64;i++){
  msg[4]=i;msg[5]=i;assert(inbox.Ingest(msg,10,key));
 }
 assert(inbox.Pending()==64);
 msg[4]=65;msg[5]=65;assert(!inbox.Ingest(msg,10,key));
 assert(inbox.Poisoned());
 assert(!inbox.Take(event));
 inbox.Reset();assert(inbox.Pending()==0&&!inbox.Poisoned());
 std::cout<<"ORIGINAL_GUEST_HOST_OBJECT_INBOX_PASS: native 64 FIFO, exact original stage/token, replay/overflow/cross-thread/abort; NO engine object write"<<std::endl;
}
