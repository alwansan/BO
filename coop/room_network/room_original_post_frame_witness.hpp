// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// ORIGINAL game-thread hook after 0x001F1080 (same boundary as proven PC
// 2-player runtime). Lifecycle observations and scoped objective replication.
#pragma once
#include "coop/room_original_post_frame_contract.hpp"
#include "coop/room_original_host_multi_actor_lanes.hpp"
#include "coop/room_original_host_setup_tick.hpp"
#include "coop/room_original_guest_canonical_source_barrier.hpp"
#include "coop/room_original_guest_host_world_witness.hpp"
#include "coop/room_original_guest_host_npc_witness.hpp"
#include "coop/room_original_guest_launch_match_witness.hpp"
#include "coop/room_original_bot_pool_witness.hpp"
#include "coop/room_original_objective_source.hpp"
#include "coop/room_world_tick.hpp"
#include "kernel/kernel.h"
#include <atomic>
#include <cstdint>
#include <new>
namespace dispatch { X86Func* Find(uint32_t); }
namespace blackcoop {
struct OriginalUnitMembershipMemory {
 bool Contains(uint32_t addr,uint32_t size)const noexcept{
   return OriginalGuestSpanCommitted(addr,size);
 }
 uint32_t Read32(uint32_t addr)const noexcept{return kernel::R32(addr);}
 uint16_t Read16(uint32_t addr)const noexcept{return kernel::R16(addr);}
 uint8_t Read8(uint32_t addr)const noexcept{
   return *static_cast<const uint8_t*>(kernel::HostPtr(addr));
 }
};

inline OriginalUnitLifecycle* OriginalUnitLifecycleOnFrameThread(bool bind=false)noexcept{
 static std::atomic<OriginalUnitLifecycle*> state{nullptr};
 if(bind){
   static OriginalUnitLifecycle observer;
   state.store(&observer,std::memory_order_release);
 }
 auto* observer=state.load(std::memory_order_acquire);
 return observer&&observer->OnFrameThread()?observer:nullptr;
}

inline void ApplyOriginalGuestObjectiveTransition(
    X86Context& ctx, uint32_t world, const OriginalGuestLaunchMatchKey& key) noexcept {
  static bool quarantined=false;
  static uint64_t lastPollAt=0, appliedSequence=0;
  static uint32_t appliedEpisode=0, appliedEpoch=0, appliedTokenHi=0, appliedTokenLo=0;
  const uint64_t now=SDL_GetTicks();
  if(quarantined||(lastPollAt&&now>=lastPollAt&&now-lastPollAt<250))return;
  lastPollAt=now;
  if((ctx.esp&3u)||ctx.esp<16u||!OriginalGuestSpanCommitted(ctx.esp-16u,16u))return;

  JNIEnv* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
  if(!env)return;
  if(env->PushLocalFrame(8)<0){
    if(env->ExceptionCheck())env->ExceptionClear();
    return;
  }
   int diagnostic=0, failedReadback=2135;
  try {
    diagnostic=[&]() -> int {
      jobject activity=static_cast<jobject>(SDL_GetAndroidActivity());
      if(env->ExceptionCheck()||!activity)return 0;
      jclass cls=env->GetObjectClass(activity);
      if(env->ExceptionCheck()||!cls)return 0;
      jmethodID poll=env->GetMethodID(cls,"coopOriginalHostObjectiveFrame","([I)[B");
      if(env->ExceptionCheck()||!poll)return 0;
      jmethodID current=env->GetMethodID(cls,"coopOriginalHostObjectiveCurrent","([IJ)Z");
      if(env->ExceptionCheck()||!current)return 0;
      const jint fields[]{
        static_cast<jint>(key.originalEpisode),key.level,key.stage,
        static_cast<jint>(key.sceneEpoch),static_cast<jint>(key.rosterRevision),
        static_cast<jint>(key.tokenHi),static_cast<jint>(key.tokenLo),key.selfSlot,key.participants
      };
      jintArray scope=env->NewIntArray(9);
      if(env->ExceptionCheck()||!scope)return 0;
      env->SetIntArrayRegion(scope,0,9,fields);
      if(env->ExceptionCheck())return 0;
      auto frame=static_cast<jbyteArray>(env->CallObjectMethod(activity,poll,scope));
      if(env->ExceptionCheck()||!frame)return 0;
      const jsize length=env->GetArrayLength(frame);
      constexpr auto stride=objective_source::kWireRecordSize;
      if(length<8||(length-8)%stride||
         (length-8)/stride>objective_source::kMaxRecords)return 0;
      std::vector<uint8_t> bytes(length);
      env->GetByteArrayRegion(frame,0,length,reinterpret_cast<jbyte*>(bytes.data()));
      if(env->ExceptionCheck())return 0;
      uint64_t sequence=0;
      for(unsigned i=0;i<8;++i)sequence|=uint64_t(bytes[i])<<(8*i);
      if(!sequence||sequence>INT64_MAX||
         (appliedEpisode==key.originalEpisode&&appliedEpoch==key.sceneEpoch&&
          appliedTokenHi==key.tokenHi&&appliedTokenLo==key.tokenLo&&sequence<=appliedSequence))
        return 0;

      OriginalUnitMembershipMemory memory;
      objective_source::Table table{};
      std::vector<uint8_t> local;
      if(!objective_source::ReadTable(memory,world,key.level,key.stage,table)||
         !CaptureOriginalObjectiveRecords(memory,world,key.level,key.stage,local))return 0;
       OriginalObjectiveActivation target{};
       const auto remote=std::span<const uint8_t>(bytes).subspan(8);
       const bool activation=SelectOriginalObjectiveActivation(local,remote,target);
       if(!activation&&!SelectOriginalSimpleObjectiveCompletion(memory,table,local,remote,target))return 0;
       const auto selected=target;
      const auto accepted=env->CallBooleanMethod(activity,current,scope,static_cast<jlong>(sequence));
      if(env->ExceptionCheck()||accepted!=JNI_TRUE)return 0;
      objective_source::Table reread{};
       if(!objective_source::ReadTable(memory,world,key.level,key.stage,reread)||reread!=table||
          !CaptureOriginalObjectiveRecords(memory,world,key.level,key.stage,local))return 0;
       if(!(activation?SelectOriginalObjectiveActivation(local,remote,target):
            SelectOriginalSimpleObjectiveCompletion(memory,table,local,remote,target))||
          target.ordinal!=selected.ordinal||target.selectorLow!=selected.selectorLow||
          target.selectorHigh!=selected.selectorHigh)return 0;
       const uint32_t descriptor=table.descriptors+target.ordinal*objective_source::kDescriptorSize;
       X86Context invocation=ctx;
       uint32_t writer=0, parameter=0;
       if(activation){
         if(memory.Read8(descriptor+0x15u)&0x3Eu)return 0; // exclude initialization rows
         invocation.edi=world+0x930u;
         writer=0x001BA770u;
         parameter=1u; // 001C2290 subtype 0, notify
       }else{
         constexpr uint32_t menuRoot=0x2D45F0u;
         if(!memory.Contains(menuRoot,4u))return 0;
         const auto menu=memory.Read32(menuRoot);
         if(!memory.Contains(menu,0x21074u)||
            memory.Read32(menu+0x21070u)!=menu+0x20F78u)return 0;
         invocation.ebx=world+0x930u;
         writer=0x001BA710u;
         parameter=memory.Read32(descriptor+0x10u); // threshold-1 path uses the local descriptor
         failedReadback=2137;
       }
       if(!dispatch::Find(writer))return 0;
       quarantined=true;
       kernel::CallGuest(invocation,writer,{target.selectorLow,target.selectorHigh,parameter});
       if(!objective_source::ReadTable(memory,world,key.level,key.stage,reread)||reread!=table||
          !CaptureOriginalObjectiveRecords(memory,world,key.level,key.stage,local)||
          !std::equal(local.begin(),local.end(),remote.begin(),remote.end()))return failedReadback;
      appliedEpisode=key.originalEpisode;appliedEpoch=key.sceneEpoch;
      appliedTokenHi=key.tokenHi;appliedTokenLo=key.tokenLo;appliedSequence=sequence;
      quarantined=false;
       return activation?2134:2136;
    }();
  } catch (...) {
     if(quarantined)diagnostic=failedReadback;
  }
  if(env->ExceptionCheck())env->ExceptionClear();
  env->PopLocalFrame(nullptr);
  if(diagnostic)SendSafeWorldCodeToAndroid(diagnostic);
}

inline void ObserveOriginalPostWorldFrame(X86Context& ctx)noexcept{
 static OriginalPostFrameGate gate;
 // Three native host-player lanes and guest canonical source are kept
  // separately on the genuine original game thread.
 static OriginalHostHumanLanes humanLanes;
 static OriginalHumanSetupLanes humanSetup; // native guest-call IO unimplemented, no active actor
 static OriginalGuestCanonicalSourceBarrier guestBarrier;
 constexpr uint32_t worldAt=0x002D45D4u,ownerAt=0x002D12B8u;
 constexpr uint32_t physicsAt=0x002D45F4u,npcAt=0x002D1980u;
 if(!OriginalGuestSpanCommitted(worldAt,4u)||
    !OriginalGuestSpanCommitted(ownerAt,4u)||
    !OriginalGuestSpanCommitted(physicsAt,4u)||
    !OriginalGuestSpanCommitted(npcAt,4u)){
   gate.Reset();return;
 }
 OriginalPostFrameSample s{};
 s.world=kernel::R32(worldAt);s.owner=kernel::R32(ownerAt);
 s.physics=kernel::R32(physicsAt);s.npcPool=kernel::R32(npcAt);
 if(!OriginalGuestSpanCommitted(s.world,0x5CC0u)||
    !OriginalGuestSpanCommitted(s.owner,0x16C0u)||
    !OriginalGuestSpanCommitted(s.physics,0x2974u)||
    !OriginalGuestSpanCommitted(s.npcPool,0x8540u)){
   gate.Reset();return;
 }
 s.corePhase=kernel::R32(s.world+0x5AE0u);
 s.unloadPhase=kernel::R32(s.world+0x5AE4u);
 s.localKind=kernel::R32(s.world+0x30u+0xC4u);
 s.localVtable=kernel::R32(s.world+0x30u); // exact ORIGINAL P1 identity
 const bool stableFrame=gate.Observe(s,true);
 if(stableFrame){
   const bool hasUnitMembership=HasOriginalWorldUnitMembership(OriginalUnitMembershipMemory{},s.world);
   OriginalUnitLifecycleOnFrameThread(true);
   SendSafeWorldCodeToAndroid(2130); // candidate, NOT P2 ready
   if(hasUnitMembership)
     SendSafeWorldCodeToAndroid(2131); // local membership only; no handles retained
 }
 if(auto* lifecycle=OriginalUnitLifecycleOnFrameThread();
    lifecycle&&lifecycle->ObserveRunningFrame(OriginalUnitMembershipMemory{},s.world))
   SendSafeWorldCodeToAndroid(2133);
 (void)humanLanes.Progress(2);(void)humanLanes.Progress(3);
 (void)humanLanes.Progress(4); // never marks a lane ready
 (void)humanSetup.At(2);(void)humanSetup.At(3);(void)humanSetup.At(4);
 OriginalGuestLaunchMatchKey signedMatch{};
 if(gate.StableOriginalFrames()>=30u&&
     GuestOriginalLaunchMatch().ReadMatchedKey(signedMatch,s.world)){
  const uint64_t nativeScene=(uint64_t(signedMatch.originalEpisode)<<32)|s.world;
   const auto now=static_cast<uint64_t>(SDL_GetTicks());
   const auto& players=OriginalGuestHostWorld();
   const auto& npcs=GuestOriginalHostNpcs();
   guestBarrier.Observe(signedMatch,nativeScene,players,npcs,now);
   if(guestBarrier.MatchesForApplication(signedMatch,nativeScene,players,npcs,now))
      ApplyOriginalGuestObjectiveTransition(ctx,s.world,signedMatch);
 }else guestBarrier.Reset();
}
} // namespace blackcoop
