#include "room_mod_placeholder_submit.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
struct IO {
  const gpu::Backend *backend = nullptr;
  bool BackendIs(const gpu::Backend &b) {
    if (!backend)
      backend = &b;
    return backend == &b;
  }
  bool allocated = false, freed = false, quarantined = false, completed = false;
  unsigned writes = 0;
  std::array<uint8_t, 576> bytes{};
  bool Allocate(blackcoop::ModPlaceholderLease &l) {
    allocated = true;
    l = {0x100000, 0x200000};
    return true;
  }
  bool Upload(const blackcoop::ModPlaceholderLease &, const void *p,
              uint32_t n) {
    assert(n == bytes.size());
    std::memcpy(bytes.data(), p, n);
    ++writes;
    return true;
  }
  void Free(const blackcoop::ModPlaceholderLease &) {
    assert(!freed);
    freed = true;
    allocated = false;
  }
  void Quarantine(const blackcoop::ModPlaceholderLease &) {
    quarantined = true;
  }
};
struct Backend : gpu::Backend {
  IO &io;
  bool drawn = false, fail = false, pending = false;
  gpu::State captured{};
  explicit Backend(IO &v) : io(v) {}
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
    assert(io.allocated && !io.freed && p == 5 && start == 0 && n == 36 && !ix);
    captured = s;
    drawn = true;
    if (fail)
      throw std::runtime_error("queued draw failure");
  }
  uint32_t Fence() override {
    assert(drawn && !io.freed);
    return 7;
  }
  void WaitFence(uint32_t f) override {
    assert(f == 7 && !io.freed);
    float x = 0;
    std::memcpy(&x, io.bytes.data(), 4);
    assert(x == 12);
    io.completed = true;
  }
  bool FencePending(uint32_t) override { return pending; }
};
int main() {
  using namespace blackcoop;
  ModPlaceholderDraw packet;
  packet.vertices[0].x = 12;
  packet.state.vs_handle = 0x42;
  packet.state.rt.header = 44;
  packet.state.streams[0] = {0, 0};
  {
    IO io;
    Backend b(io);
    unsigned checks = 0;
    assert(SubmitModPlaceholder(b, io, packet, [&] { return ++checks <= 2; }) ==
           ModPlaceholderSubmitResult::Completed);
    assert(checks == 2 && io.completed && io.freed);
    assert(b.captured.streams[0].vb == 0x100000 &&
           b.captured.streams[0].stride == 16);
    assert(packet.state.streams[0].vb == 0);
  }
  {
    IO io;
    Backend b(io);
    assert(SubmitModPlaceholder(b, io, packet, [] { return false; }) ==
           ModPlaceholderSubmitResult::Rejected);
    assert(!io.allocated && !b.drawn);
  }
  {
    IO io;
    Backend b(io);
    unsigned checks = 0;
    assert(SubmitModPlaceholder(b, io, packet, [&] { return ++checks == 1; }) ==
           ModPlaceholderSubmitResult::Rejected);
    assert(io.freed && !b.drawn);
  }
  {
    IO io;
    Backend b(io);
    b.fail = true;
    assert(SubmitModPlaceholder(b, io, packet, [] { return true; }) ==
           ModPlaceholderSubmitResult::Quarantined);
    assert(io.quarantined && !io.freed);
  }
  {
    IO io;
    Backend b(io);
    b.pending = true;
    assert(SubmitModPlaceholder(b, io, packet, [] { return true; }) ==
           ModPlaceholderSubmitResult::Quarantined);
    assert(io.quarantined && !io.freed);
  }
  {
    IO io;
    Backend b(io), other(io);
    io.backend = &other;
    assert(SubmitModPlaceholder(b, io, packet, [] { return true; }) ==
           ModPlaceholderSubmitResult::Rejected);
    assert(!io.allocated && !b.drawn);
  }
  {
    IO io;
    Backend b(io);
    auto invalid = packet;
    invalid.state.textures[0] = 9;
    assert(SubmitModPlaceholder(b, io, invalid, [] { return true; }) ==
           ModPlaceholderSubmitResult::Rejected);
    assert(!io.allocated && !b.drawn);
  }
  std::cout << "MOD_PLACEHOLDER_FENCED_SUBMIT_PASS: deferred vertex reads "
               "complete before owned storage release; no native "
               "camera/hook/rasterization claimed\n";
}
