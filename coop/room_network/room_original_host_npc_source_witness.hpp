// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// READ-ONLY original BLACK character-pool source; genuine verified mission
// update thread only. Produces compact HOST canonical NPC values (not guest
// addresses/pointers), never manipulates AI, health or original actor state.
#pragma once
#include "coop/room_original_bot_pool_witness.hpp"
#include "coop/room_original_eye_contract.hpp"
#include <jni.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>
namespace blackcoop {
inline void PublishOriginalHostNpcValues(JNIEnv* env,jobject activity,jclass cls,
    uint32_t episode,bool originalSceneVerified)noexcept{
 static uint64_t last=0;
 if(!originalSceneVerified||!episode){last=0;return;}
 const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
 if(last&&now>=last&&now-last<200u)return; // <=5Hz and no socket on this thread
 last=now;
 if(!env||!activity||!cls||episode>0x7fffffffu)return;
 constexpr uint32_t poolAddr=0x002D1980u,off=0xA0u,stride=0x3C0u;
 uint32_t root=0;
 if(!OriginalBotSlotRead(poolAddr,root)||!root||
    static_cast<uint64_t>(root)+off+stride*32u>0x100000000ull||
    !OriginalGuestSpanCommitted(root+off,stride*32u))return;
 std::array<jint,194> data{};
 data[0]=static_cast<jint>(episode);int count=0;
 for(uint32_t i=0;i<32u;i++){
  const uint32_t npc=root+off+i*stride;
  uint32_t templatePtr=0,controller=0;
  if(!OriginalBotSlotRead(npc+0x320u,templatePtr)||
     !OriginalBotSlotRead(npc+0x324u,controller))return;
  // Controllers are genuinely original AI-owned actors. A detached
  // human soldier LOOK deliberately has controller=0 and is excluded.
  if(!templatePtr||!controller||
     !OriginalGuestSpanCommitted(templatePtr,0x60u))continue;
  const float x=kernel::G<float>(npc+0xA0u),
              y=kernel::G<float>(npc+0xA4u),
              z=kernel::G<float>(npc+0xA8u),
              hp=kernel::G<float>(npc+0x2F0u);
  OriginalBodyFeetCm feet{};
  if(!QuantizeOriginalBodyFeet(x,y,z,feet)||
     !std::isfinite(hp)||hp<0.0f||hp>10000.0f)continue;
  const uint32_t dead=kernel::G<uint8_t>(npc+0x3ADu)?1u:0u;
  const int health=static_cast<int>(std::round(static_cast<double>(hp)*100.0));
  const size_t at=2u+static_cast<size_t>(count)*6u;
  data[at]=static_cast<jint>(i);data[at+1]=feet.x;data[at+2]=feet.y;
  data[at+3]=feet.z;data[at+4]=health;data[at+5]=static_cast<jint>(dead);
  ++count;
 }
 uint32_t stillRoot=0;
 if(!OriginalBotSlotRead(poolAddr,stillRoot)||stillRoot!=root)return;
 data[1]=static_cast<jint>(count);
 jmethodID method=env->GetMethodID(cls,
   "coopRecordOriginalHostNpcSampleFromGameThread","([I)V");
 if(env->ExceptionCheck()){env->ExceptionClear();return;}
 if(!method)return;
 const jsize size=static_cast<jsize>(2u+static_cast<size_t>(count)*6u);
 jintArray raw=env->NewIntArray(size);
 if(env->ExceptionCheck()||!raw){
  if(env->ExceptionCheck())env->ExceptionClear();
  return;
 }
 env->SetIntArrayRegion(raw,0,size,data.data());
 if(env->ExceptionCheck())env->ExceptionClear();
 else env->CallVoidMethod(activity,method,raw);
 if(env->ExceptionCheck())env->ExceptionClear();
 env->DeleteLocalRef(raw);
}
} // namespace blackcoop
