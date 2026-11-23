// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalPlayerStorageScene {uint64_t scene=0;uint32_t core=0,owner=0,physics=0,npcs=0;};
struct OriginalPlayerStorageView {
 uint32_t owner=0,actor=0,input=0,weapons=0,parts=0;
 uint32_t partsBytes=0x630; // 0x70+0x3C0 scratch + 0x200 model workspace
};
enum class OriginalStorageStatus {ConstructorReady,Released,AllocationFailed,UnsafeAllocation,ConstructorFailed,
 SceneMismatch,WrongThread,AlreadyOwned,EngineCleanupRequired,CleanupFailed};
// ONLY guest storage and the allocation-free original 14380 constructor.
// No actor copy, camera singleton swap, asset load, physics or player setup.
// IO: safe Span/Read32, live Scene, tracked Alloc/Owns/Free, actual Camera call.
class OriginalHostPlayerStorage {
 struct Block {uint32_t address=0,size=0;};
 std::array<Block,5> blocks_{};
 const std::thread::id thread_=std::this_thread::get_id();
 OriginalPlayerStorageScene scene_{};
 bool constructorReady_=false;
 enum class State {Empty,ConstructorOnly,EngineTouched};State state_=State::Empty;
 static bool range(uint32_t p,uint32_t n){return p>=0x10000&&n&&uint64_t(p)+n<=0x100000000ull;}
 static bool overlap(uint32_t a,uint32_t n,uint32_t b,uint32_t m){return uint64_t(a)<uint64_t(b)+m&&uint64_t(a)+n>b;}
 bool owner()const{return thread_==std::this_thread::get_id();}
 template<class IO>bool scene(IO& io,const OriginalPlayerStorageScene& b){
  uint32_t c=0,o=0,p=0,n=0;
  return b.scene&&io.Scene()==b.scene&&range(b.core,0x5CC0)&&range(b.owner,0x16C0)&&range(b.physics,0x2974)&&range(b.npcs,0x8540)&&
   io.Span(b.core,0x5CC0)&&io.Span(b.owner,0x16C0)&&io.Span(b.physics,0x2974)&&io.Span(b.npcs,0x8540)&&
   io.Read32(0x2D45D4,c)&&c==b.core&&io.Read32(0x2D12B8,o)&&o==b.owner&&
   io.Read32(0x2D45F4,p)&&p==b.physics&&io.Read32(0x2D1980,n)&&n==b.npcs;
 }
 static uint32_t constructorWord(uint32_t offset){
  // Exact writes in c5 14380..1448F, including five 0x60 embedded objects.
  constexpr uint32_t pairs[][2]={{0,0x28D12C},{0x18,0x28CDFC},{0x38,0x28CB0C},{0x50,0x28C95C},
   {0xD0,0x28C4D0},{0xF0,0x28C19C},{0x170,0x28BD78},{0x190,0x28CBE4},{0x238,0x28C4D4},
   {0x23C,0x28C130},{0x240,0x28BED0},{0x2A0,0x28D118},{0x2C0,0x28C3B8},{0x310,0x28C3B8},
   {0x370,0x28D5AC},{0x390,0x28D2B0},{0x400,0x28C918},{0x404,0x28D5AC},{0x420,0x28CEBC},
   {0x460,0x28CB10},{0x4B0,0x28CB10},{0x4F0,0x28BFB4},{0x520,0x28C48C},
   {0x550,0x28C1A0},{0x5B0,0x28C1A0},{0x610,0x28C1A0},{0x670,0x28C1A0},{0x6D0,0x28C1A0},
   {0x740,0x28C3B8},{0x790,0x28C3B8}};
  for(const auto& pair:pairs)if(offset==pair[0])return pair[1];
  return 0;
 }
 template<class IO>bool pristine(IO& io){
  for(unsigned i=0;i<blocks_.size();i++)if(blocks_[i].address){
   const auto& b=blocks_[i];if(!io.Owns(b.address,b.size)||!io.Span(b.address,b.size))return false;
   for(uint32_t off=0;off<b.size;off+=4){uint32_t word=0;
    if(!io.Read32(b.address+off,word))return false;
    if(word&&!(i==0&&word==constructorWord(off)))return false;
   }
  }
  return true;
 }
 template<class IO>bool rollback(IO& io){
  constructorReady_=false;
  bool ok=true;
  for(unsigned i=static_cast<unsigned>(blocks_.size());i-->0;)if(blocks_[i].address){
   try{if(io.Owns(blocks_[i].address,blocks_[i].size)&&io.Free(blocks_[i].address))blocks_[i]={};else ok=false;}
   catch(...){ok=false;}
  }
  state_=ok?State::Empty:State::ConstructorOnly;return ok;
 }
public:
 OriginalHostPlayerStorage()=default;
 OriginalHostPlayerStorage(const OriginalHostPlayerStorage&)=delete;
 OriginalHostPlayerStorage& operator=(const OriginalHostPlayerStorage&)=delete;
 // No destructor engine call: cleanup must occur on its owning game thread.
 OriginalPlayerStorageView View()const{return owner()?OriginalPlayerStorageView{
  blocks_[0].address,blocks_[1].address,blocks_[2].address,blocks_[3].address,blocks_[4].address,0x630}:OriginalPlayerStorageView{};}
 template<class IO>OriginalStorageStatus Allocate(IO& io,const OriginalPlayerStorageScene& binding){
  if(!owner())return OriginalStorageStatus::WrongThread;
  if(state_!=State::Empty)return OriginalStorageStatus::AlreadyOwned;
  if(!scene(io,binding))return OriginalStorageStatus::SceneMismatch;
  scene_=binding;constexpr uint32_t sizes[]={0x16C0,0x900,0x40,0x40,0x630};
  state_=State::ConstructorOnly;
  bool cameraAttempted=false;
  try{
   for(unsigned i=0;i<blocks_.size();i++){
    uint32_t address=io.Alloc(sizes[i]);
    if(!address)return rollback(io)?OriginalStorageStatus::AllocationFailed:OriginalStorageStatus::CleanupFailed;
    bool safe=range(address,sizes[i])&&address%16==0&&io.Owns(address,sizes[i])&&io.Span(address,sizes[i])&&
     !overlap(address,sizes[i],binding.core,0x5CC0)&&!overlap(address,sizes[i],binding.owner,0x16C0)&&
     !overlap(address,sizes[i],binding.physics,0x2974)&&!overlap(address,sizes[i],binding.npcs,0x8540);
    for(unsigned j=0;j<i;j++)if(overlap(address,sizes[i],blocks_[j].address,blocks_[j].size))safe=false;
    // Never free an unsafe address supplied by a faulty allocator: it may
    // name original/borrowed memory or an interior/duplicate prior allocation.
    if(!safe)return rollback(io)?OriginalStorageStatus::UnsafeAllocation:OriginalStorageStatus::CleanupFailed;
    blocks_[i]={address,sizes[i]};
   }
   if(!pristine(io)){state_=State::EngineTouched;return OriginalStorageStatus::EngineCleanupRequired;}
   if(!scene(io,binding))return rollback(io)?OriginalStorageStatus::SceneMismatch:OriginalStorageStatus::CleanupFailed;
   uint32_t result=0;bool called=false;
   // An exception during guest execution leaves uncertain side effects:
   // quarantine instead of guessing which resources its partial state owns.
   cameraAttempted=true;
   try{called=io.Camera(blocks_[0].address,result);}catch(...){state_=State::EngineTouched;return OriginalStorageStatus::EngineCleanupRequired;}
   if(!pristine(io)){state_=State::EngineTouched;return OriginalStorageStatus::EngineCleanupRequired;}
   uint32_t first=0,last=0;
   if(!called||result!=blocks_[0].address||!io.Read32(blocks_[0].address,first)||first!=0x28D12C||
    !io.Read32(blocks_[0].address+0x790,last)||last!=0x28C3B8)
    return rollback(io)?OriginalStorageStatus::ConstructorFailed:OriginalStorageStatus::CleanupFailed;
   if(!scene(io,binding))return rollback(io)?OriginalStorageStatus::SceneMismatch:OriginalStorageStatus::CleanupFailed;
   constructorReady_=true;return OriginalStorageStatus::ConstructorReady;
  }catch(...){
   if(cameraAttempted){state_=State::EngineTouched;return OriginalStorageStatus::EngineCleanupRequired;}
   return rollback(io)?OriginalStorageStatus::AllocationFailed:OriginalStorageStatus::CleanupFailed;
  }
 }
 // MUST be called BEFORE the first original setup call or any borrowed-field
 // copying. Even failed/partial setup can acquire resources: no shallow free.
 template<class IO>bool BeginOriginalSetup(IO& io){
  if(!owner()||!constructorReady_||state_!=State::ConstructorOnly||!scene(io,scene_)||!pristine(io))return false;
  state_=State::EngineTouched;return true;
 }
 template<class IO>OriginalStorageStatus ReleaseConstructorOnly(IO& io){
  if(!owner())return OriginalStorageStatus::WrongThread;
  if(state_==State::EngineTouched)return OriginalStorageStatus::EngineCleanupRequired;
  if(!pristine(io)){state_=State::EngineTouched;return OriginalStorageStatus::EngineCleanupRequired;}
  return rollback(io)?OriginalStorageStatus::Released:OriginalStorageStatus::CleanupFailed;
 }
};
}
