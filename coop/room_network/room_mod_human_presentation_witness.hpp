// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "kernel/coop_stage_resource_contract.hpp"
#include "room_mod_human_presentation.hpp"
#include <jni.h>
#include <mutex>
#include <thread>
namespace blackcoop {
struct ModHumanOriginalScene {
  uint32_t episode = 0, world = 0, level = 0, stage = 0;
  uint64_t atMs = 0;
};
class ModHumanOriginalScenes {
  std::mutex lock_;
  std::thread::id owner_{};
  bool bound_ = false;
  ModHumanOriginalScene scene_{};

public:
  void Observe(uint32_t episode, uint32_t world, uint32_t level, uint32_t stage,
               bool stable, uint64_t now) noexcept {
    try {
      std::lock_guard<std::mutex> lock(lock_);
      if (bound_ && owner_ != std::this_thread::get_id())
        return;
      if (!stable || !episode || world < 0x10000 ||
          uint64_t(world) + 0x5cc0 > 0x100000000ull || level > 7 ||
          stage > 15 || !now) {
        scene_ = {};
        return;
      }
      if (!bound_) {
        owner_ = std::this_thread::get_id();
        bound_ = true;
      }
      scene_ = {episode, world, level, stage, now};
    } catch (...) {
    }
  }
  bool Read(uint64_t now, ModHumanOriginalScene &out) noexcept {
    try {
      std::lock_guard<std::mutex> lock(lock_);
      if (!bound_ || owner_ != std::this_thread::get_id() || !scene_.episode ||
          now < scene_.atMs || now - scene_.atMs > 350)
        return false;
      out = scene_;
      return true;
    } catch (...) {
      return false;
    }
  }
  bool Matches(const ModHumanOriginalScene &expected, uint64_t now) noexcept {
    ModHumanOriginalScene s;
    return Read(now, s) && s.episode == expected.episode &&
           s.world == expected.world && s.level == expected.level &&
           s.stage == expected.stage;
  }
};
inline ModHumanOriginalScenes &OriginalModHumanScenes() {
  static ModHumanOriginalScenes source;
  return source;
}
inline bool OriginalModHumanSceneCurrent(const ModHumanOriginalScene &scene,
                                         uint64_t now) noexcept {
  return g_originalStageFileOpen.ActiveEpisode() == scene.episode &&
         OriginalModHumanScenes().Matches(scene, now);
}
// Every call owns and releases its JNI local refs. No JNI data survives the
// synchronous value copy. Java independently checks the full committed ticket
// and original local episode; no menu/room membership substitutes for either.
inline bool
ReadOriginalModHumanPresentation(JNIEnv *env, jobject activity, uint32_t grid,
                                 uint64_t now, ModHumanOriginalScene &scene,
                                 ModHumanPresentation &packet) noexcept {
  ModHumanOriginalScene s;
  if (!env || !activity || !OriginalModHumanScenes().Read(now, s) ||
      !OriginalModHumanSceneCurrent(s, now) ||
      uint64_t(s.world) + 0x4960u != grid)
    return false;
  if (env->ExceptionCheck()) {
    env->ExceptionClear();
    return false;
  }
  jclass cls = env->GetObjectClass(activity);
  if (env->ExceptionCheck() || !cls) {
    if (env->ExceptionCheck())
      env->ExceptionClear();
    if (cls)
      env->DeleteLocalRef(cls);
    return false;
  }
  jmethodID method = env->GetMethodID(
      cls, "coopPollOtherHumanPresentationFromGameThread", "()[I");
  if (env->ExceptionCheck() || !method) {
    if (env->ExceptionCheck())
      env->ExceptionClear();
    env->DeleteLocalRef(cls);
    return false;
  }
  auto reply = static_cast<jintArray>(env->CallObjectMethod(activity, method));
  env->DeleteLocalRef(cls);
  if (env->ExceptionCheck() || !reply) {
    if (env->ExceptionCheck())
      env->ExceptionClear();
    if (reply)
      env->DeleteLocalRef(reply);
    return false;
  }
  const jsize n = env->GetArrayLength(reply);
  if (env->ExceptionCheck() || (n != 22 && n != 28 && n != 34)) {
    if (env->ExceptionCheck())
      env->ExceptionClear();
    env->DeleteLocalRef(reply);
    return false;
  }
  std::array<jint, 34> raw{};
  env->GetIntArrayRegion(reply, 0, n, raw.data());
  env->DeleteLocalRef(reply);
  if (env->ExceptionCheck()) {
    env->ExceptionClear();
    return false;
  }
  std::array<int32_t, 34> f{};
  for (jsize i = 0; i < n; i++)
    f[size_t(i)] = int32_t(raw[size_t(i)]);
  ModHumanPresentation p;
  if (!DecodeModHumanPresentation(f.data(), size_t(n), s.episode, s.level,
                                  s.stage, p) ||
      !OriginalModHumanSceneCurrent(s, now))
    return false;
  scene = s;
  packet = p;
  return true;
}
// Implemented in the ORIGINAL d3d8.cpp translation unit, where the pinned
// Android backend actually lives. This is a mod visual only, never P2 setup.
using ModHumanClock = uint64_t (*)() noexcept;
void DrawOriginalModHumans(const ModHumanOriginalScene &,
                           const ModHumanPresentation &, uint32_t grid,
                           ModHumanClock clock) noexcept;
} // namespace blackcoop
