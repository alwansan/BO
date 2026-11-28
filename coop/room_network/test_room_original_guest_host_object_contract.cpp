// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_guest_host_object_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
struct ObjectIO {
 uint64_t scene=99,generation=8;
 bool resolved=false,owns=true,apply=true,verify=true;
 unsigned calls=0;uint8_t state=0;
 uint64_t OriginalScene()const{return scene;}
 bool ResolveOriginalObject(uint64_t key,uint8_t kind,uint32_t& actor,uint64_t& gen){
  if(!resolved||key!=555||kind!=1)return false;
  actor=0x100000;gen=generation;return true;
 }
 bool StillOwnsOriginalObject(uint64_t,uint8_t,uint32_t,uint64_t gen){return owns&&gen==generation;}
 bool ApplyOriginalHostState(uint32_t,uint8_t,uint8_t value){++calls;state=value;return apply;}
 bool VerifyOriginalHostState(uint32_t,uint8_t,uint8_t value){return verify&&value==state;}
};

struct PartIO:ObjectIO {
 using Sample=blackcoop::OriginalUnitPropPartSample;
 using Key=blackcoop::OriginalUnitPropKey;
 Sample sample{{0x200000u,0x210000u,0x220000u,9},0x230000u,3,0x3F800000u,1,2};
 unsigned samples=0,scopeChecks=0;
 bool available=true,recycleOnRecheck=false,expireOnRecheck=false,invalidateDefinition=true;

 bool CurrentOriginalPartEvent(const blackcoop::OriginalGuestLaunchMatchKey&,
                              const blackcoop::OriginalHostObjectEvent&){
  return ++scopeChecks!=2||!expireOnRecheck;
 }
 bool SampleOriginalPart(const Key& key,uint8_t part,Sample& out){
  assert(key.unitIndex==7&&key.recordCount==20&&key.ordinal==14&&part==1);
  if(++samples==2&&recycleOnRecheck)++sample.prop.generation;
  out=sample;
  return available;
 }
 bool DepleteOriginalPart(const Key&,const Sample& before){
  assert(before==sample);
  ++calls;
  sample.healthBits=0;
  if(invalidateDefinition)sample.definition=UINT32_MAX;
  return true;
 }
};

// Synthetic lifecycle transitions only; no native game object or asset is used.
enum class LifecycleChange { None, ResolveScene, OwnershipScene, ResolveGeneration };
struct LifecycleIO:ObjectIO {
 LifecycleChange change=LifecycleChange::None;
 unsigned resolutions=0,ownershipChecks=0,readbacks=0;
 bool ResolveOriginalObject(uint64_t key,uint8_t kind,uint32_t& actor,uint64_t& gen){
  ++resolutions;
  if(!ObjectIO::ResolveOriginalObject(key,kind,actor,gen))return false;
  if(change==LifecycleChange::ResolveScene)scene=100;
  if(change==LifecycleChange::ResolveGeneration)++generation;
  return true;
 }
 bool StillOwnsOriginalObject(uint64_t key,uint8_t kind,uint32_t actor,uint64_t gen){
  ++ownershipChecks;
  const bool live=ObjectIO::StillOwnsOriginalObject(key,kind,actor,gen);
  if(live&&change==LifecycleChange::OwnershipScene)scene=100;
  return live;
 }
 bool VerifyOriginalHostState(uint32_t actor,uint8_t kind,uint8_t value){
  ++readbacks;return ObjectIO::VerifyOriginalHostState(actor,kind,value);
 }
};

