// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original BLACK remote HUMAN soldier LOOK animation intent ONLY.
// The independent authenticated peer body is never an AI / collision actor.
#pragma once
#include <cstdint>
namespace blackcoop {
class OriginalRemoteHumanMotionPlan final {
    uint32_t epoch_=0,revision_=0,tick_=0;
    int32_t x_=0,z_=0;
    uint64_t atMs_=0;
    uint8_t streak_=0;
    bool initialized_=false,walking_=false,pendingWalk_=false;
public:
    void Reset()noexcept{
        epoch_=revision_=tick_=0;x_=z_=0;atMs_=0;
        streak_=0;initialized_=walking_=pendingWalk_=false;
    }
    // Input already verified by the story-scoped, host-only original human
    // source inbox. Two consecutive fresh movement observations are required
    // before changing clip; jitter or a single noisy body sample cannot flip.
    void Observe(uint32_t epoch,uint32_t revision,uint32_t tick,
                 int32_t xCm,int32_t zCm,uint64_t atMs)noexcept{
        if(!epoch||!revision||!tick||!atMs){Reset();return;}
        if(!initialized_||epoch_!=epoch||revision_!=revision){
            Reset();initialized_=true;epoch_=epoch;revision_=revision;
            tick_=tick;x_=xCm;z_=zCm;atMs_=atMs;return;
        }
        if(tick<=tick_||atMs<=atMs_)return;
        const uint64_t dt=atMs-atMs_;
        const double dx=static_cast<double>(xCm)-x_;
        const double dz=static_cast<double>(zCm)-z_;
        // >350ms stale data MUST NOT claim current remote movement.
        const double minimumCm=3.0+0.025*static_cast<double>(dt);
        const bool wantsWalk=dt<=350u&&dx*dx+dz*dz>minimumCm*minimumCm;
        tick_=tick;x_=xCm;z_=zCm;atMs_=atMs;
        if(wantsWalk==walking_){streak_=0;pendingWalk_=walking_;return;}
        if(streak_==0||pendingWalk_!=wantsWalk){
            pendingWalk_=wantsWalk;streak_=1;
        }else if(streak_<2)++streak_;
        if(streak_>=2){walking_=wantsWalk;streak_=0;}
    }
    bool Valid()const noexcept{return initialized_;}
    bool Walking()const noexcept{return initialized_&&walking_;}
    // 0 no source, 1 original soldier idle clip, 2 original move clip.
    unsigned DesiredClip()const noexcept{return !initialized_?0u:walking_?2u:1u;}
};
} // namespace blackcoop
