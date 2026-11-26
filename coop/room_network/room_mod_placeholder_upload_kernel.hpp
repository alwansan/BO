// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include "kernel/kernel.h"
#include "kernel/memory.h"
#include "room_mod_placeholder_submit.hpp"
#include <cstring>
#include <thread>
namespace blackcoop {
// Actual pinned SDK allocation bridge. Must outlive its pending leases and
// remain on its original thread. Destruction never frees uncertain queued
// memory. Explicit Drain after a backend recovery is required for quarantine.
class ModPlaceholderUploadKernel {
  struct Entry {
    ModPlaceholderLease lease{};
    bool quarantined = false;
  };
  gpu::Backend *const backend_;
  std::array<Entry, 8> entries_{};
  const std::thread::id owner_ = std::this_thread::get_id();
  bool owner() const noexcept { return owner_ == std::this_thread::get_id(); }
  Entry *find(const ModPlaceholderLease &l) noexcept {
    for (auto &e : entries_)
      if (e.lease.header && e.lease.header == l.header &&
          e.lease.data == l.data)
        return &e;
    return nullptr;
  }

public:
  explicit ModPlaceholderUploadKernel(gpu::Backend &backend)
      : backend_(&backend) {}
  bool BackendIs(const gpu::Backend &backend) const noexcept {
    return owner() && backend_ == &backend;
  }
  ModPlaceholderUploadKernel(const ModPlaceholderUploadKernel &) = delete;
  ModPlaceholderUploadKernel &
  operator=(const ModPlaceholderUploadKernel &) = delete;
  bool Allocate(ModPlaceholderLease &out) {
    if (!owner())
      return false;
    for (auto &e : entries_)
      if (!e.lease.header) {
        const auto data = mem::AllocContiguous(576, 0, 0xFFFFFFFFu, 0x1000);
        if (!data)
          return false;
        const auto header = mem::AllocSystem(20);
        if (!header) {
          mem::FreeContiguous(data);
          return false;
        }
        e = {{header, data}, false};
        out = e.lease;
        return true;
      }
    return false;
  }
  bool Upload(const ModPlaceholderLease &l, const void *bytes,
              uint32_t size) noexcept {
    auto *e = owner() ? find(l) : nullptr;
    if (!e || e->quarantined || !bytes || size != 576)
      return false;
    // Same resource header ABI as pinned Android d3d8.cpp UpVertices.
    std::memcpy(kernel::HostPtr(l.data), bytes, size);
    kernel::W32(l.header, 1);
    kernel::W32(l.header + 4, mem::PhysicalFromVirtual(l.data));
    return true;
  }
  void Free(const ModPlaceholderLease &l) noexcept {
    auto *e = owner() ? find(l) : nullptr;
    if (!e || e->quarantined)
      return;
    mem::FreeSystem(l.header);
    mem::FreeContiguous(l.data);
    *e = {};
  }
  void Quarantine(const ModPlaceholderLease &l) noexcept {
    auto *e = owner() ? find(l) : nullptr;
    if (e)
      e->quarantined = true;
  }
  bool Drain(gpu::Backend &backend) {
    if (!BackendIs(backend))
      return false;
    bool any = false;
    for (const auto &e : entries_)
      any |= e.quarantined;
    if (!any)
      return true;
    const uint32_t fence = backend.Fence();
    if (!fence)
      return false;
    backend.WaitFence(fence);
    if (backend.FencePending(fence))
      return false;
    for (auto &e : entries_)
      if (e.quarantined) {
        e.quarantined = false;
        Free(e.lease);
      }
    return true;
  }
};
} // namespace blackcoop
