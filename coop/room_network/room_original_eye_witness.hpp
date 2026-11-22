// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// READ ONLY! The original BLACK player-update hook already independently
// confirms the local original first-person player kind and identity.
// The source-confirmed eye at +0x100 is CAMERA EYE, NOT body root or stage ID.
// No network writing here, no guest memory writes, no scene READY signal.
#pragma once
#include "coop/room_original_eye_contract.hpp"
#include "game/layout.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <jni.h>
#include <atomic>
#include <cstdint>

namespace blackcoop {
inline void ObserveOriginalPlayerEye(uint32_t original_player)noexcept{
    if(!original_player)return;
    static std::atomic<uint64_t> lastMs{0};
    uint64_t at=static_cast<uint64_t>(SDL_GetTicks());
    uint64_t was=lastMs.load(std::memory_order_relaxed);
    if(was>0&&at>=was&&at-was<100)return; // at most 10Hz JNI samples
    if(!lastMs.compare_exchange_strong(was,at,std::memory_order_acq_rel))
        return;
    // Pinned RanggaCasper/Black-Android hooks.cpp: kPlayerEye=0x100,
    // read via G<float> only after current player object was validated.
    OriginalEyeCm result{};
    if(!QuantizeOriginalEye(kernel::G<float>(original_player+0x100),
                            kernel::G<float>(original_player+0x104),
                            kernel::G<float>(original_player+0x108),result))
        return;
    JNIEnv* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    jobject activity=static_cast<jobject>(SDL_GetAndroidActivity());
    if(!env||!activity)return;
    jclass cls=env->GetObjectClass(activity);
    if(cls){
        jmethodID method=env->GetMethodID(cls,"coopRecordOriginalPlayerEye","(III)V");
        if(method)env->CallVoidMethod(activity,method,
            static_cast<jint>(result.x),static_cast<jint>(result.y),
            static_cast<jint>(result.z));
        if(env->ExceptionCheck())env->ExceptionClear();
        env->DeleteLocalRef(cls);
    }else if(env->ExceptionCheck())env->ExceptionClear();
    env->DeleteLocalRef(activity);
}
} // namespace blackcoop
