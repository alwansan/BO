// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// A minimal native witness for the *original local player's* update callback.
// Read-only, no actor creation, guest writes, network or mission confirmation.
#pragma once
#include <atomic>
#include <cstdint>

namespace blackcoop {
enum class WorldTickSignal : uint8_t {
    None=0, FirstLocalPlayerUpdate=1, WorldPointerChanged=2
};

// One process only; the opaque 32-bit guest address never leaves process RAM.
// Pointer changes are NOT proof of mission identity, loaded scene or epoch.
class WorldTickObserver final {
    std::atomic<uint32_t> last_{0};
    std::atomic<uint32_t> launch_attempts_{0};
public:
    // The existing difficulty-select callback is only a launch *attempt*.
    // Never infer selected mission/stage or a loaded scene from this event.
    // Bounded number of fixed-code notices (not one per frame).
    bool ObserveLaunchAttempt() noexcept {
        uint32_t current=launch_attempts_.load(std::memory_order_relaxed);
        while(current<8) {
            if(launch_attempts_.compare_exchange_weak(current,current+1,
                std::memory_order_acq_rel,std::memory_order_relaxed))return true;
        }
        return false;
    }
    WorldTickSignal Observe(uint32_t candidate_world,bool is_original_local_player) noexcept {
        if(!candidate_world||!is_original_local_player)
            return WorldTickSignal::None;
        uint32_t old=last_.exchange(candidate_world,std::memory_order_acq_rel);
        if(old==candidate_world)return WorldTickSignal::None;
        return old==0?WorldTickSignal::FirstLocalPlayerUpdate:
                      WorldTickSignal::WorldPointerChanged;
    }
};
} // namespace blackcoop
