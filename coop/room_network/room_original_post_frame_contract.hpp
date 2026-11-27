// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Source-authentic ORIGINAL BLACK 0x001F1080 AFTER vanilla world tick.
// Pinned MIT PC b2512fafd3e181e4febfb298f297245b845ef628 uses exactly
// this post-update frame boundary for black_p2_tick. Read-only stage now.
// NOT a loaded-scene event, peer COMMIT, player-2 setup or shared AI proof.
#pragma once
#include "room_original_running_scene_source_gate.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <thread>
namespace blackcoop {
namespace unit_observation {
constexpr uint32_t kWorldRoot=0x002D45D4u, kObjectStride=0xF0u;
constexpr std::array<uint32_t,2> kUnitOffsets{0x49D0u,0x5250u};

struct Range {
 uint32_t objects=0, count=0, index=0;
};

template<class Memory>
uint32_t World(const Memory& memory) noexcept {
 if(!memory.Contains(kWorldRoot,4u))return 0;
 const uint32_t world=memory.Read32(kWorldRoot);
 return !(world&3u)&&memory.Contains(world,0x5CB8u)?world:0;
}

template<class Memory>
bool Running(const Memory& memory,uint32_t world) noexcept {
 return world&&World(memory)==world&&
   IsOriginalWorldRunning(memory.Read32(world+0x5AE0u),memory.Read32(world+0x5AE4u));
}

template<class Memory>
uint32_t Scene(const Memory& memory,uint32_t world) noexcept {
 return uint32_t(memory.Read8(world+0x5AECu))*256u+memory.Read8(world+0x5AEDu);
}

template<class Memory>
Range ReadRange(const Memory& memory,uint32_t unit,bool linked=true) noexcept {
   constexpr uint32_t kRecordStride=0x30u;
   if(memory.Read32(unit)!=0x1Cu||memory.Read8(unit+0x38u)!=static_cast<uint8_t>(linked))return {};
   const uint32_t index=memory.Read32(unit+8u);
   if(index==UINT32_MAX||index!=memory.Read32(unit+4u))return {};
   const uint32_t header=memory.Read32(unit+0xCu), objects=memory.Read32(unit+0x14u);
   if((header&3u)||!memory.Contains(header,0x92u))return {};
   if(memory.Read32(header)!=24u)return {};
   const uint32_t count=memory.Read16(header+0x90u), records=memory.Read32(header+0x1Cu);
   if(!count||count>1024u||(objects&3u)||(records&3u)||
      !memory.Contains(objects,count*kObjectStride)||!memory.Contains(records,count*kRecordStride))
     return {};
   for(uint32_t ordinal=0;ordinal<count;++ordinal){
     const uint32_t object=objects+ordinal*kObjectStride;
     if(memory.Read32(object)!=0x28EA60u||
        memory.Read32(object+0xD0u)!=records+ordinal*kRecordStride)return {};
   }
   return {objects,count,index};
}

template<class Memory>
int CountMembers(const Memory& memory,uint32_t world,const Range& range,
                 uint32_t stride=kObjectStride) noexcept {
   uint32_t node=memory.Read32(world+0x5CB4u), visited=0, matches=0;
   while(node){
     if(++visited>4096u||(node&3u)||!memory.Contains(node,0xB4u))return -1;
      if(node>=range.objects&&node-range.objects<range.count*stride){
        if((node-range.objects)%stride)return -1;
       ++matches;
     }
     node=memory.Read32(node+0xB0u);
   }
   return static_cast<int>(matches);
}

inline int UnitSlot(uint32_t world,uint32_t unit) noexcept {
 if(!world)return -1;
 if(unit==world+kUnitOffsets[0])return 0;
 if(unit==world+kUnitOffsets[1])return 1;
 return -1;
}
} // namespace unit_observation

// No addresses, ordinals, or lifetime handles leave this membership check.
template<class Memory>
bool HasOriginalWorldUnitMembership(const Memory& memory,uint32_t world) noexcept {
 using namespace unit_observation;
 if(!Running(memory,world))return false;
 for(const auto offset:kUnitOffsets){
   const auto range=ReadRange(memory,world+offset);
   if(range.count&&CountMembers(memory,world,range)==static_cast<int>(range.count))
     return Running(memory,world);
 }
 return false;
}

// Authored v24 kind-4 prop identity, scoped independently to the loaded
// level/stage. Live addresses and the local publication generation stay local.
struct OriginalUnitPropKey {
 uint8_t level=0,stage=0;
 uint32_t unitIndex=0,recordCount=0,ordinal=0;
};

struct OriginalUnitPropHandle {
 uint32_t unit=0,object=0,record=0;
 uint64_t generation=0;
 bool operator==(const OriginalUnitPropHandle&) const noexcept=default;
};

// Invalidate local handles around original stream, link and unlink calls,
// even when a call comes from outside the game thread. This is a publication
// fence, not a lock on original memory: resolve at the original frame boundary.
class OriginalUnitPublicationActivity final {
 std::atomic<uint64_t> generation_{1};
 std::atomic<unsigned> active_{0};
public:
 class Mutation final {
  OriginalUnitPublicationActivity& owner_;
 public:
  explicit Mutation(OriginalUnitPublicationActivity& owner) noexcept:owner_(owner){
   owner_.active_.fetch_add(1);
   auto generation=owner_.generation_.load();
   while(generation!=UINT64_MAX&&
         !owner_.generation_.compare_exchange_weak(generation,generation+1)) {}
  }
  ~Mutation(){owner_.active_.fetch_sub(1);}
  Mutation(const Mutation&)=delete;
  Mutation& operator=(const Mutation&)=delete;
 };

