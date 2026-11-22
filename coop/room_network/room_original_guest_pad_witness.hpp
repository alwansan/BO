// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Genuine ORIGINAL BLACK player update -> original SDK SDL controller input
// values -> room client TLS queue after Java verifies COMMITTED story.
// The current source covers connected original SDL gamepads (including any
// virtual gamepad exposed through SDL). Android touch/mouse-only controls
// require independently verified bindings later. NOT host body/AI writes.
#pragma once
#include "coop/room_original_guest_pad_contract.hpp"
#include "coop/room_world_tick.hpp"
#include "core/input.h"
#include <jni.h>
#include <atomic>
namespace blackcoop {
inline void OfferOriginalGuestController(JNIEnv* env,jobject activity,
                                         jclass cls)noexcept{
    if(!env||!activity||!cls)return;
    // Original Xbox SDK input::Poll also invokes the normal game's pad
    // callback; this is a duplicate local sample, NEVER a remote NPC write.
    // Must stay pinned to the source-verified ORIGINAL BLACK game thread.
    const input::Pad report=input::Poll(0);
    if(!report.connected)return;
    const auto command=QuantizeOriginalGuestPad(report.connected,report.lx,
        report.ly,report.analog[0],report.analog[7]);
    jmethodID method=env->GetMethodID(cls,
        "coopOfferOriginalGuestController","(IIIZZ)Z");
    if(env->ExceptionCheck()){env->ExceptionClear();return;}
    if(!method)return;
    // Yaw still unavailable as independently source-known actor facing.
    // Set zero, not a false original heading. Only movement and two buttons.
    const jboolean accepted=env->CallBooleanMethod(activity,method,
        static_cast<jint>(command.moveX),static_cast<jint>(command.moveY),
        static_cast<jint>(0),static_cast<jboolean>(command.fire),
        static_cast<jboolean>(command.jump));
    if(env->ExceptionCheck()){env->ExceptionClear();return;}
    if(accepted){
        static std::atomic<bool> logged{false};
        bool first=false;
        if(logged.compare_exchange_strong(first,true,std::memory_order_acq_rel))
            SendSafeWorldCodeToAndroid(65);
    }
}
} // namespace blackcoop
