// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Captures the ALREADY POLLED port-0 Xbox report AFTER Android touch::Apply.
// Source: RanggaCasper/Black-Android@f3754d6 native/input.cpp::Poll.
// Never poll input::Poll twice; no guest memory writes and no original NPC AI.
#pragma once
#include <cstdint>
#include <mutex>
namespace blackcoop {
struct OriginalLocalPadFrame {
    bool connected=false;
    bool fromTouch=false; // touch controls changed the original composite pad
    int16_t lx=0,ly=0;
    uint8_t a=0,rightTrigger=0;
    int16_t rx=0,ry=0;
};
class OriginalLocalPadTap final {
    mutable std::mutex lock_;
    OriginalLocalPadFrame frame_{};
    uint64_t timestamp_=0;
    static constexpr uint64_t kFreshMs=250;
public:
    // Keep all axes, buttons and the timestamp in the same report.
    void Capture(OriginalLocalPadFrame sample,uint64_t nowMs)noexcept {
        if(nowMs==0)return;
        std::lock_guard guard(lock_);
        frame_=sample;
        timestamp_=nowMs;
    }
    OriginalLocalPadFrame Read(uint64_t nowMs)const noexcept {
        std::lock_guard guard(lock_);
        if(!timestamp_||nowMs<timestamp_||nowMs-timestamp_>kFreshMs)return {};
        return frame_;
    }
    void Reset()noexcept{
        std::lock_guard guard(lock_);
        frame_={};
        timestamp_=0;
    }
};
inline OriginalLocalPadTap& LocalPadTap()noexcept{
    static OriginalLocalPadTap tap;
    return tap;
}
} // namespace blackcoop
