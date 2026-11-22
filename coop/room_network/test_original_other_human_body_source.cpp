#include "original_other_human_body_source.hpp"
#include <cassert>
#include <thread>
#include <iostream>
int main(){
 blackcoop::OriginalOtherHumanBodyInbox box;
 blackcoop::OriginalOtherHumanBody got{};
 assert(!box.Read(500,got));
 int32_t host[10]={1,7,3,1,2,9000,14000,11000,0,0};
 int32_t guest[10]={1,7,3,2,1,9000,14000,11000,0,0};
 assert(box.Ingest(host,10,1000));
 assert(box.Read(1349,got)&&got.peer==2&&got.self==1&&got.z==11000);
 assert(!box.Read(1351,got));
 host[2]=2;assert(!box.Ingest(host,10,1400)); // replay tick
 host[2]=4;host[8]=19000;assert(!box.Ingest(host,10,1400));
 host[8]=0;assert(box.Ingest(host,10,1400));
 host[4]=3;assert(!box.Ingest(host,10,1450)); // third human excluded
 host[4]=2;
 bool cross=false;
 std::thread t([&](){cross=box.Ingest(host,10,1500);});
 t.join();assert(!cross);
 assert(box.Ingest(guest,10,1600)); // same app crossed scope after fresh auth
 assert(box.Read(1600,got)&&got.self==2&&got.peer==1);
 int32_t zero[10]={};
 assert(box.Ingest(zero,10,1610));assert(!box.Read(1611,got));
 std::cout<<"TWO_PHONE_ORIGINAL_BODY_SOURCE_PASS: host source, guest source, stale/replay, identity and teardown; NO original engine actor"<<std::endl;
}
