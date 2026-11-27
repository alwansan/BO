#include "room_original_post_frame_contract.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
using namespace blackcoop;

struct UnitFixture {
 std::vector<uint8_t> bytes=std::vector<uint8_t>(0x300000);
 bool Contains(uint32_t addr,uint32_t size)const noexcept{
   return addr>=0x10000u&&size&&uint64_t(addr)+size<=bytes.size();
 }
 uint8_t Read8(uint32_t addr)const noexcept{assert(Contains(addr,1));return bytes[addr];}
 uint16_t Read16(uint32_t addr)const noexcept{
   return uint16_t(Read8(addr))|(uint16_t(Read8(addr+1))<<8);
 }
 uint32_t Read32(uint32_t addr)const noexcept{
   return uint32_t(Read16(addr))|(uint32_t(Read16(addr+2))<<16);
 }
 void Write32(uint32_t addr,uint32_t value){
   assert(Contains(addr,4));
   std::memcpy(&bytes[addr],&value,sizeof(value));
 }
};

static void TestUnitMembership(){
 UnitFixture memory;
 constexpr uint32_t world=0x100000, unit=world+0x5250, objects=0x110000;
 constexpr uint32_t header=0x120000, records=header+0x100, head=world+0x5CB4;
 assert(!HasOriginalWorldUnitMembership(memory,world));
 memory.Write32(0x2D45D4,world);
 memory.Write32(world+0x5AE0,0x1C);
 memory.Write32(world+0x5AE4,0x1D);
 memory.Write32(unit,0x1C);
 memory.Write32(unit+4,7);
 memory.Write32(unit+8,7);
 memory.Write32(unit+0x38,1);
 memory.Write32(unit+0xC,header);
 memory.Write32(unit+0x14,objects);
 memory.Write32(header,24);
 memory.Write32(header+0x90,2);
 memory.Write32(header+0x1C,records);
 memory.Write32(objects,0x28EA60);
 memory.Write32(objects+0xF0,0x28EA60);
 memory.Write32(objects+0xD0,records);
 memory.Write32(objects+0xF0+0xD0,records+0x30);
 memory.Write32(head,objects);
 memory.Write32(objects+0xB0,objects+0xF0);
 const auto valid=memory.bytes;
 assert(HasOriginalWorldUnitMembership(memory,world));
 assert(memory.bytes==valid);
 const std::pair<uint32_t,uint32_t> exclusions[]={
   {world+0x5AE4,0x1E}, {unit+0x38,0}, {unit+8,8},
   {unit+0x14,0xFFFFFFF0}, {header+0x1C,0xFFFFFFF0},
   {objects+0xF0+0xD0,records}, {head,objects+4},
   {objects+0xB0,0}, {objects+0xF0+0xB0,objects}
 };
 for(const auto& [address,value]:exclusions){
   const uint32_t old=memory.Read32(address);
   memory.Write32(address,value);
   assert(!HasOriginalWorldUnitMembership(memory,world));
   memory.Write32(address,old);
 }
 assert(memory.bytes==valid);
 std::cout<<"ORIGINAL_UNIT_WORLD_LIST_MEMBERSHIP_FIXTURE_PASS\n";
}

