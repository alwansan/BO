// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// WORD-width original XBE field hints. No game memory writes or scene READY.
#pragma once
#include <atomic>
#include <cstdint>
namespace blackcoop {
struct OriginalMissionFieldHints {
    uint32_t episode=0;
    int readCandidate=-1,launchBeforeCandidate=-1;
    int launchAfterCandidate=-1,menuCandidate=-1;
};
inline int ClassifyOriginalMissionFieldHint(uint32_t v)noexcept {
    return v<8?static_cast<int>(v):-1;
}
inline int EncodeOriginalMissionFieldEvent(int which,int candidate,int folder)noexcept {
    if(which<0||which>3||candidate< -1||candidate>7||
       (folder!=0&&folder!=1&&(folder<3||folder>8)))return -1;
    return 100+((which*9+(candidate+1))*9)+folder;
}
class OriginalMissionFieldObservation final {
    static constexpr uint64_t kDone=uint64_t{1}<<16;
    static constexpr uint64_t kAfterMask=uint64_t{15}<<4;
    static constexpr unsigned kEpisodeShift=17;
    std::atomic<uint64_t> snapshot_{0};
public:
    // An unfinished original launch is NEVER correlated with stage files.
    void CaptureAtOriginalLaunch(uint32_t episode,uint32_t read,
                                 uint32_t before,uint32_t menu)noexcept {
        if(!episode)return;
        uint64_t value=(static_cast<uint64_t>(episode)<<kEpisodeShift)|
            (static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(read)+1)<<12)|
            (static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(before)+1)<<8)|
            static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(menu)+1);
        snapshot_.store(value,std::memory_order_release);
    }
    // Only the exact matching original function return may finalize B_AFTER.
    bool CompleteOriginalLaunch(uint32_t episode,uint32_t after)noexcept {
        if(!episode)return false;
        uint64_t old=snapshot_.load(std::memory_order_acquire);
        while(static_cast<uint32_t>(old>>kEpisodeShift)==episode&&!(old&kDone)){
            const uint64_t next=(old&~kAfterMask)|kDone|
                (static_cast<uint64_t>(ClassifyOriginalMissionFieldHint(after)+1)<<4);
            if(snapshot_.compare_exchange_weak(old,next,std::memory_order_acq_rel,
                                               std::memory_order_acquire))return true;
        }
        return false;
    }
    OriginalMissionFieldHints ForCorrelatedEpisode(uint32_t episode)const noexcept {
        const uint64_t value=snapshot_.load(std::memory_order_acquire);
        if(!episode||static_cast<uint32_t>(value>>kEpisodeShift)!=episode||
           !(value&kDone))return {};
        return {episode,static_cast<int>((value>>12)&15)-1,
                static_cast<int>((value>>8)&15)-1,
                static_cast<int>((value>>4)&15)-1,
                static_cast<int>(value&15)-1};
    }
};
inline OriginalMissionFieldObservation g_originalMissionFieldHints;
} // namespace blackcoop
