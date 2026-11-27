// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Source: pinned Black-Android native/input.cpp::Pad: A analog[0],
// right trigger analog[7], signed left- and right-stick axes. Read-only plan.
// Does not override port0, write guest memory or validate game-world loading.
#pragma once
#include "room_original_trusted_input_contract.hpp"
#include <array>
#include <cstdint>
namespace blackcoop {
struct OriginalHostRemotePadValue {
 uint8_t port=0;bool connected=false;uint16_t buttons=0;
 int16_t lx=0,ly=0,rx=0,ry=0;
 std::array<uint8_t,8> analog{};
 int16_t yawCentiDegrees=0; // absolute yaw, never synthetic right-stick
 uint32_t epoch=0,sequence=0;
};
class OriginalHostRemotePadPlan final {
 static int16_t axis(int16_t normalized)noexcept{
  if(normalized==1000)return 32767;
  if(normalized==-1000)return -32768;
  return static_cast<int16_t>(int32_t(normalized)*32767/1000);
 }
public:
 // ORIGINAL host world confirmation and independent original actor setup
 // must be independently proven by a future game-thread native lifecycle;
 // room COMMIT alone is never valid actor readiness. No live input injected.
 static bool Preview(const OriginalHostHumanInbox& inbox,uint32_t epoch,
       uint8_t port,bool originalHostSceneVerified,
       bool originalActorSetupVerified,OriginalHostRemotePadValue& result)noexcept{
  result={}; // drop FIRE and axis on every rejected/stale input
  if(!epoch||!originalHostSceneVerified||!originalActorSetupVerified||
     port<1||port>3||inbox.Epoch()!=epoch)return false;
  TrustedHumanCommand source{};
  const uint8_t peer=static_cast<uint8_t>(port+1);
  if(!inbox.Fresh(peer,source)||source.peer!=peer||
     source.sceneEpoch!=epoch||!source.sequence||
     source.x< -1000||source.x>1000||source.y< -1000||source.y>1000||
     source.yaw< -18000||source.yaw>18000||source.flags>3)return false;
  result.connected=true;result.port=port;
   result.lx=axis(source.x);result.ly=axis(source.y);
   result.rx=source.lookX;result.ry=source.lookY;
  result.analog[0]=source.flags&2u?255u:0u; // Xbox A: jump
  result.analog[7]=source.flags&1u?255u:0u; // Xbox RT: fire
  result.yawCentiDegrees=source.yaw; // metadata only; no aim injection
  result.epoch=epoch;result.sequence=source.sequence;
  return true;
 }
};
} // namespace blackcoop
