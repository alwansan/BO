// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original BLACK local-player eye vector from pinned upstream hooks.cpp:
// player+0x100, floating XYZ (camera eye). This is NOT actor root transform,
// scene-load proof, quest state or a rendered remote bot.
#pragma once
#include <cmath>
#include <cstdint>

namespace blackcoop {
struct OriginalEyeCm {
    int32_t x=0,y=0,z=0;
};
inline bool QuantizeOriginalEye(float x,float y,float z,
                               OriginalEyeCm& result) noexcept {
    const auto safe=[](float n)noexcept{
        return std::isfinite(n)&&std::fabs(n)<=200000.0f;
    };
    if(!safe(x)||!safe(y)||!safe(z))return false;
    const double cx=std::round(static_cast<double>(x)*100.0);
    const double cy=std::round(static_cast<double>(y)*100.0);
    const double cz=std::round(static_cast<double>(z)*100.0);
    if(std::fabs(cx)>20000000.0||std::fabs(cy)>20000000.0||
       std::fabs(cz)>20000000.0)return false;
    result={static_cast<int32_t>(cx),static_cast<int32_t>(cy),
            static_cast<int32_t>(cz)};
    return true;
}
} // namespace blackcoop
