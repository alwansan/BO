// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "test_fixture.hpp"
#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
namespace bo_probe {
std::atomic<bool> enabled{true};std::mutex lock;Budget budget;
int output=1;uint64_t opened_at=1000;std::string captured;
uint64_t Now()noexcept{return 1000;}
bool Init(uint64_t){return true;}
bool Write(const char* data,size_t size){captured.append(data,size);return true;}
#include "factory_observer.inc"
}
int main(){
 Memory m;m.put(0xB0004,m.brain);m.put(0xB0008,m.descriptor);
 const auto unchanged=m.bytes;
 errno=EDOM;auto ticket=bo_probe::FactoryBegin(m.manager,0xB0000,m.bytes.data());assert(errno==EDOM&&ticket.valid);
 bo_probe::FactoryFinish(ticket,m.actor,m.bytes.data());assert(errno==EDOM);
 assert(bo_probe::captured.find("\"links_verified\":true")!=std::string::npos);
 assert(bo_probe::captured.find("\"resource_index\":2")!=std::string::npos);
 assert(m.bytes==unchanged);
 // Raw guest pointers, pose coordinates and descriptor buffers never enter JSON.
 assert(bo_probe::captured.find("524288")==std::string::npos&&bo_probe::captured.find("196608")==std::string::npos);
 for(unsigned n=1;n<40;++n){auto t=bo_probe::FactoryBegin(m.manager,0xB0000,m.bytes.data());bo_probe::FactoryFinish(t,m.actor,m.bytes.data());}
 assert(bo_probe::factory_attempts==32);assert(std::count(bo_probe::captured.begin(),bo_probe::captured.end(),'\n')==32);
 auto before=bo_probe::captured;bo_probe::enabled.store(false);
 auto stopped=bo_probe::FactoryBegin(m.manager,0xB0000,m.bytes.data());assert(!stopped.sampled);bo_probe::FactoryFinish(ticket,m.actor,m.bytes.data());assert(bo_probe::captured==before);
 std::cout<<bo_probe::captured;
}
