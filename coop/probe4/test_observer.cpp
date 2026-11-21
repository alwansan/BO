// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "../probe/probe_core.hpp"
#include "../probe3/pool_snapshot.hpp"
#include "instance_graph.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
// Run the actual observer include with an in-memory log sink and deterministic
// clock. GuestReader, budgets, safe kernel reads and formatting remain real.
namespace bo_probe {
std::atomic<bool> enabled{true};
std::mutex lock;
Budget budget;
Registry registry;
int output=1;
uint64_t clock_ms=1000;
std::string captured;
uint64_t Now() noexcept {return clock_ms;}
void End(const char*) noexcept {enabled.store(false);}
bool Write(const char* data,size_t n) {captured.append(data,n);return true;}
#include "pool_observer.inc"
}
int main() {
    std::vector<uint8_t> memory(0x2D5000);
    const auto put=[&](uint32_t address,uint32_t value) {std::memcpy(memory.data()+address,&value,4);};
    put(0x20030,0x28CFE0);put(0x203A4,7);put(0x20358,0x21000);
    put(0x21000,0x20030);put(0x21050,0x23000);put(0x21054,0x22000);
    put(0x2037C,0x24000);put(0x20380,0x25000);
    put(0x2D45D4,0x20000);put(0x2D1980,0x10000);put(0x10000,0x28D254);put(0x18524,42);
    const uint32_t headers[]={0x179A0,0x179B0,0x179C0,0x179D0};
    const uint32_t capacity[]={32,16,8,8};
    for(unsigned c=0;c<4;++c) {
        put(headers[c],c?7:0);put(headers[c]+4,capacity[c]);put(headers[c]+8,c?0:32);
        put(headers[c]+12,0x19000+c*0x100);
    }
    for(unsigned i=0;i<32;++i) {
        const uint32_t actor=0x100A0+i*0x3C0;
        put(actor,0x28C1E8);put(actor+0x250,0x179A0);put(actor+0x374,0xFFFFFFFF);
        put(actor+0x384,2);put(0x19000+i*4,actor);
    }
    bo_probe::ObservePool(memory.data());assert(bo_probe::captured.empty()); // No capture budget yet.
    bo_probe::budget.Start(1000);bo_probe::registry.World(0x20000);
    errno=EDOM;bo_probe::ObservePool(memory.data());assert(errno==EDOM);
    assert(bo_probe::captured.find("\"valid\":true")!=std::string::npos);
    assert(bo_probe::captured.find("[31,0,0,0,0,0]")!=std::string::npos); // Free serial never claimed.
    assert(bo_probe::captured.find("4294967295")==std::string::npos);
    auto first=bo_probe::captured;bo_probe::ObservePool(memory.data());assert(bo_probe::captured==first);
    bo_probe::clock_ms=2000;put(0x2D1980,0);bo_probe::ObservePool(memory.data());
    assert(bo_probe::captured.find("\"valid\":false")!=std::string::npos);
    auto second=bo_probe::captured;bo_probe::ObservePool(memory.data());assert(bo_probe::captured==second);
    bo_probe::clock_ms=3000;put(0x2D1980,0x10000);bo_probe::pool_reset_sequence.store(7);
    put(0x179A8,31);put(0x1907C,0);put(0x179B8,1);put(0x1913C,0x174E0);
    put(0x17730,0x179B0);put(0x17854,131);put(0x17864,0);
    put(0x17800,0x51F00);put(0x1783C,0x60000);
    put(0x17808,0x26000);put(0x26000,0x174E0);put(0x26050,0x28000);put(0x26054,0x27000);
    put(0x1782C,0x29000);put(0x17830,0x2A000);
    bo_probe::ObservePool(memory.data());
    assert(bo_probe::captured.find("[31,1,131,0,335616,393216]")!=std::string::npos);
    assert(bo_probe::captured.find("\"reset_sequence\":7")!=std::string::npos);
    assert(bo_probe::captured.find("[0,31,131,1,155648,159744,163840,167936,172032]")!=std::string::npos);
    assert(bo_probe::captured.find("[1,0,7,1,135168,139264,143360,147456,151552]")!=std::string::npos);
    const auto third=bo_probe::captured;bo_probe::enabled.store(false);bo_probe::clock_ms=4000;
    bo_probe::ObservePool(memory.data());assert(bo_probe::captured==third);
    std::cout<<bo_probe::captured;
}
