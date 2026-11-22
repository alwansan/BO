// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original BLACK game-thread read of authenticated HOST canonical world data
// on each connected guest. NO scene loader, actor creation, NPC AI or game
// memory writes. Uses same JNI class/activity as the 40Hz trusted input poll.
#pragma once
#include "coop/room_original_guest_host_world_contract.hpp"
#include "coop/room_world_tick.hpp"
#include <jni.h>
#include <SDL3/SDL.h>
#include <array>
#include <atomic>
#include <cstdint>
namespace blackcoop {
inline OriginalGuestHostWorldInbox& OriginalGuestHostWorld()noexcept{
    static OriginalGuestHostWorldInbox world;
    return world;
}
inline void PollOriginalGuestHostWorld(JNIEnv* env,jobject activity,jclass cls)noexcept{
    if(!env||!activity||!cls)return;
    jmethodID method=env->GetMethodID(cls,
        "coopPollHostWorldSnapshotFromGameThread","()[I");
    if(env->ExceptionCheck()){env->ExceptionClear();OriginalGuestHostWorld().Clear();return;}
    if(!method){OriginalGuestHostWorld().Clear();return;}
    jobject response=env->CallObjectMethod(activity,method);
    if(env->ExceptionCheck()){
        env->ExceptionClear();OriginalGuestHostWorld().Clear();return;
    }
    if(!response){OriginalGuestHostWorld().Clear();return;}
    jintArray frame=static_cast<jintArray>(response);
    const jsize n=env->GetArrayLength(frame);
    if(env->ExceptionCheck()||n<4||n>28){
        if(env->ExceptionCheck())env->ExceptionClear();
        OriginalGuestHostWorld().Clear();env->DeleteLocalRef(frame);return;
    }
    std::array<jint,28> received{};
    env->GetIntArrayRegion(frame,0,n,received.data());
    if(env->ExceptionCheck()){
        env->ExceptionClear();OriginalGuestHostWorld().Clear();
        env->DeleteLocalRef(frame);return;
    }
    env->DeleteLocalRef(frame);
    std::array<int32_t,28> safe{};
    for(jsize i=0;i<n;i++)safe[static_cast<size_t>(i)]=
        static_cast<int32_t>(received[static_cast<size_t>(i)]);
    const uint64_t nowMs=static_cast<uint64_t>(SDL_GetTicks());
    if(OriginalGuestHostWorld().Ingest(safe.data(),static_cast<size_t>(n),nowMs)){
        static std::atomic<bool> logged{false};
        bool prior=false;
        if(logged.compare_exchange_strong(prior,true,std::memory_order_acq_rel))
            SendSafeWorldCodeToAndroid(16);
    }
}
} // namespace blackcoop
