#pragma once
// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "../probe/probe_core.hpp"
#include "factory_contract.hpp"
#include <cassert>
#include <vector>
#include <iostream>
struct Memory {
 std::vector<uint8_t> bytes=std::vector<uint8_t>(0x300000);
 void put(uint32_t p,uint32_t v){assert(uint64_t(p)+4<=bytes.size());std::memcpy(bytes.data()+p,&v,4);}
 bo_probe::GuestReader reader()const{return bo_probe::GuestReader(reinterpret_cast<uintptr_t>(bytes.data()));}
 uint32_t manager=0x10000,root=0x40000,brain=root+0x2B00,descriptor=0x80000,actor=manager+0xA0;
 Memory(){
  put(0x2D1980,manager);put(0x2D1988,root);put(0x2D45D4,0x90000);put(0x2D45F4,0xA0000);
  put(manager+0x79B4,16);put(manager+0x79B8,3);put(manager+0x79A4,32);put(manager+0x79A8,29);
  put(descriptor,manager+0x7A20+2*0x40);put(descriptor+0x3C,0);put(descriptor+0x40,7);put(descriptor+0x44,9);
  put(actor,0x28C1E8);put(actor+0x250,manager+0x79B0);put(actor+0x320,manager+0x7A20+2*0x40);
  put(actor+0x324,brain);put(actor+0x374,11);put(actor+0x384,0);put(brain+0x8C,actor);
 }
};
