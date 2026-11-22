// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Based on SAME original XBE, MIT PC black_native_menu.inc bnm_page()
// (request page by +0x3820/0x3884), original BLACK menu input kConfirm.
// Original frontend update sub_00115030 is the SOLE game-thread caller.
// Never force stage READY, create a player, or bypass original save checks.
#pragma once
#include "coop/original_first_stage_auto_menu_contract.hpp"
#include "coop/room_world_tick.hpp"
#include "game/layout.h"
#include "kernel/kernel.h"
#include "kernel/memory.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <jni.h>
#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>
namespace blackcoop {
inline OriginalFirstStageAutoMenuGate& FirstStageAutoMenu()noexcept{
 static OriginalFirstStageAutoMenuGate gate;return gate;
}
inline bool OriginalUiSpan(uint32_t at,uint32_t size)noexcept{
 if(at<0x10000u||!size||
    static_cast<uint64_t>(at)+size>0xffffffffull)return false;
 const uint64_t first=static_cast<uint64_t>(at)&~uint64_t(0xFFF);
 const uint64_t last=(static_cast<uint64_t>(at)+size-1u)&~uint64_t(0xFFF);
 for(uint64_t page=first;page<=last;page+=0x1000u)
  if(!mem::IsCommitted(static_cast<uint32_t>(page)))return false;
 return true;
}
inline OriginalFirstStageMenuPage OriginalUiPage(const char* p)noexcept{
 if(!p)return OriginalFirstStageMenuPage::Untrusted;
 auto prefix=[&](const char* name)noexcept{
  for(size_t i=0;name[i];i++){
   unsigned char a=static_cast<unsigned char>(p[i]);
   unsigned char b=static_cast<unsigned char>(name[i]);
   if(a>='A'&&a<='Z')a=static_cast<unsigned char>(a+32);
   if(b>='A'&&b<='Z')b=static_cast<unsigned char>(b+32);
   if(a!=b)return false;
  }
  return true;
 };
 if(prefix("blacktitlemenu2"))return OriginalFirstStageMenuPage::TitleInitial;
 if(prefix("blacktitlemenu"))return OriginalFirstStageMenuPage::TitleProgress;
 if(prefix("OptionsMenuXbox"))return OriginalFirstStageMenuPage::Options;
 if(prefix("Difficulty"))return OriginalFirstStageMenuPage::Difficulty;
 return OriginalFirstStageMenuPage::Untrusted;
}
inline void ObserveOriginalFirstStageAutoMenu(X86Context& ctx)noexcept{
 static uint64_t lastPoll=0;
 const uint64_t now=static_cast<uint64_t>(SDL_GetTicks());
 if(lastPoll&&now>=lastPoll&&now-lastPoll<150)return;
 lastPoll=now;
 JNIEnv* env=static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
 jobject activity=static_cast<jobject>(SDL_GetAndroidActivity());
 if(!env||!activity)return;
 jclass cls=env->GetObjectClass(activity);
 if(!cls){
  if(env->ExceptionCheck())env->ExceptionClear();
  env->DeleteLocalRef(activity);return;
 }
 jmethodID method=env->GetMethodID(cls,
  "coopPollFirstStageAutoMenuPlanFromOriginalFrontend","()[I");
 if(env->ExceptionCheck())env->ExceptionClear();
 std::array<jint,8> frame{};
 if(method){
  jobject response=env->CallObjectMethod(activity,method);
  if(env->ExceptionCheck())env->ExceptionClear();
  else if(response){
   jintArray arr=static_cast<jintArray>(response);
   jsize n=env->GetArrayLength(arr);
   if(env->ExceptionCheck())env->ExceptionClear();
   else if(n==8){
    env->GetIntArrayRegion(arr,0,8,frame.data());
    if(env->ExceptionCheck()){
     env->ExceptionClear();frame={};
    }
   }
   env->DeleteLocalRef(response);
  }
 }
 env->DeleteLocalRef(cls);env->DeleteLocalRef(activity);
 OriginalFirstStageMenuIntent plan{};
 if(frame[0]>0&&frame[1]==0&&frame[2]==1&&frame[3]>0&&
    (frame[4]!=0||frame[5]!=0)&&
    (frame[6]==1||frame[6]==2)&&frame[7]==3)
  plan={static_cast<uint32_t>(frame[0]),
        static_cast<uint32_t>(frame[3]),
        static_cast<uint32_t>(frame[4]),
        static_cast<uint32_t>(frame[5]),
        static_cast<uint8_t>(frame[6])};
 uint32_t game=0;
 if(OriginalUiSpan(layout::kGame,4))
  game=kernel::R32(layout::kGame);
 if(!game||static_cast<uint64_t>(game)+layout::kFrontEnd+
                layout::kUiReady>0xffffffffull){
  FirstStageAutoMenu().Reset();return;
 }
 // BLACK's game + kFrontEnd identifies the GAME STATE, not the menu
 // manager. The real frontend manager is *[layout::kUi]=0x2D199C
 // (the same object passed in EBP to the original menu hook).
 // Comparing ctx.ebp to game+kFrontEnd made ALL auto-start requests
 // silently fail even while the authenticated room was live.
 const uint32_t ui=OriginalUiSpan(layout::kUi,4u)?
   kernel::R32(layout::kUi):0u;
 const bool originalFront=OriginalUiSpan(game+layout::kGameState,4u)&&
    kernel::R32(game+layout::kGameState)==game+layout::kFrontEnd;
 if(!originalFront||!ui||ctx.ebp!=ui||
    static_cast<uint64_t>(ui)+layout::kUiReady>0xffffffffull||
    !OriginalUiSpan(ui+layout::kPageReady,4u)||
    !OriginalUiSpan(ui+layout::kPageName,0x54u)||
    !OriginalUiSpan(ui+layout::kPageLeaving,4u)||
    !OriginalUiSpan(ui+layout::kUiReady,1u)||
    !OriginalUiSpan(ui+layout::kInputOn,2u)||
    !OriginalUiSpan(ui+layout::kMenuInput,4u)){
  FirstStageAutoMenu().Reset();return;
 }
 const auto page=OriginalUiPage(
   static_cast<const char*>(kernel::HostPtr(ui+layout::kPageName)));
 const bool pageIdle=
  kernel::R32(ui+layout::kPageReady)==layout::kPageRunning&&
  kernel::R32(ui+layout::kPageLeaving)==0u&&
  kernel::R32(ui+layout::kPageAsked)!=1u;
 const bool inputReady=layout::InputTaken(ui)&&
  kernel::R32(ui+layout::kMenuInput)==layout::kNoInput;
 const auto action=FirstStageAutoMenu().Observe(
  plan,page,true,pageIdle,inputReady);
 if(action==OriginalFirstStageMenuAction::RequestDifficulty){
  if(!OriginalUiSpan(ui+layout::kTargetPage,0x20u))return;
  char* at=static_cast<char*>(
   kernel::HostPtr(ui+layout::kTargetPage));
  std::memset(at,0,0x20u);
  std::memcpy(at,"Difficulty",10u);
  kernel::W32(ui+layout::kPageAsked,1u);
  SendSafeWorldCodeToAndroid(92);
 }else if(action==OriginalFirstStageMenuAction::PressConfirm){
  kernel::W32(ui+layout::kMenuInput,layout::kInputConfirm);
  SendSafeWorldCodeToAndroid(93);
 }
}
}
