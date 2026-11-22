// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Guest-source verified HOST-only value snapshot, consumed on ORIGINAL game
// thread. NEVER an actor pointer, NPC brain, render command, READY or COMMIT.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>
namespace blackcoop {
struct OriginalHostAvatar{
    uint8_t id=0;
    int32_t x=0,y=0,z=0,yaw=0,flags=0;
};
class OriginalGuestHostWorldInbox final {
    static constexpr uint64_t kFreshMs=350;
    static constexpr int32_t kMaxCm=20000000;
    std::array<OriginalHostAvatar,5> actors_{};
    std::thread::id owner_{};
    bool bound_=false;
    uint32_t scene_=0,tick_=0,self_=0;
    uint64_t lastAt_=0;
    bool onOwner()const noexcept{
        return !bound_||std::this_thread::get_id()==owner_;
    }
public:
    // Layout [sceneEpoch,tick,localGuestId,count, (id,xCm,yCm,zCm,yaw,flags)*count]
    // [0,0,0,0] means the real story/host snapshot is no longer available.
    bool Ingest(const int32_t* data,size_t n,uint64_t nowMs)noexcept{
        if(!onOwner()||!data||n<4||n>28)return false;
        if(n==4&&data[0]==0&&data[1]==0&&data[2]==0&&data[3]==0){
            Clear();return false;
        }
        const int32_t epoch=data[0],tick=data[1],self=data[2],count=data[3];
        if(epoch<=0||tick<=0||self<2||self>4||count<2||count>4||
           n!=4u+static_cast<size_t>(count)*6u||!nowMs)return false;
        std::array<OriginalHostAvatar,5> incoming{};
        int prev=0;
        bool hasSelf=false;
        for(int i=0;i<count;i++){
            const int32_t* p=data+4+i*6;
            if(p[0]<=prev||p[0]>4||p[1]< -kMaxCm||p[1]>kMaxCm||
               p[2]< -kMaxCm||p[2]>kMaxCm||p[3]< -kMaxCm||p[3]>kMaxCm||
               p[4]< -18000||p[4]>18000||p[5]<0||p[5]>3)
                return false;
            prev=p[0];
            if(p[0]==self)hasSelf=true;
            incoming[static_cast<size_t>(p[0])]={
                static_cast<uint8_t>(p[0]),p[1],p[2],p[3],p[4],p[5]};
        }
        if(!hasSelf||incoming[1].id!=1)return false;
        if(!bound_){owner_=std::this_thread::get_id();bound_=true;}
        if(scene_!=0&&(scene_!=static_cast<uint32_t>(epoch)||
                       self_!=static_cast<uint32_t>(self))){
            actors_={};tick_=0;lastAt_=0;
        }
        if(scene_==static_cast<uint32_t>(epoch)&&
           self_==static_cast<uint32_t>(self)&&
           (static_cast<uint32_t>(tick)<=tick_||nowMs<lastAt_))
            return false;
        scene_=static_cast<uint32_t>(epoch);
        self_=static_cast<uint32_t>(self);
        tick_=static_cast<uint32_t>(tick);
        lastAt_=nowMs;
        actors_=incoming;
        return true;
    }
    bool ReadRemote(uint8_t id,uint64_t nowMs,OriginalHostAvatar& out)const noexcept{
        if(!bound_||!onOwner()||!scene_||!tick_||!lastAt_||
           id<1||id>4||id==self_||actors_[id].id!=id||
           nowMs<lastAt_||nowMs-lastAt_>kFreshMs)
            return false;
        out=actors_[id];return true;
    }
    bool Clear()noexcept{
        if(!onOwner())return false;
        actors_={};scene_=tick_=self_=0;lastAt_=0;return true;
    }
    uint32_t Scene()const noexcept{return onOwner()?scene_:0;}
};
} // namespace blackcoop
