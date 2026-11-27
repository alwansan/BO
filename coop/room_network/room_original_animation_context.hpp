// Copyright (c) 2026 portingpete
// Copyright (c) 2026 sp00nz
// Copyright (c) 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Adapted from Black-2006-Recomp b2512fafd3 black_p2_vm_take/black_p2_vm.
// See room_original_animation_context.LICENSE. No original game bytes included.
#pragma once
#include <algorithm>
#include <array>
#include "room_original_host_setup_tick.hpp"
#include <cstdint>
#include <cstring>
#include <thread>
#include <utility>
#include <vector>
namespace blackcoop {
// Actual original-memory snapshot/exchange, NOT an actor or readiness flag.
// IO::Map returns a writable contiguous mapping; Scene and Generation are live
// ORIGINAL engine lifecycle witnesses, not network counters. Generation(p,n)
// must change when the range is destroyed/reused, even at the same address.
// A generation is NOT proof of heap/region provenance. Before native use the
// source IO must also prove each writable region is the actual original
// viewmodel/animation allocation or known arena; a committed-page shim is
// insufficient. Root-word and overlap guards do not establish that provenance.
// All calls on the original game thread, no yielding or engine callbacks during
// Exchange. Mapping cannot disappear between preflight and memcpy on that
// thread. No native kernel shim until original allocation-generation hooks
// exist.
class OriginalAnimationContext {
  static constexpr uint32_t kSet = 0x2D12C8, kCache = 0x2CE6B0,
                             kWork = 0x2C3260, kWorkBytes = 0x2CE710 - 0x2C3260;
  // The 001808D7 call constructs three independently allocated regions.
  static constexpr uint32_t kCacheHeaderBytes = 0x1044, kCacheEntries = 16,
                            kCacheEntryBytes = 0x284, kCacheDataBytes = 0x2004;
  struct Piece {
    uint32_t at = 0;
    uint64_t generation = 0;
    std::vector<uint8_t> kept;
  };
  struct Binding {
    uint32_t at = 0, value = 0;
  };
  std::thread::id owner_ = std::this_thread::get_id();
  uint64_t scene_ = 0;
  bool captured_ = false, entered_ = false;
  uint32_t set_ = 0, table_ = 0, tops_ = 0, count_ = 0, bytes_ = 0, cache_ = 0;
  uint64_t setGeneration_ = 0, tableGeneration_ = 0, topsGeneration_ = 0,
           poolGeneration_ = 0;
  uint32_t pool_ = 0;
  std::array<Piece, 3> emptyCache_;
  std::array<std::vector<uint8_t>, 3> primaryCache_;
  Piece workspace_;
  std::vector<Piece> pieces_, blocks_;
  std::vector<uint32_t> shadowTops_;
  std::vector<Binding> bindings_;
  static bool range(uint32_t p, uint32_t n) {
    return p >= 0x10000 && n && uint64_t(p) + n <= 0x100000000ull;
  }
  static bool overlaps(uint32_t a, uint32_t n, uint32_t b, uint32_t m) {
    return uint64_t(a) < uint64_t(b) + m && uint64_t(b) < uint64_t(a) + n;
  }
  // Original pointer roots are shared process identity, never dynamic player
  // scratch. The workspace is exchanged separately and intentionally contains
  // kCache, whose value is kept identical in both copies.
  static bool rootOverlap(uint32_t p, uint32_t n) {
    constexpr uint32_t roots[] = {
        kSet,     kCache,   0x2D0D60, 0x2D0D64, 0x2D12B8, 0x2D12D8, 0x2D12EC,
        0x2D1980, 0x2D1988, 0x2D1990, 0x2D199C, 0x2D45D4, 0x2D45E8, 0x2D45F4};
    for (uint32_t root : roots)
      if (overlaps(p, n, root, 4))
        return true;
    return false;
  }
  static uint32_t word(const uint8_t *p) {
    uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
  }
  static void put(uint8_t *p, uint32_t v) { std::memcpy(p, &v, 4); }
  bool owner() const { return owner_ == std::this_thread::get_id(); }
  bool cacheOverlap(uint32_t p, uint32_t n) const {
    for (const auto &region : emptyCache_)
      if (overlaps(p, n, region.at, static_cast<uint32_t>(region.kept.size())))
        return true;
    return false;
  }
  template <class IO> static uint8_t *map(IO &io, uint32_t p, uint32_t n) {
    return range(p, n) ? io.Map(p, n) : nullptr;
  }
  template <class IO> static bool read(IO &io, uint32_t p, uint32_t &v) {
    auto *m = map(io, p, 4);
    if (!m)
      return false;
    v = word(m);
    return true;
  }
  template <class IO>
  static bool take(IO &io, uint32_t p, uint32_t n, Piece &result) {
    auto *m = map(io, p, n);
    const auto gen = io.Generation(p, n);
    if (!m || !gen)
      return false;
    result.at = p;
    result.generation = gen;
    result.kept.assign(m, m + n);
    return true;
  }
  template <class IO> static uint8_t *live(IO &io, const Piece &p) {
    return io.Generation(p.at, static_cast<uint32_t>(p.kept.size())) ==
                   p.generation
               ? map(io, p.at, static_cast<uint32_t>(p.kept.size()))
               : nullptr;
  }
  template <class IO> bool cacheLive(IO &io) const {
    for (const auto &region : emptyCache_)
      if (region.kept.empty() || !live(io, region))
        return false;
    return true;
  }
  template <class IO> bool add(IO &io, uint32_t p, uint32_t n) {
    if (!(p == set_ + 0x398 && n == 0x970 - 0x398) &&
        overlaps(p, n, set_, 0x970))
      return false;
    for (const auto &piece : pieces_) {
      if (piece.at == p && piece.kept.size() == n)
        return true;
      if (overlaps(p, n, piece.at, static_cast<uint32_t>(piece.kept.size())))
        return false;
    }
    if (pieces_.size() >= 32 || rootOverlap(p, n) ||
        overlaps(p, n, kWork, kWorkBytes) || overlaps(p, n, pool_, 0x8540) ||
        cacheOverlap(p, n))
      return false;
    Piece piece;
    if (!take(io, p, n, piece))
      return false;
    pieces_.push_back(std::move(piece));
    return true;
  }
  template <class IO> bool bind(IO &io, uint32_t p, uint32_t &value) {
    if (!read(io, p, value))
      return false;
    bindings_.push_back({p, value});
    return true;
  }
  static void exchange(uint8_t *live, std::vector<uint8_t> &kept, uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
      std::swap(live[i], kept[i]);
  }
  template <class IO> bool shape(IO &io) {
    uint32_t value = 0;
    if (!scene_ || io.Scene() != scene_ || !read(io, kSet, value) ||
        value != set_ || !read(io, kCache, value) || value != cache_ ||
        !read(io, 0x2D1980, value) || value != pool_ ||
        io.Generation(set_, 0x970) != setGeneration_ ||
        io.Generation(table_, count_ * 4) != tableGeneration_ ||
        io.Generation(tops_, count_ * 4) != topsGeneration_ ||
        io.Generation(pool_, 0x8540) != poolGeneration_)
      return false;
    for (const auto &binding : bindings_)
      if (!read(io, binding.at, value) || value != binding.value)
        return false;
    for (uint32_t i = 0; i < count_; i++)
      if (!read(io, table_ + i * 4, value) || value != blocks_[i].at)
        return false;
    return read(io, cache_ + 8, value) && value == kCacheEntries &&
           read(io, cache_ + 0xC, value) && value == emptyCache_[1].at &&
           read(io, emptyCache_[1].at, value) && value == kCacheEntries &&
           read(io, cache_ + 0x20, value) && value == emptyCache_[2].at &&
           read(io, cache_ + 0x24, value) && value == kCacheDataBytes;
  }

public:
  OriginalAnimationContext() = default;
  OriginalAnimationContext(const OriginalAnimationContext &) = delete;
  OriginalAnimationContext &
  operator=(const OriginalAnimationContext &) = delete;
  bool Entered() const { return owner() && entered_; }
  // Invoke ONLY immediately after ORIGINAL 950F0 completed, with its real ECX.
  // The bytes are the original constructor result, never a fabricated zero
  // cache.
  template <class IO>
  bool ObserveOriginalCacheConstructor(IO &io, uint32_t cache) {
    if (!owner() || captured_ || !io.Scene())
      return false;
    uint32_t global = 0, entries = 0, start = 0, data = 0, value = 0;
    if (!read(io, kCache, global) || global != cache ||
        !map(io, cache, kCacheHeaderBytes) ||
        !read(io, cache + 8, value) || value != kCacheEntries ||
        !read(io, cache + 0xC, entries) || !range(entries, kCacheEntryBytes) ||
        !read(io, entries, value) || value != kCacheEntries ||
        !read(io, cache + 0x10, start) || start != entries + 4 ||
        !read(io, cache + 0x20, data) ||
        !read(io, cache + 0x24, value) || value != kCacheDataBytes)
      return false;
    const std::array<std::pair<uint32_t, uint32_t>, 3> ranges{{
        {cache, kCacheHeaderBytes}, {entries, kCacheEntryBytes}, {data, kCacheDataBytes}}};
    std::array<Piece, 3> next;
    for (size_t i = 0; i < ranges.size(); ++i) {
      const auto [at, bytes] = ranges[i];
      if (!range(at, bytes) || (at & 3u) || rootOverlap(at, bytes) ||
          overlaps(at, bytes, kWork, kWorkBytes))
        return false;
      for (size_t j = 0; j < i; ++j)
        if (overlaps(at, bytes, ranges[j].first, ranges[j].second))
          return false;
      if (!take(io, at, bytes, next[i]))
        return false;
      primaryCache_[i].resize(bytes);
    }
    scene_ = io.Scene();
    cache_ = cache;
    emptyCache_ = std::move(next);
    return true;
  }
  template <class IO> bool Capture(IO &io) {
    if (!owner() || captured_ || !scene_ || scene_ != io.Scene() ||
        !cacheLive(io))
      return false;
    uint32_t global = 0;
    if (!read(io, kCache, global) || global != cache_ ||
        !read(io, kSet, set_) || !map(io, set_, 0x970) ||
        !read(io, 0x2D1980, pool_) || !map(io, pool_, 0x8540))
      return false;
    if (rootOverlap(set_, 0x970) ||
        cacheOverlap(set_, 0x970) ||
        overlaps(set_, 0x970, pool_, 0x8540) ||
        overlaps(set_, 0x970, kWork, kWorkBytes) ||
        overlaps(pool_, 0x8540, kWork, kWorkBytes) ||
        cacheOverlap(pool_, 0x8540))
      return false;
    setGeneration_ = io.Generation(set_, 0x970);
    poolGeneration_ = io.Generation(pool_, 0x8540);
    pieces_.clear();
    blocks_.clear();
    bindings_.clear();
    shadowTops_.clear();
    if (!setGeneration_ || !poolGeneration_ ||
        !add(io, set_ + 0x398, 0x970 - 0x398) ||
        !take(io, kWork, kWorkBytes, workspace_))
      return false;
    uint32_t controllers[2]{};
    for (uint32_t k = 0; k < 2; k++) {
      const uint32_t slot = set_ + 0x470 + k * 0x240;
      for (uint32_t off = 0x30; off < 0x50; off += 4) {
        uint32_t address = 0;
        // The original first-person constructor passes 1 to 0017EAE0,
        // which initializes these optional transform pointers to null.
        if (!bind(io, slot + off, address) || (address && !add(io, address, 0xA0)))
          return false;
      }
      if (!bind(io, slot + 0x54, controllers[k]) ||
          !add(io, controllers[k], 0x9D0))
        return false;
    }
    if (controllers[0] == controllers[1])
      return false;
    if (!bind(io, set_ + 0x8FC, table_) || !bind(io, set_ + 0x900, tops_) ||
        !bind(io, set_ + 0x908, bytes_) || !bind(io, set_ + 0x90C, count_) ||
        !count_ || count_ > 64 || !bytes_ || bytes_ > 0x4000 ||
        !map(io, table_, count_ * 4) || !map(io, tops_, count_ * 4))
      return false;
    tableGeneration_ = io.Generation(table_, count_ * 4);
    topsGeneration_ = io.Generation(tops_, count_ * 4);
    if (!tableGeneration_ || !topsGeneration_ ||
        rootOverlap(table_, count_ * 4) || rootOverlap(tops_, count_ * 4) ||
        overlaps(table_, count_ * 4, set_, 0x970) ||
        overlaps(tops_, count_ * 4, set_, 0x970) ||
        overlaps(table_, count_ * 4, tops_, count_ * 4) ||
        overlaps(table_, count_ * 4, kWork, kWorkBytes) ||
        overlaps(tops_, count_ * 4, kWork, kWorkBytes) ||
        overlaps(table_, count_ * 4, pool_, 0x8540) ||
        overlaps(tops_, count_ * 4, pool_, 0x8540) ||
        cacheOverlap(table_, count_ * 4) ||
        cacheOverlap(tops_, count_ * 4))
      return false;
    for (const auto &p : pieces_)
      if (overlaps(p.at, static_cast<uint32_t>(p.kept.size()), tops_,
                   count_ * 4) ||
          overlaps(p.at, static_cast<uint32_t>(p.kept.size()), table_,
                   count_ * 4))
        return false;
    for (uint32_t i = 0; i < count_; i++) {
      uint32_t at = 0, top = 0;
      if (!read(io, table_ + i * 4, at) || !read(io, tops_ + i * 4, top) ||
          (top && (top < at || uint64_t(top) > uint64_t(at) + bytes_)))
        return false;
      if (rootOverlap(at, bytes_) || overlaps(at, bytes_, set_, 0x970) ||
          overlaps(at, bytes_, kWork, kWorkBytes) ||
           cacheOverlap(at, bytes_) ||
          overlaps(at, bytes_, table_, count_ * 4) ||
          overlaps(at, bytes_, tops_, count_ * 4) ||
          overlaps(at, bytes_, pool_, 0x8540))
        return false;
      for (const auto &p : pieces_)
        if (overlaps(at, bytes_, p.at, static_cast<uint32_t>(p.kept.size())))
          return false;
      for (const auto &p : blocks_)
        if (overlaps(at, bytes_, p.at, bytes_))
          return false;
      Piece block;
      if (!take(io, at, bytes_, block))
        return false;
      blocks_.push_back(std::move(block));
      shadowTops_.push_back(top);
    }
    // Clear borrowed original playing ownership only in the inactive shadow
    // copy.
    auto &set = pieces_[0].kept;
    for (uint32_t k = 0; k < 2; k++) {
      put(set.data() + k * 0x6C + 0xC, 0);
      auto *slot = set.data() + 0x470 + k * 0x240 - 0x398;
      put(slot + 0x58, 0);
      slot[0xB9] = 0;
      std::memset(slot + 0x90, 0, 0x10);
      for (auto &p : pieces_)
        if (p.at == controllers[k])
          put(p.kept.data() + 4, 0);
    }
    captured_ = true;
    // Full preflight is also required for initial capture; no original writes.
    if (!CanExchange(io)) {
      captured_ = false;
      return false;
    }
    return true;
  }
  template <class IO> bool CanExchange(IO &io) {
    if (!owner() || !captured_ || !shape(io) || !cacheLive(io) ||
        !live(io, workspace_) || !map(io, tops_, count_ * 4))
      return false;
    for (const auto &p : pieces_)
      if (!live(io, p))
        return false;
    uint64_t selected = 0;
    for (uint32_t k = 0; k < 2; k++) {
      uint32_t index = 0;
      if (!read(io, set_ + 0x470 + k * 0x240 + 0xAC, index) || index >= count_)
        return false;
      selected |= uint64_t{1} << index;
      index = word(pieces_[0].kept.data() + 0x470 + k * 0x240 + 0xAC - 0x398);
      if (index >= count_)
        return false;
      selected |= uint64_t{1} << index;
    }
    // The pool belongs to every animated model. Never exchange an NPC's block.
    for (uint32_t i = 0; i < 32; i++) {
      uint32_t model = 0, index = 0;
      if (!read(io, pool_ + 0xA0 + i * 0x3C0 + 0x328, model))
        return false;
      if (model && (!read(io, model + 0xAC, index) || index >= count_ ||
                    (selected & (uint64_t{1} << index))))
        return false;
    }
    for (uint32_t i = 0; i < count_; i++) {
      if (!live(io, blocks_[i]))
        return false;
      if (!(selected & (uint64_t{1} << i)))
        continue;
      uint32_t top = 0;
      if (!read(io, tops_ + i * 4, top) || top < blocks_[i].at ||
          uint64_t(top) > uint64_t(blocks_[i].at) + bytes_ ||
          shadowTops_[i] < blocks_[i].at ||
          uint64_t(shadowTops_[i]) > uint64_t(blocks_[i].at) + bytes_)
        return false;
    }
    return true;
  }
  template <class IO> bool Exchange(IO &io) {
    if (!CanExchange(io))
      return false;
    // Cache all writable mappings and lengths BEFORE the first byte is changed.
    std::vector<uint8_t *> mapped;
    for (const auto &p : pieces_)
      mapped.push_back(live(io, p));
    auto *work = live(io, workspace_);
    std::array<uint8_t *, 3> cacheMappings;
    for (size_t i = 0; i < cacheMappings.size(); ++i) {
      cacheMappings[i] = live(io, emptyCache_[i]);
      if (!cacheMappings[i])
        return false;
    }
    auto *tops = map(io, tops_, count_ * 4);
    if (!work || !tops ||
        std::any_of(mapped.begin(), mapped.end(), [](auto *p) { return !p; }))
      return false;
    std::vector<uint8_t *> blockMappings(count_);
    std::vector<uint32_t> lengths(count_, 0);
    for (uint32_t i = 0; i < count_; i++) {
      bool selected = false;
      for (uint32_t k = 0; k < 2; k++) {
        selected |= word(mapped[0] + 0x470 + k * 0x240 + 0xAC - 0x398) == i;
        selected |= word(pieces_[0].kept.data() + 0x470 + k * 0x240 + 0xAC -
                         0x398) == i;
      }
      if (selected) {
        blockMappings[i] = live(io, blocks_[i]);
        lengths[i] =
            std::max(word(tops + i * 4), shadowTops_[i]) - blocks_[i].at;
      }
    }
    for (uint32_t i = 0; i < count_; i++)
      if (lengths[i] && !blockMappings[i])
        return false;
    exchange(work, workspace_.kept, kWorkBytes);
    for (size_t i = 0; i < cacheMappings.size(); ++i) {
      auto *const mapping = cacheMappings[i];
      auto &primary = primaryCache_[i];
      if (entered_) {
        std::memcpy(mapping, primary.data(), primary.size());
      } else {
        // The extra context starts with an empty cache. Restore the original
        // player and shared animation entries on exit, at their same addresses.
        std::memcpy(primary.data(), mapping, primary.size());
        std::memcpy(mapping, emptyCache_[i].kept.data(), primary.size());
      }
    }
    for (uint32_t i = 0; i < count_; i++)
      if (blockMappings[i]) {
        exchange(blockMappings[i], blocks_[i].kept, lengths[i]);
        const auto held = word(tops + i * 4);
        put(tops + i * 4, shadowTops_[i]);
        shadowTops_[i] = held;
      }
    for (size_t i = 0; i < pieces_.size(); i++)
      exchange(mapped[i], pieces_[i].kept,
               static_cast<uint32_t>(pieces_[i].kept.size()));
    entered_ = !entered_;
    return true;
  }
};
// Pinned MIT black_p2_swap / black_fx_owners / black_flash_swap /
// black_shells_swap. The IO must certify exact original FX regions and
// lifetime, and map them on the original owner thread without yielding.
// No original callback or production mapping is enabled here.
class OriginalEffectsContext final {
 static constexpr uint32_t root_=0x2D1990u;
 static constexpr uint32_t fxOff_=0x65690u,flashOff_=0x4E1F0u+0xC0u,
                           shellsOff_=0x506D0u;
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t scene_=0,fxGen_=0,flashGen_=0,shellGen_=0;
 uint32_t base_=0,first_=0,extra_=0;
 bool bound_=false,entered_=false,poisoned_=false;
 std::array<uint8_t,12> flash_{};
 std::array<uint8_t,0x730> shells_{};
 static uint32_t read32(const uint8_t* p)noexcept{
  uint32_t v=0;std::memcpy(&v,p,4);return v;
 }
 static uint16_t read16(const uint8_t* p)noexcept{
  uint16_t v=0;std::memcpy(&v,p,2);return v;
 }
 static void put(uint8_t* p,uint32_t v)noexcept{std::memcpy(p,&v,4);}
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 template<class IO>bool root(IO& io)const{
  auto* p=io.Map(root_,4u);
  return owner()&&scene_&&io.Scene()==scene_&&p&&read32(p)==base_;
 }
 template<class IO>bool mapAll(IO& io,uint8_t*& fx,uint8_t*& flash,uint8_t*& shells)const{
  if(!root(io)||io.Generation(base_+fxOff_,0x3460u)!=fxGen_||
     io.Generation(base_+flashOff_,12u)!=flashGen_||
     io.Generation(base_+shellsOff_,0x730u)!=shellGen_)return false;
  fx=io.Map(base_+fxOff_,0x3460u);
  flash=io.Map(base_+flashOff_,12u);
  shells=io.Map(base_+shellsOff_,0x730u);
  if(!fx||!flash||!shells)return false;
  const auto cap=read16(fx+0x344Au),used=read16(fx+0x3448u),
             asked=read16(fx+0x3458u);
  return cap<=0x50u&&used<=cap&&asked<=0x20u&&
    io.Map(read32(fx+0x3440u),0xA0u)&&io.Map(read32(fx+0x3444u),0xA0u);
 }
 void swapOwner(uint8_t* p)const noexcept{
  const auto value=read32(p);
  if(value==first_)put(p,extra_);else if(value==extra_)put(p,first_);
 }
public:
 OriginalEffectsContext()=default;
 OriginalEffectsContext(const OriginalEffectsContext&)=delete;
 OriginalEffectsContext& operator=(const OriginalEffectsContext&)=delete;
 bool Entered()const noexcept{return owner()&&entered_;}
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
 template<class IO>bool Bind(IO& io,uint32_t first,uint32_t extra){
  if(!owner()||bound_||poisoned_||first<0x10000u||extra<0x10000u||first==extra||
     !io.Scene()||!io.OwnsEffectsActorPair(first,extra))return false;
  auto* p=io.Map(root_,4u);if(!p)return false;
  const auto base=read32(p);
  if(base<0x10000u||uint64_t(base)+fxOff_+0x3460u>0x100000000ull)return false;
  scene_=io.Scene();base_=base;first_=first;extra_=extra;
  fxGen_=io.Generation(base+fxOff_,0x3460u);
  flashGen_=io.Generation(base+flashOff_,12u);
  shellGen_=io.Generation(base+shellsOff_,0x730u);
  uint8_t *fx=nullptr,*f=nullptr,*s=nullptr;
  if(!fxGen_||!flashGen_||!shellGen_||!mapAll(io,fx,f,s))return false;
  bound_=true;return true;
 }
 template<class IO>bool Exchange(IO& io){
  if(!owner()||!bound_||poisoned_)return false;
  uint8_t *fx=nullptr,*f=nullptr,*s=nullptr;
  if(!io.OwnsEffectsActorPair(first_,extra_)||!mapAll(io,fx,f,s)){
   if(entered_)poisoned_=true;
   return false;
  }
  // All mappings validated before first write; only exchange the two
  // selected actor identities, leaving every NPC and other human intact.
  for(unsigned i=0;i<0x50u;++i)swapOwner(fx+0x2300u+4u*i);
  for(unsigned i=0;i<read16(fx+0x3458u);++i)
   swapOwner(fx+0x26C0u+i*0x60u+0x44u);
  for(unsigned i=0;i<flash_.size();++i)std::swap(f[i],flash_[i]);
  for(unsigned i=0;i<shells_.size();++i)std::swap(s[i],shells_[i]);
  entered_=!entered_;return true;
 }
};
// The original bank selection at 001BC5D0 reads [[2D0D60]+D3C8]+C.
// Exchange that selector and the manager root (2D12D8). The audio registry
// retains addresses of embedded objects, so both constructed pools stay put.
class OriginalWeaponSoundContext final {
 const std::thread::id thread_=std::this_thread::get_id();
 static constexpr uint32_t managerRoot_=0x2D12D8u, soundRoot_=0x2D0D60u;
 static constexpr uint32_t bytes_=0x1330u;
 // The provider must prove registry membership for the stationary pools,
 // independent lifetime for the effects objects and their row/table backing,
 // and exclusive access to the selectors for the entire scope.
 struct Region {uint32_t address=0,bytes=0;uint64_t generation=0;};
 Region firstManager_,extraManager_,firstPool_,extraPool_,firstEffects_,extraEffects_,selector_;
 uint64_t scene_=0;
 uint32_t selectorParent_=0;
 bool bound_=false,entered_=false,poisoned_=false,voicesRetired_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
 static uint32_t word(const uint8_t* p){uint32_t v=0;std::memcpy(&v,p,4);return v;}
 static bool span(uint32_t p,uint32_t n){
  return p>=0x10000u&&!(p&3u)&&uint64_t(p)+n<=0x100000000ull;
 }
 static bool overlap(const Region& a,const Region& b){
  return uint64_t(a.address)<uint64_t(b.address)+b.bytes&&
         uint64_t(b.address)<uint64_t(a.address)+a.bytes;
 }
 template<class IO>static uint32_t resolveSelector(IO& io,uint32_t& parent){
  auto* root=io.Map(soundRoot_,4u);if(!root)return 0;
  uint32_t a=word(root);if(!span(a,0xD3CCu))return 0;
  parent=a+0xD3C8u;
  auto* second=io.Map(parent,4u);if(!second)return 0;
   uint32_t b=word(second);if(!span(b,0x18u))return 0;
   return b+0xCu;
  }
  template<class IO>bool resolveExchange(IO& io,uint8_t*& manager,uint8_t*& selectors)const{
  uint32_t parent=0;
  if(!owner()||!bound_||poisoned_||voicesRetired_||io.Scene()!=scene_||
     resolveSelector(io,parent)!=selector_.address||parent!=selectorParent_)return false;
   for(const auto& region:{firstManager_,extraManager_,firstPool_,extraPool_,
                           firstEffects_,extraEffects_,selector_}){
   if(!io.Map(region.address,region.bytes)||
      io.Generation(region.address,region.bytes)!=region.generation)return false;
  }
  manager=io.Map(managerRoot_,4u);
   selectors=io.Map(selector_.address,selector_.bytes);
   return manager&&selectors&&manager!=selectors&&
     word(manager)==(entered_?extraManager_:firstManager_).address&&
     word(selectors)==(entered_?extraPool_:firstPool_).address&&
     word(selectors+8u)==(entered_?extraEffects_:firstEffects_).address&&
     io.OwnsLiveWeaponSoundContext(firstManager_.address,extraManager_.address,
                                   firstPool_.address,extraPool_.address)&&
     io.OwnsLiveAudioEffectsContext(firstEffects_.address,extraEffects_.address);
 }
public:
 OriginalWeaponSoundContext()=default;
 OriginalWeaponSoundContext(const OriginalWeaponSoundContext&)=delete;
 OriginalWeaponSoundContext& operator=(const OriginalWeaponSoundContext&)=delete;
 bool Entered()const noexcept{return owner()&&entered_;}
 bool Quarantined()const noexcept{return owner()&&poisoned_;}
  template<class IO>bool Bind(IO& io,uint32_t extraManager,uint32_t extraPool,uint32_t extraEffects){
  if(!owner()||bound_||poisoned_||!io.Scene())return false;
  uint32_t parent=0;
  const auto selector=resolveSelector(io,parent);
  if(!selector||parent==managerRoot_||parent==soundRoot_)return false;
  auto* managerRoot=io.Map(managerRoot_,4u);
   auto* selectors=io.Map(selector,0xCu);
   if(!managerRoot||!selectors||managerRoot==selectors)return false;
   std::array<Region,7> regions{{{word(managerRoot),0xD0u},{extraManager,0xD0u},
      {word(selectors),bytes_},{extraPool,bytes_},{word(selectors+8u),0x290u},
      {extraEffects,0x290u},{selector,0xCu}}};
  for(unsigned i=0;i<regions.size();++i){
   auto& item=regions[i];
   if(!span(item.address,item.bytes)||overlap(item,{managerRoot_,4u})||
      overlap(item,{soundRoot_,4u})||overlap(item,{parent,4u})||
      !io.Map(item.address,item.bytes))return false;
   for(unsigned j=0;j<i;++j)if(overlap(item,regions[j]))return false;
   item.generation=io.Generation(item.address,item.bytes);
   if(!item.generation)return false;
  }
   if(!io.OwnsLiveWeaponSoundContext(regions[0].address,extraManager,regions[2].address,extraPool)||
      !io.OwnsLiveAudioEffectsContext(regions[4].address,extraEffects))
    return false;
   firstManager_=regions[0];extraManager_=regions[1];
   firstPool_=regions[2];extraPool_=regions[3];
   firstEffects_=regions[4];extraEffects_=regions[5];selector_=regions[6];
  selectorParent_=parent;
  scene_=io.Scene();bound_=true;return true;
 }
 template<class IO>bool CanExchange(IO& io)const{
   uint8_t *manager=nullptr,*selectors=nullptr;
   return resolveExchange(io,manager,selectors);
 }
 template<class IO>bool Exchange(IO& io){
  if(!owner()||!bound_||poisoned_)return false;
   uint8_t *manager=nullptr,*selectors=nullptr;
   if(!resolveExchange(io,manager,selectors)){
   if(entered_)poisoned_=true;
   return false;
  }
   const auto nextManager=(entered_?firstManager_:extraManager_).address;
   const auto nextPool=(entered_?firstPool_:extraPool_).address;
   const auto nextEffects=(entered_?firstEffects_:extraEffects_).address;
   std::memcpy(manager,&nextManager,4u);
   std::memcpy(selectors,&nextPool,4u);
   std::memcpy(selectors+8u,&nextEffects,4u);
  entered_=!entered_;return true;
 }

