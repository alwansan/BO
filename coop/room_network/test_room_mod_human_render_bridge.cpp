// Production draw bridge + actual SDK ABI, synthetic memory/JNI/deferred
// backend.
#include "room_mod_human_presentation_witness.hpp"
#include "room_original_world_camera_witness.hpp"
#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <vector>
static std::vector<uint8_t> memory(0x300000);
static unsigned allocations = 0, frees = 0, draws = 0;
static bool pending = false;
static void (*afterAllocation)() = nullptr;
static unsigned clockCalls = 0, expireAfter = 0;
static uint64_t TestClock() noexcept {
  ++clockCalls;
  return expireAfter && clockCalls >= expireAfter ? 451 : 101;
}
namespace kernel {
uint8_t *g_base = memory.data();
}
namespace mem {
bool IsCommitted(uint32_t p) {
  return p >= 0x10000 && uint64_t(p) + 4096 <= memory.size();
}
uint32_t AllocContiguous(uint32_t n, uint32_t, uint32_t, uint32_t) {
  assert(n == 576 && !pending);
  ++allocations;
  if (afterAllocation) afterAllocation();
  return 0x100000;
}
uint32_t AllocSystem(uint32_t n, uint32_t) {
  assert(n == 20);
  return 0x110000;
}
void FreeContiguous(uint32_t p) {
  assert(p == 0x100000 && !pending);
  ++frees;
}
void FreeSystem(uint32_t p) { assert(p == 0x110000 && !pending); }
uint32_t PhysicalFromVirtual(uint32_t p) { return p; }
} // namespace mem
struct DeferredBackend : gpu::Backend {
  gpu::State queued{};
  bool Init(uint32_t, uint32_t) override { return true; }
  void Clear(const gpu::State &, uint32_t, const uint8_t *, uint32_t, uint32_t,
             float, uint32_t) override {}
  void Copy(const gpu::Surface &, const gpu::Surface &, uint32_t, uint32_t,
            uint32_t) override {}
  void Present(const gpu::Surface &) override {}
  void Sync(uint32_t, uint32_t) override {}
  void SyncLate(uint32_t) override {}
  void Flush() override {}
  void SetGamma(const uint8_t *) override {}
  void Draw(const gpu::State &s, uint32_t p, uint32_t start, uint32_t n,
            const uint8_t *ix) override {
    assert(!pending && p == 5 && start == 0 && n == 36 && !ix);
    assert(s.vs_handle == 0x42 && s.streams[0].stride == 16);
    queued = s;
    pending = true;
    ++draws;
  }
  uint32_t Fence() override {
    assert(pending);
    return draws;
  }
  void WaitFence(uint32_t f) override {
    assert(f == draws && pending);
    const auto data = kernel::R32(queued.streams[0].vb + 4);
    blackcoop::ModPlaceholderVertex v{};
    std::memcpy(&v, kernel::HostPtr(data), sizeof(v));
    assert(v.argb != 0 && v.x > -200000 && v.x < 200000);
    pending = false;
  }
  bool FencePending(uint32_t) override { return pending; }
};
namespace d3d {
static gpu::State state;
static std::recursive_mutex mutex;
std::unique_ptr<gpu::Backend> g_backend = std::make_unique<DeferredBackend>();
gpu::State &State() { return state; }
std::recursive_mutex &Lock() { return mutex; }
} // namespace d3d
#include "room_mod_human_render_bridge.inc"
static std::array<jint, 34> values{
    1,   1,   2,   0,    1, 1, 0,   123, 15,  1, 7, 3, 0,   1,   2,   3, 2,
    100, 200, 300, 4500, 0, 3, 400, 500, 600, 0, 0, 4, 700, 800, 900, 0, 0};
