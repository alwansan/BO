// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original stage streaming candidate native hook source-only observer.
// Original XBE private read-only CI44979193377 verified BYTE register
// loads from +0x2020C and +0x2020E in sub_001EDC90 window.
// The source-linked PC labels are LEVEL and STAGE but not necessarily
// host mission selection or game world ready. NEVER write guest memory.
#pragma once
#include "coop/room_original_streaming_stage_contract.hpp"
#include "kernel/coop_stage_resource_contract.hpp"
#include "game/layout.h"
#include "xdk/hle.h"
#include <cstdint>
namespace blackcoop {
inline void ObserveOriginalStreamingStageEntry() noexcept {
    const uint32_t episode=g_originalStageFileOpen.ActiveEpisode();
    if(!episode)return; // no original frontend launch episode
    const uint32_t game=kernel::R32(layout::kGame);
    if(!game)return;
    // Pinned PC source references original function 0x001EDC90 and these
    // exact byte-width offsets. Source XBE confirms the direct reads.
    // Read only; subsequent source resource-pair+player gates are separate.
    const uint32_t level=kernel::G<uint8_t>(game+0x2020C);
    const uint32_t stage=kernel::G<uint8_t>(game+0x2020E);
    g_originalStreamingStageHints.CaptureAtStreamingEntry(episode,level,stage);
}
} // namespace blackcoop
