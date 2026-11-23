// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original BLACK P2 actor/owner source snapshot and independent shadow plan.
// READ ONLY game RAM; never creates/sets up actor, calls guest, swaps P1,
// edits original AI/physics, or certifies playable co-op.
#pragma once
#include "room_original_host_player_storage.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <thread>
namespace blackcoop {
class OriginalHostPlayerShadowPlan final {
 std::thread::id thread_=std::this_thread::get_id();
 std::array<uint8_t,0x900> actor_{};
 // Untouched originals: compare before copying into guest P2 to reject a
 // stale source frame. Never expose donor bytes outside the native stage.
 std::array<uint8_t,0x900> donorActor_{};
 std::array<uint8_t,0x16C0> donorOwner_{};
 std::array<uint8_t,0x16C0> owner_{};
 std::array<uint8_t,0x630> parts_{}; // +0x430..+0x62F: separate 0x200 model workspace
 std::array<uint8_t,0x40> input_{}; // independent P2 controller snapshot
 bool ready_=false;
 bool modelIsolated_=false;
 bool inputIsolated_=false;
 OriginalPlayerStorageScene boundScene_{};
 OriginalPlayerStorageView boundTarget_{};
 uint32_t donorPartA_=0,donorPartB_=0,donorModel_=0;
 uint32_t donorManager_=0,donorInput_=0;
 static bool range(uint32_t p,uint32_t n)noexcept{
  return p>=0x10000u&&n&&uint64_t(p)+n<=0x100000000ull;
 }
 static bool overlap(uint32_t p,uint32_t n,uint32_t q,uint32_t m)noexcept{
  return uint64_t(p)<uint64_t(q)+m&&uint64_t(q)<uint64_t(p)+n;
 }
 static uint32_t get(const uint8_t* p)noexcept{
  uint32_t v=0;std::memcpy(&v,p,4);return v;
 }
 static void put(uint8_t* p,uint32_t v)noexcept{std::memcpy(p,&v,4);}
 bool onThread()const noexcept{return thread_==std::this_thread::get_id();}
public:
 OriginalHostPlayerShadowPlan()=default;
 OriginalHostPlayerShadowPlan(const OriginalHostPlayerShadowPlan&)=delete;
 OriginalHostPlayerShadowPlan& operator=(const OriginalHostPlayerShadowPlan&)=delete;
 ~OriginalHostPlayerShadowPlan(){if(onThread())Reset();}
 void Reset()noexcept{
  if(!onThread())return;
  actor_.fill(0);owner_.fill(0);parts_.fill(0);input_.fill(0);
  donorActor_.fill(0);donorOwner_.fill(0);
  ready_=false;modelIsolated_=false;inputIsolated_=false;
  boundScene_={};boundTarget_={};
  donorPartA_=donorPartB_=donorModel_=donorManager_=donorInput_=0;
 }
 bool Ready()const noexcept{return onThread()&&ready_;}
 // A separate donor model copy is an additional invariant; Ready alone does
 // not authorize original engine actor setup or native model ownership.
 bool ModelIsolated()const noexcept{return Ready()&&modelIsolated_;}
 bool InputIsolated()const noexcept{return Ready()&&inputIsolated_;}
 bool PreviewInputWord(uint32_t off,uint32_t& v)const noexcept{
  if(!InputIsolated()||off>0x3Cu||(off&3u))return false;
  v=get(input_.data()+off);return true;
 }
 bool PreviewModelWord(uint32_t off,uint32_t& v)const noexcept{
  if(!ModelIsolated()||off>0x1FCu||(off&3u))return false;
  v=get(parts_.data()+0x430u+off);return true;
 }
 // Preview only already-sanitized stage data; never output it to JNI/logs.
 bool PreviewActorWord(uint32_t off,uint32_t& v)const noexcept{
  if(!Ready()||off>0x8FCu||(off&3u))return false;
  v=get(actor_.data()+off);return true;
 }
 bool PreviewOwnerWord(uint32_t off,uint32_t& v)const noexcept{
  if(!Ready()||off>0x16BCu||(off&3u))return false;
  v=get(owner_.data()+off);return true;
 }
 template<class IO>bool Prepare(IO& io,const OriginalPlayerStorageScene& b,
                                const OriginalPlayerStorageView& t)noexcept{
  if(!onThread())return false;
  Reset();
  if(!b.scene||io.Scene()!=b.scene||!range(b.core,0x5CC0)||!range(b.owner,0x16C0)||
     !range(b.physics,0x2974)||!range(b.npcs,0x8540)||
     !range(t.owner,0x16C0)||!range(t.actor,0x900)||
     !range(t.input,0x40)||!range(t.weapons,0x40)||
     !range(t.parts,0x630)||t.partsBytes!=0x630)return false;
  const uint32_t targets[]={t.owner,t.actor,t.input,t.weapons,t.parts};
  const uint32_t sizes[]={0x16C0,0x900,0x40,0x40,0x630};
  const uint32_t roots[]={b.core,b.owner,b.physics,b.npcs};
  const uint32_t rootSizes[]={0x5CC0,0x16C0,0x2974,0x8540};
  for(unsigned i=0;i<5;i++){
   if(!io.Owns(targets[i],sizes[i])||!io.Span(targets[i],sizes[i]))return false;
   for(unsigned j=0;j<i;j++)
    if(overlap(targets[i],sizes[i],targets[j],sizes[j]))return false;
   for(unsigned j=0;j<4;j++)
    if(overlap(targets[i],sizes[i],roots[j],rootSizes[j]))return false;
  }
  uint32_t core=0,owner=0,physics=0,npcs=0;
  if(!io.Read32(0x2D45D4,core)||core!=b.core||
     !io.Read32(0x2D12B8,owner)||owner!=b.owner||
     !io.Read32(0x2D45F4,physics)||physics!=b.physics||
     !io.Read32(0x2D1980,npcs)||npcs!=b.npcs||
     !io.Span(b.core,0x5CC0)||!io.Span(b.owner,0x16C0)||
     !io.Span(b.physics,0x2974)||!io.Span(b.npcs,0x8540))return false;
  const uint32_t local=b.core+0x30u;
  uint32_t table=0,kind=0,partA=0,partB=0,sourceModel=0;
  uint32_t inputManager=0,sourceInput=0;
  if(!io.Read32(local,table)||table!=0x28CFE0u||
     !io.Read32(local+0xC4,kind)||kind!=2u||
     !io.Read32(local+0x34C,partA)||!io.Read32(local+0x350,partB)||
     !io.Read32(local+0x320,sourceModel)||
     !range(partA,0x70)||!range(partB,0x3C0)||
     !io.Span(partA,0x70)||!io.Span(partB,0x3C0))return false;
  for(unsigned i=0;i<5;i++)
   if(overlap(partA,0x70,targets[i],sizes[i])||
      overlap(partB,0x3C0,targets[i],sizes[i]))return false;
  // Pinned PC black_p2_update: source1 = GUEST32(GUEST32(core+0x354)+0xA8).
  // During the scripted intro there may be no source; partial stage only.
  if(!io.Read32(b.core+0x354u,inputManager))return false;
  if(inputManager){
   if(!range(inputManager,0xACu)||!io.Span(inputManager,0xACu)||
      !io.Read32(inputManager+0xA8u,sourceInput))return false;
   if(sourceInput){
    if(!range(sourceInput,0x40u)||!io.Span(sourceInput,0x40u))return false;
    for(unsigned i=0;i<5;i++)
     if(overlap(sourceInput,0x40u,targets[i],sizes[i]))return false;
   }
  }
  // Pinned MIT PC black_p2_swap isolates the original first-person model
  // at actor+0x320 into parts+0x430 (exactly 0x200 bytes). Do it only in
  // detached local bytes; NEVER keep a pointer into another player's model
  // in a model-isolated result. Absent model means partial staging only.
  if(sourceModel){
   if((sourceModel&3u)||!range(sourceModel,0x200)||!io.Span(sourceModel,0x200))return false;
   for(unsigned i=0;i<5;i++)
    if(overlap(sourceModel,0x200,targets[i],sizes[i]))return false;
  }
  if(!io.CopyGuest(local,actor_.data(),0x900)||
     !io.CopyGuest(b.owner,owner_.data(),0x16C0)||
     !io.CopyGuest(partA,parts_.data(),0x70)||
     !io.CopyGuest(partB,parts_.data()+0x70,0x3C0)||
     (sourceModel&&!io.CopyGuest(sourceModel,parts_.data()+0x430,0x200))||
     (sourceInput&&!io.CopyGuest(sourceInput,input_.data(),0x40))){
   Reset();return false;
  }
  if(get(actor_.data()+0x320)!=sourceModel){Reset();return false;}
  // Capture the source *before* rewriting actor and owner pointers.
  std::memcpy(donorActor_.data(),actor_.data(),0x900u);
  std::memcpy(donorOwner_.data(),owner_.data(),0x16C0u);
  // Installing the real second-pad pointer is reserved for a proven original
  // game update: the detached snapshot never sends a controller report.
  if(sourceInput)inputIsolated_=true;
  // Full 0x200 model bytes are independently owned in the proposed P2
  // parts allocation, but their internal runtime references remain unproven.
  if(sourceModel){
   put(actor_.data()+0x320,t.parts+0x430u);
   modelIsolated_=true;
  }
  // Pinned PC black_p2_start fields, isolated in HOST-only bytes.
  put(actor_.data()+0x30,0);
  put(actor_.data()+0x34,t.actor+0x34);
  put(actor_.data()+0x38,t.actor+0x34);
  put(actor_.data()+0xB4,0); // P1's collision proxy is never inherited
  put(actor_.data()+0x298,t.weapons);
  put(actor_.data()+0x29C,0);put(actor_.data()+0x2A0,0);
  actor_[0x2BB]=0xFF;
  put(actor_.data()+0x34C,t.parts);
  put(actor_.data()+0x350,t.parts+0x70);
  put(actor_.data()+0x8E4,1);put(actor_.data()+0x8E8,0);
  const uint64_t lo=b.owner,hi=lo+0x16C0;
  for(uint32_t off=0;off<0x16C0;off+=4){
   const uint32_t v=get(owner_.data()+off);
   if(uint64_t(v)>=lo&&uint64_t(v)<hi)
    put(owner_.data()+off,t.owner+(v-b.owner));
  }
  for(uint32_t off=0;off<0x900;off+=4){
   const uint32_t v=get(actor_.data()+off);
   if(uint64_t(v)>=lo&&uint64_t(v)<hi)
    put(actor_.data()+off,t.owner+(v-b.owner));
   else if(sourceInput&&v==sourceInput)
    put(actor_.data()+off,t.input);
  }
  uint32_t sameWorld=0,sameOwner=0,sameModel=0,samePartA=0,samePartB=0,sameManager=0,sameSource=0;
  if(io.Scene()!=b.scene||!io.Read32(0x2D45D4,sameWorld)||sameWorld!=b.core||
     !io.Read32(0x2D12B8,sameOwner)||sameOwner!=b.owner||
     !io.Read32(local+0x320,sameModel)||sameModel!=sourceModel||
     !io.Read32(local+0x34C,samePartA)||samePartA!=partA||
     !io.Read32(local+0x350,samePartB)||samePartB!=partB||
     !io.Read32(b.core+0x354u,sameManager)||sameManager!=inputManager||
     (inputManager&&(!io.Read32(inputManager+0xA8u,sameSource)||sameSource!=sourceInput))){
   Reset();return false;
  }
  boundScene_=b;boundTarget_=t;
  donorPartA_=partA;donorPartB_=partB;donorModel_=sourceModel;
  donorManager_=inputManager;donorInput_=sourceInput;
  ready_=true;return true;
 }
 // Read-only source freshness gate. No caller may carry the prepared P2
 // owner/actor/model/input across a P1 game update without re-preparing it.
 // The original actor's world-grid node is included, not ignored.
 template<class IO>bool SourceStillCurrent(IO& io)const{
  if(!Ready()||!ModelIsolated()||!InputIsolated()||
     !boundScene_.scene||io.Scene()!=boundScene_.scene)return false;
  uint32_t core=0,owner=0,physics=0,npcs=0,input=0;
  if(!io.Read32(0x2D45D4,core)||core!=boundScene_.core||
     !io.Read32(0x2D12B8,owner)||owner!=boundScene_.owner||
     !io.Read32(0x2D45F4,physics)||physics!=boundScene_.physics||
     !io.Read32(0x2D1980,npcs)||npcs!=boundScene_.npcs||
     !donorManager_||!donorModel_||!donorInput_||
     !io.Read32(donorManager_+0xA8u,input)||input!=donorInput_)return false;
  std::array<uint8_t,0x900> actor{};
  std::array<uint8_t,0x16C0> camera{};
  std::array<uint8_t,0x630> parts{};
  std::array<uint8_t,0x40> controller{};
  if(!io.CopyGuest(boundScene_.core+0x30u,actor.data(),0x900u)||
     !io.CopyGuest(boundScene_.owner,camera.data(),0x16C0u)||
     !io.CopyGuest(donorPartA_,parts.data(),0x70u)||
     !io.CopyGuest(donorPartB_,parts.data()+0x70u,0x3C0u)||
     !io.CopyGuest(donorModel_,parts.data()+0x430u,0x200u)||
     !io.CopyGuest(donorInput_,controller.data(),0x40u))return false;
  return std::memcmp(actor.data(),donorActor_.data(),0x900u)==0&&
         std::memcmp(camera.data(),donorOwner_.data(),0x16C0u)==0&&
         std::memcmp(parts.data(),parts_.data(),0x630u)==0&&
         std::memcmp(controller.data(),input_.data(),0x40u)==0;
 }
private:
 // Only Stage may write into P2 guest allocations, because it must first
 // transition original owned storage to EngineTouched / quarantine. The
 // public ShadowPlan may prepare and inspect but must NEVER copy directly.
 friend class OriginalHostPlayerStage;
 // Explicit opt-in, NEVER called by current Android gameplay. Writes only
 // five exact, separately tracked P2 allocations. Caller must mark the
 // storage engine-touched BEFORE entering: a partial write requires cleanup.
 // Actor is LAST so no partially copied actor can be mistaken for prepared.
 template<class IO>bool CopyIntoOwned(IO& io,
   const OriginalPlayerStorageScene& b,const OriginalPlayerStorageView& t){
  if(!Ready()||!ModelIsolated()||!InputIsolated()||!b.scene||
     b.scene!=boundScene_.scene||b.core!=boundScene_.core||
     b.owner!=boundScene_.owner||b.physics!=boundScene_.physics||
     b.npcs!=boundScene_.npcs||
     t.owner!=boundTarget_.owner||t.actor!=boundTarget_.actor||
     t.input!=boundTarget_.input||t.weapons!=boundTarget_.weapons||
     t.parts!=boundTarget_.parts||t.partsBytes!=0x630u||
     io.Scene()!=b.scene)return false;
  const uint32_t addresses[]={t.owner,t.actor,t.input,t.weapons,t.parts};
  const uint32_t sizes[]={0x16C0u,0x900u,0x40u,0x40u,0x630u};
  for(unsigned i=0;i<5;i++)
   if(!io.Owns(addresses[i],sizes[i])||!io.Span(addresses[i],sizes[i]))return false;
  uint32_t core=0,owner=0,physics=0,npcs=0,phase=0;
  uint32_t model=0,partA=0,partB=0,manager=0,input=0;
  const uint32_t local=b.core+0x30u;
  if(!io.Read32(0x2D45D4,core)||core!=b.core||
     !io.Read32(0x2D12B8,owner)||owner!=b.owner||
     !io.Read32(0x2D45F4,physics)||physics!=b.physics||
     !io.Read32(0x2D1980,npcs)||npcs!=b.npcs||
     !io.Read32(b.core+0x5AE0u,phase)||phase!=0x1Cu||
     !io.Read32(local+0x320u,model)||model!=donorModel_||
     !io.Read32(local+0x34Cu,partA)||partA!=donorPartA_||
     !io.Read32(local+0x350u,partB)||partB!=donorPartB_||
     !io.Read32(b.core+0x354u,manager)||manager!=donorManager_||
     !manager||!io.Read32(manager+0xA8u,input)||input!=donorInput_)
   return false;
  // Do not commit a stale source even if its root pointers are unchanged.
  if(!SourceStillCurrent(io))return false;
  // This operation does NOT register an original second actor, call setup,
  // swap original P1 context, attach a physical proxy or send input.
  const std::array<uint8_t,0x40> blankWeapons{};
  return io.WriteOwned(t.parts,parts_.data(),0x630u)&&
         io.WriteOwned(t.input,input_.data(),0x40u)&&
         io.WriteOwned(t.weapons,blankWeapons.data(),0x40u)&&
         io.WriteOwned(t.owner,owner_.data(),0x16C0u)&&
         io.WriteOwned(t.actor,actor_.data(),0x900u);
 }
};

// Phase A/B only: bind independently owned camera constructor allocations to
// a detached P1-derived actor snapshot. This does NOT write P2 into original
// game RAM, initialize weapons/physics/AI, swap P1 or prove a shared mission.
enum class OriginalHostPlayerStageStatus {
 Prepared, AlreadyOwned, WrongThread, StorageRejected, ShadowRejected, CleanupRequired, Incomplete, Materialized
};
class OriginalHostPlayerStage final {
 OriginalHostPlayerStorage storage_{};
 OriginalHostPlayerShadowPlan shadow_{};
 OriginalPlayerStorageScene binding_{};
 bool touched_=false,materialized_=false;
 const std::thread::id thread_=std::this_thread::get_id();
 bool onThread()const noexcept{return thread_==std::this_thread::get_id();}
public:
 OriginalHostPlayerStage()=default;
 OriginalHostPlayerStage(const OriginalHostPlayerStage&)=delete;
 OriginalHostPlayerStage& operator=(const OriginalHostPlayerStage&)=delete;
 // No destructor-driven guest free: owning game thread must release resources.
 bool Ready()const noexcept{return onThread()&&!touched_&&shadow_.Ready()&&storage_.View().actor!=0;}
 bool Materialized()const noexcept{return onThread()&&touched_&&materialized_;}
 bool ModelIsolated()const noexcept{return Ready()&&shadow_.ModelIsolated();}
 bool InputIsolated()const noexcept{return Ready()&&shadow_.InputIsolated();}
 OriginalPlayerStorageView View()const noexcept{
  return Ready()?storage_.View():OriginalPlayerStorageView{};
 }
 bool PreviewActorWord(uint32_t off,uint32_t& word)const noexcept{
  return Ready()&&shadow_.PreviewActorWord(off,word);
 }
 template<class IO>OriginalHostPlayerStageStatus Prepare(
     IO& io,const OriginalPlayerStorageScene& binding){
  if(!onThread())return OriginalHostPlayerStageStatus::WrongThread;
  if(storage_.View().owner||shadow_.Ready())return OriginalHostPlayerStageStatus::AlreadyOwned;
  const auto allocated=storage_.Allocate(io,binding);
  if(allocated!=OriginalStorageStatus::ConstructorReady){
   if(allocated==OriginalStorageStatus::EngineCleanupRequired||
      allocated==OriginalStorageStatus::CleanupFailed)
    return OriginalHostPlayerStageStatus::CleanupRequired;
   if(allocated==OriginalStorageStatus::AlreadyOwned)
    return OriginalHostPlayerStageStatus::AlreadyOwned;
   if(allocated==OriginalStorageStatus::WrongThread)
    return OriginalHostPlayerStageStatus::WrongThread;
   return OriginalHostPlayerStageStatus::StorageRejected;
  }
  if(!shadow_.Prepare(io,binding,storage_.View())){
   const auto released=storage_.ReleaseConstructorOnly(io);
   return released==OriginalStorageStatus::Released?
     OriginalHostPlayerStageStatus::ShadowRejected:
     OriginalHostPlayerStageStatus::CleanupRequired;
  }
  binding_=binding;
  return OriginalHostPlayerStageStatus::Prepared;
 }
 // The caller must retain this stage/IO for the scene lifetime. A failed
 // or successful guest-owned write permanently bars shallow FreeSystem.
 // There is NO automatic game setup, no readiness/room announcement.
 template<class IO>OriginalHostPlayerStageStatus MaterializeOwned(IO& io){
  if(!onThread())return OriginalHostPlayerStageStatus::WrongThread;
  if(touched_)return OriginalHostPlayerStageStatus::CleanupRequired;
  if(!Ready()||!ModelIsolated()||!InputIsolated())
   return OriginalHostPlayerStageStatus::Incomplete;
  uint32_t phase=0;
  if(io.Scene()!=binding_.scene||
     !io.Read32(binding_.core+0x5AE0u,phase)||phase!=0x1Cu||
     !shadow_.SourceStillCurrent(io))
   return OriginalHostPlayerStageStatus::Incomplete;
  if(!storage_.BeginOriginalSetup(io))return OriginalHostPlayerStageStatus::StorageRejected;
  touched_=true;
  try{
   if(!shadow_.CopyIntoOwned(io,binding_,storage_.View()))
    return OriginalHostPlayerStageStatus::CleanupRequired;
  }catch(...){return OriginalHostPlayerStageStatus::CleanupRequired;}
  materialized_=true;
  return OriginalHostPlayerStageStatus::Materialized;
 }
 template<class IO>OriginalStorageStatus ReleaseConstructorOnly(IO& io){
  if(!onThread())return OriginalStorageStatus::WrongThread;
  const auto result=storage_.ReleaseConstructorOnly(io);
  if(result==OriginalStorageStatus::Released){
   shadow_.Reset();binding_={};touched_=materialized_=false;
  }
  return result;
 }
};
} // namespace blackcoop
