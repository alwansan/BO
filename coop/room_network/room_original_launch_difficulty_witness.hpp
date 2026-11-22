// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// ORIGINAL BLACK launch call wrapper sub_00124CF0, which the pinned source
// documents as "mission starts on chosen difficulty, nothing if closed."
// Observe the source-confirmed difficulty BEFORE that call; ZERO original
// game writes. Never auto prepare the story, READY, COMMIT, or call a loader.
#pragma once
#include "coop/room_original_launch_difficulty_contract.hpp"
#include "coop/room_world_tick.hpp"
#include "kernel/coop_stage_resource_contract.hpp"
#include "coop/room_original_mission_field_hint.hpp"
#include "game/layout.h"
#include "xdk/hle.h"
#include <cstdint>
namespace blackcoop {
inline void ObserveOriginalLaunchDifficulty() noexcept{
    // Source proof: pinned Black-Android cheats.cpp kDifficulty=0x2014C,
    // layout.h kGame=0x2D45F0. This field is not mission/stage selection.
    // New original launch attempt invalidates all previous observed files.
    // Its epoch is diagnostic only; it NEVER creates a story ticket.
    const uint32_t epoch=g_originalStageFileOpen.BeginOriginalLaunchAttempt();
    // Private original XBE WIDTH source evidence CI44977857722:
    // 0x2D0D60 and 0x2D0D68 accessed as WORD, not DWORD! A 32-bit
    // read included unrelated neighboring memory. The menu 0x2EC598
    // is another WORD-write candidate; no field is a proven mission ID.
    // All reads strictly source-sized and NEVER written to original game.
    g_originalMissionFieldHints.CaptureAtOriginalLaunch(
        epoch,kernel::R16(0x2D0D60),kernel::R16(0x2D0D68),
        kernel::R16(0x2EC598));
    constexpr uint32_t kDifficulty=0x2014C;
    uint32_t game=kernel::R32(layout::kGame);
    uint32_t value=game?kernel::R32(game+kDifficulty):0xffffffffu;
    static OriginalLaunchDifficultyOnce events;
    const int code=events.Observe(game!=0,value);
    if(code)SendSafeWorldCodeToAndroid(static_cast<jint>(code));
}
} // namespace blackcoop
