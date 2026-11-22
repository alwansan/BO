// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Host's NPC state arrives on an original BLACK GUEST engine thread via
// authenticated TLS->JNI. Values ONLY; never edits guest NPC AI or health.
#pragma once
#include "coop/room_original_guest_host_npc_contract.hpp"
#include <jni.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <array>
#include <cstdint>
namespace blackcoop {
inline OriginalGuestHostNpcInbox& GuestOriginalHostNpcs()noexcept {
 static OriginalGuestHostNpcInbox inbox;return inbox;
}
inline void PollHostNpcValuesOnOriginalGuestThread(
 JNIEnv* env,jobject activity,jclass cls,bool originalSceneVerified)noexcept{
 if(!originalSceneVerified){GuestOriginalHostNpcs().Clear();return;}
 if(!env||!activity||!cls){GuestOriginalHostNpcs().Clear();return;}
 jmethodID method=env->GetMethodID(cls,
  "coopPollHostOriginalNpcWorldFromGameThread","()[I");
 if(env->ExceptionCheck())env->ExceptionClear();
 if(!method){GuestOriginalHostNpcs().Clear();return;}
 jobject reply=env->CallObjectMethod(activity,method);
 if(env->ExceptionCheck()){
  env->ExceptionClear();GuestOriginalHostNpcs().Clear();return;
 }
 if(!reply){GuestOriginalHostNpcs().Clear();return;}
 jintArray arr=static_cast<jintArray>(reply);
 const jsize n=env->GetArrayLength(arr);
 if(env->ExceptionCheck()||n<3||n>195){
  if(env->ExceptionCheck())env->ExceptionClear();
  GuestOriginalHostNpcs().Clear();env->DeleteLocalRef(arr);return;
 }
 std::array<jint,195> raw{};
 env->GetIntArrayRegion(arr,0,n,raw.data());
 if(env->ExceptionCheck()){
  env->ExceptionClear();GuestOriginalHostNpcs().Clear();
  env->DeleteLocalRef(arr);return;
 }
 env->DeleteLocalRef(arr);
 std::array<int32_t,195> safe{};
 for(jsize i=0;i<n;i++)safe[static_cast<size_t>(i)]=
   static_cast<int32_t>(raw[static_cast<size_t>(i)]);
 const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
 if(GuestOriginalHostNpcs().Ingest(safe.data(),static_cast<size_t>(n),now)){
  static bool logged=false;
  if(!logged&&safe[0]>0){logged=true;SendSafeWorldCodeToAndroid(102);}
 }
}
} // namespace blackcoop
