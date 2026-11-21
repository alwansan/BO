#pragma once
#include <cstdint>
struct X86Context;
namespace game {
bool CoopTitleLabel(X86Context& ctx, uint32_t clip);
void RequestCoopRoom(bool mission_title);
}
