// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once

#include "room_original_running_scene_source_gate.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include <vector>

namespace blackcoop {
namespace objective_source {

constexpr uint32_t kMaxRecords = 4096;
constexpr uint32_t kDescriptorSize = 0x18;
constexpr uint32_t kLiveRowSize = 0xC;
constexpr uint32_t kWireRecordSize = 16;

struct Table {
    uint32_t levelData, descriptors, rows, count;
    bool operator==(const Table&) const = default;
};

// 001EDC90 keeps LevelDat at world+0x5B2C. 000B70E0 relocates
// file+8 and 001C2420 retains each authored descriptor in a live row.
template<class Memory>
bool ReadTable(const Memory& memory, uint32_t world, uint8_t level, uint8_t stage,
               Table& out) {
    if ((world & 3u) || !memory.Contains(0x2D45D4u, 4) ||
        !memory.Contains(world, 0x5C00u) || memory.Read32(0x2D45D4u) != world ||
        !IsOriginalWorldRunning(memory.Read32(world + 0x5AE0u),
                                memory.Read32(world + 0x5AE4u)) ||
        memory.Read8(world + 0x5AECu) != level || memory.Read8(world + 0x5AEDu) != stage)
        return false;
    const uint32_t levelData = memory.Read32(world + 0x5B2Cu);
    const uint32_t rows = memory.Read32(world + 0x930u);
    const uint32_t count = memory.Read32(world + 0x934u);
    if ((levelData & 3u) || !memory.Contains(levelData, 0x3A0u) ||
        memory.Read32(levelData) != 17u || memory.Read32(levelData + 4u) != count ||
        count > kMaxRecords) return false;
    const uint32_t descriptors = memory.Read32(levelData + 8u);
    if (!count) {
        out = {levelData, descriptors, rows, 0};
        return true;
    }
    if ((descriptors & 3u) || (rows & 3u) ||
        uint64_t(descriptors) < uint64_t(levelData) + 0x3A0u ||
        !memory.Contains(descriptors, count * kDescriptorSize) ||
        !memory.Contains(rows, count * kLiveRowSize) ||
        (uint64_t(rows) + count * kLiveRowSize > levelData &&
         rows < uint64_t(descriptors) + count * kDescriptorSize)) return false;
    out = {levelData, descriptors, rows, count};
    return true;
}

inline void Write32(std::vector<uint8_t>& bytes, uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) {
        bytes.push_back(static_cast<uint8_t>(value));
        value >>= 8;
    }
}

}  // namespace objective_source

// The caller supplies an independently matched original level and stage.
// Return only RoomObjectiveChunks values: authored ID bytes, counter, state.
// No native pointer or allocation index crosses the room boundary.
template<class Memory>
bool CaptureOriginalObjectiveRecords(const Memory& memory, uint32_t world,
                                     uint8_t level, uint8_t stage,
                                     std::vector<uint8_t>& records) {
    using namespace objective_source;
    Table table{};
    if (!ReadTable(memory, world, level, stage, table)) return false;

    std::vector<uint8_t> snapshot;
    std::vector<uint64_t> ids;
    snapshot.reserve(table.count * kWireRecordSize);
    ids.reserve(table.count);
    for (uint32_t i = 0; i < table.count; ++i) {
        const uint32_t row = table.rows + i * kLiveRowSize;
        const uint32_t descriptor = table.descriptors + i * kDescriptorSize;
        if (memory.Read32(row) != descriptor) return false;
        const uint32_t low = memory.Read32(descriptor), high = memory.Read32(descriptor + 4u);
        const uint32_t counter = memory.Read32(row + 4u), state = memory.Read32(row + 8u);
        if (!(low || high) || counter > 65535u || state > 3u) return false;
        ids.push_back(uint64_t(high) << 32u | low);
        Write32(snapshot, low);
        Write32(snapshot, high);
        Write32(snapshot, counter);
        Write32(snapshot, state);
    }
    std::sort(ids.begin(), ids.end());
    if (std::adjacent_find(ids.begin(), ids.end()) != ids.end()) return false;

    Table current{};
    if (!ReadTable(memory, world, level, stage, current) || current != table) return false;
    records.swap(snapshot);
    return true;
}

struct OriginalObjectiveActivation {
    uint32_t ordinal = 0;
    uint32_t selectorLow = 0;
    uint32_t selectorHigh = 0;
};

// Match one state difference with unchanged authored identity and counter.
// Local comes from CaptureOriginalObjectiveRecords on the original thread.
// Remote is a complete current host snapshot, never an identity source.
namespace objective_source {
inline bool SelectOneStateTransition(std::span<const uint8_t> local,
                                     std::span<const uint8_t> remote,
                                     uint8_t from, uint8_t to,
                                     OriginalObjectiveActivation& out) noexcept {
    constexpr auto stride = objective_source::kWireRecordSize;
    if (local.size() != remote.size() || local.size() % stride ||
        local.size() / stride > objective_source::kMaxRecords) return false;
    OriginalObjectiveActivation selected{};
    bool found = false;
    for (size_t offset = 0; offset < local.size(); offset += stride) {
        const auto current = local.subspan(offset, stride);
        const auto host = remote.subspan(offset, stride);
        if (std::equal(current.begin(), current.end(), host.begin())) continue;
        if (found || current[12] != from || host[12] != to ||
            !std::equal(current.begin(), current.begin() + 12, host.begin()) ||
            !std::equal(current.begin() + 13, current.end(), host.begin() + 13)) return false;
        selected.ordinal = static_cast<uint32_t>(offset / stride);
        for (unsigned i = 0; i < 4; ++i) {
            selected.selectorLow |= uint32_t(current[i]) << (8 * i);
            selected.selectorHigh |= uint32_t(current[4 + i]) << (8 * i);
        }
        found = true;
    }
    if (!found) return false;
    out = selected;
    return true;
}
}  // namespace objective_source

inline bool SelectOriginalObjectiveActivation(std::span<const uint8_t> local,
                                              std::span<const uint8_t> remote,
                                              OriginalObjectiveActivation& out) noexcept {
    return objective_source::SelectOneStateTransition(local, remote, 0, 1, out);
}

template<class Memory>
bool SelectOriginalSimpleObjectiveCompletion(const Memory& memory, const objective_source::Table& table,
                                             std::span<const uint8_t> local,
                                             std::span<const uint8_t> remote,
                                             OriginalObjectiveActivation& out) noexcept {
    OriginalObjectiveActivation selected{};
    if (local.size() != uint64_t(table.count) * objective_source::kWireRecordSize ||
        !objective_source::SelectOneStateTransition(local, remote, 1, 3, selected)) return false;
    const uint32_t descriptor = table.descriptors + selected.ordinal * objective_source::kDescriptorSize;
    // 001BA430 completes threshold 1 without incrementing the row counter.
    // Other flags can request statistics and original scene/menu transitions.
    if (!memory.Contains(descriptor, objective_source::kDescriptorSize) ||
        memory.Read32(descriptor) != selected.selectorLow ||
        memory.Read32(descriptor + 4u) != selected.selectorHigh ||
        memory.Read8(descriptor + 0x14u) != 1u ||
        memory.Read8(descriptor + 0x15u) != 0u) return false;
    out = selected;
    return true;
}

}  // namespace blackcoop
