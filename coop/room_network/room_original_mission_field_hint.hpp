// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Read-only, bounded candidate selector globals observed by private original
// XBE static xrefs: 0x2D0D60 read in level choice, 0x2D0D68 written near
// launch candidate. Neither is PROVEN to hold the original mission index.
// Never pass these candidates into a stage loader or story ticket.
#pragma once
#include <atomic>
#include <cstdint>
namespace blackcoop {
struct OriginalMissionFieldHints {
    uint32_t episode=0;
    int readCandidate=-1,writeCandidate=-1;
};
// Preserve at most two 0..7 values, bucket other values as -1.
// (Owner mission count is eight; these are *numeric hypotheses* only.)
inline int ClassifyOriginalMissionFieldHint(uint32_t value)noexcept{
    return value<8?static_cast<int>(value):-1;
}
class OriginalMissionFieldObservation final {
    std::atomic<uint64_t> observation_{0};
public:
    void CaptureAtOriginalLaunch(uint32_t episode,uint32_t readValue,
                                 uint32_t writeValue)noexcept{
        if(episode==0)return;
        const uint64_t encoded=
            (static_cast<uint64_t>(episode)<<8)|
            (static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(readValue)+1)<<4)|
            static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(writeValue)+1);
        observation_.store(encoded,std::memory_order_release);
    }
    OriginalMissionFieldHints ForCorrelatedEpisode(uint32_t episode)const noexcept{
        const uint64_t encoded=observation_.load(std::memory_order_acquire);
        if(episode==0||static_cast<uint32_t>(encoded>>8)!=episode)
            return {};
        return {episode,
            static_cast<int>((encoded>>4)&15)-1,
            static_cast<int>(encoded&15)-1};
    }
};
inline OriginalMissionFieldObservation g_originalMissionFieldHints;
} // namespace blackcoop