void TestObjectPreApplyLifecycle(){
 using namespace blackcoop;
 OriginalGuestLaunchMatchKey key{};
 key.originalEpisode=77;key.sceneEpoch=7;key.rosterRevision=5;
 key.tokenHi=3;key.tokenLo=9;key.selfSlot=2;key.participants=3;
 const int32_t message[]={7,5,3,9,1,6,0,555,1,1};
 for(const auto change:{LifecycleChange::ResolveScene,LifecycleChange::OwnershipScene}){
  OriginalGuestHostObjectInbox inbox;LifecycleIO io;
  io.resolved=true;io.change=change;
  assert(inbox.Ingest(message,10,key));
  assert(inbox.ApplyNext(io,key,99)==OriginalObjectApplyResult::Rejected);
  assert(io.scene==100&&io.resolutions==1);
  assert(io.ownershipChecks==(change==LifecycleChange::ResolveScene?0u:1u));
  assert(io.calls==0&&io.readbacks==0&&io.state==0);
  assert(inbox.Pending()==1&&!inbox.Poisoned());
  // Another poll cannot write into the replacement scene. Do not pretend
  // that restoring a numeric scene ID proves a real engine teardown.
  assert(inbox.ApplyNext(io,key,99)==OriginalObjectApplyResult::Rejected);
  assert(io.resolutions==1&&io.calls==0&&io.readbacks==0);
  OriginalHostObjectEvent retained{};
  assert(inbox.Take(retained)&&inbox.Pending()==0);
  assert(retained.sceneEpoch==7&&retained.revision==5&&retained.sequence==1&&
         retained.tick==6&&retained.token==((uint64_t(3)<<32)|9u)&&
         retained.objectKey==555&&retained.kind==1&&retained.state==1);
 }
 {
  OriginalGuestHostObjectInbox inbox;LifecycleIO io;
  io.resolved=true;io.change=LifecycleChange::ResolveGeneration;
  assert(inbox.Ingest(message,10,key));
  assert(inbox.ApplyNext(io,key,99)==OriginalObjectApplyResult::WaitingForIdentity);
  assert(io.scene==99&&io.generation==9&&io.ownershipChecks==1);
  assert(io.calls==0&&io.readbacks==0&&io.state==0);
  assert(inbox.Pending()==1&&!inbox.Poisoned());
  // Resolve the replacement generation afresh; never reuse the old output.
  io.change=LifecycleChange::None;
  assert(inbox.ApplyNext(io,key,99)==OriginalObjectApplyResult::Applied);
  assert(io.resolutions==2&&io.ownershipChecks==3);
  assert(io.calls==1&&io.readbacks==1&&io.state==1&&inbox.Pending()==0);
  assert(inbox.ApplyNext(io,key,99)==OriginalObjectApplyResult::Empty);
  assert(io.resolutions==2&&io.calls==1&&io.readbacks==1);
 }
 std::cout<<"SYNTHETIC_OBJECT_LIFECYCLE_PASS scenarios=3 NO_ANDROID_ENGINE\n";
}

