// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "test_fixture.hpp"
#include <atomic>
#include <mutex>
#include <algorithm>
namespace bo_probe {
std::atomic<bool> enabled{true};std::mutex lock;Budget budget;uint64_t opened_at=1000,clock_ms=1000;std::string captured;
uint64_t Now()noexcept{return clock_ms;}
bool Init(uint64_t){return true;}
bool Write(const char* text,size_t n){captured.append(text,n);return true;}
#include "model_observer.inc"
}
int main(){
 Memory m;const auto before=m.bytes;
 errno=EDOM;bo_probe::ObserveDefaultModel(m.actor,m.bytes.data());assert(errno==EDOM&&m.bytes==before);
 assert(bo_probe::captured.find("\"body_candidate\":true")!=std::string::npos);
 auto first=bo_probe::captured;bo_probe::ObserveDefaultModel(m.actor,m.bytes.data());assert(first==bo_probe::captured);
 for(unsigned i=2;i<20;i++){bo_probe::clock_ms=i*1000;bo_probe::ObserveDefaultModel(m.actor,m.bytes.data());}
 assert(std::count(bo_probe::captured.begin(),bo_probe::captured.end(),'\n')==8&&m.bytes==before);
 std::cout<<bo_probe::captured;
}
