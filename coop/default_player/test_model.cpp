// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "test_fixture.hpp"
int main(){
 Memory m;bo_player::ModelInfo info;assert(bo_player::ReadModel(m.reader(),m.actor,info));
 assert(info.has_model&&info.complete&&info.bones==3&&info.head&&info.torso&&info.legs&&info.draw_lists==(1u<<5));
 auto old=info;assert(!bo_player::ReadModel(m.reader(),m.actor+4,info));assert(info==old);
 m.put(m.model+0x60,0xFFFFFFF0);assert(!bo_player::ReadModel(m.reader(),m.actor,info));assert(info==old);
 m.put(m.actor+0x320,0);assert(bo_player::ReadModel(m.reader(),m.actor,info));assert(!info.has_model&&!info.complete);
 std::cout<<"DEFAULT_MODEL_PASS: relocated original player metadata, default draw list, bounds and unchanged failed result; synthetic memory only\n";
}
