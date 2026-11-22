// Synthetic GPU declaration classification, never original shader/vertex data.
#include "gpu_decl_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <atomic>
using namespace blackcoop::gpu_probe;
int main() {
    Decl d{};
    assert(Classify(d)==0);
    d.stride[0]=36;
    d.input[0]={0,0,0x32}; // POSITION FLOAT3, 12 bytes
    assert(Classify(d)==kStride36);
    d.input[1]={0,12,0x44}; // PBYTE4
    d.input[2]={0,16,0x40}; // D3DCOLOR
    d.input[3]={0,20,0x34}; // packed byte3, 4 stored
    d.input[4]={0,24,0x16}; // NORMPACKED3, 11+11+10 bits
    d.input[5]={0,28,0x22}; // FLOAT2
    constexpr uint32_t all=(1u<<6)-1;
    assert(Classify(d)==all);
    d.input[2].type=0x32; // FLOAT3 is NOT packed four bytes
    assert(Classify(d)==(all&~kSlot16Byte4));
    d.input[2].type=0x40;
    d.input[5].stream=-1;
    assert(Classify(d)==(all&~kSlot28Float2));
    d.input[5]={0,28,0x22};
    d.input[6]={16,12,0x44}; // stream index out of range; safe
    assert(Classify(d)==all);
    d.stride[0]=24;
    assert(Classify(d)==0);
    d.stride[0]=36;
    Once once;
    assert(once.NewBits(kStride36)==kStride36);
    assert(once.NewBits(kStride36)==0);
    assert(once.NewBits(all)==(all&~kStride36));
    assert(once.NewBits(all)==0);
    Once concurrent;
    std::atomic<uint32_t> sum{0};
    std::thread a([&]{sum.fetch_or(concurrent.NewBits(all));});
    std::thread b([&]{sum.fetch_or(concurrent.NewBits(all));});
    a.join();b.join();
    assert(sum==all);
    assert(concurrent.NewBits(all)==0);
    // No code may claim a model's actual skin weights or loaded scene.
    std::cout<<"GPU_V36_DECL_CONTRACT_PASS: bounded fixed codes once, not proven model skinning or game scene\n";
}
