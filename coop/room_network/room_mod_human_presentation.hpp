// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "room_mod_placeholder_submit.hpp"
#include "room_original_world_camera_frame.hpp"
namespace blackcoop {
struct ModHumanPresentation {
  uint32_t originalEpisode = 0, level = 0, stage = 0, epoch = 0, revision = 0;
  uint32_t tokenHi = 0, tokenLo = 0, participants = 0, self = 0, tick = 0,
           count = 0;
  std::array<uint32_t, 4> session{};
  struct Body {
    uint32_t peer = 0;
    int32_t x = 0, y = 0, z = 0, yaw = 0, flags = 0;
  };
  std::array<Body, 3> bodies{};
};
// Source is the synchronous RoomSession JNI facade, not a UDP packet. The
// independent ORIGINAL scene episode and loaded level/stage MUST be supplied
// by the original eye/scene witness. Full room identity stays with the values.
inline bool DecodeModHumanPresentation(const int32_t *f, size_t n,
                                       uint32_t episode, uint32_t level,
                                       uint32_t stage,
                                       ModHumanPresentation &out) noexcept {
  if (!f || !episode || n < 22 || n > 34 || f[0] != 1 || f[1] <= 0 ||
      uint32_t(f[1]) != episode || f[2] < 0 || f[2] > 7 ||
      uint32_t(f[2]) != level || f[3] < 0 || f[3] > 15 ||
      uint32_t(f[3]) != stage || f[4] <= 0 || !uint32_t(f[5]) || f[6] < 0 ||
      (!f[6] && !f[7]) || f[9] < 1 || f[9] > 4 || f[10] <= 0 || f[11] < 1 ||
      f[11] > 3 || n != 16u + 6u * uint32_t(f[11]))
    return false;
  const uint32_t total = uint32_t(f[11]) + 1;
  if (uint32_t(f[8]) != (1u << total) - 1u || uint32_t(f[9]) > total)
    return false;
  ModHumanPresentation p;
  p.originalEpisode = episode;
  p.level = level;
  p.stage = stage;
  p.epoch = uint32_t(f[4]);
  p.revision = uint32_t(f[5]);
  p.tokenHi = uint32_t(f[6]);
  p.tokenLo = uint32_t(f[7]);
  p.participants = uint32_t(f[8]);
  p.self = uint32_t(f[9]);
  p.tick = uint32_t(f[10]);
  p.count = uint32_t(f[11]);
  for (size_t i = 0; i < 4; i++)
    p.session[i] = uint32_t(f[12 + i]);
  uint32_t mask = 1u << (p.self - 1), previous = 0;
  for (uint32_t i = 0; i < p.count; i++) {
    const size_t j = 16u + 6u * i;
    const int32_t id = f[j];
    if (id <= 0 || uint32_t(id) <= previous || uint32_t(id) > total ||
        uint32_t(id) == p.self || f[j + 1] < -20000000 || f[j + 1] > 20000000 ||
        f[j + 2] < -20000000 || f[j + 2] > 20000000 || f[j + 3] < -20000000 ||
        f[j + 3] > 20000000 || f[j + 4] < -18000 || f[j + 4] > 18000 ||
        f[j + 5] < 0 || f[j + 5] > 3)
      return false;
    p.bodies[i] = {uint32_t(id), f[j + 1], f[j + 2],
                   f[j + 3],     f[j + 4], f[j + 5]};
    previous = uint32_t(id);
    mask |= 1u << (previous - 1);
  }
  if (mask != p.participants)
    return false;
  out = p;
  return true;
}
inline bool SameModHumanTicket(const ModHumanPresentation &a,
                               const ModHumanPresentation &b) noexcept {
  return a.originalEpisode == b.originalEpisode && a.level == b.level &&
         a.stage == b.stage && a.epoch == b.epoch && a.revision == b.revision &&
         a.tokenHi == b.tokenHi && a.tokenLo == b.tokenLo &&
         a.participants == b.participants && a.self == b.self &&
         a.session == b.session;
}
// Merely a mod-owned visual marker. It has no original actor, collision,
// weapons, model resources, AI registration or gameplay readiness semantics.
template <class IO, class Scope>
uint32_t SubmitModHumanPresentation(gpu::Backend &backend, IO &io,
                                    const gpu::State &target,
                                    const OriginalWorldCameraFrame &camera,
                                    const ModHumanPresentation &p,
                                    Scope scope) {
  if (p.count < 1 || p.count > 3)
    return 0;
  constexpr std::array<uint32_t, 4> colors{0xffffc040u, 0xff40c0ffu,
                                           0xff70e080u, 0xffd080ffu};
  uint32_t completed = 0;
  for (uint32_t i = 0; i < p.count; i++) {
    const auto &b = p.bodies[i];
    if (b.peer < 1 || b.peer > 4 || b.peer == p.self)
      break;
    ModPlaceholderDraw draw;
    if (!PrepareModPlaceholderDraw(target, camera.view, camera.projection,
                                   {float(b.x) / 100.f, float(b.y) / 100.f,
                                    float(b.z) / 100.f, .45f, 1.7f, .45f,
                                    colors[b.peer - 1]},
                                   draw))
      break;
    if (SubmitModPlaceholder(backend, io, draw, scope) !=
        ModPlaceholderSubmitResult::Completed)
      break;
    ++completed;
  }
  return completed;
}
} // namespace blackcoop