 uint64_t Generation() const noexcept {
  const auto value=generation_.load();
  if(value==UINT64_MAX||active_.load()!=0||generation_.load()!=value)return 0;
  return value;
 }
};

// The caller's game-thread generation source must revoke a publication on
// unlink/reload, including same-address reuse. Readable spans cannot do that.
template<class Memory,class Generation>
bool ResolveOriginalUnitProp(const Memory& memory,Generation generation,
      uint32_t world,const OriginalUnitPropKey& key,OriginalUnitPropHandle& out) noexcept {
 using namespace unit_observation;
 constexpr uint32_t kStride=0x200u,kRecordStride=0x50u;
 const uint32_t scene=uint32_t(key.level)*256u+key.stage;
 if(key.unitIndex==UINT32_MAX||!key.recordCount||key.ordinal>=key.recordCount||
    key.recordCount>UINT16_MAX||!Running(memory,world)||Scene(memory,world)!=scene)
  return false;

 uint32_t unit=0;
 for(const uint32_t offset:kUnitOffsets){
  const uint32_t candidate=world+offset;
  if(memory.Read32(candidate)!=0x1Cu||memory.Read8(candidate+0x38u)!=1u||
     memory.Read32(candidate+8u)!=key.unitIndex)
   continue;
  if(unit||memory.Read32(candidate+4u)!=key.unitIndex)return false;
  unit=candidate;
 }
 if(!unit)return false;
 const uint64_t serial=generation(world,unit);
 if(!serial)return false;

 // 001D3610 packs a subset of the authored rows in increasing order;
 // the packed slot cannot be used as an identity across runtimes.
 const uint32_t header=memory.Read32(unit+0xCu),objects=memory.Read32(unit+0x1Cu);
 const uint32_t count=memory.Read32(unit+0x34u);
 if(!count||count>4096u||count>key.recordCount||(header&3u)||(objects&3u)||
    !memory.Contains(header,0x94u)||memory.Read32(header)!=24u||
    memory.Read16(header+0x92u)!=key.recordCount||!memory.Contains(objects,count*kStride))
  return false;
 const uint32_t table=memory.Read32(header+0x2Cu);
 if((table&3u)||uint64_t(table)<uint64_t(header)+0x94u||
    !memory.Contains(table,key.recordCount*kRecordStride))
  return false;

 const uint32_t record=table+key.ordinal*kRecordStride;
 uint32_t object=0,previous=0;
 for(uint32_t i=0;i<count;++i){
  const uint32_t candidate=objects+i*kStride,row=memory.Read32(candidate+0x114u);
  if(memory.Read32(candidate)!=0x28ED98u||memory.Read32(candidate+0xC4u)!=4u||
     row<table||row<=previous||(row-table)%kRecordStride||
     (row-table)/kRecordStride>=key.recordCount||
     memory.Read32(candidate+8u)!=memory.Read32(row+0x48u)||
     memory.Read32(candidate+0xCu)!=memory.Read32(row+0x4Cu))
   return false;
  if(row==record)object=candidate;
  previous=row;
 }
 if(!object||CountMembers(memory,world,{object,1,0},kStride)!=1||
    !Running(memory,world)||Scene(memory,world)!=scene||
    generation(world,unit)!=serial)
  return false;
 out={unit,object,record,serial};
 return true;
}

struct OriginalUnitPropPartSample {
 OriginalUnitPropHandle prop{};
 uint32_t part=0,definition=0,healthBits=0;
 uint8_t partIndex=0,partCount=0;
};

// 001E5070 writes health at the head of each 0x0C part row. 001D4A50
// reads the definition at +4; UINT32_MAX is the invalidated descriptor.
// Neither field alone proves collision or removal of the entire prop.
template<class Memory,class Generation>
bool SampleOriginalUnitPropPart(const Memory& memory,Generation generation,
      uint32_t world,const OriginalUnitPropKey& key,uint8_t partIndex,
      OriginalUnitPropPartSample& out) noexcept {
 OriginalUnitPropHandle prop;
 if(!ResolveOriginalUnitProp(memory,generation,world,key,prop))return false;
 const uint32_t parts=memory.Read32(prop.object+0x1A0u);
 const uint8_t count=memory.Read8(prop.object+0x1F5u);
 if(partIndex>=count||(parts&3u)||!memory.Contains(parts,uint32_t(count)*0xCu))return false;
 const uint32_t part=parts+uint32_t(partIndex)*0xCu;
 const uint32_t health=memory.Read32(part),definition=memory.Read32(part+4u);
 if(health>=0x7F800000u||memory.Read32(prop.object+0x1A0u)!=parts||
    memory.Read8(prop.object+0x1F5u)!=count||
    !unit_observation::Running(memory,world)||
    unit_observation::Scene(memory,world)!=uint32_t(key.level)*256u+key.stage||
    generation(world,prop.unit)!=prop.generation)
  return false;
 out={prop,part,definition,health,partIndex,count};
 return true;
}

inline bool IsOriginalPartDepletion(const OriginalUnitPropPartSample& before,
                                    const OriginalUnitPropPartSample& after) noexcept {
 return before.prop.generation&&before.prop==after.prop&&before.part&&before.part==after.part&&
   before.partCount==after.partCount&&before.partIndex==after.partIndex&&
   before.partIndex<before.partCount&&before.definition!=UINT32_MAX&&
   before.healthBits&&before.healthBits<0x7F800000u&&
   after.healthBits==0&&after.definition==UINT32_MAX;
}

// Local diagnostic ledger only. The removals never export their ranges.
class OriginalUnitLifecycle final {
 using Range=unit_observation::Range;
 struct Entry {
   uint32_t scene=0, index=0, count=0, publishedWorld=0, publishedObjects=0;
   bool linked=false;
 };
 std::array<Entry,2> entries_{};
 const std::thread::id thread_=std::this_thread::get_id();
 public:
 struct UnlinkSample {
   uint32_t world=0, scene=0;
   int slot=-1;
   Range range{};
 };
 bool OnFrameThread()const noexcept{return thread_==std::this_thread::get_id();}

