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
    // No phantom draw: both parsed layout and actual bound stream must match.
    BoundStreams bs{};
    assert(ClassifyBoundDraw(d,bs,true)==0);
    bs[0]={true,36};
    assert(ClassifyBoundDraw(d,bs,true)==
           (kDrawIndexedActive36|kDrawPacked12And20SameStream));
    assert(ClassifyBoundDraw(d,bs,false)==
           (kDrawArrayActive36|kDrawPacked12And20SameStream));
    bs[0].stride=40;
    assert(ClassifyBoundDraw(d,bs,true)==0);
    bs[0].stride=36;
    bs[0].has_vertex_buffer=false;
    assert(ClassifyBoundDraw(d,bs,false)==0);
    bs[0].has_vertex_buffer=true;
    d.stride[0]=32;
    assert(ClassifyBoundDraw(d,bs,true)==0);
    d.stride[0]=36;
    d.input[3].type=0x32;
    assert(ClassifyBoundDraw(d,bs,true)==kDrawIndexedActive36);
    d.input[3].type=0x34;
    // Never combine packed offsets from two different streams.
    Decl other{};
    other.stride[0]=36;other.stride[1]=36;
    other.input[0]={0,12,0x44};
    other.input[1]={1,20,0x40};
    bs[1]={true,36};
    assert(ClassifyBoundDraw(other,bs,true)==kDrawIndexedActive36);
    other.input[2]={1,12,0x44};
    assert(ClassifyBoundDraw(other,bs,true)==
           (kDrawIndexedActive36|kDrawPacked12And20SameStream));
    bs[1].stride=0;
    assert(ClassifyBoundDraw(other,bs,true)==kDrawIndexedActive36);
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
    std::cout<<"GPU_V36_DECL_CONTRACT_PASS: fixed draw/layout codes, no MODEL identity, skinning or scene proof\n";
}
