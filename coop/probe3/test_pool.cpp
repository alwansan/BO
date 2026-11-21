// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "../probe/probe_core.hpp"
#include "pool_snapshot.hpp"
#include <cassert>
#include <vector>
#include <iostream>
#include <sys/mman.h>

// Actual kernel self-process reads. Bytes represent guest memory layout, not
// a live game, and carry no user capture or game assets.
struct Fixture {
    std::vector<uint8_t> bytes=std::vector<uint8_t>(0x12000);
    static constexpr uint32_t manager=0x10000;
    bo_probe::GuestReader reader() const {return bo_probe::GuestReader(reinterpret_cast<uintptr_t>(bytes.data())-manager);}
    void put(uint32_t guest,uint32_t value) {assert(guest>=manager && guest+4<=manager+bytes.size());std::memcpy(bytes.data()+guest-manager,&value,4);}
    Fixture() {
        put(manager,0x28D254);put(manager+0x8524,123);
        const uint32_t headers[]={0x79A0,0x79B0,0x79C0,0x79D0};
        const uint32_t cap[]={32,16,8,8};
        for(unsigned i=0;i<4;++i) {
            put(manager+headers[i],i?7:0);put(manager+headers[i]+4,cap[i]);
            put(manager+headers[i]+8,i==0?31:(i==1?1:0));
            put(manager+headers[i]+12,manager+0x9000+i*0x100);
        }
        for(unsigned i=0;i<32;++i) {
            uint32_t actor=manager+0xA0+i*0x3C0;
            put(actor,0x28C1E8);put(actor+0x250,manager+(i==31?0x79B0:0x79A0));
            put(actor+0x374,100+i);put(actor+0x384,0);
            if(i!=31)put(manager+0x9000+i*4,actor);
        }
        // One occupied actor after fifteen holes. occupied!=capacity.
        put(manager+0x9100+15*4,manager+0x74E0);
    }
};
struct ChangingReader {
    Fixture& fixture; bool reorder=false; mutable unsigned reads=0;
    template<class T> bool Read(uint32_t va,T& value) const noexcept {
        if(va==0x10000 && ++reads==2) {
            if(reorder) {fixture.put(0x19000,0x10460);fixture.put(0x19004,0x100A0);}
            else fixture.put(0x18524,124);
        }
        return fixture.reader().Read(va,value);
    }
    template<class T> bool Field(uint32_t object,uint32_t offset,T& value) const noexcept {
        return fixture.reader().Field(object,offset,value);
    }
};
int main() {
    Fixture f;bo_probe3::PoolSnapshot out;
    assert(bo_probe3::ReadPool(f.reader(),Fixture::manager,out));
    assert(out.counter==123 && out.actors[31].serial==131);
    assert(out.actors[31].container==1 && out.actors[31].state==0);
    assert(out.containers[1].occupied==1 && out.containers[1].capacity==16);
    // Mutation catches scanning only [0,occupied), or treating +4 as count.
    const auto reject=[&](Fixture& bad) {
        bo_probe3::PoolSnapshot saved=out;
        assert(!bo_probe3::ReadPool(bad.reader(),Fixture::manager,out));
        assert(out==saved);
    };
    {Fixture bad;bad.put(0x19100,0x174E0);reject(bad);} // duplicate across holes
    {Fixture bad;bad.put(0x179B8,17);reject(bad);} // occupied > capacity
    {Fixture bad;bad.put(0x179B4,32);reject(bad);} // wrong known capacity
    {Fixture bad;bad.put(0x1913C,0x174E4);reject(bad);} // not slot start
    {Fixture bad;bad.put(0x1913C,0x19000);reject(bad);} // outside pool
    {Fixture bad;bad.put(0x17730,0x179C0);reject(bad);} // actor+250 wrong owner
    {Fixture bad;bad.put(0x174E0,0x28CFE0);reject(bad);} // player table not NPC slot
    {Fixture bad;bad.put(0x179B8,0);reject(bad);} // physical nonzero != occupied
    {Fixture bad;bad.put(0x179BC,0xFFFFFFFF);reject(bad);} // pointer range overflow
    {Fixture bad;bad.put(0x179BC,0x10000);reject(bad);} // header bytes cannot impersonate array
    {Fixture bad;bad.put(0x179A8,30);bad.put(0x19078,0);reject(bad);} // missing slot
    {Fixture bad;bad.put(0x10000,0);reject(bad);}
    assert(!bo_probe3::ReadPool(f.reader(),0,out));
    assert(!bo_probe3::ReadPool(f.reader(),0xFFFF8000,out));
    // A real inaccessible guest copy must fail without publishing partial data.
    void* page=mmap(nullptr,4096,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    assert(page!=MAP_FAILED);
    const auto inaccessible=bo_probe::GuestReader(reinterpret_cast<uintptr_t>(page)-0x10000);
    const auto before=out;
    assert(!bo_probe3::ReadPool(inaccessible,0x10000,out) && out==before);
    assert(munmap(page,4096)==0);
    // Simulate an actual counter write between observations while retaining
    // real GuestReader copies. A single-pass reader would wrongly accept.
    Fixture moving; ChangingReader moving_reader{moving};
    assert(!bo_probe3::ReadPool(moving_reader,0x10000,out) && out==before);
    Fixture reordered; ChangingReader reorder_reader{reordered,true};
    assert(!bo_probe3::ReadPool(reorder_reader,0x10000,out) && out==before);
    // Late failure: first container and its31 actors read successfully, then
    // the second sparse array runs into a real PROT_NONE page after entry0.
    Fixture late;late.put(0x179BC,0x1AFFC);late.put(0x1AFFC,0x174E0);
    void* region=mmap(nullptr,0x12000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    assert(region!=MAP_FAILED);std::memcpy(region,late.bytes.data(),late.bytes.size());
    assert(mprotect(static_cast<uint8_t*>(region)+0xB000,4096,PROT_NONE)==0);
    const auto late_reader=bo_probe::GuestReader(reinterpret_cast<uintptr_t>(region)-0x10000);
    assert(!bo_probe3::ReadPool(late_reader,0x10000,out) && out==before);
    assert(munmap(region,0x12000)==0);
    std::cout<<"PASS sparse pool, membership, bounds and fail-closed output (synthetic memory only)\n";
}
