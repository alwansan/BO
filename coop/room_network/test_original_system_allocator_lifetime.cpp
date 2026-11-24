// Runs the actual patched pinned SDK allocator, not a mocked allocation map.
#include "kernel/memory.h"
#include "core/log.h"
#include <cassert>
#include <iostream>
namespace logx { void Write(Level,const char*,...) {} }
int main(){
 assert(mem::Init());
 const uint32_t bytes=0x900;
 auto a=mem::AllocSystem(bytes);
 assert(a);
 auto g=mem::CoopSystemAllocationGeneration(a,bytes);
 assert(g);
 assert(!mem::CoopSystemAllocationGeneration(a+16,bytes-16));
 assert(!mem::CoopSystemAllocationGeneration(a,bytes-16));
 auto b=mem::AllocSystem(bytes);
 auto h=mem::CoopSystemAllocationGeneration(b,bytes);
 assert(b&&b!=a&&h&&h!=g);
 mem::FreeSystem(a);
 assert(!mem::CoopSystemAllocationGeneration(a,bytes));
 auto reused=mem::AllocSystem(bytes);
 assert(reused==a);
 auto k=mem::CoopSystemAllocationGeneration(reused,bytes);
 assert(k&&k!=g&&k!=h);
 assert(mem::CoopSystemAllocationGeneration(b,bytes)==h);
 mem::FreeSystem(reused);
 mem::FreeSystem(b);
 auto page=mem::AllocSystem(0x1000,0x1000);
 assert(page&&mem::CoopSystemAllocationGeneration(page,0x1000));
 mem::Decommit(page,0x1000);
 assert(!mem::CoopSystemAllocationGeneration(page,0x1000));
 assert(mem::Commit(page,0x1000));
 assert(!mem::CoopSystemAllocationGeneration(page,0x1000));
 mem::FreeSystem(page);
 std::cout<<"PINNED_SDK_REAL_SYSTEM_ALLOCATOR_LIFETIME_PASS_NO_GAME_HEAP_PROOF\n";
}
