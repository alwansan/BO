#include "room_original_guest_host_npc_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
struct NpcIO{
 uint64_t scene=99;bool missing=false,alias=false,verify=true;unsigned calls=0;
 uint64_t OriginalScene()const{return scene;}
 bool ResolveOriginalNpc(uint8_t slot,uint32_t& actor,uint64_t& generation){
  if(missing&&slot==15)return false;
  actor=0x100000u+(alias?0u:slot*0x1000u);generation=slot+1u;return true;
 }
 bool StillOwnsOriginalNpc(uint8_t,uint32_t,uint64_t){return true;}
 bool ApplyOriginalNpc(uint32_t,const blackcoop::OriginalNpcValue&){++calls;return true;}
 bool VerifyOriginalNpc(uint32_t,const blackcoop::OriginalNpcValue&){return verify;}
};
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
 const int32_t older[]={6,100,1,15,0,0,0,9000,0};
 assert(!s.Ingest(older,9,1401)&&s.Epoch()==7);
 assert(s.Fresh(1401));
 bool accepted=true;std::thread t([&]{accepted=s.Ingest(f,sizeof(f)/4,1410);});
 t.join();assert(!accepted);
 {
  using namespace blackcoop;
  OriginalGuestNpcApplyBatch batch;NpcIO io;
  io.missing=true;
  assert(batch.Apply(io,s,99,7,1401)==OriginalNpcApplyStatus::WaitingForIdentity&&io.calls==0);
  io.missing=false;io.alias=true;
  assert(batch.Apply(io,s,99,7,1401)==OriginalNpcApplyStatus::WaitingForIdentity&&io.calls==0);
  io.alias=false;
  assert(batch.Apply(io,s,99,7,1401)==OriginalNpcApplyStatus::Applied&&io.calls==3);
  assert(batch.Apply(io,s,99,7,1402)==OriginalNpcApplyStatus::AlreadyApplied&&io.calls==3);
  f[1]=5;assert(s.Ingest(f,sizeof(f)/4,1410));
  io.verify=false;
  assert(batch.Apply(io,s,99,7,1410)==OriginalNpcApplyStatus::Quarantined&&io.calls==4);
  assert(batch.Apply(io,s,99,7,1410)==OriginalNpcApplyStatus::Quarantined&&io.calls==4);
  assert(!batch.AfterVerifiedEngineTeardown(false)&&batch.AfterVerifiedEngineTeardown(true));
 }
 int32_t tombstone[]={0,0,0};
 assert(s.Ingest(tombstone,3,1450));assert(!s.Read(15,1450,v));
 std::cout<<"ORIGINAL_HOST_NPC_GUEST_INBOX_PASS: 32 host slots, strict ordered/health/death, fresh/no replay/cross-thread/abort, NO local NPC writes"<<std::endl;
}
