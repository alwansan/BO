// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_host_player_storage.hpp"
#include <cassert>
#include <map>
#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>
struct StorageIO {
 uint64_t scene=3;uint32_t next=0x90000;unsigned failAt=0,allocations=0,calls=0;
 bool wrongReturn=false,borrow=false,duplicate=false,partial=false,throwCamera=false,failFree=false;std::map<uint32_t,uint32_t> words;
 std::map<uint32_t,uint32_t> owned;std::vector<uint32_t> freed;
 StorageIO(){words[0x2D45D4]=0x10000;words[0x2D12B8]=0x20000;words[0x2D45F4]=0x30000;words[0x2D1980]=0x50000;}
 uint64_t Scene()const{return scene;}
 bool Span(uint32_t p,uint32_t n)const{return p>=0x10000&&uint64_t(p)+n<0x400000;}
 bool Read32(uint32_t p,uint32_t& value){if(!Span(p,4))return false;value=words[p];return true;}
 uint32_t Alloc(uint32_t n){++allocations;if(failAt==allocations)return 0;if(borrow)return 0x10000;if(duplicate&&allocations==2)return 0x90000;
  uint32_t p=next;next+=(n+0xFFF)&~0xFFFu;owned[p]=n;return p;}
 bool Owns(uint32_t p,uint32_t n)const{auto i=owned.find(p);return i!=owned.end()&&i->second==n;}
 bool Free(uint32_t p){assert(owned.count(p));assert(std::find(freed.begin(),freed.end(),p)==freed.end());if(failFree)return false;freed.push_back(p);owned.erase(p);return true;}
 bool Camera(uint32_t owner,uint32_t& result){++calls;assert(Owns(owner,0x16C0));words[owner]=0x28D12C;words[owner+0x790]=0x28C3B8;
  if(partial)words[owner+4]=0x77700;
  if(throwCamera)throw 1;
  result=wrongReturn?0:owner;return true;}
};
int main(){using namespace blackcoop;
 OriginalPlayerStorageScene binding{3,0x10000,0x20000,0x30000,0x50000};
 {StorageIO io;OriginalHostPlayerStorage storage;assert(storage.Allocate(io,binding)==OriginalStorageStatus::ConstructorReady);
  auto view=storage.View();assert(view.owner&&view.actor&&view.input&&view.weapons&&view.parts);
  assert(view.partsBytes==0x630&&io.allocations==5&&io.calls==1);
  assert(storage.ReleaseConstructorOnly(io)==OriginalStorageStatus::Released);assert(io.owned.empty()&&io.freed.size()==5);}
 for(unsigned fail=1;fail<=5;fail++){StorageIO io;io.failAt=fail;OriginalHostPlayerStorage storage;
  assert(storage.Allocate(io,binding)==OriginalStorageStatus::AllocationFailed);assert(io.calls==0&&io.owned.empty());assert(io.freed.size()==fail-1);}
 {StorageIO io;io.failAt=3;io.failFree=true;OriginalHostPlayerStorage storage;
  assert(storage.Allocate(io,binding)==OriginalStorageStatus::CleanupFailed);assert(!storage.BeginOriginalSetup(io));
  io.failFree=false;assert(storage.ReleaseConstructorOnly(io)==OriginalStorageStatus::Released);assert(io.owned.empty());}
 {StorageIO io;io.borrow=true;OriginalHostPlayerStorage storage;assert(storage.Allocate(io,binding)==OriginalStorageStatus::UnsafeAllocation);assert(io.freed.empty()&&io.calls==0);}
 {StorageIO io;io.duplicate=true;OriginalHostPlayerStorage storage;assert(storage.Allocate(io,binding)==OriginalStorageStatus::UnsafeAllocation);assert(io.freed.size()==1&&io.calls==0);}
 {StorageIO io;io.wrongReturn=true;OriginalHostPlayerStorage storage;assert(storage.Allocate(io,binding)==OriginalStorageStatus::ConstructorFailed);assert(io.calls==1&&io.owned.empty());}
 for(unsigned mode=0;mode<2;mode++){StorageIO io;io.partial=mode==0;io.throwCamera=mode==1;OriginalHostPlayerStorage storage;
  assert(storage.Allocate(io,binding)==OriginalStorageStatus::EngineCleanupRequired);
  assert(io.freed.empty()&&io.owned.size()==5);assert(storage.ReleaseConstructorOnly(io)==OriginalStorageStatus::EngineCleanupRequired);}
 {StorageIO io;OriginalHostPlayerStorage storage;assert(storage.Allocate(io,binding)==OriginalStorageStatus::ConstructorReady);
  assert(storage.BeginOriginalSetup(io));assert(!storage.BeginOriginalSetup(io));assert(storage.ReleaseConstructorOnly(io)==OriginalStorageStatus::EngineCleanupRequired);assert(io.freed.empty());}
 {StorageIO io;io.scene++;OriginalHostPlayerStorage storage;assert(storage.Allocate(io,binding)==OriginalStorageStatus::SceneMismatch);assert(io.allocations==0);}
 {StorageIO io;OriginalHostPlayerStorage storage;assert(storage.Allocate(io,binding)==OriginalStorageStatus::ConstructorReady);
  io.words[storage.View().actor+0xB4]=0x77700;assert(storage.ReleaseConstructorOnly(io)==OriginalStorageStatus::EngineCleanupRequired);assert(io.freed.empty());}
 {StorageIO io;OriginalHostPlayerStorage storage;OriginalStorageStatus status{};std::thread t([&]{status=storage.Allocate(io,binding);});t.join();assert(status==OriginalStorageStatus::WrongThread);assert(io.allocations==0);}
 std::cout<<"ORIGINAL_PLAYER_STORAGE_PASS: transactional owned guest buffers, real camera call contract and guarded cleanup lifecycle\n";
}
