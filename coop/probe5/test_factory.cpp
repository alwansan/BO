// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "test_fixture.hpp"
int main(){
 Memory m;bo_probe5::Entry entry;assert(bo_probe5::ReadEntry(m.reader(),m.manager,m.brain,m.descriptor,entry));
 assert(entry.resource_index==2&&entry.controller_index==0&&entry.live==3&&entry.free==29&&entry.pose_finite);
 bo_probe5::Result result;assert(bo_probe5::ReadResult(m.reader(),entry,m.actor,result));
 assert(result.slot==0&&result.serial==11&&result.brain_matches&&result.resource_matches&&result.backlink);
 auto saved=entry;m.put(m.descriptor,m.manager+0x7A20+20*0x40);assert(!bo_probe5::ReadEntry(m.reader(),m.manager,m.brain,m.descriptor,entry));assert(entry==saved);
 m.put(m.descriptor,m.manager+0x7A20);assert(!bo_probe5::ReadEntry(m.reader(),m.manager,m.brain+1,m.descriptor,entry));
 auto old=result;assert(!bo_probe5::ReadResult(m.reader(),saved,0,result));assert(result==old);
 m.put(m.brain+0x8C,m.actor+0x3C0);assert(!bo_probe5::ReadResult(m.reader(),saved,m.actor,result));assert(result==old);
 m.put(m.brain+0x8C,m.actor);m.put(0x2D1980,0x11000);assert(!bo_probe5::ReadResult(m.reader(),saved,m.actor,result));assert(result==old);
 std::cout<<"FACTORY_CONTRACT_PASS: bounded entry/resource/controller indices and post-return ownership, null and changed scope rejected; synthetic memory only\n";
}
