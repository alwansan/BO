// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Record shape and compatibility bounds informed by the MIT PC reference:
// portingpete/Black-2006-Recomp@b2512fafd3, src/black_checkpoint.inc.
// See room_original_objective_snapshot.LICENSE and the source note in docs.
// Detached values ONLY: no game pointers, checkpoint files, native memory,
// authenticated ticket, live objective callback or network protocol is implied.
#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace blackcoop {
using OriginalObjectiveDescriptorId=std::array<uint8_t,8>;
struct OriginalObjectiveValue {
 OriginalObjectiveDescriptorId descriptorId{};
 uint32_t counter=0,state=0; // state remains opaque; no native enum is proven
 bool operator==(const OriginalObjectiveValue&)const=default;
};
// These are the reference checkpoint's acceptance bounds, not proven limits
// for every Android build or every possible live objective transition.
inline constexpr size_t kOriginalObjectiveRecordBytes=16u;
inline constexpr size_t kOriginalObjectiveMaxRecords=4096u;
inline constexpr uint32_t kOriginalObjectiveMaxCounter=65535u;
inline constexpr uint32_t kOriginalObjectiveMaxState=3u;

namespace objective_values_detail {
inline uint32_t ReadLe32(const uint8_t* p)noexcept{
 return uint32_t(p[0])|(uint32_t(p[1])<<8u)|
        (uint32_t(p[2])<<16u)|(uint32_t(p[3])<<24u);
}
inline void WriteLe32(uint8_t* p,uint32_t value)noexcept{
 for(unsigned i=0;i<4u;++i)p[i]=static_cast<uint8_t>(value>>(8u*i));
}
inline bool Valid(const OriginalObjectiveValue& value)noexcept{
 return value.counter<=kOriginalObjectiveMaxCounter&&
        value.state<=kOriginalObjectiveMaxState;
}
} // namespace objective_values_detail

// Explicit portable record bytes: ID[8] unchanged, counter LE32, state LE32.
// This is not RoomObjectEventWire and is not a complete checkpoint format.
// Duplicate/zero/high-bit IDs are representable without modifying their bits.
// Validation failure or allocation exceptions leave output unchanged.
inline bool EncodeOriginalObjectiveValues(std::span<const OriginalObjectiveValue> values,
                                         std::vector<uint8_t>& output){
 if(values.size()>kOriginalObjectiveMaxRecords)return false;
 for(const auto& value:values)if(!objective_values_detail::Valid(value))return false;
 std::vector<uint8_t> candidate(values.size()*kOriginalObjectiveRecordBytes);
 for(size_t i=0;i<values.size();++i){
  auto* row=candidate.data()+i*kOriginalObjectiveRecordBytes;
  std::copy(values[i].descriptorId.begin(),values[i].descriptorId.end(),row);
  objective_values_detail::WriteLe32(row+8u,values[i].counter);
  objective_values_detail::WriteLe32(row+12u,values[i].state);
 }
 output.swap(candidate);return true;
}

// Prepare a detached result only for the exact expected descriptor layout.
// expectedIds must eventually come from an INDEPENDENT verified local source,
// never from copiedRecords themselves. Matching this table does not establish
// signed mission identity, native lifetime or permission to apply game state.
// Duplicate rejection is an additional conservative co-op policy; the PC
// reference does not establish native descriptor uniqueness across instances.
inline bool PrepareOriginalObjectiveValues(std::span<const uint8_t> copiedRecords,
       std::span<const OriginalObjectiveDescriptorId> expectedIds,
       std::vector<OriginalObjectiveValue>& output){
 if(expectedIds.size()>kOriginalObjectiveMaxRecords||
    copiedRecords.size()!=expectedIds.size()*kOriginalObjectiveRecordBytes)return false;
 std::vector<OriginalObjectiveDescriptorId> sortedIds;
 sortedIds.reserve(expectedIds.size());
 for(const auto& id:expectedIds)sortedIds.push_back(id);
 std::sort(sortedIds.begin(),sortedIds.end());
 if(std::adjacent_find(sortedIds.begin(),sortedIds.end())!=sortedIds.end())return false;
 std::vector<OriginalObjectiveValue> candidate;
 candidate.reserve(expectedIds.size());
 for(size_t i=0;i<expectedIds.size();++i){
  const auto* row=copiedRecords.data()+i*kOriginalObjectiveRecordBytes;
  if(!std::equal(expectedIds[i].begin(),expectedIds[i].end(),row))return false;
  OriginalObjectiveValue value{expectedIds[i],
   objective_values_detail::ReadLe32(row+8u),objective_values_detail::ReadLe32(row+12u)};
  if(!objective_values_detail::Valid(value))return false;
  candidate.push_back(value);
 }
 output.swap(candidate);return true;
}
} // namespace blackcoop