  // Final use of the two independently bound first-person voice records.
  // The caller must have stopped P2 updates and provide exclusive use of the
  // selectors. invokeRelease calls 001BC580 with EDX=record and checks AL.
  // Returning the reservations does not unpublish or destroy audio objects.
  template<class IO,class Release>bool RetireWeaponVoices(IO& io,Release invokeRelease){
   if(!owner()||entered_||!CanExchange(io))return false;
   const auto* pool=io.Map(extraPool_.address,extraPool_.bytes);
   const auto* records=io.Map(extraManager_.address,extraManager_.bytes);
   if(!pool||!records)return false;
   const uint32_t firstBank=word(pool),secondBank=word(pool+0x10u);
   if(!firstBank||!secondBank||firstBank==secondBank)return false;
   for(uint32_t i=0;i<2u;++i){
    const auto* record=records+8u+i*0x38u;
    const uint32_t sound=extraPool_.address+0x360u+i*2u*0x290u;
    const auto* bank=pool+i*0x10u;
    if(word(record+8u)!=sound+0x290u||word(record+0xCu)!=sound||
       word(record+0x10u)!=word(bank)||bank[0xFu]!=1u)return false;
   }
   if(!Exchange(io))return false;
   bool released=false;
   try{
    released=true;
    for(uint32_t i=0;i<2u&&released;++i){
     const uint32_t address=extraManager_.address+8u+i*0x38u;
     const bool returned=invokeRelease(address);
     if(!returned||!entered_||!CanExchange(io)){released=false;break;}
     const auto* record=io.Map(address,0x38u);
     const auto* bank=io.Map(extraPool_.address+i*0x10u,0x10u);
     released=record&&bank&&!word(record+8u)&&!word(record+0xCu)&&
              !word(record+0x10u)&&!record[0x30u]&&!bank[0xFu];
    }
   }catch(...){released=false;}
   bool restored=false;
   try{restored=(!entered_||Exchange(io))&&CanExchange(io);}catch(...){}
   voicesRetired_=released&&restored;
   if(!voicesRetired_)poisoned_=true;
   return voicesRetired_;
  }
};
// One shared arbiter for all P2/P3/P4 presentation scopes in a scene.
// Retain through original engine teardown. No parallel access to singleton
// animation, weapon manager or FX pools, even though players are independent.
class OriginalPresentationArbiter final {
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t scene_=0;
 uint8_t active_=0;
 bool poisoned_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
public:
 bool Available(uint64_t scene,uint8_t peer)const noexcept{
  return owner()&&!poisoned_&&!active_&&scene&&peer>=2&&peer<=4&&
    (!scene_||scene_==scene);
 }
 bool Acquire(uint64_t scene,uint8_t peer)noexcept{
  if(!Available(scene,peer))return false;
  scene_=scene;active_=peer;return true;
 }
 bool Held(uint64_t scene,uint8_t peer)const noexcept{
  return owner()&&!poisoned_&&scene_==scene&&active_==peer&&peer>=2&&peer<=4;
 }
 bool Release(uint64_t scene,uint8_t peer,bool restored)noexcept{
  if(!Held(scene,peer))return false;
  if(!restored){poisoned_=true;return false;}
  active_=0;return true;
 }
 bool AfterVerifiedEngineTeardown(bool verified)noexcept{
  if(!owner()||!verified)return false;
  scene_=0;active_=0;poisoned_=false;return true;
 }
};
// Composes all three independently prepared original presentation contexts.
// IO still needs real allocator and constructor provenance. No production
// caller or permissive default. Restore every entered component in reverse
// order; uncertain side effects permanently bar the other human lanes.
template<class IO,class AnimationIO=IO>class OriginalPresentationTransaction final {
 IO& io_;
 AnimationIO& animationIo_;
 OriginalPresentationArbiter& arbiter_;
 OriginalAnimationContext& animation_;
 OriginalWeaponSoundContext& sound_;
 OriginalEffectsContext& effects_;
 const OriginalSetupScene scope_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool acquired_=false,attempted_=false,poisoned_=false;
 bool matches(const OriginalSetupScene& s)const noexcept{
  return thread_==std::this_thread::get_id()&&s.nativeScene==scope_.nativeScene&&
    s.signedEpoch==scope_.signedEpoch&&s.peer==scope_.peer&&s.world==scope_.world&&
    s.p1Actor==scope_.p1Actor&&s.p1Owner==scope_.p1Owner&&
    s.ownedActor==scope_.ownedActor&&s.ownedOwner==scope_.ownedOwner;
 }
public:
  OriginalPresentationTransaction(IO& io,OriginalPresentationArbiter& arbiter,
     OriginalAnimationContext& animation,OriginalWeaponSoundContext& sound,
     OriginalEffectsContext& effects,const OriginalSetupScene& scope)
   :OriginalPresentationTransaction(io,io,arbiter,animation,sound,effects,scope){}
  OriginalPresentationTransaction(IO& io,AnimationIO& animationIo,OriginalPresentationArbiter& arbiter,
     OriginalAnimationContext& animation,OriginalWeaponSoundContext& sound,
     OriginalEffectsContext& effects,const OriginalSetupScene& scope)
   :io_(io),animationIo_(animationIo),arbiter_(arbiter),animation_(animation),sound_(sound),effects_(effects),scope_(scope){}
 OriginalPresentationTransaction(const OriginalPresentationTransaction&)=delete;
 OriginalPresentationTransaction& operator=(const OriginalPresentationTransaction&)=delete;
 bool CanEnterOriginal(const OriginalSetupScene& s){
  return matches(s)&&!attempted_&&!poisoned_&&io_.Scene()==s.nativeScene&&
    arbiter_.Available(s.nativeScene,s.peer)&&!animation_.Entered()&&
    !sound_.Entered()&&!effects_.Entered()&&!sound_.Quarantined()&&
      !effects_.Quarantined()&&animation_.CanExchange(animationIo_)&&sound_.CanExchange(io_)&&
    io_.OwnsCompletePresentation(s);
 }
 bool EnterOriginal(const OriginalSetupScene& s){
  if(!CanEnterOriginal(s))return false;
  if(!arbiter_.Acquire(s.nativeScene,s.peer))return false;
  acquired_=attempted_=true;
  // Exchange implementations do not call guest code. An exception is
  // treated as uncertain even if all observable flags remain unchanged.
  try{
    return animation_.Exchange(animationIo_)&&sound_.Exchange(io_)&&effects_.Exchange(io_);
  }catch(...){poisoned_=true;return false;}
 }
 bool OriginalContextActive(const OriginalSetupScene& s){
  return matches(s)&&!poisoned_&&acquired_&&arbiter_.Held(s.nativeScene,s.peer)&&
     io_.Scene()==s.nativeScene&&animation_.Entered()&&sound_.Entered()&&
     sound_.CanExchange(io_)&&effects_.Entered();
 }
 bool RestoreOriginal(const OriginalSetupScene& s){
  if(!matches(s))return false;
  if(!attempted_)return !poisoned_&&!acquired_&&!animation_.Entered()&&
    !sound_.Entered()&&!effects_.Entered();
  bool ok=!poisoned_;
  try{if(effects_.Entered()&&!effects_.Exchange(io_))ok=false;}catch(...){ok=false;}
  try{if(sound_.Entered()&&!sound_.Exchange(io_))ok=false;}catch(...){ok=false;}
   try{if(animation_.Entered()&&!animation_.Exchange(animationIo_))ok=false;}catch(...){ok=false;}
  ok=ok&&!animation_.Entered()&&!sound_.Entered()&&!effects_.Entered()&&
    !sound_.Quarantined()&&!effects_.Quarantined();
  if(!arbiter_.Release(s.nativeScene,s.peer,ok))ok=false;
  attempted_=acquired_=false;poisoned_=!ok;return ok;
 }
  bool OriginalP1Restored(const OriginalSetupScene& s){
   return matches(s)&&!poisoned_&&!attempted_&&!acquired_&&
     io_.Scene()==s.nativeScene&&!animation_.Entered()&&!sound_.Entered()&&sound_.CanExchange(io_)&&
    !effects_.Entered()&&arbiter_.Available(s.nativeScene,s.peer)&&
    io_.VerifyRestoredPresentation(s);
 }
};
// Weapon heaps follow black_p2_banks_make/take. The original Android pool
// constructor at 00152180 uses six sound buffers and two pairs of banks.
// P3/P4 cannot reuse the two PC P2 heap IDs (16,18). Each lane needs
// two independently verified unused original descriptors and its own
// backing memory. This planner does not write heap tables.
struct OriginalWeaponBankLayout {
 uint32_t weapon[2]{},sound[6]{},bank[4]{};
 static constexpr uint32_t WeaponBytes=0xE1800u,SoundBytes=0x18000u,
   BankBytes=0x38000u,SmallBankBytes=0xE000u,
   Total=2*WeaponBytes+6*SoundBytes+2*BankBytes+2*SmallBankBytes;
 static bool Build(uint32_t base,uint32_t size,OriginalWeaponBankLayout& out)noexcept{
  if(base<0x10000u||(base&0xFFFu)||size<Total||
     uint64_t(base)+Total>0x100000000ull)return false;
  OriginalWeaponBankLayout next{};
  uint32_t at=base;
  for(auto& p:next.weapon){p=at;at+=WeaponBytes;}
  for(auto& p:next.sound){p=at;at+=SoundBytes;}
  for(unsigned i=0;i<4;++i){
   next.bank[i]=at;
   at+=i<2?BankBytes:SmallBankBytes;
  }
  out=next;return true;
 }
};
class OriginalWeaponHeapReservations final {
 struct Entry{uint32_t first=0,second=0,base=0;uint64_t generation=0;};
 std::array<Entry,3> slots_{};
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t scene_=0;
public:
 template<class IO>bool Reserve(IO& io,uint8_t peer,uint32_t first,uint32_t second,
    uint32_t base,uint32_t size,OriginalWeaponBankLayout& layout){
  if(thread_!=std::this_thread::get_id()||peer<2||peer>4||
     first==second||first==13||first==14||second==13||second==14||
     slots_[peer-2].base||!io.Scene()||(scene_&&scene_!=io.Scene()))return false;
  OriginalWeaponBankLayout next{};
  if(!OriginalWeaponBankLayout::Build(base,size,next)||
     !io.VerifiedUnusedOriginalWeaponHeap(first)||
     !io.VerifiedUnusedOriginalWeaponHeap(second))return false;
  const auto gen=io.IndependentWeaponBackingGeneration(base,OriginalWeaponBankLayout::Total);
  if(!gen)return false;
  for(const auto& e:slots_)if(e.base){
   if(first==e.first||first==e.second||second==e.first||second==e.second||
      (uint64_t(base)<uint64_t(e.base)+OriginalWeaponBankLayout::Total&&
       uint64_t(e.base)<uint64_t(base)+OriginalWeaponBankLayout::Total))return false;
  }
  scene_=io.Scene();slots_[peer-2]={first,second,base,gen};layout=next;return true;
 }
 // Recheck lifetime and the still-unused descriptors immediately before
 // handing out the planned backing. No reservation alone grants writes.
 template<class IO>bool ReadyForConstruction(IO& io,uint8_t peer,
      OriginalWeaponBankLayout& layout)const{
  if(thread_!=std::this_thread::get_id()||peer<2||peer>4||
     !scene_||io.Scene()!=scene_)return false;
  const auto& e=slots_[peer-2];
  if(!e.base||!e.generation||
     io.IndependentWeaponBackingGeneration(e.base,OriginalWeaponBankLayout::Total)!=e.generation||
     !io.VerifiedUnusedOriginalWeaponHeap(e.first)||
     !io.VerifiedUnusedOriginalWeaponHeap(e.second))return false;
  return OriginalWeaponBankLayout::Build(e.base,OriginalWeaponBankLayout::Total,layout);
 }
};
// Real allocator backing owner for the source-reviewed weapon/sound
// layout. Allocator operations are supplied separately so the actual SDK
// allocator can be exercised without an original game executable.
// No destructor frees guest memory. Once handed to original constructors,
// engine teardown is necessary before the allocation can be released.
template<class Allocator>class OriginalWeaponBackingOwner final {
 Allocator& allocator_;
 const std::thread::id thread_=std::this_thread::get_id();
 uint32_t base_=0;uint64_t generation_=0;
 bool touched_=false;
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
public:
 explicit OriginalWeaponBackingOwner(Allocator& a):allocator_(a){}
 OriginalWeaponBackingOwner(const OriginalWeaponBackingOwner&)=delete;
 OriginalWeaponBackingOwner& operator=(const OriginalWeaponBackingOwner&)=delete;
 bool Live()const noexcept{
  return owner()&&base_&&generation_&&
    allocator_.Generation(base_,OriginalWeaponBankLayout::Total)==generation_;
 }
 bool Allocate(OriginalWeaponBankLayout& layout){
  if(!owner()||base_||touched_)return false;
  base_=allocator_.Allocate(OriginalWeaponBankLayout::Total);
  if(!base_)return false;
  generation_=allocator_.Generation(base_,OriginalWeaponBankLayout::Total);
  // Fail closed: preserve an uncertain allocation for inspection.
  if(!Live()||!OriginalWeaponBankLayout::Build(base_,OriginalWeaponBankLayout::Total,layout))return false;
  return true;
 }
 uint64_t Generation(uint32_t base,uint32_t size)const noexcept{
  return base==base_&&size==OriginalWeaponBankLayout::Total&&Live()?generation_:0;
 }
 bool MarkEngineTouched()noexcept{
  if(!Live()||touched_)return false;
  touched_=true;return true;
 }
 bool Release(bool originalTeardownVerified){
  if(!Live()||(touched_&&!originalTeardownVerified))return false;
  allocator_.Free(base_);
  if(allocator_.Generation(base_,OriginalWeaponBankLayout::Total))return false;
  base_=0;generation_=0;touched_=false;return true;
 }
};
// Detached original weapon-manager initialization from black_p2_banks_take.
// The caller supplies a fresh verified P1 snapshot and independently
// constructed banks. No live game writes, heap installation or audio calls.
class OriginalWeaponManagerImage final {
public:
 using Manager=std::array<uint8_t,0xD0u>;
 using Pool=std::array<uint8_t,0x1330u>;
private:
 static uint32_t get(const uint8_t* p)noexcept{
  uint32_t v=0;std::memcpy(&v,p,4u);return v;
 }
 static void put(uint8_t* p,uint32_t v)noexcept{std::memcpy(p,&v,4u);}
 static bool range(uint32_t p,uint32_t n)noexcept{
  return p>=0x10000u&&!(p&3u)&&uint64_t(p)+n<=0x100000000ull;
 }
 static bool overlap(uint32_t a,uint32_t n,uint32_t b,uint32_t m)noexcept{
  return uint64_t(a)<uint64_t(b)+m&&uint64_t(b)<uint64_t(a)+n;
 }
public:
 static bool Build(const Manager& donor,const Pool& pool,uint32_t firstManager,
   uint32_t extraManager,const std::array<uint32_t,2>& extraBanks,
   const std::array<uint32_t,2>& heaps,Manager& result)noexcept{
  if(!donor[0xC4u]||get(pool.data()+0x130Cu)!=9u||
     !range(firstManager,0xD0u)||!range(extraManager,0xD0u)||
     overlap(firstManager,0xD0u,extraManager,0xD0u)||
     heaps[0]>255u||heaps[1]>255u||heaps[0]==heaps[1]||
     heaps[0]==13u||heaps[0]==14u||heaps[1]==13u||heaps[1]==14u)return false;
  const std::array<uint32_t,2> firstBanks{get(pool.data()),get(pool.data()+0x10u)};
  for(unsigned i=0;i<2;i++){
   if(!range(firstBanks[i],0xC0u)||!range(extraBanks[i],0xC0u)||
      overlap(extraBanks[i],0xC0u,firstManager,0xD0u)||
      overlap(extraBanks[i],0xC0u,extraManager,0xD0u))return false;
   for(unsigned j=0;j<2;j++)if(overlap(extraBanks[i],0xC0u,firstBanks[j],0xC0u))return false;
  }
  if(overlap(extraBanks[0],0xC0u,extraBanks[1],0xC0u)||
     overlap(firstBanks[0],0xC0u,firstBanks[1],0xC0u))return false;
  Manager next=donor;
  for(uint32_t off:{0x7Cu,0x80u}){
   const auto v=get(next.data()+off);
   if(uint32_t(v-firstManager)<0xD0u)put(next.data()+off,extraManager+(v-firstManager));
   else if(v)return false; // no unexplained borrowed record pointer
  }
  for(unsigned k=0;k<2;k++){
   auto* rec=next.data()+8u+k*0x38u;
   const auto had=get(rec+0x10u);
   if(had&&had!=firstBanks[0]&&had!=firstBanks[1])return false;
    put(rec,heaps[k]);put(rec+4u,0);
    // The native manager constructor clears the two active sound pointers.
    // Detached pool ownership does not make the donor's voices shareable.
    put(rec+8u,0);put(rec+0xCu,0);put(rec+0x10u,0);
   for(unsigned j=0;j<2;j++)if(had==firstBanks[j])put(rec+0x10u,extraBanks[j]);
   put(rec+0x14u,0);put(rec+0x18u,0);put(rec+0x1Cu,0);
   std::memset(rec+0x20u,0,0x12u);
  }
  put(next.data()+0x78u,0);
  result=next;return true;
 }
};
// Exact nine-word descriptor initialization from pinned PC black_p2_banks_take.
// Only the reserved pair can be installed; original P1 heaps 13/14 never
// written. Native IO currently knows only source-reviewed P2 slots 16/18.
class OriginalWeaponHeapInstall final {
 const std::thread::id thread_=std::this_thread::get_id();
 uint64_t scene_=0,generation_=0;
 uint32_t base_=0,ids_[2]{};
 uint8_t peer_=0;
 bool installed_=false,uncertain_=false;
 static constexpr uint32_t table_=0x2D12ECu;
 using Words=std::array<uint32_t,9>;
 std::array<Words,2> before_{};
 static Words descriptor(uint32_t p)noexcept{
  return {1u,0u,p,p+OriginalWeaponBankLayout::WeaponBytes,p,4u,0u,0xFFFFFFFFu,p};
 }
 bool owner()const noexcept{return thread_==std::this_thread::get_id();}
public:
 OriginalWeaponHeapInstall()=default;
 OriginalWeaponHeapInstall(const OriginalWeaponHeapInstall&)=delete;
 OriginalWeaponHeapInstall& operator=(const OriginalWeaponHeapInstall&)=delete;
 template<class IO,class Backing>bool Install(IO& io,Backing& backing,
     uint8_t peer,uint32_t first,uint32_t second,
     const OriginalWeaponBankLayout& layout){
  if(!owner()||installed_||uncertain_||!io.Scene()||peer<2||peer>4||
     first==second||first==13||first==14||second==13||second==14||
     first>255||second>255)return false;
  OriginalWeaponBankLayout exact{};
  const auto base=layout.weapon[0];
  if(!OriginalWeaponBankLayout::Build(base,OriginalWeaponBankLayout::Total,exact)||
     std::memcmp(&exact,&layout,sizeof(exact))!=0)return false;
  const auto gen=backing.Generation(base,OriginalWeaponBankLayout::Total);
  if(!gen||!io.OwnsReservedHeapPair(peer,first,second,base,gen))return false;
  const uint32_t ids[]={first,second};
  std::array<uint8_t*,2> maps{};
  for(unsigned i=0;i<2;++i){
   if(!io.VerifiedUnusedOriginalWeaponHeap(ids[i]))return false;
   maps[i]=io.MapHeapDescriptor(table_+ids[i]*0x24u,0x24u);
   if(!maps[i])return false;
   std::memcpy(before_[i].data(),maps[i],0x24u);
   if(before_[i][0])return false;
  }
  if(maps[0]==maps[1]||!backing.MarkEngineTouched())return false;
  uncertain_=true;scene_=io.Scene();generation_=gen;base_=base;
   ids_[0]=first;ids_[1]=second;peer_=peer;
  for(unsigned i=0;i<2;++i){
   const auto words=descriptor(layout.weapon[i]);
   std::memcpy(maps[i],words.data(),0x24u);
  }
  installed_=true;
  if(!Live(io,backing)){installed_=false;return false;}
  uncertain_=false;return true;
 }
 template<class IO,class Backing>bool Live(IO& io,Backing& backing)const{
  if(!owner()||!installed_||io.Scene()!=scene_||
      backing.Generation(base_,OriginalWeaponBankLayout::Total)!=generation_||
      !io.OwnsReservedHeapPair(peer_,ids_[0],ids_[1],base_,generation_))return false;
  for(unsigned i=0;i<2;++i){
   auto* p=io.MapHeapDescriptor(table_+ids_[i]*0x24u,0x24u);
   if(!p)return false;
   Words words{};std::memcpy(words.data(),p,0x24u);
   const auto at=base_+i*OriginalWeaponBankLayout::WeaponBytes;
   // Cursor/usage may evolve during original loads. Check ownership fields.
    if(words[0]!=1u||words[2]!=at||words[3]!=at+OriginalWeaponBankLayout::WeaponBytes||
       words[4]<at||words[4]>words[3]||
       words[8]!=at)return false;
  }
  return true;
 }
 bool Quarantined()const noexcept{return owner()&&uncertain_;}
 // No shallow undo: original weapon and sound teardown must be proven
 // before any descriptor or backing may be reclaimed.
};
} // namespace blackcoop