static jsize length = 34;
static bool exception = false, missingMethod = false;
static unsigned deleted = 0, regions = 0;
int main() {
  using namespace blackcoop;
  constexpr uint32_t world = 0x10000, renderer = 0x30000, rw = 0x80000;
  auto &source = OriginalModHumanScenes();
  const auto episode = g_originalStageFileOpen.BeginOriginalLaunchAttempt();
  assert(episode == 1);
  source.Observe(episode, world, 2, 0, true, 100);
  using Table =
      std::remove_cv_t<std::remove_pointer_t<decltype(JNIEnv{}.functions)>>;
  Table table{};
  table.GetObjectClass = [](JNIEnv *, jobject) -> jclass {
    return reinterpret_cast<jclass>(1);
  };
  table.GetMethodID = [](JNIEnv *, jclass, const char *name,
                         const char *signature) -> jmethodID {
    assert(std::strcmp(name, "coopPollOtherHumanPresentationFromGameThread") ==
               0 &&
           std::strcmp(signature, "()[I") == 0);
    return missingMethod ? nullptr : reinterpret_cast<jmethodID>(2);
  };
  table.CallObjectMethodV = [](JNIEnv *, jobject, jmethodID,
                               va_list) -> jobject {
    return reinterpret_cast<jobject>(3);
  };
  table.ExceptionCheck = [](JNIEnv *) -> jboolean { return exception; };
  table.ExceptionClear = [](JNIEnv *) { exception = false; };
  table.DeleteLocalRef = [](JNIEnv *, jobject) { ++deleted; };
  table.GetArrayLength = [](JNIEnv *, jarray) -> jsize { return length; };
  table.GetIntArrayRegion = [](JNIEnv *, jintArray, jsize first, jsize n,
                               jint *out) {
    assert(first == 0 && n == 34);
    ++regions;
    std::copy_n(values.begin(), n, out);
  };
  JNIEnv env{};
  env.functions = &table;
  auto activity = reinterpret_cast<jobject>(4);
  ModHumanOriginalScene scene;
  ModHumanPresentation packet;
  assert(ReadOriginalModHumanPresentation(&env, activity, world + 0x4960, 101,
                                          scene, packet));
  assert(deleted == 2 && regions == 1 && packet.count == 3);
  length = 16;
  assert(!ReadOriginalModHumanPresentation(&env, activity, world + 0x4960, 101,
                                           scene, packet));
  assert(deleted == 4 && regions == 1);
  length = 34;
  missingMethod = true;
  assert(!ReadOriginalModHumanPresentation(&env, activity, world + 0x4960, 101,
                                           scene, packet));
  assert(deleted == 5);
  missingMethod = false;
  exception = true;
  assert(!ReadOriginalModHumanPresentation(&env, activity, world + 0x4960, 101,
                                           scene, packet));
  assert(!exception && deleted == 5);
  assert(!ReadOriginalModHumanPresentation(&env, activity, world + 0x4960, 451,
                                           scene, packet));
  assert(!ReadOriginalModHumanPresentation(&env, activity, world, 101, scene,
                                           packet));
  kernel::W32(0x2D45D4, world);
  kernel::W32(0x2D67F4, renderer);
  kernel::W32(world + 0x5AE0, 0x1C);
  kernel::W32(world + 0x5AE4, 0x1D);
  kernel::G<uint8_t>(world + 0x5AEC) = 2;
  kernel::W32(renderer + 0xD4A8, rw);
  kernel::W32(renderer + 0xD590, renderer + 0xD450);
  auto &s = d3d::State();
  s.rt = {1, 2, 3, 4};
  s.zs = {5, 6, 7, 8};
  s.viewport = {0, 0, 640, 480, 0, 1};
  s.zscale = 1;
  s.transforms[2] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  s.transforms[3] = s.transforms[2];
  const gpu::State before = s;
  const auto original = memory;
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  DrawOriginalModHumans(scene, packet, world + 0x4960, TestClock, &env, activity);
  assert(draws == 3 && allocations == 3 && frees == 3 && !pending);
  assert(s.transforms == before.transforms &&
         s.streams[0].vb == before.streams[0].vb);
  DrawOriginalModHumans(scene, packet, world + 0x4960, TestClock, &env, activity);
  assert(draws == 3);
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  DrawOriginalModHumans(scene, packet, world, TestClock, &env, activity);
  assert(draws == 3);
  kernel::G<uint8_t>(world + 0x5AED) = 1;
  DrawOriginalModHumans(scene, packet, world + 0x4960, TestClock, &env, activity);
  assert(draws == 3);
  kernel::G<uint8_t>(world + 0x5AED) = 0;
  // The real wall clock is re-read before upload/draw, not frozen before
  // JNI or a long fence wait. Expiration AFTER allocation frees safely.
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  clockCalls = 0;
  expireAfter = 5;
  DrawOriginalModHumans(scene, packet, world + 0x4960, TestClock, &env, activity);
  assert(draws == 3 && allocations == 4 && frees == 4 && !pending);
  expireAfter = 0;
  // An ABORT/roster change after JNI succeeds must revoke the batch before
  // upload can reach Draw. A replacement committed ticket also cannot borrow
  // the old packet even if the episode and native world remain unchanged.
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  afterAllocation = [] { ++values[5]; };
  DrawOriginalModHumans(scene, packet, world + 0x4960, TestClock, &env, activity);
  afterAllocation = nullptr;
  assert(draws == 3 && allocations == 5 && frees == 5 && !pending);
  --values[5];
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  afterAllocation = [] { length = 16; };
  DrawOriginalModHumans(scene, packet, world + 0x4960, TestClock, &env, activity);
  afterAllocation = nullptr;
  length = 34;
  assert(draws == 3 && allocations == 6 && frees == 6 && !pending);
  assert(g_originalStageFileOpen.BeginOriginalLaunchAttempt() == 2);
  DrawOriginalModHumans(scene, packet, world + 0x4960, TestClock, &env, activity);
  assert(draws == 3);
  std::thread foreign([&] {
    ModHumanOriginalScene reject;
    assert(!source.Read(101, reject));
  });
  foreign.join();
  // All guest game bytes outside the two mod-owned allocations remain exact.
  for (size_t i = 0; i < memory.size(); i++) {
    if ((i >= 0x100000 && i < 0x100240) || (i >= 0x110000 && i < 0x110014))
      continue;
    assert(memory[i] == original[i]);
  }
  std::cout << "MOD_HUMAN_NATIVE_BRIDGE_PASS JNI ownership, full source scene, "
               "camera/grid, deferred fence; synthetic only\n";
}
