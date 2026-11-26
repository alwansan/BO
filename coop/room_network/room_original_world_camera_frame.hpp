// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "gpu/state.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <thread>
namespace blackcoop {
struct OriginalCameraIdentity {
  uint32_t world = 0, renderer = 0, rwCamera = 0;
};
struct OriginalWorldCameraFrame {
  OriginalCameraIdentity source{};
  uint64_t serial = 0;
  std::array<float, 16> view{}, projection{};
  gpu::Surface color{}, depth{};
  gpu::Viewport viewport{};
  std::array<uint32_t, 32> scissors{};
  uint32_t scissorCount = 0;
  float zscale = 0;
};
// Source-only capture AFTER ORIGINAL RW device camera begin has set BOTH
// actual SDK matrices. This is not room authorization, an actor, a render
// command or allocation-generation proof. Never read at arbitrary frame-end.
class OriginalWorldCameraFrames {
  std::thread::id owner_{};
  bool bound_ = false;
  OriginalWorldCameraFrame frame_{};
  uint64_t serial_ = 0;
  bool valid_ = false;
  bool owner() const noexcept {
    return !bound_ || owner_ == std::this_thread::get_id();
  }
  static uint32_t bits(float v) noexcept {
    uint32_t n;
    std::memcpy(&n, &v, 4);
    return n;
  }
  static bool finite(float v) noexcept {
    return (bits(v) & 0x7f800000u) != 0x7f800000u;
  }
  static bool matrix(const std::array<float, 16> &a) noexcept {
    bool nonzero = false;
    for (float v : a) {
      if (!finite(v))
        return false;
      nonzero |= (bits(v) & 0x7fffffffu) != 0;
    }
    return nonzero;
  }
  static bool same(const gpu::Surface &a, const gpu::Surface &b) noexcept {
    return a.header == b.header && a.data == b.data && a.format == b.format &&
           a.size == b.size;
  }

public:
  void Invalidate() noexcept {
    if (owner()) {
      valid_ = false;
      frame_ = {};
    }
  }
  bool Observe(OriginalCameraIdentity id, const gpu::State &current) noexcept {
    if (!owner())
      return false;
    Invalidate();
    if (!id.world || !id.renderer || !id.rwCamera || !current.rt.header ||
        !current.zs.header || !current.viewport.w || !current.viewport.h ||
        current.scissor_count > 8 || !finite(current.viewport.min_z) ||
        !finite(current.viewport.max_z) || !finite(current.zscale) ||
        !matrix(current.transforms[2]) || !matrix(current.transforms[3]))
      return false;
    if (!bound_) {
      owner_ = std::this_thread::get_id();
      bound_ = true;
    }
    if (++serial_ == 0)
      return false;
    frame_.source = id;
    frame_.serial = serial_;
    frame_.view = current.transforms[2];
    frame_.projection = current.transforms[3];
    frame_.color = current.rt;
    frame_.depth = current.zs;
    frame_.viewport = current.viewport;
    frame_.scissors = current.scissors;
    frame_.scissorCount = current.scissor_count;
    frame_.zscale = current.zscale;
    valid_ = true;
    return true;
  }
  bool Read(OriginalCameraIdentity id, const gpu::State &current,
            OriginalWorldCameraFrame &out) const noexcept {
    if (!bound_ || !owner() || !valid_ || id.world != frame_.source.world ||
        id.renderer != frame_.source.renderer ||
        id.rwCamera != frame_.source.rwCamera ||
        !same(current.rt, frame_.color) || !same(current.zs, frame_.depth) ||
        current.viewport.x != frame_.viewport.x ||
        current.viewport.y != frame_.viewport.y ||
        current.viewport.w != frame_.viewport.w ||
        current.viewport.h != frame_.viewport.h ||
        bits(current.viewport.min_z) != bits(frame_.viewport.min_z) ||
        bits(current.viewport.max_z) != bits(frame_.viewport.max_z) ||
        bits(current.zscale) != bits(frame_.zscale) ||
        current.scissor_count != frame_.scissorCount ||
        std::memcmp(current.scissors.data(), frame_.scissors.data(),
                    frame_.scissorCount * 4u * sizeof(uint32_t)) ||
        std::memcmp(current.transforms[2].data(), frame_.view.data(), 64) ||
        std::memcmp(current.transforms[3].data(), frame_.projection.data(), 64))
      return false;
    out = frame_;
    return true;
  }
};
} // namespace blackcoop