 template<class Memory>
 UnlinkSample BeforeUnlink(const Memory& memory,uint32_t stack) noexcept {
   using namespace unit_observation;
   if(!OnFrameThread()||(stack&3u)||!memory.Contains(stack,8u))return {};
   const uint32_t world=World(memory), unit=memory.Read32(stack+4u);
   const int slot=UnitSlot(world,unit);
   if(slot<0)return {};
   if(memory.Read8(unit+0x38u)!=1u){
     if(entries_[slot].linked)entries_[slot]={};
     return {};
   }
   entries_[slot]={};
   const uint32_t returnAddress=memory.Read32(stack);
   constexpr std::array<uint32_t,2> kEarlyUnlinkReturns{0x001EBB62u,0x001EBB6Eu};
   if(memory.Read32(world+0x5AE4u)!=0x1Du||returnAddress!=kEarlyUnlinkReturns[slot])return {};
   const auto range=ReadRange(memory,unit);
   if(!range.count||CountMembers(memory,world,range)!=static_cast<int>(range.count))return {};
   return {world,Scene(memory,world),slot,range};
 }

 template<class Memory>
 uint32_t BeforePoll(const Memory& memory,uint32_t unit) const noexcept {
   using namespace unit_observation;
   if(!OnFrameThread())return 0;
   const int slot=UnitSlot(World(memory),unit);
   return slot>=0&&entries_[slot].count&&memory.Read32(unit)!=0x1Cu?unit:0;
 }

 template<class Memory>
 void AfterPoll(const Memory& memory,uint32_t unit,uint32_t result) noexcept {
   using namespace unit_observation;
   if(!OnFrameThread()||!unit||(result&0xFFu)!=1u)return;
   const uint32_t world=World(memory);
   const int slot=UnitSlot(world,unit);
   if(slot<0)return;
   auto& entry=entries_[slot];
   const auto range=ReadRange(memory,unit,false);
   if(!range.count||range.count!=entry.count||range.index!=entry.index||Scene(memory,world)!=entry.scene){
     entry={};
     return;
   }
   entry.publishedWorld=world;
   entry.publishedObjects=range.objects;
   entry.linked=false;
 }

