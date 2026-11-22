// BO-COOP: bounded GPU vertex INPUT DECLARATION evidence, synthetic classifier.
// No shader code/IDs, no XBE, no MODEL resource pointer, no guest writes.
// Observing a D3D declaration does NOT establish that a particular character
// MODEL uses it, and NEVER authorizes STORY_READY or remote body creation.
#pragma once
#include <array>
#include <atomic>
#include <cstdint>

namespace blackcoop::gpu_probe {
struct Input {
    int stream=-1;
    uint32_t offset=0;
    uint32_t type=0; // original Xbox D3DVSDT, not a vertex byte value
};
struct Decl {
    std::array<Input,16> input{};
    std::array<uint32_t,16> stride{};
};
enum Category : uint32_t {
    kStride36=1u<<0,
    kSlot12Byte4=1u<<1,
    kSlot16Byte4=1u<<2,
    kSlot20Byte4=1u<<3,
    kSlot24PackedNormal=1u<<4,
    kSlot28Float2=1u<<5,
};
inline uint32_t Classify(const Decl& d) noexcept {
    uint32_t bits=0;
    for(const Input& in:d.input) {
        if(in.stream<0||in.stream>=16||d.stride[static_cast<unsigned>(in.stream)]!=36)
            continue;
        bits|=kStride36;
        const bool byte4=(in.type==0x40 || in.type==0x44 || in.type==0x34);
        if(in.offset==12 && byte4)bits|=kSlot12Byte4;
        if(in.offset==16 && byte4)bits|=kSlot16Byte4;
        if(in.offset==20 && byte4)bits|=kSlot20Byte4;
        if(in.offset==24 && in.type==0x16)bits|=kSlot24PackedNormal;
        if(in.offset==28 && in.type==0x22)bits|=kSlot28Float2;
    }
    return bits;
}
// Source-only draw classification: both the original parsed declaration and
// the CURRENT D3D8 bound vertex stream must independently agree on 36 bytes.
// It observes a draw call, but does not associate the stream with a MODEL.
struct BoundStream {
    bool has_vertex_buffer=false;
    uint32_t stride=0;
};
using BoundStreams=std::array<BoundStream,16>;
enum DrawCategory : uint32_t {
    kDrawIndexedActive36=1u<<0,
    kDrawArrayActive36=1u<<1,
    kDrawPacked12And20SameStream=1u<<2,
};
inline uint32_t ClassifyBoundDraw(const Decl& decl,
                                  const BoundStreams& bound,
                                  bool indexed) noexcept {
    bool active=false;
    uint32_t packed12=0,packed20=0;
    for(const Input& in:decl.input) {
        if(in.stream<0||in.stream>=16)continue;
        const unsigned slot=static_cast<unsigned>(in.stream);
        if(!bound[slot].has_vertex_buffer ||
           bound[slot].stride!=36 || decl.stride[slot]!=36)continue;
        active=true;
        const bool byte4=(in.type==0x40 || in.type==0x44 || in.type==0x34);
        if(in.offset==12 && byte4)packed12|=1u<<slot;
        if(in.offset==20 && byte4)packed20|=1u<<slot;
    }
    if(!active)return 0;
    uint32_t flags=indexed?kDrawIndexedActive36:kDrawArrayActive36;
    if((packed12&packed20)!=0)flags|=kDrawPacked12And20SameStream;
    return flags;
}
class Once final {
    std::atomic<uint32_t> delivered_{0};
public:
    // Duplicate shader creation, even on multiple threads, can't spam
    // the async room diagnostics with repeated events.
    uint32_t NewBits(uint32_t classified) noexcept {
        constexpr uint32_t allowed=(1u<<6)-1;
        classified&=allowed;
        return classified & ~delivered_.fetch_or(classified,std::memory_order_acq_rel);
    }
};
} // namespace blackcoop::gpu_probe
