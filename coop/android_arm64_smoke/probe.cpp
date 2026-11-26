// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include <jni.h>
#if !defined(__aarch64__) || !defined(__ANDROID__)
#error This fixture must execute Android AArch64 native code.
#endif
extern "C" JNIEXPORT jlong JNICALL
Java_org_bocoop_arm64smoke_Arm64Instrumentation_executeArm64(JNIEnv*,jclass,jlong value){
    static_assert(sizeof(void*)==8,"Expected 64-bit Android process");
    return value+64;
}
