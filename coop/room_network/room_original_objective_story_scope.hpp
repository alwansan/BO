// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Detached objective values scoped by the exact RoomStoryWire.Ticket fields.
// This is not a native scene/lifetime proof, authenticated sender proof,
// network message, checkpoint loader or live objective application adapter.
#pragma once
#include "room_original_objective_snapshot.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>

namespace blackcoop {
struct OriginalObjectiveStoryScope {
 std::array<uint8_t,16> session{};
 int64_t token=0,rosterRevision=0;
 int32_t mission=0,stage=0,sceneEpoch=0,participants=0;
 bool operator==(const OriginalObjectiveStoryScope&)const=default;
};

// Mirror RoomStoryWire.Ticket's constructor, including its full unsigned
// 32-bit revision and unsigned 16-bit stage ranges. The fixed-size session
// is allowed to contain zero bytes; this function does not authenticate it.
inline bool ValidOriginalObjectiveStoryScope(const OriginalObjectiveStoryScope& scope)noexcept{
 return scope.token>0&&scope.rosterRevision>0&&
        scope.rosterRevision<=0xffffffffLL&&
        scope.mission>=0&&scope.mission<=7&&
        scope.stage>=0&&scope.stage<=65535&&scope.sceneEpoch>0&&
        scope.participants>=3&&scope.participants<=15&&
        (scope.participants&1)!=0;
}

struct OriginalObjectiveStorySnapshot {
 OriginalObjectiveStoryScope scope{};
 std::vector<uint8_t> records;
 bool operator==(const OriginalObjectiveStorySnapshot&)const=default;
};
struct PreparedOriginalObjectiveStory {
 OriginalObjectiveStoryScope scope{};
 std::vector<OriginalObjectiveValue> values;
 bool operator==(const PreparedOriginalObjectiveStory&)const=default;
};

// Capture caller-owned values, never game memory. All validation/allocation
// completes before publishing the owned records and their scope together.
// Validation failure or allocation exceptions leave output unchanged.
inline bool CaptureOriginalObjectiveStorySnapshot(
       const OriginalObjectiveStoryScope& scope,
       std::span<const OriginalObjectiveValue> values,
       OriginalObjectiveStorySnapshot& output){
 if(!ValidOriginalObjectiveStoryScope(scope))return false;
 OriginalObjectiveStorySnapshot candidate;
 candidate.scope=scope;
 if(!EncodeOriginalObjectiveValues(values,candidate.records))return false;
 std::swap(output,candidate);return true;
}

// currentScope and expectedIds must come from independent, verified current
// sources, not from the received snapshot. Callers supply stable copies.
// A consumer must compare the retained scope to its current ticket again
// before any later application. These value functions do not authorize it.
// There is deliberately no clock/sequence: an older snapshot within the SAME
// ticket is still accepted. No freshness or latest-snapshot guarantee exists.
// Publishing is transactional for this caller, not cross-thread atomicity.
inline bool PrepareOriginalObjectiveStorySnapshot(
       const OriginalObjectiveStorySnapshot& snapshot,
       const OriginalObjectiveStoryScope& currentScope,
       std::span<const OriginalObjectiveDescriptorId> expectedIds,
       PreparedOriginalObjectiveStory& output){
 if(!ValidOriginalObjectiveStoryScope(snapshot.scope)||
    !ValidOriginalObjectiveStoryScope(currentScope)||
    !(snapshot.scope==currentScope))return false;
 PreparedOriginalObjectiveStory candidate;
 candidate.scope=currentScope;
 if(!PrepareOriginalObjectiveValues(snapshot.records,expectedIds,candidate.values))return false;
 std::swap(output,candidate);return true;
}
} // namespace blackcoop
