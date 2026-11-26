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

int main(){
 TestUnitMembership();
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
