#include "room_original_animation_context.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
using namespace blackcoop;
struct IO {
  std::vector<uint8_t> m = std::vector<uint8_t>(0x700000);
  uint64_t scene = 7, generation = 1;
  uint32_t blocked = 0;
  bool OwnsCompletePresentation(const OriginalSetupScene& s)const {return s.nativeScene==scene;}
  bool VerifyRestoredPresentation(const OriginalSetupScene& s)const {return s.nativeScene==scene;}
  bool VerifiedUnusedOriginalWeaponHeap(uint32_t id)const{return id==16||id==18||id==20||id==21||id==22||id==23;}
  uint64_t IndependentWeaponBackingGeneration(uint32_t,uint32_t)const{return generation;}
  bool OwnsReservedHeapPair(uint8_t p,uint32_t a,uint32_t b,uint32_t base,uint64_t g)const{
    return p==2&&a==16&&b==18&&base==0x1000000u&&g==1;
  }
  uint8_t* MapHeapDescriptor(uint32_t p,uint32_t n){return Map(p,n);}
  uint64_t Scene() const { return scene; }
  bool OwnsEffectsActorPair(uint32_t first,uint32_t extra) const {
    return first==0x100030u&&extra==0x180000u;
  }
  bool OwnsLiveWeaponSoundContext(uint32_t first,uint32_t extra,
      uint32_t firstPool,uint32_t extraPool) const {
    return first==0x180000u&&extra==0x181000u&&firstPool==0x190000u&&extraPool==0x194000u;
  }
  uint64_t Generation(uint32_t, uint32_t) const { return generation; }
  uint8_t *Map(uint32_t p, uint32_t n) {
    return p >= 0x10000 && uint64_t(p) + n <= m.size() &&
                   !(blocked && p <= blocked && uint64_t(p) + n > blocked)
               ? m.data() + p
               : nullptr;
  }
  void w(uint32_t p, uint32_t v) { std::memcpy(m.data() + p, &v, 4); }
  uint32_t r(uint32_t p) {
    uint32_t v;
    std::memcpy(&v, m.data() + p, 4);
    return v;
  }
  IO() {
    w(0x2D12C8, 0x300000);
    w(0x2CE6B0, 0x600000);
    w(0x600020, 0x600040);
    w(0x600024, 0x80);
    w(0x2D1980, 0x400000);
    const uint32_t s = 0x300000;
    w(s + 0x8FC, 0x330000);
    w(s + 0x900, 0x330100);
    w(s + 0x908, 0x100);
    w(s + 0x90C, 4);
    for (unsigned i = 0; i < 4; i++) {
      w(0x330000 + i * 4, 0x500000 + i * 0x1000);
      w(0x330100 + i * 4, 0x500020 + i * 0x1000);
      std::memset(m.data() + 0x500000 + i * 0x1000, int(10 + i), 0x100);
    }
    for (unsigned k = 0; k < 2; k++) {
      const auto slot = s + 0x470 + k * 0x240;
      for (unsigned i = 0; i < 8; i++)
        w(slot + 0x30 + i * 4, 0x310000 + (k * 8 + i) * 0x100);
      w(slot + 0x54, 0x320000 + k * 0x1000);
      w(slot + 0x58, 99);
      w(slot + 0xAC, k);
      w(slot + 0x90, 88);
      w(0x320004 + k * 0x1000, 77);
      w(s + 0x398 + k * 0x6C + 0xC, 66);
    }
    w(0x4000A0 + 0x328, 0x340000);
    w(0x340000 + 0xAC, 2);
  }
};
int main() {
  IO io;
  OriginalAnimationContext c;
  assert(!c.Capture(io));
  assert(c.ObserveOriginalCacheConstructor(io, 0x600000));
  auto before = io.m;
  assert(c.Capture(io));
  assert(io.m == before);
  io.w(0x600080, 123);
  assert(c.Exchange(io));
  assert(c.Entered());
  assert(io.r(0x300000 + 0x470 + 0x58) == 0);
  assert(io.r(0x320004) == 0);
  assert(io.r(0x600080) == 0);
  assert(io.r(0x300398 + 0xC) == 0);
  io.w(0x500000, 0xabcdef);
  io.w(0x502000, 0x123456);
  io.w(0x320010, 0x998877);
  assert(c.Exchange(io));
  assert(!c.Entered());
  assert(io.r(0x300000 + 0x470 + 0x58) == 99);
  assert(io.r(0x320004) == 77);
  assert(io.r(0x500000) == 0x0a0a0a0a);
  assert(io.r(0x502000) == 0x123456);
  assert(c.Exchange(io));
  assert(io.r(0x500000) == 0xabcdef);
  assert(io.r(0x320010) == 0x998877);
  assert(c.Exchange(io));
  // A block reused by a real NPC is never swapped, including late ownership.
  io.w(0x340000 + 0xAC, 0);
  before = io.m;
  assert(!c.Exchange(io));
  assert(io.m == before);
  io.w(0x340000 + 0xAC, 2);
  io.w(0x330100, 0x500101);
  before = io.m;
  assert(!c.Exchange(io));
  assert(io.m == before);
  io.w(0x330100, 0x500020);
  io.blocked = 0x320000;
  before = io.m;
  assert(!c.Exchange(io));
  assert(io.m == before);
  io.blocked = 0;
  ++io.generation;
  before = io.m;
  assert(!c.Exchange(io));
  assert(io.m == before);
  --io.generation;
  ++io.scene;
  before = io.m;
  assert(!c.Exchange(io));
  assert(io.m == before);
  --io.scene;
  bool result = true;
  std::thread t([&] { result = c.Exchange(io); });
  t.join();
  assert(!result);
  IO overlap;
  overlap.w(0x300000 + 0x470 + 0x30, 0x300400);
  OriginalAnimationContext bad;
  assert(bad.ObserveOriginalCacheConstructor(overlap, 0x600000));
  before = overlap.m;
  assert(!bad.Capture(overlap));
  assert(overlap.m == before);
  IO npcAlias;
  npcAlias.w(0x300000 + 0x470 + 0x30, 0x400200);
  OriginalAnimationContext npcBad;
  assert(npcBad.ObserveOriginalCacheConstructor(npcAlias, 0x600000));
  before = npcAlias.m;
  assert(!npcBad.Capture(npcAlias));
  assert(npcAlias.m == before);
  IO shape;
  OriginalAnimationContext shapeCtx;
  assert(shapeCtx.ObserveOriginalCacheConstructor(shape, 0x600000));
  assert(shapeCtx.Capture(shape));
  shape.w(0x300000 + 0x470 + 0x54, 0x350000);
  before = shape.m;
  assert(!shapeCtx.Exchange(shape));
  assert(shape.m == before);
  IO cache;
  OriginalAnimationContext cacheCtx;
  assert(cacheCtx.ObserveOriginalCacheConstructor(cache, 0x600000));
  assert(cacheCtx.Capture(cache));
  cache.w(0x600024, 0x84);
  before = cache.m;
  assert(!cacheCtx.Exchange(cache));
  assert(cache.m == before);
  // Aliasing untouched viewmodel metadata is still unsafe: cache/table/tops/
  // pool exchange must not overwrite ANY part of the owning set.
  for (unsigned kind = 0; kind < 5; kind++) {
    IO alias;
    uint32_t cacheAddress = 0x600000;
    if (kind == 0) {
      cacheAddress = 0x300040;
      alias.w(0x2CE6B0, cacheAddress);
      alias.w(cacheAddress + 0x20, cacheAddress + 0x40);
      alias.w(cacheAddress + 0x24, 0x80);
    }
    if (kind == 1) {
      for (unsigned i = 0; i < 4; i++)
        alias.w(0x300040 + i * 4, alias.r(0x330000 + i * 4));
      alias.w(0x3008FC, 0x300040);
    }
    if (kind == 2) {
      for (unsigned i = 0; i < 4; i++)
        alias.w(0x300040 + i * 4, alias.r(0x330100 + i * 4));
      alias.w(0x300900, 0x300040);
    }
    if (kind == 3) {
      alias.w(0x330000, 0x300100);
      alias.w(0x330100, 0x300120);
    }
    if (kind == 4)
      alias.w(0x300000 + 0x470 + 0x30, 0x300040);
    OriginalAnimationContext rejected;
    assert(rejected.ObserveOriginalCacheConstructor(alias, cacheAddress));
    before = alias.m;
    assert(!rejected.Capture(alias));
    assert(alias.m == before);
  }
  // A mapped controller can straddle fixed original root words. Its shadow
  // controller+4 clear must never become a write to the live viewmodel root.
  for (uint32_t root : {0x2D12C8u, 0x2D1980u, 0x2D45D4u}) {
    IO alias;
    alias.w(0x300000 + 0x470 + 0x54, root - 4u);
    OriginalAnimationContext rejected;
    assert(rejected.ObserveOriginalCacheConstructor(alias, 0x600000));
    before = alias.m;
    assert(!rejected.Capture(alias));
    assert(alias.m == before);
  }
  // Every other dynamic write region has the same fixed-root exclusion.
  for (unsigned kind = 0; kind < 4; ++kind) {
    IO alias;
    OriginalAnimationContext rejected;
    if (kind == 0) {
      const uint32_t address = 0x2D45D4u - 0x80u;
      alias.w(0x2CE6B0, address);
      alias.w(address + 0x20, address + 0x40);
      alias.w(address + 0x24, 0x80);
      before = alias.m;
      assert(!rejected.ObserveOriginalCacheConstructor(alias, address));
      assert(alias.m == before);
      continue;
    }
    if (kind == 1) {
      alias.w(0x330000, 0x2D45D4u - 0x20u);
      alias.w(0x330100, 0x2D45D4u + 0x10u);
    }
    if (kind == 2) {
      for (unsigned i = 0; i < 4; ++i)
        alias.w(0x2D45D4u + i * 4u, alias.r(0x330100 + i * 4u));
      alias.w(0x300900, 0x2D45D4u);
    }
    if (kind == 3) {
      for (unsigned i = 0; i < 4; ++i)
        alias.w(0x2D45D4u + i * 4u, alias.r(0x330000 + i * 4u));
      alias.w(0x3008FC, 0x2D45D4u);
    }
    assert(rejected.ObserveOriginalCacheConstructor(alias, 0x600000));
    before = alias.m;
    assert(!rejected.Capture(alias));
    assert(alias.m == before);
  }
  {
    IO fx;OriginalEffectsContext effects;
    constexpr uint32_t base=0x100000u,at=base+0x65690u,
        flash=base+0x4E1F0u+0xC0u,shell=base+0x506D0u;
    fx.w(0x2D1990u,base);
    fx.w(at+0x3440u,0x190000u);fx.w(at+0x3444u,0x191000u);
    fx.w(at+0x3448u,0x00500002u);fx.w(at+0x3458u,2u);
    fx.w(at+0x2300u,0x100030u);fx.w(at+0x2304u,0x180000u);
    fx.w(at+0x2308u,0x200000u);
    fx.w(at+0x26C0u+0x44u,0x100030u);
    fx.w(at+0x26C0u+0x60u+0x44u,0x200000u);
    fx.w(flash,11u);fx.w(shell,22u);
    assert(effects.Bind(fx,0x100030u,0x180000u));
    assert(effects.Exchange(fx)&&effects.Entered());
    assert(fx.r(at+0x2300u)==0x180000u&&fx.r(at+0x2304u)==0x100030u);
    assert(fx.r(at+0x2308u)==0x200000u);
    assert(fx.r(at+0x26C0u+0x44u)==0x180000u);
    assert(fx.r(at+0x26C0u+0x60u+0x44u)==0x200000u);
    assert(fx.r(flash)==0&&fx.r(shell)==0);
    fx.w(flash,33u);fx.w(shell,44u);
    assert(effects.Exchange(fx)&&!effects.Entered());
    assert(fx.r(flash)==11u&&fx.r(shell)==22u);
    assert(effects.Exchange(fx)&&fx.r(flash)==33u&&fx.r(shell)==44u);
    fx.w(at+0x3458u,33u);before=fx.m;
    assert(!effects.Exchange(fx)&&effects.Quarantined()&&fx.m==before);
  }
  {
    IO snd;OriginalWeaponSoundContext sound;
    snd.w(0x2D12D8u,0x180000u);snd.w(0x2D0D60u,0x100000u);
    snd.w(0x100000u+0xD3C8u,0x170000u);snd.w(0x17000Cu,0x190000u);
    snd.w(0x190000u,11u);
    snd.w(0x194000u,44u);
    assert(!sound.Bind(snd,0x181000u,0x190000u));
    assert(!sound.Bind(snd,0x181000u,0x195000u)); // unproven pool
    assert(sound.Bind(snd,0x181000u,0x194000u));
    before=snd.m;
    snd.blocked=0x17000Cu;
    assert(!sound.CanExchange(snd)&&!sound.Exchange(snd)&&snd.m==before);
    snd.blocked=0;
    assert(sound.Exchange(snd)&&sound.Entered());
    assert(snd.r(0x2D12D8u)==0x181000u&&snd.r(0x17000Cu)==0x194000u);
    assert(snd.r(0x190000u)==11u&&snd.r(0x194000u)==44u);
    snd.w(0x194000u,55u);
    assert(sound.Exchange(snd)&&!sound.Entered());
    assert(snd.r(0x2D12D8u)==0x180000u&&snd.r(0x17000Cu)==0x190000u);
    assert(snd.r(0x190000u)==11u&&snd.r(0x194000u)==55u);
    assert(sound.Exchange(snd)&&snd.r(0x17000Cu)==0x194000u);
    ++snd.generation;before=snd.m;
    assert(!sound.Exchange(snd)&&sound.Quarantined()&&snd.m==before);
  }
  {
    IO io;OriginalAnimationContext anim;OriginalWeaponSoundContext sound;
    OriginalEffectsContext effects;OriginalPresentationArbiter arbiter;
    assert(anim.ObserveOriginalCacheConstructor(io,0x600000)&&anim.Capture(io));
    io.w(0x2D12D8u,0x180000u);io.w(0x2D0D60u,0x100000u);
    io.w(0x100000u+0xD3C8u,0x170000u);io.w(0x17000Cu,0x190000u);
    assert(sound.Bind(io,0x181000u,0x194000u));
    constexpr uint32_t at=0x100000u+0x65690u;
    io.w(0x2D1990u,0x100000u);
    io.w(at+0x3440u,0x192000u);io.w(at+0x3444u,0x193000u);
    assert(effects.Bind(io,0x100030u,0x180000u));
    OriginalSetupScene scope{7,1,0x100000u,0x100030u,0x120000u,0x180000u,0x181000u,2};
    OriginalPresentationTransaction tx(io,arbiter,anim,sound,effects,scope);
    assert(tx.CanEnterOriginal(scope)&&tx.EnterOriginal(scope)&&tx.OriginalContextActive(scope));
    assert(!arbiter.Acquire(7,3)&&!arbiter.Acquire(7,4));
    assert(tx.RestoreOriginal(scope)&&tx.OriginalP1Restored(scope));
    assert(arbiter.Acquire(7,3)&&arbiter.Release(7,3,true));
    // Failed FX entry after animation/audio entry must unwind both.
    io.w(at+0x3458u,33u);
    assert(!tx.EnterOriginal(scope));
    assert(tx.RestoreOriginal(scope)&&tx.OriginalP1Restored(scope));
    io.w(at+0x3458u,0u);
    assert(tx.EnterOriginal(scope));
    ++io.generation;
    assert(!tx.RestoreOriginal(scope)&&!arbiter.Available(7,4));
    assert(!arbiter.AfterVerifiedEngineTeardown(false));
  }
  {
    IO io;OriginalWeaponHeapReservations heaps;OriginalWeaponBankLayout p2{},p3{},p4{};
    constexpr auto bytes=OriginalWeaponBankLayout::Total;
    assert(!heaps.Reserve(io,2,13,18,0x1000000u,bytes,p2));
    assert(heaps.Reserve(io,2,16,18,0x1000000u,bytes,p2));
    assert(p2.weapon[0]==0x1000000u&&p2.weapon[1]==p2.weapon[0]+OriginalWeaponBankLayout::WeaponBytes);
    assert(p2.bank[1]+OriginalWeaponBankLayout::BankBytes==0x1000000u+bytes);
    assert(!heaps.Reserve(io,3,16,20,0x2000000u,bytes,p3));
    assert(!heaps.Reserve(io,3,20,21,0x1001000u,bytes,p3));
    assert(heaps.Reserve(io,3,20,21,0x2000000u,bytes,p3));
    assert(heaps.Reserve(io,4,22,23,0x3000000u,bytes,p4));
    assert(heaps.ReadyForConstruction(io,2,p2)&&heaps.ReadyForConstruction(io,3,p3));
    ++io.generation;
    assert(!heaps.ReadyForConstruction(io,2,p2)&&!heaps.ReadyForConstruction(io,4,p4));
  }
  {
    IO io;OriginalWeaponHeapInstall install;
    struct Backing {
      bool touched=false;
      uint64_t Generation(uint32_t p,uint32_t n)const{
       return p==0x1000000u&&n==OriginalWeaponBankLayout::Total?1u:0u;
      }
      bool MarkEngineTouched(){if(touched)return false;touched=true;return true;}
    } backing;
    OriginalWeaponBankLayout layout{};
    assert(OriginalWeaponBankLayout::Build(0x1000000u,OriginalWeaponBankLayout::Total,layout));
    constexpr uint32_t a=0x2D12ECu+16u*0x24u,b=0x2D12ECu+18u*0x24u;
    io.w(b,1);before=io.m;
    assert(!install.Install(io,backing,2,16,18,layout)&&!backing.touched&&io.m==before);
    io.w(b,0);
    assert(install.Install(io,backing,2,16,18,layout)&&backing.touched&&install.Live(io,backing));
    assert(io.r(a+8)==layout.weapon[0]&&io.r(b+8)==layout.weapon[1]);
    assert(io.r(a+0xC)==layout.weapon[0]+OriginalWeaponBankLayout::WeaponBytes);
    io.w(a+0x10,layout.weapon[0]+16u);assert(install.Live(io,backing));
    io.w(b+8,0);assert(!install.Live(io,backing));
  }
  {
    OriginalWeaponManagerImage::Manager donor{},out{};
    OriginalWeaponManagerImage::Pool pool{};
    auto put=[](auto& image,unsigned off,uint32_t v){std::memcpy(image.data()+off,&v,4u);};
    auto get=[](const auto& image,unsigned off){uint32_t v=0;std::memcpy(&v,image.data()+off,4u);return v;};
    const uint32_t first=0x180000u,extra=0x181000u;
    const std::array<uint32_t,2> banks{0x1B0000u,0x1B1000u},heaps{16u,18u};
    donor[0xC4u]=1;
    put(pool,0,0x190000u);put(pool,0x10u,0x191000u);put(pool,0x130Cu,9);
    put(donor,0x7Cu,first+8);put(donor,0x80u,first+0x40u);
    put(donor,0x18u,0x190000u);put(donor,0x50u,0x191000u);
    put(donor,0xCu,77);put(donor,0x24u,99);put(donor,0x28u,88);
    const auto saved=donor;
    assert(OriginalWeaponManagerImage::Build(donor,pool,first,extra,banks,heaps,out));
    assert(donor==saved);
    assert(get(out,8)==16u&&get(out,0x40u)==18u);
    assert(get(out,0x18u)==banks[0]&&get(out,0x50u)==banks[1]);
    assert(get(out,0x7Cu)==extra+8&&get(out,0x80u)==extra+0x40u);
    assert(get(out,0xCu)==0&&get(out,0x24u)==0&&get(out,0x28u)==0);
    const auto ready=out;
    assert(!OriginalWeaponManagerImage::Build(donor,pool,first,extra,{banks[0],banks[0]},heaps,out)&&out==ready);
    put(donor,0x18u,0x199000u);
    assert(!OriginalWeaponManagerImage::Build(donor,pool,first,extra,banks,heaps,out)&&out==ready);
  }
  std::cout
      << "ORIGINAL_ANIMATION_CONTEXT_PASS: real memory exchange, original "
         "cache reset, resource separation and preflight atomicity\n";
}