int main(){
 using namespace blackcoop;
 OriginalGuestHostObjectInbox inbox;
 OriginalGuestLaunchMatchKey key{};
 key.originalEpisode=77;key.sceneEpoch=7;key.rosterRevision=5;
 key.tokenHi=3;key.tokenLo=9;key.selfSlot=2;key.participants=3;
 {
  OriginalGuestHostObjectInbox parts;
  int32_t part[]={7,5,3,9,1,6,7,(20<<16)|14,4,1,1,2,3,0x3F800000};
  assert(!parts.Ingest(part,10,key));
  part[10]=2;assert(!parts.Ingest(part,std::size(part),key));part[10]=1;
  part[12]=-1;assert(!parts.Ingest(part,std::size(part),key));part[12]=3;
  part[13]=0;assert(!parts.Ingest(part,std::size(part),key));part[13]=0x3F800000;
   assert(parts.Ingest(part,std::size(part),key));
   ObjectIO io;
   assert(parts.ApplyNext(io,key,99)==OriginalObjectApplyResult::WaitingForIdentity&&io.calls==0);
   OriginalHostObjectEvent value{};
   OriginalUnitPropKey authored{};
   assert(parts.PeekPart(key,value,authored)&&parts.Pending()==1&&value.sequence==1&&
          authored.unitIndex==7&&authored.recordCount==20&&authored.ordinal==14);
   std::thread foreign([&]{
    OriginalHostObjectEvent offThread{};
    OriginalUnitPropKey ignored{};
    assert(!parts.PeekPart(key,offThread,ignored));
   });
   foreign.join();
   assert(parts.Take(value)&&value.kind==OriginalHostObjectEvent::PartDepleted&&
          value.partIndex==1&&value.partCount==2&&value.partDefinition==3&&
           value.previousHealthBits==0x3F800000u);
   assert(!parts.PeekPart(key,value,authored));
   for(unsigned mode=0;mode<6;++mode){
    OriginalGuestHostObjectInbox transaction;
    PartIO writer;
    if(mode==1)writer.available=false;
    if(mode==2)writer.expireOnRecheck=true;
    if(mode==3)writer.recycleOnRecheck=true;
    if(mode==4)writer.invalidateDefinition=false;
    if(mode==5)writer.sample.healthBits=0x40000000u;
    assert(transaction.Ingest(part,std::size(part),key));
    const auto result=transaction.ApplyNext(writer,key,99);
    if(mode==0){
     assert(result==OriginalObjectApplyResult::Applied&&transaction.Pending()==0&&writer.calls==1);
     assert(transaction.ApplyNext(writer,key,99)==OriginalObjectApplyResult::Empty&&writer.calls==1);
    }else if(mode<=2){
     assert(result==(mode==1?OriginalObjectApplyResult::WaitingForIdentity:
                             OriginalObjectApplyResult::Rejected));
     assert(transaction.Pending()==1&&writer.calls==0);
    }else{
     assert(result==OriginalObjectApplyResult::Quarantined&&transaction.Poisoned());
     assert(writer.calls==(mode==4?1u:0u));
     assert(transaction.ApplyNext(writer,key,99)==OriginalObjectApplyResult::Quarantined);
     assert(writer.calls==(mode==4?1u:0u));
    }
   }
 }
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
 auto rollback=key;rollback.sceneEpoch=7;rollback.tokenLo=9;
 int32_t obsolete[]={7,5,3,9,1,1,0,1000,1,1};
 assert(!inbox.Ingest(obsolete,10,rollback));
 assert(inbox.Pending()==1);
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
 // Revocation must happen even if the next poll has no object event.
 for(int variant=0;variant<3;variant++){
  OriginalGuestHostObjectInbox fresh;
  msg[4]=1;msg[5]=1;
  assert(fresh.Ingest(msg,10,key)&&fresh.Pending()==1);
  auto altered=key;
  if(variant==0)++altered.originalEpisode;
  if(variant==1)++altered.level;
  if(variant==2)++altered.stage;
  assert(!fresh.CheckLocalLaunch(altered)&&fresh.Poisoned());
  assert(!fresh.Take(event)&&fresh.Pending()==0);
 }
 {
  OriginalGuestHostObjectInbox fresh;
  assert(fresh.Ingest(msg,10,key));
  auto next=key;++next.sceneEpoch;++next.tokenLo;
  assert(fresh.CheckLocalLaunch(next)&&fresh.Pending()==0);
  assert(!fresh.Take(event));
  msg[4]=2;msg[5]=2;
  assert(!fresh.Ingest(msg,10,key)&&!fresh.CheckLocalLaunch(key));
  msg[4]=1;msg[5]=1;
  // A new signed scope can start from sequence 1, not inherit old events.
  msg[0]=static_cast<int32_t>(next.sceneEpoch);
  msg[3]=static_cast<int32_t>(next.tokenLo);
  msg[4]=2;
  assert(!fresh.Ingest(msg,10,next));
  assert(!fresh.CheckLocalLaunch(key)); // malformed new first packet did not reset revocation
  msg[4]=1;
  assert(fresh.Ingest(msg,10,next)&&fresh.Pending()==1);
 }
 {
  OriginalGuestHostObjectInbox fresh;ObjectIO io;
  msg[0]=static_cast<int32_t>(key.sceneEpoch);
  msg[3]=static_cast<int32_t>(key.tokenLo);msg[4]=1;msg[5]=1;
  assert(fresh.Ingest(msg,10,key));
  assert(fresh.ApplyNext(io,key,99)==OriginalObjectApplyResult::WaitingForIdentity);
  assert(fresh.Pending()==1&&io.calls==0);
  io.resolved=true;
  assert(fresh.ApplyNext(io,key,100)==OriginalObjectApplyResult::Rejected);
  assert(fresh.ApplyNext(io,key,99)==OriginalObjectApplyResult::Applied);
  assert(fresh.Pending()==0&&io.calls==1&&io.state==1);
  msg[4]=2;msg[5]=2;assert(fresh.Ingest(msg,10,key));
  io.verify=false;
  assert(fresh.ApplyNext(io,key,99)==OriginalObjectApplyResult::Quarantined);
  assert(fresh.ApplyNext(io,key,99)==OriginalObjectApplyResult::Quarantined);
  assert(io.calls==2);
 }
 TestObjectPreApplyLifecycle();
 std::cout<<"ORIGINAL_GUEST_HOST_OBJECT_INBOX_PASS: native 64 FIFO, exact original stage/token, replay/overflow/cross-thread/abort; NO engine object write"<<std::endl;
}
