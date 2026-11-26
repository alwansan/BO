// SDK-linked production observer with synthetic memory. No original engine run.
#include "room_original_world_camera_witness.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
static std::vector<uint8_t> memory(0x300000);
namespace kernel {
uint8_t *g_base = memory.data();
}
namespace mem {
bool IsCommitted(uint32_t p) {
  return p >= 0x10000 && uint64_t(p) + 4096 <= memory.size();
}
} // namespace mem
namespace d3d {
gpu::State state;
std::recursive_mutex mutex;
gpu::State &State() { return state; }
std::recursive_mutex &Lock() { return mutex; }
} // namespace d3d
int main() {
  using namespace blackcoop;
  constexpr uint32_t world = 0x10000, renderer = 0x30000, rw = 0x80000;
  kernel::W32(0x2D45D4, world);
  kernel::W32(0x2D67F4, renderer);
  kernel::W32(world + 0x5AE0, 0x1C);
  kernel::W32(renderer + 0xD4A8, rw);
  kernel::W32(renderer + 0xD590, renderer + 0xD450);
  auto &s = d3d::State();
  s.rt = {1, 2, 3, 4};
  s.zs = {5, 6, 7, 8};
  s.viewport = {0, 0, 640, 480, 0, 1};
  s.transforms[2] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  s.transforms[3] = s.transforms[2];
  const auto originalMemory = memory;
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  assert(OriginalWorldCameraAvailable().load());
  OriginalWorldCameraFrame frame;
  assert(ReadCurrentOriginalWorldCameraFrame(frame));
  // Rejected foreign camera waits behind a valid owner callback. It must
  // revoke AFTER taking the SDK lock, even when owner just republished true.
  std::unique_lock<std::recursive_mutex> lock(d3d::Lock());
  std::thread rejected([&] { ObserveOriginalWorldCameraAfterBegin(rw, 0); });
  const auto deadline =
      std::chrono::steady_clock::now() + std::chrono::seconds(3);
  while (OriginalWorldCameraAvailable().load()) {
    assert(std::chrono::steady_clock::now() < deadline);
    std::this_thread::yield();
  }
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  assert(OriginalWorldCameraAvailable().load());
  lock.unlock();
  rejected.join();
  assert(!OriginalWorldCameraAvailable().load());
  assert(!ReadCurrentOriginalWorldCameraFrame(frame));
  ObserveOriginalWorldCameraAfterBegin(rw, 1);
  assert(ReadCurrentOriginalWorldCameraFrame(frame));
  s.transforms[3][12] = 1;
  assert(!ReadCurrentOriginalWorldCameraFrame(frame));
  s.transforms[3][12] = 0;
  ObserveOriginalWorldCameraAfterBegin(rw + 4, 1);
  assert(!ReadCurrentOriginalWorldCameraFrame(frame));
  assert(memory == originalMemory);
  std::cout << "ORIGINAL_CAMERA_NATIVE_WITNESS_PASS: real observer revocation "
               "ordering under SDK mutex, no guest writes\n";
}
