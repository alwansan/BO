#include "room_original_world_camera_frame.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
int main() {
  using namespace blackcoop;
  gpu::State s;
  s.rt = {1, 2, 3, 4};
  s.zs = {5, 6, 7, 8};
  s.viewport = {0, 0, 640, 480, 0, 1};
  s.zscale = 1;
  s.transforms[2] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  s.transforms[3] = s.transforms[2];
  OriginalCameraIdentity id{0x10000, 0x20000, 0x30000};
  OriginalWorldCameraFrames source;
  OriginalWorldCameraFrame frame;
  assert(!source.Read(id, s, frame));
  assert(source.Observe(id, s));
  assert(source.Read(id, s, frame));
  assert(frame.serial == 1 && frame.view == s.transforms[2]);
  auto original = s.transforms[2];
  s.transforms[2][12] = 10;
  assert(!source.Read(id, s, frame));
  s.transforms[2] = original;
  auto changed = id;
  changed.rwCamera++;
  assert(!source.Read(changed, s, frame));
  assert(source.Observe(id, s));
  assert(source.Read(id, s, frame) && frame.serial == 2);
  s.rt.data++;
  assert(!source.Read(id, s, frame));
  s.rt.data--;
  bool accepted = true;
  std::thread other([&] { accepted = source.Observe(id, s); });
  other.join();
  assert(!accepted);
  assert(source.Read(id, s, frame));
  uint32_t nan = 0x7fc00000;
  std::memcpy(&s.transforms[3][0], &nan, 4);
  assert(!source.Observe(id, s));
  s.transforms[3] = original;
  assert(!source.Read(id, s, frame));
  assert(source.Observe(id, s));
  source.Invalidate();
  assert(!source.Read(id, s, frame));
  // Unused scissor storage does not affect the current rasterization.
  assert(source.Observe(id, s));
  s.scissors[31] = 123;
  assert(source.Read(id, s, frame));
  s.scissor_count = 1;
  assert(source.Observe(id, s));
  s.scissors[0] = 9;
  assert(!source.Read(id, s, frame));
  std::cout
      << "ORIGINAL_WORLD_CAMERA_FRAME_PASS: exact SDK matrices, targets and "
         "source identity; no GPU draw or room authorization claimed\n";
}
