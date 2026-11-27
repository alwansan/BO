// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original guest BLACK thread consumes exact host-authenticated object events.
// Only values are queued. No door/NPC object, engine memory, or AI is changed.
#pragma once
#include "coop/room_original_guest_host_object_contract.hpp"
#include "coop/room_original_guest_launch_match_witness.hpp"
#include "coop/room_world_tick.hpp"
#include <jni.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <array>
#include <cstdint>
namespace blackcoop {
inline OriginalGuestHostObjectInbox& GuestOriginalHostObjects()noexcept{
 static OriginalGuestHostObjectInbox inbox;
 return inbox;
}
inline void PollHostObjectEventsOnOriginalGuestThread(
 JNIEnv* env,jobject activity,jclass cls,bool verifiedOriginalScene)noexcept{
 if(!verifiedOriginalScene||!env||!activity||!cls){
  GuestOriginalHostObjects().Reset();return;
 }
 OriginalGuestLaunchMatchKey key{};
 if(!GuestOriginalLaunchMatch().ReadMatchedKey(key)){
  GuestOriginalHostObjects().Reset();return;
 }
 if(!GuestOriginalHostObjects().CheckLocalLaunch(key))return;
 jmethodID method=env->GetMethodID(cls,
    "coopPollHostObjectEventFromGameThread","()[I");
 if(env->ExceptionCheck()){env->ExceptionClear();GuestOriginalHostObjects().Reset();return;}
 if(!method){GuestOriginalHostObjects().Reset();return;}
 jobject response=env->CallObjectMethod(activity,method);
 if(env->ExceptionCheck()){
  env->ExceptionClear();GuestOriginalHostObjects().Reset();return;
 }
 if(!response){GuestOriginalHostObjects().Reset();return;}
 jintArray arr=static_cast<jintArray>(response);
 const jsize n=env->GetArrayLength(arr);
  constexpr auto kObjectWords=static_cast<jsize>(OriginalGuestHostObjectInbox::kFrameWords);
  constexpr auto kPartWords=static_cast<jsize>(OriginalGuestHostObjectInbox::kPartFrameWords);
  if(env->ExceptionCheck()||(n!=1&&n!=kObjectWords&&n!=kPartWords)){
  if(env->ExceptionCheck())env->ExceptionClear();
  GuestOriginalHostObjects().Reset();env->DeleteLocalRef(arr);return;
 }
  std::array<jint,kPartWords> raw{};
 env->GetIntArrayRegion(arr,0,n,raw.data());
 if(env->ExceptionCheck()){
  env->ExceptionClear();GuestOriginalHostObjects().Reset();
  env->DeleteLocalRef(arr);return;
 }
 env->DeleteLocalRef(arr);
 if(n==1){
  if(raw[0]==-1)GuestOriginalHostObjects().Reset(); // uncommitted/abort
  else if(raw[0]!=0)GuestOriginalHostObjects().Reset();
  return; // zero means "no new event", NOT a disconnect
 }
  std::array<int32_t,kPartWords> safe{};
  for(jsize i=0;i<n;i++)safe[static_cast<size_t>(i)]=
  static_cast<int32_t>(raw[static_cast<size_t>(i)]);
  if(GuestOriginalHostObjects().Ingest(safe.data(),static_cast<size_t>(n),key)){
  static bool first=false;
  if(!first){first=true;SendSafeWorldCodeToAndroid(103);}
 }
}
} // namespace blackcoop
