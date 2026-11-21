// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
namespace bo_probe {
void ObservePlayer(uint32_t object,const uint8_t* base) noexcept;
void ObserveAnimation(uint32_t stack,const uint8_t* base) noexcept;
void ObserveLod(uint32_t stack,const uint8_t* base) noexcept;
}
