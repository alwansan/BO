// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// ONE authenticated game-thread input inbox for original player2/3/4.
// Sharing this inline symbol across original game-thread hooks and native
// input::Poll prevents accidental second per-translation-unit queues.
#pragma once
#include "room_original_trusted_input_contract.hpp"
namespace blackcoop {
inline OriginalHostHumanInbox& OriginalHostHumanInputs()noexcept {
    static OriginalHostHumanInbox inbox;
    return inbox;
}
} // namespace blackcoop
