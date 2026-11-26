// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_objective_snapshot.hpp"
#include <cassert>
#include <iostream>
using namespace blackcoop;
namespace {
constexpr OriginalObjectiveDescriptorId kA{0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x80};
constexpr OriginalObjectiveDescriptorId kB{0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0xff};

void TestExactBytes(){
 const OriginalObjectiveValue input{kA,0x1234u,3u};
 const std::array<OriginalObjectiveValue,1> values{input};
 const std::array<OriginalObjectiveDescriptorId,1> expected{kA};
 // Independent golden bytes: do not use the encoder to create this oracle.
 const std::vector<uint8_t> golden{
  0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x80,0x34,0x12,0,0,3,0,0,0};
 std::vector<uint8_t> encoded{0xaa};
 assert(EncodeOriginalObjectiveValues(values,encoded)&&encoded==golden);
 std::vector<OriginalObjectiveValue> prepared;
 assert(PrepareOriginalObjectiveValues(golden,expected,prepared));
 assert(prepared.size()==1&&prepared[0]==input);
 assert(prepared[0].descriptorId[7]==0x80);
}

void TestIndependentProgress(){
 std::array<OriginalObjectiveValue,1> input{OriginalObjectiveValue{kA,2u,1u}};
 const std::array<OriginalObjectiveDescriptorId,1> expected{kA};
 std::vector<uint8_t> before,after;
 assert(EncodeOriginalObjectiveValues(input,before));
 input[0].counter=3;
 assert(EncodeOriginalObjectiveValues(input,after));
 assert(before!=after&&before[8]==2&&after[8]==3);
 assert(std::equal(before.begin(),before.begin()+8,after.begin()));
 assert(std::equal(before.begin()+12,before.end(),after.begin()+12));
 std::vector<OriginalObjectiveValue> first,second;
 assert(PrepareOriginalObjectiveValues(before,expected,first));
 assert(PrepareOriginalObjectiveValues(after,expected,second));
 assert(first[0].counter==2&&second[0].counter==3);
 assert(first[0].state==1&&second[0].state==1);
 assert(first[0].descriptorId==second[0].descriptorId);
}

void TestDetachedCopies(){
 std::vector<OriginalObjectiveValue> host{{kA,7u,1u},{kB,9u,2u}};
 const auto original=host;
 std::vector<OriginalObjectiveDescriptorId> expected{kA,kB};
 const auto originalIds=expected;
 std::vector<uint8_t> bytes;
 assert(EncodeOriginalObjectiveValues(host,bytes));
 const auto originalBytes=bytes;
 std::array<std::vector<OriginalObjectiveValue>,3> guests;
 for(auto& guest:guests){
  assert(PrepareOriginalObjectiveValues(bytes,expected,guest));
  assert(guest==original);
 }
 assert(host==original&&expected==originalIds&&bytes==originalBytes);
 assert(guests[0].data()!=guests[1].data()&&guests[1].data()!=guests[2].data());
 // Relocate/discard the caller-owned buffers; prepared values own no views.
 host.clear();expected.clear();std::fill(bytes.begin(),bytes.end(),0);
 for(const auto& guest:guests)assert(guest==original);
 guests[0][0].counter=0;
 assert(guests[1][0].counter==7&&guests[2][0].counter==7);
}

void TestExactLayoutAndAmbiguity(){
 const std::array<OriginalObjectiveValue,2> values{
  OriginalObjectiveValue{kA,7u,1u},OriginalObjectiveValue{kB,9u,2u}};
 const std::array<OriginalObjectiveDescriptorId,2> expected{kA,kB};
 const std::vector<OriginalObjectiveValue> sentinel{{kB,91u,2u}};
 std::vector<OriginalObjectiveValue> output=sentinel;
 std::vector<uint8_t> bytes;
 assert(EncodeOriginalObjectiveValues(values,bytes));
 auto wrong=expected;wrong[0][7]^=0x80;
 assert(!PrepareOriginalObjectiveValues(bytes,wrong,output)&&output==sentinel);
 const std::array<OriginalObjectiveDescriptorId,2> reordered{kB,kA};
 assert(!PrepareOriginalObjectiveValues(bytes,reordered,output)&&output==sentinel);
 assert(!PrepareOriginalObjectiveValues(bytes,std::span(expected).first(1),output)&&output==sentinel);
 const std::array<OriginalObjectiveValue,2> duplicate{
  OriginalObjectiveValue{kA,7u,1u},OriginalObjectiveValue{kA,9u,2u}};
 const std::array<OriginalObjectiveDescriptorId,2> duplicateIds{kA,kA};
 assert(EncodeOriginalObjectiveValues(duplicate,bytes)); // encoding is lossless
 assert(bytes.size()==32&&bytes[8]==7&&bytes[24]==9);
 assert(!PrepareOriginalObjectiveValues(bytes,duplicateIds,output)&&output==sentinel);
}

void TestValidationIsAtomic(){
 std::array<OriginalObjectiveValue,2> values{
  OriginalObjectiveValue{kA,7u,1u},OriginalObjectiveValue{kB,9u,2u}};
 const std::array<OriginalObjectiveDescriptorId,2> expected{kA,kB};
 const std::vector<uint8_t> byteSentinel{0xaa,0xbb};
 std::vector<uint8_t> outputBytes=byteSentinel,valid;
 assert(EncodeOriginalObjectiveValues(values,valid));
 values[1].counter=65536;
 assert(!EncodeOriginalObjectiveValues(values,outputBytes)&&outputBytes==byteSentinel);
 values[1].counter=9;values[1].state=4;
 assert(!EncodeOriginalObjectiveValues(values,outputBytes)&&outputBytes==byteSentinel);
 const std::vector<OriginalObjectiveValue> sentinel{{kA,42u,2u}};
 std::vector<OriginalObjectiveValue> output=sentinel;
 auto bad=valid;bad[26]=1; // second row counter exceeds 65535
 assert(!PrepareOriginalObjectiveValues(bad,expected,output)&&output==sentinel);
 bad=valid;bad[28]=4; // second row state exceeds the reference's accepted range
 assert(!PrepareOriginalObjectiveValues(bad,expected,output)&&output==sentinel);
 bad=valid;bad.pop_back();
 assert(!PrepareOriginalObjectiveValues(bad,expected,output)&&output==sentinel);
 bad=valid;bad.push_back(0);
 assert(!PrepareOriginalObjectiveValues(bad,expected,output)&&output==sentinel);
 assert(valid.size()==32&&valid[24]==9&&valid[28]==2);
}

void TestEmptyAndCapacityBounds(){
 std::vector<uint8_t> bytes{1,2};
 std::vector<OriginalObjectiveValue> output{{kA,3u,1u}};
 assert(EncodeOriginalObjectiveValues({},bytes)&&bytes.empty());
 assert(PrepareOriginalObjectiveValues({}, {},output)&&output.empty());
 // The source does not require a nonzero descriptor ID.
 const std::array<OriginalObjectiveValue,1> zero{OriginalObjectiveValue{{},0u,0u}};
 const std::array<OriginalObjectiveDescriptorId,1> zeroId{};
 assert(EncodeOriginalObjectiveValues(zero,bytes));
 assert(PrepareOriginalObjectiveValues(bytes,zeroId,output)&&output[0]==zero[0]);
 std::vector<OriginalObjectiveValue> values;
 std::vector<OriginalObjectiveDescriptorId> expected;
 for(size_t i=0;i<kOriginalObjectiveMaxRecords;++i){
  OriginalObjectiveDescriptorId id{};
  id[0]=static_cast<uint8_t>(i);id[1]=static_cast<uint8_t>(i>>8u);id[7]=0x80;
  expected.push_back(id);values.push_back({id,65535u,3u});
 }
 assert(EncodeOriginalObjectiveValues(values,bytes));
 assert(bytes.size()==4096u*16u);
 assert(PrepareOriginalObjectiveValues(bytes,expected,output)&&output==values);
 const auto acceptedBytes=bytes;
 values.push_back({kB,1u,1u});expected.push_back(kB);
 assert(!EncodeOriginalObjectiveValues(values,bytes)&&bytes==acceptedBytes);
 const auto acceptedOutput=output;
 const std::vector<uint8_t> tooManyRecords(4097u*16u);
 assert(!PrepareOriginalObjectiveValues(tooManyRecords,expected,output)&&output==acceptedOutput);
}
} // namespace

int main(){
 TestExactBytes();TestIndependentProgress();TestDetachedCopies();
 TestExactLayoutAndAmbiguity();TestValidationIsAtomic();TestEmptyAndCapacityBounds();
 std::cout<<"ORIGINAL_OBJECTIVE_VALUES_PASS scenarios=6 POINTER_FREE_SYNTHETIC_ONLY\n";
}
