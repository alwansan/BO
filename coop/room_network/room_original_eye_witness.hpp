// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// READ ONLY! The original BLACK player-update hook already independently
// confirms the local original first-person player kind and identity.
// The source-confirmed eye at +0x100 is CAMERA EYE, NOT body root or stage ID.
// No network writing here, no guest memory writes, no scene READY signal.
#pragma once
#include "coop/room_original_eye_contract.hpp"
#include "coop/room_nonfrontend_original_player.hpp"
#include "coop/room_world_tick.hpp"
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
    // Same frontend state predicate as pinned original exit_menu.cpp.
    // A stable non-frontend original player is a diagnostic candidate only.
    // DO NOT send STORY_READY, PREPARE, COMMIT or modify original game state.
    uint32_t game=kernel::R32(layout::kGame);
    uint32_t world=kernel::R32(layout::kWorld);
    uint32_t state=game?kernel::R32(game+layout::kGameState):0;
    static NonFrontendOriginalPlayerWitness stable;
    if(stable.Observe(game,state,layout::kFrontEnd,world,true))
        SendSafeWorldCodeToAndroid(13); // coarse fixed event, NOT scene proof
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
