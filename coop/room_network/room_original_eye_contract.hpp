// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original BLACK local-player eye vector from pinned upstream hooks.cpp:
// player+0x100, floating XYZ (camera eye). Separate original body-feet
// source +0xA0 is from MIT PC port black_puppet_follow (pinned b2512fafd).
// Actual original actor forward aiming vector is player+0xF0..+0xFB:
// pinned original RanggaCasper/Black-Android hooks.cpp kPlayerForward.
// Heading is an ORIGINAL scene value, not a gamepad right-stick angle.
// Neither indicates engine-loaded scene, quest or rendered remote actor.
#pragma once
#include <cmath>
#include <cstdint>

namespace blackcoop {
struct OriginalEyeCm {
    int32_t x=0,y=0,z=0;
};
struct OriginalBodyFeetCm {
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
// BODY orientation from the ORIGINAL actor's gun/aim forward vector.
// Horizontal vector angles use signed centidegrees [-18000,18000]. No yaw
// is invented from the Xbox stick or from a remote packet's raw address.
inline bool QuantizeOriginalForwardYaw(float forwardX,float forwardZ,
                                      int32_t& yawCentiDeg)noexcept{
    if(!std::isfinite(forwardX)||!std::isfinite(forwardZ)||
       std::fabs(forwardX)>2.0f||std::fabs(forwardZ)>2.0f)return false;
    const double norm=static_cast<double>(forwardX)*forwardX+
                      static_cast<double>(forwardZ)*forwardZ;
    if(norm<0.0025||norm>4.0)return false;
    constexpr double kRadToCentiDeg=18000.0/3.14159265358979323846;
    const double angle=std::round(std::atan2(
        static_cast<double>(forwardX),static_cast<double>(forwardZ))*kRadToCentiDeg);
    if(!std::isfinite(angle)||angle< -18000.0||angle>18000.0)return false;
    yawCentiDeg=static_cast<int32_t>(angle);return true;
}
// Apply a delta between TWO source-verified headings to the original
// soldier's horizontal basis. Leaves pitch/y untouched; not an NPC actor.
inline bool RotateOriginalSoldierHorizontalBasis(
    float x,float z,int32_t originalYaw,int32_t remoteYaw,
    float& outputX,float& outputZ)noexcept{
    if(!std::isfinite(x)||!std::isfinite(z)||
       std::fabs(x)>4.f||std::fabs(z)>4.f||
       originalYaw< -18000||originalYaw>18000||
       remoteYaw< -18000||remoteYaw>18000)return false;
    constexpr double kCentiDegToRad=3.14159265358979323846/18000.0;
    const double delta=(static_cast<double>(remoteYaw)-originalYaw)*kCentiDegToRad;
    const double co=std::cos(delta),si=std::sin(delta);
    const double nx=co*static_cast<double>(x)+si*static_cast<double>(z);
    const double nz=co*static_cast<double>(z)-si*static_cast<double>(x);
    if(!std::isfinite(nx)||!std::isfinite(nz))return false;
    outputX=static_cast<float>(nx);outputZ=static_cast<float>(nz);return true;
}
inline bool QuantizeOriginalBodyFeet(float x,float y,float z,OriginalBodyFeetCm& result)noexcept{
    OriginalEyeCm valid{};
    if(!QuantizeOriginalEye(x,y,z,valid))return false;
    result={valid.x,valid.y,valid.z};return true;
}
} // namespace blackcoop
