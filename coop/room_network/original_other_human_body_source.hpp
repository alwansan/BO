// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Network source to future original BLACK BOT-LOOK renderer: sanitized
// authenticated host-authoritative two-person body pose only.
// Not a drawable original model, not actor collision, not a shared AI world.
#pragma once
#include <cstddef>
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalOtherHumanBody {
    uint32_t epoch=0,revision=0,tick=0;
    uint8_t self=0,peer=0;
    int32_t x=0,y=0,z=0;
    int16_t yaw=0;
    uint8_t flags=0;
};
class OriginalOtherHumanBodyInbox final {
    OriginalOtherHumanBody last_{};
    uint64_t lastAtMs_=0;
    std::thread::id thread_{};
    bool bound_=false;
public:
    void Reset()noexcept{
        if(bound_&&std::this_thread::get_id()!=thread_)return;
        last_={};lastAtMs_=0;
    }
    bool Ingest(const int32_t* f,size_t size,uint64_t nowMs)noexcept{
        if(bound_&&thread_!=std::this_thread::get_id())return false;
        if(!f||size!=10||!nowMs)return false;
        // [0]*10 revokes a game world but cannot self-authorize one.
        bool empty=true;
        for(size_t i=0;i<10;i++)empty&=f[i]==0;
        if(empty){Reset();return true;}
        if(f[0]<=0||f[1]<=0||f[2]<=0||
           !((f[3]==1&&f[4]==2)||(f[3]==2&&f[4]==1))||
           f[5]<-20000000||f[5]>20000000||
           f[6]<-20000000||f[6]>20000000||
           f[7]<-20000000||f[7]>20000000||
           f[8]<-18000||f[8]>18000||f[9]<0||f[9]>3)
            return false;
        if(last_.epoch==static_cast<uint32_t>(f[0])&&
           last_.revision==static_cast<uint32_t>(f[1])&&
           last_.self==static_cast<uint8_t>(f[3])&&
           last_.peer==static_cast<uint8_t>(f[4])&&
           static_cast<uint32_t>(f[2])<last_.tick)
            return false;
        if(!bound_){thread_=std::this_thread::get_id();bound_=true;}
        last_={static_cast<uint32_t>(f[0]),static_cast<uint32_t>(f[1]),
            static_cast<uint32_t>(f[2]),static_cast<uint8_t>(f[3]),
            static_cast<uint8_t>(f[4]),f[5],f[6],f[7],
            static_cast<int16_t>(f[8]),static_cast<uint8_t>(f[9])};
        lastAtMs_=nowMs;
        return true;
    }
    bool Read(uint64_t nowMs,OriginalOtherHumanBody& out)const noexcept{
        if(!bound_||thread_!=std::this_thread::get_id()||
           !last_.epoch||nowMs<lastAtMs_||nowMs-lastAtMs_>350)
            return false;
        out=last_;return true;
    }
};
}
