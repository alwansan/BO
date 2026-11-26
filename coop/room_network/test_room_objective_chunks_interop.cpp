// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Synthetic C++ -> Java chunk assembler -> C++ value preparation fixture.
// Only files under the caller's isolated test output directory are used.
#include "room_original_objective_story_scope.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace blackcoop;
namespace {
constexpr size_t kCount=4096u;
constexpr size_t kBytes=kCount*16u;

OriginalObjectiveStoryScope Scope(){
 OriginalObjectiveStoryScope scope;
 for(size_t i=0;i<scope.session.size();++i)scope.session[i]=static_cast<uint8_t>(0x80u+i);
 scope.token=0x0102030405060708LL;scope.rosterRevision=0xffffffffLL;
 scope.mission=7;scope.stage=65535;scope.sceneEpoch=2147483647;
 scope.participants=15;return scope;
}
OriginalObjectiveDescriptorId Id(size_t i){
 OriginalObjectiveDescriptorId id{};
 id[0]=static_cast<uint8_t>(i);id[1]=static_cast<uint8_t>(i>>8u);id[7]=0x80u;
 return id;
}
std::vector<OriginalObjectiveValue> Values(){
 std::vector<OriginalObjectiveValue> values;values.reserve(kCount);
 for(size_t i=0;i<kCount;++i)
  values.push_back({Id(i),static_cast<uint32_t>(i),static_cast<uint32_t>(i%4u)});
 return values;
}
std::vector<OriginalObjectiveDescriptorId> IndependentIds(){
 std::vector<OriginalObjectiveDescriptorId> ids;ids.reserve(kCount);
 for(size_t i=0;i<kCount;++i)ids.push_back(Id(i));
 return ids;
}
std::vector<uint8_t> Read(const std::filesystem::path& path){
 std::ifstream in(path,std::ios::binary);
 if(!in)throw std::runtime_error("Missing synthetic guest output");
 std::vector<uint8_t> data(kBytes);
 in.read(reinterpret_cast<char*>(data.data()),static_cast<std::streamsize>(data.size()));
 if(!in||in.peek()!=std::char_traits<char>::eof())
  throw std::runtime_error("Synthetic output has wrong length");
 return data;
}
void WriteHost(const std::filesystem::path& path){
 OriginalObjectiveStorySnapshot snapshot;
 const auto scope=Scope();const auto values=Values();
 assert(CaptureOriginalObjectiveStorySnapshot(scope,values,snapshot));
 assert(snapshot.scope==scope&&snapshot.records.size()==kBytes);
 // The first descriptor has its high bit set and a zero counter/state.
 const std::array<uint8_t,16> first{0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0};
 assert(std::equal(first.begin(),first.end(),snapshot.records.begin()));
 std::ofstream out(path,std::ios::binary|std::ios::trunc);
 out.write(reinterpret_cast<const char*>(snapshot.records.data()),
           static_cast<std::streamsize>(snapshot.records.size()));
 out.close();if(!out)throw std::runtime_error("Cannot write synthetic fixture");
 std::cout<<"OBJECTIVE_CHUNKS_CPP_FIXTURE_WRITE_PASS records=4096 bytes=65536\n";
}
void VerifyGuests(const std::filesystem::path& directory){
 const auto current=Scope();const auto expectedValues=Values();
 const auto expectedIds=IndependentIds();
 std::array<PreparedOriginalObjectiveStory,3> prepared;
 for(size_t i=0;i<prepared.size();++i){
  const auto path=directory/("guest-"+std::to_string(i+2u)+".bin");
  OriginalObjectiveStorySnapshot assembled{current,Read(path)};
  assert(PrepareOriginalObjectiveStorySnapshot(assembled,current,expectedIds,prepared[i]));
  assert(prepared[i].scope==current&&prepared[i].values==expectedValues);
  assert(prepared[i].values.front().descriptorId[7]==0x80u);
  assert(prepared[i].values.back().counter==4095u&&prepared[i].values.back().state==3u);
 }
 assert(prepared[0].values.data()!=prepared[1].values.data());
 assert(prepared[1].values.data()!=prepared[2].values.data());
 prepared[0].values[0].counter=65535u;
 assert(prepared[1].values==expectedValues&&prepared[2].values==expectedValues);
 std::cout<<"OBJECTIVE_CHUNKS_CPP_INTEROP_PASS guests=3 records=4096 FULL_ID_COUNTER_STATE NO_ANDROID_OR_TLS\n";
}
} // namespace

int main(int argc,char** argv){
 try{
  if(argc!=3)throw std::runtime_error("Expected write PATH or verify DIRECTORY");
  const std::string mode=argv[1];
  if(mode=="write")WriteHost(argv[2]);
  else if(mode=="verify")VerifyGuests(argv[2]);
  else throw std::runtime_error("Unknown synthetic fixture operation");
 }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
 return 0;
}
