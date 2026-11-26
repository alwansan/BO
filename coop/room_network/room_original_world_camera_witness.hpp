// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "coop/room_original_world_camera_frame.hpp"
#include "room_original_running_scene_source_gate.hpp"
#include "kernel/kernel.h"
#include "kernel/memory.h"
#include "xdk/d3d8.h"
#include <atomic>
#include <mutex>
namespace blackcoop {
inline bool OriginalCameraSpan(uint32_t p, uint32_t n) noexcept {
  if (p < 0x10000 || !n || uint64_t(p) + n > 0x100000000ull)
    return false;
  const uint64_t last = (uint64_t(p) + n - 1) & ~uint64_t(0xfff);
  for (uint64_t page = uint64_t(p) & ~uint64_t(0xfff); page <= last;
       page += 0x1000)
    if (!mem::IsCommitted(static_cast<uint32_t>(page)))
      return false;
  return true;
}
inline OriginalWorldCameraFrames &OriginalWorldCameraSource() {
  static OriginalWorldCameraFrames source;
  return source;
}
inline std::atomic<bool> &OriginalWorldCameraAvailable() {
  static std::atomic<bool> flag{false};
  return flag;
}
// Exact c5 source callback ABI: camera begin 2B830 is cdecl(0,rw,0),
// rw is TRUE entry ESP+8, boolean EAX=1; captures only after the original.
// Never changes context, SDK State, guest memory, renderer, AI or assets.
inline void ObserveOriginalWorldCameraAfterBegin(uint32_t rw,
                                                 uint32_t result) noexcept {
  OriginalWorldCameraAvailable().store(false);
  try {
    std::lock_guard<std::recursive_mutex> lock(d3d::Lock());
    // A rejected callback may have waited behind an owner publication.
    // Revoke again IN lock order, not only before waiting for the lock.
    OriginalWorldCameraAvailable().store(false);
    auto &source = OriginalWorldCameraSource();
    if (result != 1 || !OriginalCameraSpan(0x2D45D4, 4) ||
        !OriginalCameraSpan(0x2D67F4, 4)) {
      source.Invalidate();
      return;
    }
    const uint32_t world = kernel::R32(0x2D45D4),
                   renderer = kernel::R32(0x2D67F4);
    if (!OriginalCameraSpan(world, 0x5CC0) ||
        !IsOriginalWorldRunning(kernel::R32(world + 0x5AE0),
                                kernel::R32(world + 0x5AE4)) ||
        !OriginalCameraSpan(renderer, 0xD4F0) || !rw ||
        kernel::R32(renderer + 0xD4A8) != rw) {
      source.Invalidate();
      return;
    }
    OriginalWorldCameraAvailable().store(
        source.Observe({world, renderer, rw}, d3d::State()));
  } catch (...) {
    // Failed lock acquisition cannot safely mutate the shared snapshot.
    // No exception escapes into original guest execution.
  }
}
// Numerical source witness only. Caller must ALSO prove signed story scope
// and current original scene/frame at use; this is not co-op readiness.
inline bool
ReadCurrentOriginalWorldCameraFrame(OriginalWorldCameraFrame &out) noexcept {
  if (!OriginalWorldCameraAvailable().load())
    return false;
  try {
    std::lock_guard<std::recursive_mutex> lock(d3d::Lock());
    if (!OriginalWorldCameraAvailable().load() ||
        !OriginalCameraSpan(0x2D45D4, 4) || !OriginalCameraSpan(0x2D67F4, 4))
      return false;
    const auto world = kernel::R32(0x2D45D4), renderer = kernel::R32(0x2D67F4);
    if (!OriginalCameraSpan(world, 0x5CC0) ||
        !IsOriginalWorldRunning(kernel::R32(world + 0x5AE0),
                                kernel::R32(world + 0x5AE4))) {
      OriginalWorldCameraAvailable().store(false);
      OriginalWorldCameraSource().Invalidate();
      return false;
    }
    if (!OriginalCameraSpan(renderer, 0xD594) ||
        kernel::R32(renderer + 0xD590) != renderer + 0xD450)
      return false;
    return OriginalWorldCameraSource().Read(
        {world, renderer, kernel::R32(renderer + 0xD4A8)}, d3d::State(), out);
  } catch (...) {
    return false;
  }
}
} // namespace blackcoop
