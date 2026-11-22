// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Stage-plan correlation is source-only, not original guest scene launch.
#pragma once
#include "coop/room_original_guest_launch_match_contract.hpp"
namespace blackcoop{
inline OriginalGuestLaunchMatchGate& GuestOriginalLaunchMatch()noexcept{
    static OriginalGuestLaunchMatchGate gate;
    return gate;
}
} // namespace blackcoop
