// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "../probe/probe_core.hpp"
#include "instance_graph.hpp"
#include <cassert>
#include <vector>
#include <iostream>
struct Memory {
    std::vector<uint8_t> bytes=std::vector<uint8_t>(0x20000);
    void put(uint32_t address,uint32_t value) {assert(address+4<=bytes.size());std::memcpy(bytes.data()+address,&value,4);}
    bo_probe::GuestReader reader() const {return bo_probe::GuestReader(reinterpret_cast<uintptr_t>(bytes.data()));}
    Memory() {
        put(0x10000,0x28C1E8);put(0x10250,0x179B0);put(0x10374,17);
        put(0x10320,0x19000);put(0x10328,0x18000);put(0x1034C,0x1A000);put(0x10350,0x1B000);
        put(0x18000,0x10000);put(0x18050,0x1C000);put(0x18054,0x1D000);
    }
};
struct ChangingReader {
    Memory& memory;mutable unsigned controller_reads=0;bool recycle=false;
    template<class T>bool Read(uint32_t address,T& value) const noexcept {
        if(address==0x18000 && ++controller_reads==2)memory.put(recycle?0x10374:0x18000,recycle?18:0x11000);
        return memory.reader().Read(address,value);
    }
    template<class T>bool Field(uint32_t address,uint32_t offset,T& value) const noexcept {
        if(uint64_t(address)+offset>UINT32_MAX)return false;
        return Read(address+offset,value);
    }
};
struct CountingReader {
    bo_probe::GuestReader reader;mutable unsigned calls=0;
    template<class T>bool Read(uint32_t address,T& value)const noexcept {++calls;return reader.Read(address,value);}
    template<class T>bool Field(uint32_t address,uint32_t offset,T& value)const noexcept {
        if(uint64_t(address)+offset>UINT32_MAX)return false;
        return Read(address+offset,value);
    }
};
int main() {
    Memory memory;bo_probe4::InstanceGraph graph;
    const bo_probe4::InstanceKey key{0x10000,17,0x179B0,0x28C1E8};
    assert(bo_probe4::ReadInstance(memory.reader(),key,graph));
    assert(graph.controller==0x18000 && graph.runtime==0x1D000 && graph.binding==0x1C000);
    assert(graph.scratchA==0x1A000 && graph.scratchB==0x1B000);
    CountingReader counted{memory.reader()};
    assert(bo_probe4::ReadInstance(counted,key,graph) && counted.calls<=8);
    const auto before=graph;
    for(unsigned which=0;which<5;++which) {
        Memory bad;
        if(which==0)bad.put(0x18000,0x11000);
        if(which==1)bad.put(0x10374,18);
        if(which==2)bad.put(0x10250,0x179C0);
        if(which==3)bad.put(0x10328,0xFFFFFFF0);
        if(which==4)bad.put(0x10000,0x28CFE0);
        assert(!bo_probe4::ReadInstance(bad.reader(),key,graph));assert(graph==before);
    }
    Memory changing;ChangingReader moving{changing};
    assert(!bo_probe4::ReadInstance(moving,key,graph) && graph==before);
    Memory reused;ChangingReader recycled{reused,0,true};
    assert(!bo_probe4::ReadInstance(recycled,key,graph) && graph==before);
    auto invalid=key;invalid.actor=0xFFFFFE00;
    assert(!bo_probe4::ReadInstance(memory.reader(),invalid,graph) && graph==before);
    // The local player shares base layout but must not be accepted as a pool NPC.
    Memory local;local.put(0x10000,0x28CFE0);local.put(0x10250,0);
    assert(bo_probe4::ReadInstance(local.reader(),{0x10000,17,0,0x28CFE0},graph));
    std::cout<<"PASS instance backlink, slot reuse, bounds, local distinction and unchanged failed output (synthetic memory)\n";
}
