// Copyright (c) 2026 portingpete
// Copyright (c) 2026 sp00nz
// Copyright (c) 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Adapted from Black-2006-Recomp b2512fafd3 black_p2_vm_take/black_p2_vm.
// See room_original_animation_context.LICENSE. No original game bytes included.
#pragma once
#include <algorithm>
#include <array>
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
  uint32_t pool_ = 0, cacheData_ = 0, cacheDataBytes_ = 0;
  Piece emptyCache_, workspace_;
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
        overlaps(p, n, cache_, static_cast<uint32_t>(emptyCache_.kept.size())))
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
    return read(io, cache_ + 0x20, value) && value == cacheData_ &&
           read(io, cache_ + 0x24, value) && value == cacheDataBytes_;
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
    uint32_t global = 0, data = 0, size = 0;
    if (!read(io, kCache, global) || global != cache || !map(io, cache, 0x30) ||
        !read(io, cache + 0x20, data) || !read(io, cache + 0x24, size))
      return false;
    const uint64_t end = uint64_t(data) + size;
    if (data < cache || !size || end <= cache || end - cache > 0x10000 ||
        end > 0x100000000ull ||
        rootOverlap(cache, static_cast<uint32_t>(end - cache)) ||
        overlaps(cache, static_cast<uint32_t>(end - cache), kWork, kWorkBytes))
      return false;
    Piece next;
    if (!take(io, cache, static_cast<uint32_t>(end - cache), next))
      return false;
    scene_ = io.Scene();
    cache_ = cache;
    cacheData_ = data;
    cacheDataBytes_ = size;
    emptyCache_ = std::move(next);
    return true;
  }
  template <class IO> bool Capture(IO &io) {
    if (!owner() || captured_ || !scene_ || scene_ != io.Scene() ||
        emptyCache_.kept.empty() || !live(io, emptyCache_))
      return false;
    uint32_t global = 0;
    if (!read(io, kCache, global) || global != cache_ ||
        !read(io, kSet, set_) || !map(io, set_, 0x970) ||
        !read(io, 0x2D1980, pool_) || !map(io, pool_, 0x8540))
      return false;
    if (rootOverlap(set_, 0x970) ||
        overlaps(set_, 0x970, cache_,
                 static_cast<uint32_t>(emptyCache_.kept.size())) ||
        overlaps(set_, 0x970, pool_, 0x8540) ||
        overlaps(set_, 0x970, kWork, kWorkBytes) ||
        overlaps(pool_, 0x8540, kWork, kWorkBytes) ||
        overlaps(pool_, 0x8540, cache_,
                 static_cast<uint32_t>(emptyCache_.kept.size())))
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
        if (!bind(io, slot + off, address) || !add(io, address, 0xA0))
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
        overlaps(table_, count_ * 4, cache_,
                 static_cast<uint32_t>(emptyCache_.kept.size())) ||
        overlaps(tops_, count_ * 4, cache_,
                 static_cast<uint32_t>(emptyCache_.kept.size())))
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
          top < at || uint64_t(top) > uint64_t(at) + bytes_)
        return false;
      if (rootOverlap(at, bytes_) || overlaps(at, bytes_, set_, 0x970) ||
          overlaps(at, bytes_, kWork, kWorkBytes) ||
          overlaps(at, bytes_, cache_,
                   static_cast<uint32_t>(emptyCache_.kept.size())) ||
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
    if (!owner() || !captured_ || !shape(io) || !live(io, emptyCache_) ||
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
    auto *cache = live(io, emptyCache_);
    auto *tops = map(io, tops_, count_ * 4);
    if (!work || !cache || !tops ||
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
    std::memcpy(cache, emptyCache_.kept.data(), emptyCache_.kept.size());
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
} // namespace blackcoop
