// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original BLACK AI target-table survey; NO target insertion or AI authority.
// MIT PC reference: [[0x002D1988]]+0xFAC, 32 entries x 12 bytes,
// record kind+4 (0 in-use, 5 free), bound actor+8. Player +0x378 index.
#pragma once
#include <array>
#include <cstdint>
namespace blackcoop {
struct OriginalAiTargetRecord{
 uint32_t kind=0xffffffffu,actor=0;
};
struct OriginalAiTargets {
 uint8_t inUse=0,free=0,unknown=0;
 bool localOriginalPlayerBound=false;
 // 96: no free, 97: one, 98: two, 99: three+.
 uint8_t freeCode()const noexcept{
  return free==0?96:free==1?97:free==2?98:99;
 }
};
inline OriginalAiTargets CountOriginalAiTargets(
 const std::array<OriginalAiTargetRecord,32>& records,
 uint32_t originalPlayer,uint32_t originalPlayerTargetIndex)noexcept{
 OriginalAiTargets result{};
 for(uint32_t i=0;i<records.size();i++){
  const auto& e=records[i];
  if(e.kind==5u)++result.free;
  else if(e.kind==0u&&e.actor!=0u)++result.inUse;
  else ++result.unknown;
  if(i==originalPlayerTargetIndex&&originalPlayer!=0u&&
     e.kind==0u&&e.actor==originalPlayer)
   result.localOriginalPlayerBound=true;
 }
 return result;
}
} // namespace blackcoop
