// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original Xbox input::Poll ports 1..3 <- authenticated host TLS commands.
// Source of Pad: pinned TGP482/RanggaCasper native/input.cpp.
// THIS IS NOT ACTIVE GAMEPLAY until original native P2-P4 actors exist.
#pragma once
#include "room_original_host_remote_pad_plan.hpp"
#include <cstddef>
#include <cstdint>
namespace blackcoop {
enum class OriginalHostPadRoute : uint8_t { Unowned, NeutralReserved, Applied };
class OriginalHostPadPortBridge final {
public:
    // verifiedHostOriginalScene and independentlySetUpActor MUST be derived
    // from the ORIGINAL engine (not room COMMIT, actor snapshot or JNI input).
    // Never call from a network worker: inbox enforces original owner thread.
    template<class Pad>
    static OriginalHostPadRoute Route(
        Pad& report,const OriginalHostHumanInbox& inbox,uint32_t epoch,
        int port,bool verifiedHostOriginalScene,
        bool independentlySetUpActor)noexcept {
        if(port<1||port>3||!verifiedHostOriginalScene||
           !independentlySetUpActor)return OriginalHostPadRoute::Unowned;
        // A reserved human slot must NOT inherit physical USB/Android TOUCH
        // or another player's FIRE when the TLS peer input is stale/lost.
        report=Pad{};
        if(!epoch||inbox.Epoch()!=epoch)return OriginalHostPadRoute::NeutralReserved;
        OriginalHostRemotePadValue value{};
        if(!OriginalHostRemotePadPlan::Preview(inbox,epoch,
                 static_cast<uint8_t>(port),true,true,value))
            return OriginalHostPadRoute::NeutralReserved;
        report.connected=true;
        report.buttons=value.buttons;
        report.lx=value.lx;report.ly=value.ly;
        report.rx=value.rx;report.ry=value.ry;
        for(size_t i=0;i<8;++i)report.analog[i]=value.analog[i];
        // Yaw is absolute metadata, not synthesized right-stick movement.
        return OriginalHostPadRoute::Applied;
    }
};
} // namespace blackcoop
