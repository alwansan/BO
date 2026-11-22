// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Host-authenticated 2–4 human mission0/stage1 START -> genuine BLACK
// frontend page/confirm actions. Never fakes game-loaded or AI sync.
#pragma once
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalFirstStageMenuIntent {
 uint32_t epoch=0,revision=0,tokenHi=0,tokenLo=0;
 uint8_t self=0;
 bool valid()const noexcept{
  return epoch&&revision&&(tokenHi||tokenLo)&&(self>=1&&self<=4);
 }
 bool same(const OriginalFirstStageMenuIntent& x)const noexcept{
  return epoch==x.epoch&&revision==x.revision&&
   tokenHi==x.tokenHi&&tokenLo==x.tokenLo&&self==x.self;
 }
};
enum class OriginalFirstStageMenuPage:uint8_t{
 Untrusted,TitleInitial,TitleProgress,Options,Difficulty
};
enum class OriginalFirstStageMenuAction:uint8_t{
 None,RequestDifficulty,PressConfirm
};
class OriginalFirstStageAutoMenuGate final {
 OriginalFirstStageMenuIntent intent_{};
 bool pageRequested_=false,confirmSent_=false;
 std::thread::id owner_{};
 bool owned_=false;
public:
 void Reset()noexcept{
  if(owned_&&owner_!=std::this_thread::get_id())return;
  intent_={};pageRequested_=confirmSent_=false;
 }
 OriginalFirstStageMenuAction Observe(
  const OriginalFirstStageMenuIntent& in,
  OriginalFirstStageMenuPage page,bool stableFrontEnd,bool menuIdle,
  bool inputReady)noexcept{
  if(owned_&&owner_!=std::this_thread::get_id())
   return OriginalFirstStageMenuAction::None;
  if(!in.valid()){Reset();return OriginalFirstStageMenuAction::None;}
  if(!owned_){owned_=true;owner_=std::this_thread::get_id();}
  if(!intent_.same(in)){
   intent_=in;pageRequested_=confirmSent_=false;
  }
  if(!stableFrontEnd||!menuIdle||confirmSent_)
   return OriginalFirstStageMenuAction::None;
  if(page==OriginalFirstStageMenuPage::Difficulty){
   if(!inputReady)return OriginalFirstStageMenuAction::None;
   confirmSent_=true;return OriginalFirstStageMenuAction::PressConfirm;
  }
  if(!pageRequested_&&
     (page==OriginalFirstStageMenuPage::TitleInitial||
      page==OriginalFirstStageMenuPage::TitleProgress||
      page==OriginalFirstStageMenuPage::Options)){
   pageRequested_=true;return OriginalFirstStageMenuAction::RequestDifficulty;
  }
  return OriginalFirstStageMenuAction::None;
 }
};
}
