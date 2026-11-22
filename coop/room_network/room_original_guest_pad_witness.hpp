// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Genuine ORIGINAL BLACK player update -> already polled and TOUCH-MERGED
// Xbox port0 values -> room GUEST TLS queue after Java verifies COMMIT.
// Android touchscreen is confirmed by source native/input.cpp::Poll/Apply
// and by owner CL/video 2026-11-22. No duplicate Poll or guest memory writes.
#pragma once
#include "coop/room_original_guest_pad_contract.hpp"
#include "coop/room_original_local_pad_tap.hpp"
#include "coop/room_world_tick.hpp"
#include <jni.h>
#include <atomic>
namespace blackcoop {
inline void OfferOriginalGuestController(JNIEnv* env,jobject activity,
                                         jclass cls)noexcept{
    if(!env||!activity||!cls)return;
    // SOURCE capture is made in native/input.cpp only once, AFTER touch::Apply
    // but BEFORE the ORIGINAL upstream app::kGame.pad callback. No second
    // Poll is allowed here: the original game controls must remain intact.
    const OriginalLocalPadFrame report=LocalPadTap().Read(
        static_cast<uint64_t>(SDL_GetTicks()));
    // A disconnected/expired touch frame must send a NEUTRAL release, not
    // leave FIRE/JUMP latched remotely after the player raises a finger.
    const auto command=QuantizeOriginalGuestPad(report.connected,report.lx,
        report.ly,report.a,report.rightTrigger);
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
        static std::atomic<bool> firstAccepted{false},firstTouch{false};
        bool expected=false;
        if(firstAccepted.compare_exchange_strong(expected,true,
             std::memory_order_acq_rel))
            SendSafeWorldCodeToAndroid(65);
        expected=false;
        if(report.fromTouch&&firstTouch.compare_exchange_strong(expected,true,
             std::memory_order_acq_rel))
            SendSafeWorldCodeToAndroid(66);
    }
}
} // namespace blackcoop
