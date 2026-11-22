// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original XBE source-confirmed BYTE reads in streaming candidate 0x001EDC90.
// PC recomp labels game+0x2020C level and +0x2020E stage, but their actual
// original host mission identity, loader ABI, and scene completion are NOT
// independently proven. NEVER publish STORY_READY from these hints.
#pragma once
#include <atomic>
#include <cstdint>
namespace blackcoop {
struct OriginalStreamingStageHints {
    uint32_t episode=0;
    int level=-1,stage=-1;
};
inline int BucketOriginalStreamingLevel(uint32_t value)noexcept {
    return value<=8?static_cast<int>(value):-1;
}
inline int BucketOriginalStreamingStage(uint32_t value)noexcept {
    return value<=15?static_cast<int>(value):-1;
}
// 1530 fixed safe codes 500..2029; no original pointers/strings exported.
inline int EncodeOriginalStreamingStageHint(int level,int stage,int folder)noexcept {
    if(level< -1||level>8||stage< -1||stage>15||
       (folder!=0&&folder!=1&&(folder<3||folder>8)))return -1;
    return 500+(((level+1)*17+(stage+1))*9)+folder;
}
class OriginalStreamingStageObservation final {
    std::atomic<uint64_t> value_{0};
    static constexpr unsigned kEpisodeShift=10;
    static constexpr uint64_t kWitness=uint64_t{1}<<9;
public:
    // Preserve FIRST streaming entry from this exact launch episode.
    // Later stale calls cannot overwrite a newer episode; repeat stage
    // callbacks cannot relabel the first entry for this mission.
    bool CaptureAtStreamingEntry(uint32_t episode,uint32_t rawLevel,
                                 uint32_t rawStage)noexcept {
        if(!episode)return false;
        const uint64_t next=(static_cast<uint64_t>(episode)<<kEpisodeShift)|
            kWitness|
            (static_cast<uint64_t>(BucketOriginalStreamingLevel(rawLevel)+1)<<5)|
            static_cast<uint64_t>(BucketOriginalStreamingStage(rawStage)+1);
        uint64_t old=value_.load(std::memory_order_acquire);
        while(static_cast<uint32_t>(old>>kEpisodeShift)<episode) {
            if(value_.compare_exchange_weak(old,next,std::memory_order_acq_rel,
                                            std::memory_order_acquire))return true;
        }
        return false;
    }
    OriginalStreamingStageHints ForCorrelatedEpisode(uint32_t episode)const noexcept {
        const uint64_t snapshot=value_.load(std::memory_order_acquire);
        if(!episode||static_cast<uint32_t>(snapshot>>kEpisodeShift)!=episode||
           !(snapshot&kWitness))return {};
        return {episode,static_cast<int>((snapshot>>5)&15)-1,
                static_cast<int>(snapshot&31)-1};
    }
};
inline OriginalStreamingStageObservation g_originalStreamingStageHints;
} // namespace blackcoop
