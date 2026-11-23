// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_remote_human_motion_contract.hpp"
#include <cassert>
#include <iostream>
int main(){
 using blackcoop::OriginalRemoteHumanMotionPlan;
 OriginalRemoteHumanMotionPlan plan;
 assert(!plan.Valid()&&plan.DesiredClip()==0);
 plan.Observe(1,2,1,1000,3000,1000);
 assert(plan.Valid()&&plan.DesiredClip()==1&&!plan.Walking());
 plan.Observe(1,2,2,1020,3000,1100);
 assert(plan.DesiredClip()==1);
 plan.Observe(1,2,3,1040,3000,1200);
 assert(plan.DesiredClip()==2&&plan.Walking());
 plan.Observe(1,2,3,1040,3000,1201);
 assert(plan.DesiredClip()==2);
 plan.Observe(1,2,4,1041,3000,1300);
 assert(plan.DesiredClip()==2);
 plan.Observe(1,2,5,1041,3000,1400);
 assert(plan.DesiredClip()==1);
 plan.Observe(1,2,4,5000,3000,1500);
 assert(plan.DesiredClip()==1);
 plan.Observe(1,2,6,5000,3000,2000);
 plan.Observe(1,2,7,9000,3000,2600);
 assert(plan.DesiredClip()==1);
 plan.Observe(2,2,1,5000,3000,2700);
 assert(plan.DesiredClip()==1);
 plan.Observe(2,3,1,5000,3000,2701);
 assert(plan.DesiredClip()==1);
 plan.Observe(0,0,0,0,0,0);
 assert(!plan.Valid()&&plan.DesiredClip()==0);
 std::cout<<"ORIGINAL_REMOTE_HUMAN_MOTION_PASS: source-stamped idle/move, two-sample jitter guard, stale/replay/epoch clear, no extra original actor"<<std::endl;
}