static void TestAuthoredPropResolution(){
 UnitFixture memory;
 constexpr uint32_t world=0x100000, unit=world+0x5250, otherUnit=world+0x49D0;
 constexpr uint32_t objects=0x110000, header=0x120000, table=header+0x100;
 const auto publish=[&](uint32_t base){
   memory.Write32(unit+0x1C,base);
   memory.Write32(world+0x5CB4,base);
   for(uint32_t i=0;i<2;++i){
     const uint32_t object=base+i*0x200u;
     const uint32_t record=table+(i?4u:1u)*0x50u;
     memory.Write32(object,0x28ED98u);
     memory.Write32(object+0xC4u,4);
     memory.Write32(object+0x114u,record);
     memory.Write32(object+0xB0u,i?0:base+0x200u);
     // Identical selector pairs in the authored rows do not alias identity.
     memory.Write32(record+0x48u,9);
     memory.Write32(object+8u,9);
   }
 };
 memory.Write32(0x2D45D4,world);
 memory.Write32(world+0x5AE0,0x1C);
 memory.Write32(world+0x5AE4,0x1D);
 memory.Write32(world+0x5AEC,0x100);
 memory.Write32(unit,0x1C);
 memory.Write32(unit+4,7);
 memory.Write32(unit+8,7);
 memory.Write32(unit+0x38,1);
 memory.Write32(unit+0xC,header);
 memory.Write32(unit+0x34,2);
 memory.Write32(header,24);
 memory.Write32(header+0x90,5u<<16);
 memory.Write32(header+0x2C,table);
 publish(objects);
 uint64_t serial=10;
 const auto generation=[&](uint32_t currentWorld,uint32_t currentUnit) noexcept {
   return currentWorld==world&&currentUnit==unit?serial:0u;
 };
 constexpr OriginalUnitPropKey key{0,1,7,5,4};
 OriginalUnitPropHandle first;
 assert(ResolveOriginalUnitProp(memory,generation,world,key,first));
  assert(first.object==objects+0x200&&first.record==table+4*0x50&&first.generation==10);
  OriginalUnitPropKey reverse;
  assert(IdentifyOriginalUnitProp(memory,generation,world,first.object,reverse)&&reverse==key);
  assert(!IdentifyOriginalUnitProp(memory,generation,world,first.object+4,reverse));
 OriginalUnitPropHandle other;
 assert(ResolveOriginalUnitProp(memory,generation,world,{0,1,7,5,1},other));
 assert(other.object==objects);
 assert(!ResolveOriginalUnitProp(memory,generation,world,{0,1,7,5,0},other));
 assert(!ResolveOriginalUnitProp(memory,generation,world,{0,1,7,2,1},other));
 assert(!ResolveOriginalUnitProp(memory,generation,world,{1,1,7,5,4},other));
 memory.Write32(otherUnit,0x1C);
 memory.Write32(otherUnit+8u,7);
 memory.Write32(otherUnit+0x38u,1);
  assert(!ResolveOriginalUnitProp(memory,generation,world,key,other));
  assert(!IdentifyOriginalUnitProp(memory,generation,world,first.object,reverse));
  memory.Write32(otherUnit+0x38u,0);
 memory.Write32(first.object+0xB0u,first.object);
 assert(!ResolveOriginalUnitProp(memory,generation,world,key,other));
 memory.Write32(first.object+0xB0u,0);
 auto changingGeneration=[&](uint32_t,uint32_t) noexcept {return serial++;};
 assert(!ResolveOriginalUnitProp(memory,changingGeneration,world,key,other));
 assert(ResolveOriginalUnitProp(memory,generation,world,key,other)&&other!=first);
 assert(other.object==first.object); // same-address republication invalidates the old handle
 serial=0;
 assert(!ResolveOriginalUnitProp(memory,generation,world,key,other));
 serial=20;
 publish(objects+0x2000);
 assert(ResolveOriginalUnitProp(memory,generation,world,key,other)&&other!=first);
 assert(other.object==objects+0x2200&&other.record==first.record);
 OriginalUnitPublicationActivity activity;
 const auto live=[&](uint32_t,uint32_t) noexcept {return activity.Generation();};
 assert(ResolveOriginalUnitProp(memory,live,world,key,first));
 {
   const OriginalUnitPublicationActivity::Mutation unlink(activity);
   {
     const OriginalUnitPublicationActivity::Mutation nestedStream(activity);
     assert(!ResolveOriginalUnitProp(memory,live,world,key,other));
   }
   assert(!ResolveOriginalUnitProp(memory,live,world,key,other));
 }
 std::thread streamWorker([&]{
   const OriginalUnitPublicationActivity::Mutation link(activity);
   assert(!activity.Generation());
 });
 streamWorker.join();
 assert(ResolveOriginalUnitProp(memory,live,world,key,other)&&other!=first);
 assert(other.object==first.object&&other.generation>first.generation);
 constexpr uint32_t parts=0x140000;
 memory.Write32(other.object+0x1A0u,parts);
 memory.Write32(other.object+0x1F4u,2u<<8);
 memory.Write32(parts+0xCu,0x3F800000u);
 memory.Write32(parts+0x10u,3);
 OriginalUnitPropPartSample beforePart,afterPart;
 assert(SampleOriginalUnitPropPart(memory,live,world,key,1,beforePart));
 assert(beforePart.prop==other&&beforePart.part==parts+0xCu&&beforePart.definition==3);
 assert(!SampleOriginalUnitPropPart(memory,live,world,key,2,afterPart));
 memory.Write32(parts+0xCu,0);
 assert(SampleOriginalUnitPropPart(memory,live,world,key,1,afterPart));
 assert(!IsOriginalPartDepletion(beforePart,afterPart)); // health alone
 memory.Write32(parts+0x10u,UINT32_MAX);
 assert(SampleOriginalUnitPropPart(memory,live,world,key,1,afterPart));
 assert(IsOriginalPartDepletion(beforePart,afterPart));
 {
   const OriginalUnitPublicationActivity::Mutation republish(activity);
   assert(!SampleOriginalUnitPropPart(memory,live,world,key,1,afterPart));
 }
 assert(SampleOriginalUnitPropPart(memory,live,world,key,1,afterPart));
 assert(!IsOriginalPartDepletion(beforePart,afterPart)); // recycled identity
 std::cout<<"ORIGINAL_AUTHORED_PROP_RESOLUTION_PASS: packed ordinal, live membership, scene and publication scope\n";
}

