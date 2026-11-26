#include "room_mod_human_presentation.hpp"
#include <cassert>
#include <iostream>
int main() {
  using namespace blackcoop;
  std::array<int32_t, 34> f{
      1,   90,  2,      0,          1, -1,  0x12345678, -2,  15,    1, 7, 3,
      0,   1,   -1,     0x12345678, 2, 100, 200,        300, 4500,  0, 3, 400,
      500, 600, -18000, 1,          4, 700, 800,        900, 18000, 3};
  ModHumanPresentation p{};
  assert(DecodeModHumanPresentation(f.data(), f.size(), 90, 2, 0, p));
  assert(p.revision == 0xffffffffu && p.tokenLo == 0xfffffffeu &&
         p.session[2] == 0xffffffffu);
  assert(p.count == 3 && p.bodies[2].peer == 4 && p.bodies[0].y == 200);
  const auto original = p;
  auto bad = [&](size_t at, int32_t value) {
    auto x = f;
    x[at] = value;
    assert(!DecodeModHumanPresentation(x.data(), x.size(), 90, 2, 0, p));
    assert(SameModHumanTicket(p, original) && p.bodies[0].x == 100);
  };
  bad(0, 0);
  bad(1, 91);
  bad(2, 3);
  bad(3, 1);
  bad(4, 0);
  bad(5, 0);
  bad(6, -1);
  bad(8, 7);
  bad(9, 5);
  bad(10, 0);
  bad(11, 2);
  bad(16, 1);
  bad(22, 2);
  bad(17, 20000001);
  bad(20, 18001);
  bad(21, 4);
  auto zeroToken = f;
  zeroToken[6] = zeroToken[7] = 0;
  assert(!DecodeModHumanPresentation(zeroToken.data(), 34, 90, 2, 0, p));
  assert(!DecodeModHumanPresentation(f.data(), 33, 90, 2, 0, p));
  assert(!DecodeModHumanPresentation(nullptr, 34, 90, 2, 0, p));
  assert(!DecodeModHumanPresentation(f.data(), 34, 91, 2, 0, p));
  auto other = original;
  other.session[3] ^= 1;
  assert(!SameModHumanTicket(other, original));
  other = original;
  other.tokenLo ^= 1;
  assert(!SameModHumanTicket(other, original));
  other = original;
  other.originalEpisode++;
  assert(!SameModHumanTicket(other, original));
  other = original;
  other.tick++;
  assert(SameModHumanTicket(other, original));
  for (int self = 1; self <= 4; self++) {
    auto guest = f;
    guest[9] = self;
    int next = 1;
    for (int i = 0; i < 3; i++) {
      if (next == self)
        next++;
      guest[16 + 6 * i] = next++;
    }
    assert(DecodeModHumanPresentation(guest.data(), 34, 90, 2, 0, p) &&
           p.self == uint32_t(self));
  }
  for (int total = 2; total <= 3; total++) {
    auto smaller = f;
    smaller[8] = (1 << total) - 1;
    smaller[11] = total - 1;
    assert(DecodeModHumanPresentation(smaller.data(), 16 + 6 * (total - 1), 90,
                                      2, 0, p));
    assert(p.count == uint32_t(total - 1));
  }
  std::cout << "MOD_HUMAN_PRESENTATION_DECODE_PASS full identity and original "
               "scene; no gameplay\n";
}
