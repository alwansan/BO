// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#include "room_original_objective_source.hpp"
#include <cassert>
#include <iostream>

struct ObjectiveMemory {
    static constexpr uint32_t world = 0x20000, levelData = 0x30000;
    static constexpr uint32_t descriptors = levelData + 0x3A0, rows = 0x31000;
    std::vector<uint8_t> bytes = std::vector<uint8_t>(0x300000);
    mutable unsigned worldReads = 0;
    bool replaceWorldOnReadback = false;

    bool Contains(uint32_t at, uint32_t size) const {
        return at >= 0x10000 && size && uint64_t(at) + size <= bytes.size();
    }
    uint8_t Read8(uint32_t at) const {
        assert(Contains(at, 1));
        return bytes[at];
    }
    uint32_t Read32(uint32_t at) const {
        if (at == 0x2D45D4 && ++worldReads == 2 && replaceWorldOnReadback) return 0;
        uint32_t value = 0;
        for (unsigned i = 0; i < 4; ++i) value |= uint32_t(Read8(at + i)) << (i * 8);
        return value;
    }
    void Write32(uint32_t at, uint32_t value) {
        assert(Contains(at, 4));
        for (unsigned i = 0; i < 4; ++i) bytes[at + i] = uint8_t(value >> (i * 8));
    }
};

int main() {
    ObjectiveMemory memory;
    constexpr auto world = ObjectiveMemory::world, data = ObjectiveMemory::levelData;
    constexpr auto descriptors = ObjectiveMemory::descriptors, rows = ObjectiveMemory::rows;
    memory.Write32(0x2D45D4, world);
    memory.Write32(world + 0x5AE0, 0x1C);
    memory.Write32(world + 0x5AE4, 0x1D);
    memory.Write32(world + 0x5AEC, 0x101);
    memory.Write32(world + 0x5B2C, data);
    memory.Write32(world + 0x930, rows);
    memory.Write32(world + 0x934, 2);
    memory.Write32(data, 17);
    memory.Write32(data + 4, 2);
    memory.Write32(data + 8, descriptors);
    memory.Write32(rows, descriptors);
    memory.Write32(rows + 4, 65535);
    memory.Write32(rows + 8, 1);
    memory.Write32(rows + 12, descriptors + 24);
    memory.Write32(rows + 20, 3);
    memory.Write32(descriptors, 0x04030201);
    memory.Write32(descriptors + 4, 0x88070605);
    memory.Write32(descriptors + 24, 0x14);

    std::vector<uint8_t> records;
    using blackcoop::CaptureOriginalObjectiveRecords;
    assert(CaptureOriginalObjectiveRecords(memory, world, 1, 1, records));
    const std::vector<uint8_t> expected{
        1,2,3,4,5,6,7,0x88,255,255,0,0,1,0,0,0,
        0x14,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,
    };
    assert(records == expected);
    auto local = records;
    local[12] = 0;
    blackcoop::OriginalObjectiveActivation plan{};
    using blackcoop::SelectOriginalObjectiveActivation;
    assert(SelectOriginalObjectiveActivation(local, records, plan));
    assert(plan.ordinal == 0 && plan.selectorLow == 0x04030201 &&
           plan.selectorHigh == 0x88070605);
    assert(!SelectOriginalObjectiveActivation(records, records, plan));
    assert(!SelectOriginalObjectiveActivation(records, local, plan));
    local[16] ^= 1;
    assert(!SelectOriginalObjectiveActivation(local, records, plan));
    local[16] ^= 1;
    local[8] ^= 1;
    assert(!SelectOriginalObjectiveActivation(local, records, plan));
    local[8] ^= 1;
    auto unsupported = records;
    unsupported[28] = 1;
    local[28] = 0;
    assert(!SelectOriginalObjectiveActivation(local, unsupported, plan));

    using blackcoop::SelectOriginalSimpleObjectiveCompletion;
    blackcoop::objective_source::Table table{};
    assert(blackcoop::objective_source::ReadTable(memory, world, 1, 1, table));
    auto completed = records;
    completed[12] = 3;
    assert(!SelectOriginalSimpleObjectiveCompletion(memory, table, records, completed, plan));
    memory.Write32(descriptors + 0x14, 1);
    assert(SelectOriginalSimpleObjectiveCompletion(memory, table, records, completed, plan));
    assert(plan.ordinal == 0 && plan.selectorLow == 0x04030201 && plan.selectorHigh == 0x88070605);
    assert(!SelectOriginalSimpleObjectiveCompletion(memory, table, local, completed, plan));
    completed[8] ^= 1;
    assert(!SelectOriginalSimpleObjectiveCompletion(memory, table, records, completed, plan));
    completed[8] ^= 1;
    completed[28] = 1;
    assert(!SelectOriginalSimpleObjectiveCompletion(memory, table, records, completed, plan));
    completed[28] = 3;
    memory.Write32(descriptors + 0x14, 0x101);
    assert(!SelectOriginalSimpleObjectiveCompletion(memory, table, records, completed, plan));
    memory.Write32(descriptors + 0x14, 2);
    assert(!SelectOriginalSimpleObjectiveCompletion(memory, table, records, completed, plan));

    assert(!CaptureOriginalObjectiveRecords(memory, world, 1, 0, records));
    memory.Write32(rows + 12, descriptors);
    assert(!CaptureOriginalObjectiveRecords(memory, world, 1, 1, records));
    memory.Write32(rows + 12, descriptors + 24);
    memory.Write32(descriptors + 24, 0x04030201);
    memory.Write32(descriptors + 28, 0x88070605);
    assert(!CaptureOriginalObjectiveRecords(memory, world, 1, 1, records));
    memory.Write32(descriptors + 24, 0x14);
    memory.Write32(descriptors + 28, 0);
    memory.worldReads = 0;
    memory.replaceWorldOnReadback = true;
    assert(!CaptureOriginalObjectiveRecords(memory, world, 1, 1, records));
    assert(records == expected);
    std::cout << "ORIGINAL_OBJECTIVE_SOURCE_PASS\n";
}
