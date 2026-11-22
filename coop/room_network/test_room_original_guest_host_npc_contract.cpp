#include "room_original_guest_host_npc_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
int main(){
 blackcoop::OriginalGuestHostNpcInbox s;
 blackcoop::OriginalNpcValue v{};
 int32_t f[]={7,3,3,0,100,200,300,30000,0,
              15,-500,100,200,800,1,
              31,0,0,0,0,1};
 assert(s.Ingest(f,sizeof(f)/4,1000));
 assert(s.Read(15,1349,v)&&v.flags==1&&v.healthCenti==800);
 assert(!s.Read(1,1349,v));
 assert(!s.Read(15,1351,v));
 assert(!s.Ingest(f,sizeof(f)/4,1400)); // repeat tick no freshness bump
 assert(s.Read(15,1349,v));             // last valid state remains
 f[1]=4;f[9]=0;assert(!s.Ingest(f,sizeof(f)/4,1400)); // unsorted
 f[9]=15;f[14]=2;assert(!s.Ingest(f,sizeof(f)/4,1400)); // invalid dead
 f[14]=1;assert(s.Ingest(f,sizeof(f)/4,1400));
 bool accepted=true;std::thread t([&]{accepted=s.Ingest(f,sizeof(f)/4,1410);});
 t.join();assert(!accepted);
 int32_t tombstone[]={0,0,0};
 assert(s.Ingest(tombstone,3,1450));assert(!s.Read(15,1450,v));
 std::cout<<"ORIGINAL_HOST_NPC_GUEST_INBOX_PASS: 32 host slots, strict ordered/health/death, fresh/no replay/cross-thread/abort, NO local NPC writes"<<std::endl;
}
