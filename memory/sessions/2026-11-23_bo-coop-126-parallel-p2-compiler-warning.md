# Session126 — P2 parallel native test compile fix

### Session126 (2026-11-23): repair P2 parallel CI compiler warning without relaxing regression gates

- New concurrent workflow 45251920286 executed all THREE source tracks simultaneously: P2 native actor/camera/physics job FAILED only because GCC `-Werror=range-loop-construct` on an existing synthetic test structured-binding copy (`test_room_original_host_proxy_engine.cpp` line 21). P3/P4 TLS/control job SUCCESS; canonical host NPC/avatar/object story job SUCCESS. The failure was a compiler warning in test fixture, NOT original engine crash or broken game physics.
- Fixed exactly `for(const auto [a,serial,col]:` -> `for(const auto& [a,serial,col]:` in synthetic host proxy test, retaining `-Wall -Wextra -Werror -pedantic -fsanitize=address,undefined` and all original assertions. Re-run should now verify P2 lane; do not call it successful until results are checked. The existing source transform 45251338131 SUCCESS and UDP 45251338270 SUCCESS remain genuine. Android build 45251338108 still must be inspected separately.
- Owner instruction remains ALL concurrent: actual original Android P2/P3/P4 engine constructor/setup and safe teardown, distinct camera/animation/FX/audio/weapon/physics/AI/visual model, native Xbox ports, and original host enemy/door/objective state applied in guest engine. Runtime remains incomplete. Only `experiment/black-coop` write, preserve main/CL/biso/ISO/XBE/saves/private logs; no microtests.

Existing original proxy fixture used structured-bindings by value; new GCC warning-as-error catches unnecessary tuple copy. `const auto&` fixes the warning without changing the tested original engine semantics.