static void TestUnitLifecycle(){
 constexpr uint32_t oldWorld=0x100000,newWorld=0x180000,slotOffset=0x49D0;
 constexpr uint32_t stack=0x130000;
 UnitFixture memory;
 OriginalUnitLifecycle lifecycle;
 const auto prepare=[&](uint32_t world,uint32_t offset,uint32_t objects,uint32_t index,bool linked){
   const uint32_t unit=world+offset,header=objects+0x1000,records=header+0x100;
   memory.Write32(0x2D45D4,world);
   memory.Write32(world+0x5AE0,0x1C);
   memory.Write32(world+0x5AE4,0x1D);
   memory.Write32(world+0x5AEC,0x100);
   memory.Write32(world+0x5CB4,linked?objects:0);
   memory.Write32(unit,0x1C);
   memory.Write32(unit+4,index);
   memory.Write32(unit+8,index);
   memory.Write32(unit+0x38,linked?1:0);
   memory.Write32(unit+0xC,header);
   memory.Write32(unit+0x14,objects);
   memory.Write32(header,24);
   memory.Write32(header+0x90,1);
   memory.Write32(header+0x1C,records);
   memory.Write32(objects,0x28EA60);
   memory.Write32(objects+0xD0,records);
   memory.Write32(objects+0xB0,0);
   memory.Write32(stack+4,unit);
   return unit;
 };
 const auto remove=[&] {
   const uint32_t unit=prepare(oldWorld,slotOffset,0x110000,7,true);
   memory.Write32(stack,0x001EB6DC); // ordinary unlink path
   assert(lifecycle.BeforeUnlink(memory,stack).slot==-1);
   memory.Write32(stack,0x001EBB6E); // early path, wrong unit slot
   assert(lifecycle.BeforeUnlink(memory,stack).slot==-1);
   memory.Write32(stack,0x001EBB62);
   std::thread foreign([&] { assert(lifecycle.BeforeUnlink(memory,stack).slot==-1); });
   foreign.join();
   const auto before=lifecycle.BeforeUnlink(memory,stack);
   assert(before.slot==0);
   memory.Write32(unit+0x38,0);
   assert(!lifecycle.AfterUnlink(memory,before)); // object is still in the world list
   memory.Write32(oldWorld+0x5CB4,0);
   assert(lifecycle.AfterUnlink(memory,before));
   assert(!lifecycle.ObserveRunningFrame(memory,oldWorld));
   assert(lifecycle.BeforeUnlink(memory,stack).slot==-1); // redundant teardown call
 };
 const auto relink=[&](uint32_t index,uint32_t offset,uint32_t scene,bool publish=true){
   constexpr uint32_t objects=0x190000;
   const uint32_t unit=prepare(newWorld,offset,objects,index,false);
   memory.Write32(newWorld+0x5AE4,0);
   memory.Write32(newWorld+0x5AEC,scene);
   assert(!lifecycle.BeforeLink(memory,stack)); // mapped memory alone is not a publication
   if(publish){
     memory.Write32(unit,0);
     const auto loading=lifecycle.BeforePoll(memory,unit);
     assert(loading==(offset==slotOffset?unit:0));
     memory.Write32(unit,0x1C);
     lifecycle.AfterPoll(memory,loading,1);
   }
   const auto candidate=lifecycle.BeforeLink(memory,stack);
   assert(candidate==((publish&&offset==slotOffset&&scene==0x100&&index==7)?unit:0));
   memory.Write32(unit+0x38,1);
   memory.Write32(newWorld+0x5CB4,objects);
   lifecycle.AfterLink(memory,candidate);
   assert(!lifecycle.ObserveRunningFrame(memory,newWorld));
   memory.Write32(newWorld+0x5AE4,0x1D);
 };
 remove();
 relink(7,slotOffset,0x100,false);
 assert(!lifecycle.ObserveRunningFrame(memory,newWorld));
 remove();
 relink(8,slotOffset,0x100); // different authored index
 assert(!lifecycle.ObserveRunningFrame(memory,newWorld));
 remove();
 relink(7,0x5250,0x100); // same authored index, different role
 assert(!lifecycle.ObserveRunningFrame(memory,newWorld));
 remove();
 relink(7,slotOffset,0x200); // same role and index, different scene
 assert(!lifecycle.ObserveRunningFrame(memory,newWorld));
 remove();
 relink(7,slotOffset,0x100);
 memory.Write32(newWorld+0x5CB4,0);
 assert(!lifecycle.ObserveRunningFrame(memory,newWorld)); // lost membership after link
 remove();
 relink(7,slotOffset,0x100);
 assert(lifecycle.ObserveRunningFrame(memory,newWorld));
 assert(!lifecycle.ObserveRunningFrame(memory,newWorld));
 std::cout<<"ORIGINAL_EARLY_UNIT_UNLINK_AND_RELINK_FIXTURE_PASS\n";
}

