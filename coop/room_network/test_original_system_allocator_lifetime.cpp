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
 // Actual contiguous physical memory allocator: exact canonical base,
 // no interior/write-combine alias and a new generation after address reuse.
 constexpr uint32_t bankBytes=0x293000u;
 auto physical=mem::AllocContiguous(bankBytes,0,0xffffffffu,0x1000u);
 assert(physical);
 auto pg=mem::CoopContiguousAllocationGeneration(physical,bankBytes);
 assert(pg&&!mem::CoopSystemAllocationGeneration(physical,bankBytes));
 assert(!mem::CoopContiguousAllocationGeneration(physical+0x1000u,bankBytes-0x1000u));
 assert(!mem::CoopContiguousAllocationGeneration(
   mem::kWriteCombineBase+(physical-mem::kContiguousBase),bankBytes));
 mem::FreeContiguous(physical);
 assert(!mem::CoopContiguousAllocationGeneration(physical,bankBytes));
 auto again=mem::AllocContiguous(bankBytes,0,0xffffffffu,0x1000u);
 assert(again==physical);
 assert(mem::CoopContiguousAllocationGeneration(again,bankBytes)!=pg);
 assert(mem::CoopContiguousAllocationGeneration(again,bankBytes));
 mem::FreeContiguous(again);
 std::cout<<"PINNED_SDK_REAL_SYSTEM_ALLOCATOR_LIFETIME_PASS_NO_GAME_HEAP_PROOF\n";
}
