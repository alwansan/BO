// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <chrono>
#include <jni.h>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>

// Called ONLY from BLACK's game/UI thread; socket workers never call guest APIs.
namespace blackcoop {
inline void OpenAndroidLobby(int row) {
    if (row != -102 && row != -103) return;
    auto* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    auto activity=static_cast<jobject>(SDL_GetAndroidActivity());
    if (!env || !activity) return;
    jclass cls=env->GetObjectClass(activity);
    if (cls) {
        jmethodID method=env->GetMethodID(cls,"openCoopRoom","(I)V");
        if (method) env->CallVoidMethod(activity,method,static_cast<jint>(row));
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(cls);
    }
    env->DeleteLocalRef(activity);
}

inline std::string AndroidRoomRow(int row) {
    if (row < -105 || row > -101) return " ";
    struct Cache {
        std::array<std::string,5> rows{{"OFFLINE","OPEN HOST FORM","OPEN JOIN FORM","0/4 HUMANS","LAN / VPN"}};
        std::chrono::steady_clock::time_point last{};
        bool initialized=false;
    };
    // On game/UI thread only; refresh every 500ms instead of JNI per rendered frame.
    static Cache cache;
    const auto now=std::chrono::steady_clock::now();
    if (!cache.initialized || now-cache.last >= std::chrono::milliseconds(500)) {
        cache.last=now;cache.initialized=true;
        auto* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
        auto activity=static_cast<jobject>(SDL_GetAndroidActivity());
        if (env && activity) {
            jclass cls=env->GetObjectClass(activity);
            if (cls) {
                jmethodID method=env->GetMethodID(cls,"coopRoomStatusLine","(I)Ljava/lang/String;");
                if (method) for (int key=-105;key<=-101;key++) {
                    jstring result=static_cast<jstring>(env->CallObjectMethod(activity,method,static_cast<jint>(key)));
                    if (env->ExceptionCheck()) {env->ExceptionClear();break;}
                    if (result) {
                        const char* data=env->GetStringUTFChars(result,nullptr);
                        if (data) {
                            std::string value(data);
                            if (value.size()<=80) cache.rows[static_cast<size_t>(-101-key)]=value;
                            env->ReleaseStringUTFChars(result,data);
                        }
                        env->DeleteLocalRef(result);
                    }
                }
                if (env->ExceptionCheck()) env->ExceptionClear();
                env->DeleteLocalRef(cls);
            }
            env->DeleteLocalRef(activity);
        }
    }
    return cache.rows[static_cast<size_t>(-101-row)];
}
}  // namespace blackcoop
