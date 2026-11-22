// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Genuine BLACK original thread receives HOST-only synchronized poses for
// every other human in the current 2-4 player room. NO actor pointers or AI.
#pragma once
#include "coop/original_all_other_humans_contract.hpp"
#include <jni.h>
#include <SDL3/SDL.h>
#include <array>
#include <cstdint>
namespace blackcoop {
inline OriginalAllOtherHumansSource& AllOtherHumanBodies()noexcept{
 static OriginalAllOtherHumansSource source;return source;
}
inline void PollOriginalOtherHumanBodySource(JNIEnv* env,jobject activity,jclass cls,
 bool originalLocalSceneStable)noexcept{
 if(!originalLocalSceneStable){AllOtherHumanBodies().Reset();return;}
 if(!env||!activity||!cls){AllOtherHumanBodies().Reset();return;}
 jmethodID method=env->GetMethodID(cls,
  "coopPollAllOtherHumanBodiesFromGameThread","()[I");
 if(env->ExceptionCheck())env->ExceptionClear();
 if(!method){AllOtherHumanBodies().Reset();return;}
 jobject reply=env->CallObjectMethod(activity,method);
 if(env->ExceptionCheck()){
  env->ExceptionClear();AllOtherHumanBodies().Reset();return;
 }
 if(!reply){AllOtherHumanBodies().Reset();return;}
 jintArray arr=static_cast<jintArray>(reply);
 const jsize n=env->GetArrayLength(arr);
 if(env->ExceptionCheck()||(n!=5&&n!=11&&n!=17&&n!=23)){
  if(env->ExceptionCheck())env->ExceptionClear();
  AllOtherHumanBodies().Reset();
  env->DeleteLocalRef(arr);return;
 }
 std::array<jint,23> raw{};
 env->GetIntArrayRegion(arr,0,n,raw.data());
 if(env->ExceptionCheck()){
  env->ExceptionClear();AllOtherHumanBodies().Reset();
  env->DeleteLocalRef(arr);return;
 }
 env->DeleteLocalRef(arr);
 std::array<int32_t,23> safe{};
 for(jsize i=0;i<n;i++)
  safe[static_cast<size_t>(i)]=static_cast<int32_t>(raw[static_cast<size_t>(i)]);
 if(!AllOtherHumanBodies().Ingest(safe.data(),
       static_cast<size_t>(n),static_cast<uint64_t>(SDL_GetTicks())))
  AllOtherHumanBodies().Reset();
}
} // namespace blackcoop
