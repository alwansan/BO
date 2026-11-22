// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// The PINNED original BLACK FRONTEND sub_00115030 tick runs while the guest
// has no player character. It can read the authentic host's PREPARE even
// BEFORE a local mission has launched, unlike the player-update-only hook.
// Fail closed: values only, no original loader/AI/actor memory write.
#pragma once
#include "coop/room_original_guest_prepared_story_contract.hpp"
#include "coop/room_world_tick.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <jni.h>
#include <array>
#include <cstddef>
#include <cstdint>

namespace blackcoop {
inline OriginalGuestPreparedStoryInbox& GuestOriginalFrontendPrepare()noexcept{
    static OriginalGuestPreparedStoryInbox inbox;
    return inbox;
}
inline void ObserveOriginalGuestPreparedStoryFrontend()noexcept{
    static uint64_t lastPoll=0;
    const uint64_t at=static_cast<uint64_t>(SDL_GetTicks());
    if(lastPoll&&at>=lastPoll&&at-lastPoll<150)return;
    lastPoll=at;
    JNIEnv* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    jobject activity=static_cast<jobject>(SDL_GetAndroidActivity());
    if(!env||!activity){GuestOriginalFrontendPrepare().Clear();return;}
    jclass cls=env->GetObjectClass(activity);
    if(!cls){
        if(env->ExceptionCheck())env->ExceptionClear();
        GuestOriginalFrontendPrepare().Clear();
        env->DeleteLocalRef(activity);return;
    }
    jmethodID method=env->GetMethodID(
        cls,"coopPollGuestPreparedStoryFromOriginalFrontend","()[I");
    if(env->ExceptionCheck())env->ExceptionClear();
    if(!method){GuestOriginalFrontendPrepare().Clear();}
    else{
        jobject response=env->CallObjectMethod(activity,method);
        if(env->ExceptionCheck()){
            env->ExceptionClear();GuestOriginalFrontendPrepare().Clear();
        }else if(!response){GuestOriginalFrontendPrepare().Clear();}
        else{
            jintArray frame=static_cast<jintArray>(response);
            const jsize n=env->GetArrayLength(frame);
            if(env->ExceptionCheck()||n!=8){
                if(env->ExceptionCheck())env->ExceptionClear();
                GuestOriginalFrontendPrepare().Clear();
            }else{
                std::array<jint,8> raw{};
                env->GetIntArrayRegion(frame,0,8,raw.data());
                if(env->ExceptionCheck()){
                    env->ExceptionClear();GuestOriginalFrontendPrepare().Clear();
                }else{
                    std::array<int32_t,8> safe{};
                    for(size_t i=0;i<8;i++)safe[i]=static_cast<int32_t>(raw[i]);
                    const int code=GuestOriginalFrontendPrepare().Ingest(
                        safe.data(),safe.size(),at);
                    // Fixed coarse code only; no room address, player names,
                    // scene token, stage bytes or private game pointers logged.
                    if(code==76||code==77)SendSafeWorldCodeToAndroid(code);
                }
            }
            env->DeleteLocalRef(frame);
        }
    }
    env->DeleteLocalRef(cls);
    env->DeleteLocalRef(activity);
}
} // namespace blackcoop
