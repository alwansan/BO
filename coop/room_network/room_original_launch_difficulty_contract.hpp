// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// The pinned original Black-Android cheats.cpp names game+0x2014C as
// kDifficulty, range 0..3 (easy, normal, hard, black ops).
// This is NOT a mission ID, selected level, stage-loaded event or loader.
#pragma once
#include <cstdint>

namespace blackcoop {
enum class OriginalLaunchDifficulty: uint8_t {
    Easy=0,Normal=1,Hard=2,BlackOps=3,Unavailable=4
};
inline OriginalLaunchDifficulty ClassifyOriginalLaunchDifficulty(
    bool gamePresent,uint32_t rawValue)noexcept{
    if(!gamePresent||rawValue>3)return OriginalLaunchDifficulty::Unavailable;
    return static_cast<OriginalLaunchDifficulty>(rawValue);
}
class OriginalLaunchDifficultyOnce final {
    uint8_t observedMask_=0;
public:
    // Fixed event code 20..24, at most five notices / process.
    // Never exposes raw memory, source pointers, game-save flags or assets.
    int Observe(bool gamePresent,uint32_t rawValue)noexcept{
        const auto value=ClassifyOriginalLaunchDifficulty(gamePresent,rawValue);
        const auto kind=static_cast<uint8_t>(value);
        if((observedMask_ & (1u << kind)) != 0)return 0;
        observedMask_=static_cast<uint8_t>(observedMask_ | (1u << kind));
        return 20+kind;
    }
};
} // namespace blackcoop
