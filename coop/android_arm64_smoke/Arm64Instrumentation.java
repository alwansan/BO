// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package org.bocoop.arm64smoke;
import android.app.Instrumentation;
import android.os.Bundle;
import java.util.Arrays;
public final class Arm64Instrumentation extends Instrumentation {
    private static native long executeArm64(long value);
    @Override public void onStart(){
        Bundle result=new Bundle();
        try{
            if(!Arrays.asList(android.os.Build.SUPPORTED_ABIS).contains("arm64-v8a"))
                throw new AssertionError("ARM64 ABI unavailable");
            System.loadLibrary("bo_arm64_smoke");
            if(executeArm64(0x12345678L)!=0x123456b8L)
                throw new AssertionError("AArch64 native result mismatch");
            result.putString("stream","BO_ARM64_JNI_PASS: actual AArch64-only JNI function executed; ABI arm64-v8a offered; no game/runtime verification");
            finish(-1,result);
        }catch(Throwable failure){
            result.putString("stream","BO_ARM64_JNI_FAILED: "+failure.getClass().getSimpleName());
            finish(0,result);
        }
    }
}
