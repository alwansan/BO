// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_host_extra_actor_contract.hpp"
#include <cassert>
#include <iostream>
int main(){
 using namespace blackcoop;
 OriginalHostExtraActorPreconditions s{};
 assert(s.coarseStatus()==2120);
 s.originalEpisode=1;s.originalWorld=0x11000;s.sourceMatched=true;
 assert(s.coarseStatus()==2121);
 s.authenticatedOtherMask=2;assert(s.coarseStatus()==2122);
 s.signedStoryEpoch=7;s.inputEpoch=8;
 assert(s.coarseStatus()==2122);
 s.inputEpoch=7;assert(s.coarseStatus()==2123);
 s.freshInputMask=2;assert(s.coarseStatus()==2124);
 s.physicsReadable=true;s.physical.localPlayerMotionValid=true;
 s.physical.free=5;assert(s.coarseStatus()==2125);
 s.aiReadable=true;s.targets.free=3;s.targets.localOriginalPlayerBound=true;
 assert(s.coarseStatus()==2126);
 s.botReadable=true;s.appearances.sourceLooks=1;s.appearances.emptyVisualSlots=3;
 assert(s.coarseStatus()==2127);
 // Signed 3- and 4-human room scopes require EVERY real authenticated
 // input and one spare original physics proxy for host-owned NPC AI.
 s.authenticatedOtherMask=6;s.freshInputMask=2;
 assert(s.extraHumans()==2&&s.coarseStatus()==2123);
 s.freshInputMask=6;assert(s.coarseStatus()==2127);
 s.authenticatedOtherMask=14;s.freshInputMask=14;
 assert(s.extraHumans()==3&&s.coarseStatus()==2127);
 s.physical.free=3;assert(s.coarseStatus()==2124);
 s.physical.free=4;s.targets.free=2;assert(s.coarseStatus()==2125);
 s.targets.free=3;s.appearances.emptyVisualSlots=2;assert(s.coarseStatus()==2126);
 s.appearances.emptyVisualSlots=3;s.freshInputMask=6;assert(s.coarseStatus()==2123);
 s.freshInputMask=15;assert(s.coarseStatus()==2123);
 s.freshInputMask=14;assert(s.coarseStatus()==2127);
 s.authenticatedOtherMask=10;assert(s.extraHumans()==0&&s.coarseStatus()==2121);
 s.authenticatedOtherMask=14;s.sourceMatched=false;assert(s.coarseStatus()==2120);
 std::cout<<"ORIGINAL_HOST_EXTRA_HUMAN_GATE_PASS: 2-4 real input coverage, original proxy reserve, AI targets, NPC body source and blanks; NO ORIGINAL GAME ACTOR CREATED"<<std::endl;
}
