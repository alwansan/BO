#pragma once
// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "../probe/probe_core.hpp"
#include "model_contract.hpp"
#include <vector>
#include <cassert>
#include <iostream>
struct Memory {
 std::vector<uint8_t> bytes=std::vector<uint8_t>(0x2E0000);
 uint32_t world=0x10000,actor=world+0x30,model=0x30000;
 void put(uint32_t p,uint32_t v){assert(uint64_t(p)+4<=bytes.size());std::memcpy(bytes.data()+p,&v,4);}
 bo_probe::GuestReader reader()const{return bo_probe::GuestReader(reinterpret_cast<uintptr_t>(bytes.data()));}
 Memory(){
  put(0x2D45D4,world);put(actor,0x28CFE0);put(actor+0xC4,2);put(actor+0x320,0x20000);put(0x20000,model);
  put(model+0x10,0xB8000000);put(model+0x14,0x95820AD4);put(model+0x18,35);
  put(model+0x5C,3);put(model+0x60,0x31000);put(model+0x24,1);put(model+0x1C,0x32000);bytes[0x32019]=5;
  for(unsigned i=0;i<3;i++)put(0x31000+i*4,0x33000+i*32);
  std::memcpy(bytes.data()+0x33000,"Head",5);std::memcpy(bytes.data()+0x33020,"Spine",6);std::memcpy(bytes.data()+0x33040,"Foot",5);
 }
};
