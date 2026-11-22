// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Captures the ALREADY POLLED port-0 Xbox report AFTER Android touch::Apply.
// Source: RanggaCasper/Black-Android@f3754d6 native/input.cpp::Poll.
// Never poll input::Poll twice; no guest memory writes and no original NPC AI.
#pragma once
#include <atomic>
#include <cstdint>
namespace blackcoop {
struct OriginalLocalPadFrame {
    bool connected=false;
    bool fromTouch=false; // touch controls changed the original composite pad
    int16_t lx=0,ly=0;
    uint8_t a=0,rightTrigger=0;
};
class OriginalLocalPadTap final {
    std::atomic<uint64_t> word_{0};
    std::atomic<uint64_t> timestamp_{0};
    static constexpr uint64_t kFreshMs=250;
    static uint64_t Encode(OriginalLocalPadFrame p)noexcept {
        return static_cast<uint64_t>(static_cast<uint16_t>(p.lx))|
          (static_cast<uint64_t>(static_cast<uint16_t>(p.ly))<<16)|
          (static_cast<uint64_t>(p.a)<<32)|
          (static_cast<uint64_t>(p.rightTrigger)<<40)|
          (p.connected?uint64_t{1}<<48:0)|
          (p.fromTouch?uint64_t{1}<<49:0);
    }
    static OriginalLocalPadFrame Decode(uint64_t bits)noexcept {
        OriginalLocalPadFrame r{};
        r.lx=static_cast<int16_t>(static_cast<uint16_t>(bits));
        r.ly=static_cast<int16_t>(static_cast<uint16_t>(bits>>16));
        r.a=static_cast<uint8_t>(bits>>32);
        r.rightTrigger=static_cast<uint8_t>(bits>>40);
        r.connected=(bits&(uint64_t{1}<<48))!=0;
        r.fromTouch=(bits&(uint64_t{1}<<49))!=0;
        return r;
    }
public:
    // Input path already generated the report. Keep this path O(1), lock-free.
    void Capture(OriginalLocalPadFrame sample,uint64_t nowMs)noexcept {
        if(nowMs==0)return;
        word_.store(Encode(sample),std::memory_order_release);
        timestamp_.store(nowMs,std::memory_order_release);
    }
    OriginalLocalPadFrame Read(uint64_t nowMs)const noexcept {
        if(nowMs==0)return {};
        for(int attempt=0;attempt<2;++attempt){
            uint64_t before=timestamp_.load(std::memory_order_acquire);
            uint64_t bits=word_.load(std::memory_order_acquire);
            uint64_t after=timestamp_.load(std::memory_order_acquire);
            if(before!=after)continue;
            if(before==0||nowMs<before||nowMs-before>kFreshMs)return {};
            return Decode(bits);
        }
        return {};
    }
    void Reset()noexcept{
        // Reset is used for unit tests and shutdown, not for a concurrently
        // active source stream. Guest story resets happen at JNI/TLS scope.
        word_.store(0,std::memory_order_release);
        timestamp_.store(0,std::memory_order_release);
    }
};
inline OriginalLocalPadTap& LocalPadTap()noexcept{
    static OriginalLocalPadTap tap;
    return tap;
}
} // namespace blackcoop
