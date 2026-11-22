// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Authenticated ORIGINAL HUMAN INPUT: bounded native game-thread inbox.
// This is a transport -> game-thread bridge, NOT gameplay/actor integration.
// Never turn a network command into a bot AI command or original guest write.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>

namespace blackcoop {
struct TrustedHumanCommand {
    uint8_t peer=0;
    uint32_t sequence=0,sceneEpoch=0;
    int16_t x=0,y=0,yaw=0;
    uint8_t flags=0;
};
class OriginalHostHumanInbox final {
    static constexpr size_t kMaxPeers=3;
    static constexpr uint32_t kExpiryPolls=3;
    std::array<TrustedHumanCommand,5> commands_{};
    std::array<uint32_t,5> lastSequence_{};
    std::array<uint32_t,5> lastPoll_{};
    uint32_t epoch_=0,polls_=0;
    std::thread::id owner_{};
    bool owned_=false;
    bool onOwner()const noexcept{
        return !owned_||owner_==std::this_thread::get_id();
    }
public:
    // Java supplies [committed sceneEpoch, count, 6 fields/peer].
    // Each row = [authenticated TLS peer ID, sequence, moveX, moveY, yaw,
    //            fire|jump bits]. No names, game addresses, or credentials.
    // [0,0] proves "not in committed game" and purges stale commands.
    // [epoch,0] retains prior state only for bounded poll freshness.
    // NEVER assume Java has proved BLACK game scene loaded: the producing
    // network/story bridge is only valid after its own real COMMIT.
    unsigned Ingest(const int32_t* data,size_t length)noexcept{
        if(!onOwner()||!data||length<2||length>20)return 0;
        const int32_t rawEpoch=data[0],count=data[1];
        if(rawEpoch==0&&count==0&&length==2){
            Reset();
            return 0;
        }
        if(rawEpoch<=0||count<0||count>static_cast<int32_t>(kMaxPeers)||
           length!=2u+static_cast<size_t>(count)*6u)return 0;
        std::array<TrustedHumanCommand,kMaxPeers> pending{};
        uint8_t prev=1;
        for(int i=0;i<count;++i){
            const int32_t* row=data+2+i*6;
            const int32_t id=row[0],sequence=row[1],x=row[2],y=row[3],yaw=row[4],flags=row[5];
            if(id<=prev||id>4||sequence<=0||
               x< -1000||x>1000||y< -1000||y>1000||
               yaw< -18000||yaw>18000||flags<0||flags>3)return 0;
            prev=static_cast<uint8_t>(id);
            pending[static_cast<size_t>(i)]={
                static_cast<uint8_t>(id),static_cast<uint32_t>(sequence),
                static_cast<uint32_t>(rawEpoch),static_cast<int16_t>(x),
                static_cast<int16_t>(y),static_cast<int16_t>(yaw),
                static_cast<uint8_t>(flags)};
        }
        if(!owned_){owner_=std::this_thread::get_id();owned_=true;}
        const uint32_t epoch=static_cast<uint32_t>(rawEpoch);
        if(epoch_!=epoch){
            epoch_=epoch;lastSequence_={};lastPoll_={};commands_={};polls_=0;
        }
        // Integer overflow cannot revalidate expired inputs.
        if(++polls_==0){
            polls_=1;lastPoll_={};commands_={};
        }
        unsigned accepted=0;
        for(int i=0;i<count;++i){
            const auto& cmd=pending[static_cast<size_t>(i)];
            if(cmd.sequence<=lastSequence_[cmd.peer])continue;
            lastSequence_[cmd.peer]=cmd.sequence;
            commands_[cmd.peer]=cmd;
            lastPoll_[cmd.peer]=polls_;
            ++accepted;
        }
        return accepted;
    }
    bool Fresh(uint8_t peer,TrustedHumanCommand& out)const noexcept{
        if(!owned_||!onOwner()||peer<2||peer>4||!epoch_||
           !commands_[peer].sequence||!lastPoll_[peer]||
           polls_<lastPoll_[peer]||polls_-lastPoll_[peer]>kExpiryPolls)return false;
        out=commands_[peer];
        return true;
    }
    bool Reset()noexcept{
        if(!onOwner())return false;
        epoch_=polls_=0;commands_={};lastSequence_={};lastPoll_={};
        return true;
    }
    uint32_t Epoch()const noexcept{return onOwner()?epoch_:0;}
};
} // namespace blackcoop
