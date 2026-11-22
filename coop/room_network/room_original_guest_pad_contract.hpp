// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// SOURCE: pinned original Xbox Android SDK TGP482/XboxXBERecomp@99078b1,
// runtime/core/input.h::Pad, input.cpp::Poll(0) SDL gamepad report.
// Native on the ORIGINAL BLACK player game thread; no original guest writes.
#pragma once
#include <cstdint>
namespace blackcoop {
struct OriginalGuestPadCommand {
    int32_t moveX=0,moveY=0;
    bool fire=false,jump=false;
};
inline int32_t QuantizeOriginalPadAxis(int32_t raw)noexcept{
    if(raw< -32768||raw>32767)return 0;
    // Avoid phantom movement from ordinary stick drift.
    if(raw>=-4096&&raw<=4096)return 0;
    const int32_t scaled=raw*1000/32768;
    return scaled< -1000?-1000:scaled>1000?1000:scaled;
}
inline OriginalGuestPadCommand QuantizeOriginalGuestPad(
    bool connected,int32_t lx,int32_t ly,
    uint8_t a,uint8_t rightTrigger)noexcept{
    if(!connected)return {};
    return {QuantizeOriginalPadAxis(lx),
            QuantizeOriginalPadAxis(ly),
            rightTrigger>=128,a>=128};
}
} // namespace blackcoop