int main(){
 TestUnitMembership();
 TestAuthoredPropResolution();
 TestUnitLifecycle();
 OriginalPostFrameGate gate;
 const OriginalPostFrameSample valid{0x100000,0x120000,0x140000,0x150000,0x1C,0x1D,2,0x28CFE0};
 assert(!gate.Observe(valid,false));
 for(unsigned n=0;n<29;n++)assert(!gate.Observe(valid,true));
 assert(gate.StableOriginalFrames()==29u);
 assert(gate.Observe(valid,true));
 for(unsigned n=0;n<500;n++)assert(!gate.Observe(valid,true));
 assert(!gate.OriginalPlayer2Ready());
 bool foreignSaw=false;
 std::thread wrongThread([&]{
  foreignSaw=gate.Observe(valid,true);
  assert(gate.StableOriginalFrames()==0u);
  gate.Reset(); // foreign caller cannot reset the real game-thread state
 });wrongThread.join();
 assert(!foreignSaw&&gate.StableOriginalFrames()==30u);
 auto unloading=valid;unloading.unloadPhase=0x1E;
 assert(!gate.Observe(unloading,true)&&gate.StableOriginalFrames()==0u);
 for(unsigned n=0;n<29;n++)assert(!gate.Observe(valid,true));
 assert(gate.Observe(valid,true));
 auto bad=valid;bad.localKind=1;
 assert(!gate.Observe(bad,true)&&gate.StableOriginalFrames()==0);
 for(unsigned n=0;n<29;n++)assert(!gate.Observe(valid,true));
 assert(gate.Observe(valid,true));
 bad=valid;bad.world++;
 assert(!gate.Observe(bad,true));
 assert(gate.StableOriginalFrames()==1);
 for(unsigned n=0;n<28;n++)assert(!gate.Observe(bad,true));
 assert(gate.Observe(bad,true));
 bad=valid;bad.localVtable=0x28CFD0;
 assert(!gate.Observe(bad,true)&&gate.StableOriginalFrames()==0);
 bad=valid;bad.corePhase=0x1B;
 assert(!gate.Observe(bad,true)&&gate.StableOriginalFrames()==0);
 bad=valid;bad.physics=0;
 assert(!gate.Observe(bad,true));
 bad=valid;bad.owner=0;
 assert(!gate.Observe(bad,true));
 bad=valid;bad.npcPool=0;
 assert(!gate.Observe(bad,true));
 gate.Reset();assert(gate.StableOriginalFrames()==0);
 std::cout<<"ORIGINAL_POST_1F1080_FRAME_SOURCE_GATE_PASS\n";
}
