// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "gpu/backend.h"
#include "room_mod_placeholder_draw.hpp"
namespace blackcoop {
struct ModPlaceholderLease {
  uint32_t header = 0, data = 0;
};
enum class ModPlaceholderSubmitResult { Rejected, Completed, Quarantined };
// IO owns both allocations. Free/Quarantine must be noexcept. Scope must
// independently validate the live ticket, scene, frame, target and camera;
// this routine does NOT establish that the prepared matrices are BLACK's.
// Deliberately synchronous initial implementation: never reuse vertex bytes
// while a queued render-thread draw can still read them. No original State
// changes and no original actor, NPC, resource or game-memory access.
template <class IO, class Scope>
ModPlaceholderSubmitResult
SubmitModPlaceholder(gpu::Backend &backend, IO &io,
                     const ModPlaceholderDraw &packet, Scope scope) {
  ModPlaceholderLease lease{};
  bool allocated = false, mayBeQueued = false;
  try {
    if (packet.state.vs_handle != 0x42u || packet.state.vs || packet.state.ps)
      return ModPlaceholderSubmitResult::Rejected;
    for (const auto &stream : packet.state.streams)
      if (stream.vb || stream.stride)
        return ModPlaceholderSubmitResult::Rejected;
    for (auto texture : packet.state.textures)
      if (texture)
        return ModPlaceholderSubmitResult::Rejected;
    if (!io.BackendIs(backend) || !scope())
      return ModPlaceholderSubmitResult::Rejected;
    if (!io.Allocate(lease))
      return ModPlaceholderSubmitResult::Rejected;
    allocated = true;
    if (!io.Upload(lease, packet.vertices.data(), sizeof(packet.vertices)) ||
        !scope()) {
      io.Free(lease);
      return ModPlaceholderSubmitResult::Rejected;
    }
    gpu::State detached = packet.state;
    detached.streams[0] = {lease.header, sizeof(ModPlaceholderVertex)};
    mayBeQueued =
        true; // even a throwing backend may already have recorded work
    backend.Draw(detached, 5, 0, 36, nullptr); // pinned SDK triangle list
    const uint32_t fence = backend.Fence();
    if (!fence) {
      io.Quarantine(lease);
      return ModPlaceholderSubmitResult::Quarantined;
    }
    backend.WaitFence(fence);
    if (backend.FencePending(fence)) {
      io.Quarantine(lease);
      return ModPlaceholderSubmitResult::Quarantined;
    }
    io.Free(lease);
    return ModPlaceholderSubmitResult::Completed;
  } catch (...) {
    if (allocated) {
      if (mayBeQueued)
        io.Quarantine(lease);
      else
        io.Free(lease);
    }
    return mayBeQueued ? ModPlaceholderSubmitResult::Quarantined
                       : ModPlaceholderSubmitResult::Rejected;
  }
}
} // namespace blackcoop