 template<class Memory>
 bool AfterUnlink(const Memory& memory,const UnlinkSample& before) noexcept {
   using namespace unit_observation;
   if(!OnFrameThread()||before.slot<0||World(memory)!=before.world||
      Scene(memory,before.world)!=before.scene||
      memory.Read8(before.world+kUnitOffsets[before.slot]+0x38u)||
      CountMembers(memory,before.world,before.range)!=0)return false;
   entries_[before.slot]={before.scene,before.range.index,before.range.count};
   return true;
 }

 template<class Memory>
 uint32_t BeforeLink(const Memory& memory,uint32_t stack) const noexcept {
   using namespace unit_observation;
   if(!OnFrameThread()||(stack&3u)||!memory.Contains(stack,8u))return 0;
   const uint32_t unit=memory.Read32(stack+4u);
   const uint32_t world=World(memory);
   const int slot=UnitSlot(world,unit);
   return slot>=0&&entries_[slot].publishedWorld==world&&memory.Read32(unit)==0x1Cu&&
     !memory.Read8(unit+0x38u)?unit:0;
 }

 template<class Memory>
 void AfterLink(const Memory& memory,uint32_t unit) noexcept {
   using namespace unit_observation;
   if(!OnFrameThread()||!unit)return;
   const uint32_t world=World(memory);
   const int slot=UnitSlot(world,unit);
   if(slot<0)return;
   auto& entry=entries_[slot];
   const auto range=ReadRange(memory,unit);
   if(!range.count||range.count!=entry.count||range.index!=entry.index||
      world!=entry.publishedWorld||range.objects!=entry.publishedObjects||
      Scene(memory,world)!=entry.scene||
      CountMembers(memory,world,range)!=static_cast<int>(range.count)){
     entry={};
     return;
   }
   entry.linked=true;
 }

 template<class Memory>
 bool ObserveRunningFrame(const Memory& memory,uint32_t world) noexcept {
   using namespace unit_observation;
   if(!OnFrameThread()||(!entries_[0].count&&!entries_[1].count)||!Running(memory,world))return false;
   const uint32_t scene=Scene(memory,world);
   bool relinked=false;
   for(unsigned slot=0;slot<entries_.size();++slot){
     auto& entry=entries_[slot];
     if(!entry.count)continue;
     if(scene!=entry.scene){entry={};continue;}
     if(!entry.linked)continue;
     if(entry.publishedWorld==world){
       const auto range=ReadRange(memory,world+kUnitOffsets[slot]);
       if(range.objects==entry.publishedObjects&&range.count==entry.count&&range.index==entry.index&&
          CountMembers(memory,world,range)==static_cast<int>(range.count))relinked=true;
     }
     entry={};
   }
   return relinked&&Running(memory,world)&&Scene(memory,world)==scene;
 }
};

struct OriginalPostFrameSample {
 uint32_t world=0,owner=0,physics=0,npcPool=0;
 uint32_t corePhase=0,unloadPhase=0,localKind=0,localVtable=0;
};
class OriginalPostFrameGate final {
 const std::thread::id thread_=std::this_thread::get_id();
 bool onThread()const noexcept{return thread_==std::this_thread::get_id();}
 OriginalPostFrameSample current_{};
 unsigned consecutive_=0;
 bool logged_=false;
 static bool same(const OriginalPostFrameSample& a,const OriginalPostFrameSample& b)noexcept{
  return a.world==b.world&&a.owner==b.owner&&
    a.physics==b.physics&&a.npcPool==b.npcPool;
 }
public:
 void Reset()noexcept{if(!onThread())return;current_={};consecutive_=0;logged_=false;}
 // Returns true only ONCE after 30 consecutive genuine stable frames.
 bool Observe(const OriginalPostFrameSample& sample,bool originalPostTick)noexcept{
  if(!onThread()||!originalPostTick||sample.world<0x10000u||sample.owner<0x10000u||
     sample.physics<0x10000u||sample.npcPool<0x10000u||
     !IsOriginalWorldRunning(sample.corePhase,sample.unloadPhase)||sample.localKind!=2u||
     sample.localVtable!=0x0028CFE0u){
   Reset();return false;
  }
  if(!same(current_,sample)){
   current_=sample;consecutive_=0;logged_=false;
  }
  if(consecutive_<30u)++consecutive_;
  if(logged_||consecutive_<30u)return false;
  logged_=true;return true;
 }
 unsigned StableOriginalFrames()const noexcept{return onThread()?consecutive_:0u;}
 // Source candidate only; this never authorizes original native P2 setup.
 bool OriginalPlayer2Ready()const noexcept{return false;}
};
} // namespace blackcoop
