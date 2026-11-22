#include "original_all_other_humans_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
int main(){
 blackcoop::OriginalAllOtherHumansSource b;
 blackcoop::OriginalOtherHumanBody out{};
 int32_t host[]={7,19,30,1,3,
   2,100,200,300,0,0, 3,400,500,600,1200,1,
   4,700,800,900,-1500,2};
 static_assert(sizeof(host)/sizeof(host[0])==23);
 assert(b.Ingest(host,23,1000));
 assert(b.Self()==1);
 for(uint8_t id=2;id<=4;id++)
   assert(b.Read(id,1349,out)&&out.peer==id&&out.self==1);
 assert(!b.Read(1,1349,out));
 assert(!b.Read(3,1351,out));
 assert(!b.Ingest(host,23,1400)); // replay tick
 assert(b.Read(3,1349,out)&&out.peer==3); // same tick MUST NOT clear it
 host[2]=31;host[5]=3;assert(!b.Ingest(host,23,1400)); // unordered duplicate
 host[5]=2;host[10]=4;assert(!b.Ingest(host,23,1400)); // impossible flags
 host[10]=0;host[16]=19000;assert(!b.Ingest(host,23,1400));
 host[16]=1;assert(b.Ingest(host,23,1400));
 bool cross=false;
 std::thread other([&](){cross=b.Ingest(host,23,1410);});
 other.join();assert(!cross);
 // GUEST slot 3 sees HOST slot 1, slot 2, slot 4.
 int32_t guest[]={8,20,1,3,3,
   1,10,20,30,0,0, 2,100,200,300,1000,1,
   4,700,800,900,-1500,2};
 assert(b.Ingest(guest,23,1500));
 assert(b.Read(1,1501,out)&&out.peer==1&&out.self==3);
 assert(!b.Read(3,1501,out));
 int32_t incomplete[]={8,20,2,1,2,
   2,1,2,3,0,0, 4,1,2,3,0,0};
 assert(!b.Ingest(incomplete,17,1510)); // P3 absent from roster
 int32_t clear[5]={};
 assert(b.Ingest(clear,5,1600));
 assert(!b.Read(1,1601,out));
 std::cout<<"ALL_OTHER_HUMANS_SOURCE_PASS: 4 signed roster slots, 3 peers per host/guest, replay, out of order, range, stale, cross thread, abort; no game actor"<<std::endl;
}
