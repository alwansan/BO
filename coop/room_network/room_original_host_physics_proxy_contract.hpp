// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// PC source-linked original BLACK physics proxy pool capacity check:
// [0x2D45F4]+0x2960 has 20 occupancy bytes. DF400 allocates one, DBBE0
// releases one. This pure contract NEVER allocates or accesses game memory.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace blackcoop {
struct OriginalHostPhysicsProxySummary {
 static constexpr unsigned kCapacity=20u,kLeaveForNpc=1u;
 unsigned occupied=0u,free=0u;
 bool localPlayerMotionValid=false;
 bool canReserve(unsigned extraHumans)const noexcept{
  return localPlayerMotionValid&&extraHumans>=1u&&extraHumans<=3u&&
         free>=extraHumans+kLeaveForNpc;
 }
 int coarseCode(unsigned extraHumans)const noexcept{
  if(!localPlayerMotionValid)return 2101;
  if(extraHumans<1u||extraHumans>3u||
     free<extraHumans+kLeaveForNpc)return 2102;
  return 2103; // source capacity only, no actor/proxy was created
 }
};
inline OriginalHostPhysicsProxySummary CountOriginalHostPhysicsProxies(
 const std::array<uint8_t,20>& occupancy,bool localProxyValid)noexcept{
 OriginalHostPhysicsProxySummary out{};
 out.localPlayerMotionValid=localProxyValid;
 for(auto occupied:occupancy){
  if(occupied)++out.occupied;
  else ++out.free;
 }
 return out;
}
} // namespace blackcoop
