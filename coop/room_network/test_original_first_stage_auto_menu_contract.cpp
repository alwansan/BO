#include "original_first_stage_auto_menu_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
int main(){
 using namespace blackcoop;
 OriginalFirstStageAutoMenuGate gate;
 const OriginalFirstStageMenuIntent host{1,7,123,456,1};
 const OriginalFirstStageMenuIntent guest{1,7,123,456,2};
 const OriginalFirstStageMenuIntent third{1,7,123,456,3};
 const OriginalFirstStageMenuIntent fourth{1,7,123,456,4};
 auto ask=OriginalFirstStageMenuAction::RequestDifficulty;
 auto go=OriginalFirstStageMenuAction::PressConfirm;
 auto none=OriginalFirstStageMenuAction::None;
 assert(gate.Observe({},OriginalFirstStageMenuPage::TitleInitial,true,true,true)==none);
 assert(gate.Observe(host,OriginalFirstStageMenuPage::Untrusted,true,true,true)==none);
 assert(gate.Observe(host,OriginalFirstStageMenuPage::TitleInitial,false,true,true)==none);
 assert(gate.Observe(host,OriginalFirstStageMenuPage::TitleInitial,true,true,true)==ask);
 assert(gate.Observe(host,OriginalFirstStageMenuPage::TitleInitial,true,true,true)==none);
 assert(gate.Observe(host,OriginalFirstStageMenuPage::Difficulty,true,false,true)==none);
 assert(gate.Observe(host,OriginalFirstStageMenuPage::Difficulty,true,true,false)==none);
 assert(gate.Observe(host,OriginalFirstStageMenuPage::Difficulty,true,true,true)==go);
  assert(gate.Observe(host,OriginalFirstStageMenuPage::Difficulty,true,true,true)==none);
  gate.Reset();
  assert(gate.Observe({},OriginalFirstStageMenuPage::Untrusted,false,false,false)==none);
  assert(gate.Observe(host,OriginalFirstStageMenuPage::TitleInitial,true,true,true)==none);
  assert(gate.Observe(host,OriginalFirstStageMenuPage::Difficulty,true,true,true)==none);
 bool cross=false;
 std::thread t([&]{
  cross=gate.Observe(host,OriginalFirstStageMenuPage::Difficulty,
                     true,true,true)!=none;
 });
 t.join();assert(!cross);
 assert(gate.Observe(guest,OriginalFirstStageMenuPage::TitleProgress,true,true,true)==ask);
 assert(gate.Observe(guest,OriginalFirstStageMenuPage::Difficulty,true,true,true)==go);
 gate.Reset();
 assert(gate.Observe(third,OriginalFirstStageMenuPage::TitleInitial,true,true,true)==ask);
 assert(gate.Observe(third,OriginalFirstStageMenuPage::Difficulty,true,true,true)==go);
 gate.Reset();
 assert(gate.Observe(fourth,OriginalFirstStageMenuPage::TitleProgress,true,true,true)==ask);
 assert(gate.Observe(fourth,OriginalFirstStageMenuPage::Difficulty,true,true,true)==go);
 gate.Reset();
 assert(gate.Observe(host,OriginalFirstStageMenuPage::Options,true,true,true)==ask);
 std::cout<<"ORIGINAL_TWO_PHONE_FIRST_STAGE_AUTO_MENU_GATE_PASS: true ticket, page, single confirm, no cross-thread; runtime menu still UNTESTED"<<std::endl;
}
