// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Read-only, bounded candidate selector globals observed by private original
// XBE static xrefs: 0x2D0D60 read in level choice, 0x2D0D68 written near
// launch, and 0x2EC598 written near level choice/difficulty. All three are
// source-observed WORD-width fields. NONE is a proven mission index.
// Never pass these candidates into a stage loader or story ticket.
#pragma once
#include <atomic>
#include <cstdint>
namespace blackcoop {
struct OriginalMissionFieldHints {
    uint32_t episode=0;
    int readCandidate=-1,writeCandidate=-1,menuCandidate=-1;
};
// Preserve at most three 0..7 values, bucket other values as -1.
// (Owner mission count is eight; these are *numeric hypotheses* only.)
inline int ClassifyOriginalMissionFieldHint(uint32_t value)noexcept{
    return value<8?static_cast<int>(value):-1;
}
inline int EncodeOriginalMissionFieldEvent(int which,int candidate,int folder)noexcept{
    // This numeric tuple is UNTRUSTED evidence: never trigger stage READY.
    if((which<0||which>2)||candidate< -1||candidate>7||
       (folder!=0&&folder!=1&&(folder<3||folder>8)))return -1;
    return 100+((which*9+(candidate+1))*9)+folder;
}
class OriginalMissionFieldObservation final {
    std::atomic<uint64_t> observation_{0};
public:
    void CaptureAtOriginalLaunch(uint32_t episode,uint32_t readValue,
                                 uint32_t writeValue,uint32_t menuValue)noexcept{
        if(episode==0)return;
        const uint64_t encoded=
            (static_cast<uint64_t>(episode)<<12)|
            (static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(readValue)+1)<<8)|
            (static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(writeValue)+1)<<4)|
            static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(menuValue)+1);
        observation_.store(encoded,std::memory_order_release);
    }
    OriginalMissionFieldHints ForCorrelatedEpisode(uint32_t episode)const noexcept{
        const uint64_t encoded=observation_.load(std::memory_order_acquire);
        if(episode==0||static_cast<uint32_t>(encoded>>12)!=episode)
            return {};
        return {episode,
            static_cast<int>((encoded>>8)&15)-1,
            static_cast<int>((encoded>>4)&15)-1,
            static_cast<int>(encoded&15)-1};
    }
};
inline OriginalMissionFieldObservation g_originalMissionFieldHints;
} // namespace blackcoop
