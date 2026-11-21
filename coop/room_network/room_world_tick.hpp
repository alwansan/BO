// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Narrow read-only Android notification from the ALREADY existing original
// local-player update hook in the pinned BLACK Android source.
// NEVER assert scene loaded or call the guest. JNI is invoked ONLY once per
// observed opaque world pointer (or when it changes), not once per frame.
#pragma once
#include "coop/world_tick_contract.hpp"
#include <jni.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>

namespace blackcoop {
inline void ObserveLocalWorldTick(uint32_t opaque_world) noexcept {
    static WorldTickObserver observer;
    const WorldTickSignal signal=observer.Observe(opaque_world,true);
    if(signal==WorldTickSignal::None)return;
    JNIEnv* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    jobject activity=static_cast<jobject>(SDL_GetAndroidActivity());
    if(!env||!activity)return;
    jclass cls=env->GetObjectClass(activity);
    if(cls){
        jmethodID method=env->GetMethodID(cls,"coopRecordWorldTick","(I)V");
        if(method)env->CallVoidMethod(activity,method,static_cast<jint>(signal));
        if(env->ExceptionCheck())env->ExceptionClear();
        env->DeleteLocalRef(cls);
    }else if(env->ExceptionCheck())env->ExceptionClear();
    env->DeleteLocalRef(activity);
}
} // namespace blackcoop
