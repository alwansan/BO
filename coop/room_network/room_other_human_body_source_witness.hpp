// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Genuine original BLACK game-thread reads only *host-authoritative* 2-phone
// remote-human source. Future renderer can consume after its own scene gate.
#pragma once
#include "coop/original_other_human_body_source.hpp"
#include <jni.h>
#include <SDL3/SDL.h>
#include <array>
#include <cstdint>
namespace blackcoop {
inline OriginalOtherHumanBodyInbox& OtherHumanBodySource()noexcept{
 static OriginalOtherHumanBodyInbox inbox;return inbox;
}
inline void PollOriginalOtherHumanBodySource(JNIEnv* env,jobject activity,jclass cls,
 bool realOriginalLocalSceneStable)noexcept{
 if(!realOriginalLocalSceneStable){OtherHumanBodySource().Reset();return;}
 if(!env||!activity||!cls)return;
 jmethodID method=env->GetMethodID(cls,
  "coopPollOtherHumanBodySourceFromGameThread","()[I");
 if(env->ExceptionCheck()){env->ExceptionClear();OtherHumanBodySource().Reset();return;}
 if(!method){OtherHumanBodySource().Reset();return;}
 jobject reply=env->CallObjectMethod(activity,method);
 if(env->ExceptionCheck()){env->ExceptionClear();OtherHumanBodySource().Reset();return;}
 if(!reply){OtherHumanBodySource().Reset();return;}
 jintArray arr=static_cast<jintArray>(reply);
 jsize n=env->GetArrayLength(arr);
 if(env->ExceptionCheck()||n!=10){
  if(env->ExceptionCheck())env->ExceptionClear();
  OtherHumanBodySource().Reset();env->DeleteLocalRef(arr);return;
 }
 std::array<jint,10> raw{};
 env->GetIntArrayRegion(arr,0,10,raw.data());
 if(env->ExceptionCheck()){
  env->ExceptionClear();OtherHumanBodySource().Reset();
  env->DeleteLocalRef(arr);return;
 }
 env->DeleteLocalRef(arr);
 std::array<int32_t,10> safe{};
 for(size_t i=0;i<safe.size();i++)safe[i]=static_cast<int32_t>(raw[i]);
 OtherHumanBodySource().Ingest(safe.data(),safe.size(),
                              static_cast<uint64_t>(SDL_GetTicks()));
}
} // namespace blackcoop
