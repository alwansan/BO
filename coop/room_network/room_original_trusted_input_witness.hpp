// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Real TLS trusted remote human input -> ORIGINAL BLACK game thread inbox.
// No AI, original actor writes, guest CallGuest, client snapshots or READY.
// A source-verified local player update is the sole caller of this adapter.
#pragma once
#include "coop/room_original_trusted_input_contract.hpp"
#include "coop/room_world_tick.hpp"
#include <jni.h>
#include <array>
#include <atomic>
#include <cstdint>
namespace blackcoop {
inline OriginalHostHumanInbox& OriginalHostHumanInputs()noexcept{
    static OriginalHostHumanInbox inbox;
    return inbox;
}
inline void PollOriginalHostHumanInputs(JNIEnv* env,jobject activity,jclass cls)noexcept{
    if(!env||!activity||!cls)return;
    jmethodID method=env->GetMethodID(cls,
        "coopPollTrustedHumanInputsFromGameThread","()[I");
    if(env->ExceptionCheck()){env->ExceptionClear();return;}
    if(!method)return;
    jobject reply=env->CallObjectMethod(activity,method);
    if(env->ExceptionCheck()){
        env->ExceptionClear();
        OriginalHostHumanInputs().Reset();
        return;
    }
    if(!reply){OriginalHostHumanInputs().Reset();return;}
    jintArray frame=static_cast<jintArray>(reply);
    const jsize n=env->GetArrayLength(frame);
    if(env->ExceptionCheck()){
        env->ExceptionClear();env->DeleteLocalRef(frame);return;
    }
    if(n<2||n>20){
        OriginalHostHumanInputs().Reset();
        env->DeleteLocalRef(frame);
        return;
    }
    std::array<jint,20> raw{};
    env->GetIntArrayRegion(frame,0,n,raw.data());
    if(env->ExceptionCheck()){
        env->ExceptionClear();
        OriginalHostHumanInputs().Reset();
        env->DeleteLocalRef(frame);
        return;
    }
    env->DeleteLocalRef(frame);
    std::array<int32_t,20> safe{};
    for(jsize i=0;i<n;++i)safe[static_cast<size_t>(i)]=static_cast<int32_t>(raw[static_cast<size_t>(i)]);
    if(OriginalHostHumanInputs().Ingest(safe.data(),static_cast<size_t>(n))!=0){
        static std::atomic<bool> first{false};
        bool expected=false;
        if(first.compare_exchange_strong(expected,true,std::memory_order_acq_rel))
            SendSafeWorldCodeToAndroid(15); // first verified game-thread input ONLY
    }
}
} // namespace blackcoop
