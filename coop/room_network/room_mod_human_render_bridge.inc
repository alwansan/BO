// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Appended to pinned Android d3d8.cpp. Original g_backend has process lifetime:
// EnsureBackend constructs once, no reset/recreation in this pinned source.
#include "coop/room_mod_human_presentation_witness.hpp"
#include "coop/room_mod_placeholder_upload_kernel.hpp"
#include "coop/room_original_world_camera_witness.hpp"
void blackcoop::DrawOriginalModHumans(const ModHumanOriginalScene &scene,
                                      const ModHumanPresentation &packet,
                                      uint32_t grid,
                                      ModHumanClock clock, JNIEnv *env,
                                      jobject activity) noexcept {
  try {
    if (!clock || !OriginalModHumanSceneCurrent(scene, clock()) ||
        packet.originalEpisode != scene.episode ||
        packet.level != scene.level || packet.stage != scene.stage ||
        uint64_t(scene.world) + 0x4960u != grid)
      return;
    std::lock_guard<std::recursive_mutex> lock(d3d::Lock());
    if (!d3d::g_backend)
      return;
    OriginalWorldCameraFrame camera;
    if (!ReadCurrentOriginalWorldCameraFrame(camera) ||
        camera.source.world != scene.world)
      return;
    const auto stageMatches = [&] {
      return kernel::G<uint8_t>(scene.world + 0x5AECu) == scene.level &&
             kernel::G<uint8_t>(scene.world + 0x5AEDu) == scene.stage;
    };
    if (!stageMatches())
      return;
    // One batch per actual original camera begin, including repeated grid
    // callbacks. Do not retry a possibly queued failed batch in this frame.
    static uint64_t attemptedSerial = 0;
    if (camera.serial == attemptedSerial)
      return;
    attemptedSerial = camera.serial;
    static ModPlaceholderUploadKernel upload(*d3d::g_backend);
    if (!upload.BackendIs(*d3d::g_backend))
      return;
    const auto scope = [&] {
      OriginalWorldCameraFrame current;
      ModHumanOriginalScene liveScene;
      ModHumanPresentation livePacket;
      // Room membership and COMMIT can be revoked by a network worker after
      // the initial JNI copy, including while Allocate or WaitFence runs.
      // Re-read the authoritative Java facade before every queued draw.
      return ReadOriginalModHumanPresentation(env, activity, grid, clock(),
                                              liveScene, livePacket) &&
             SameModHumanTicket(packet, livePacket) &&
             livePacket.tick >= packet.tick &&
             OriginalModHumanSceneCurrent(scene, clock()) &&
             ReadCurrentOriginalWorldCameraFrame(current) &&
             current.serial == camera.serial &&
             current.source.world == scene.world && stageMatches();
    };
    (void)SubmitModHumanPresentation(*d3d::g_backend, upload, d3d::State(),
                                     camera, packet, scope);
    // Completed leases are freed by the real fence path. Quarantine remains
    // bounded inside upload; never free uncertain queued data on room leave.
  } catch (...) {
  } // no exception escapes into original guest execution
}
