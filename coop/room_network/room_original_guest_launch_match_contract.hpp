// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Genuine original frontend launch + original stream-selected stage + stable
// ORIGINAL player/world may be correlated with the authenticated HOST PREPARE.
// Evidence only. No original engine writes, loader, scene READY or fake world.
#pragma once
#include "room_original_guest_prepared_story_contract.hpp"
#include <cstdint>
#include <mutex>

namespace blackcoop {
struct OriginalGuestLaunchMatchKey {
    uint32_t originalEpisode=0,sceneEpoch=0,rosterRevision=0;
    uint32_t tokenHi=0,tokenLo=0;
    uint8_t level=0,stage=0,selfSlot=0,participants=0;
};
class OriginalGuestLaunchMatchGate final {
    mutable std::mutex mutex_;
    OriginalGuestLaunchMatchKey evidence_{};
    uint32_t sourceEpisode_=0,world_=0;
    uint32_t samples_=0;
    bool hasSample_=false,matched_=false,signaled_=false;
    void resetLocked()noexcept {
        evidence_={};sourceEpisode_=world_=samples_=0;
        hasSample_=matched_=signaled_=false;
    }
public:
    void Reset()noexcept{
        std::lock_guard<std::mutex> guard(mutex_);resetLocked();
    }
    // Exact first original streaming entry of one original frontend
    // launch; NO fabricated launch from host network packets.
    // 78 = original selected stage MATCHES host plan,
    // 79 = original selected stage DISAGREES with host plan.
    int ObserveOriginalStreamingEntry(uint32_t episode,int chosenLevel,
                                      int chosenStage,
                                      const OriginalGuestPreparedStory* host)noexcept{
        std::lock_guard<std::mutex> guard(mutex_);
        if(!episode||chosenLevel<0||chosenLevel>8||
           chosenStage<0||chosenStage>15||!host||
           host->sceneEpoch==0||host->revision==0||
           host->mission>7||host->stage>15||
           host->selfSlot<2||host->selfSlot>4||
           (host->participants!=3&&host->participants!=7&&
            host->participants!=15)||
           !(host->participants&(1u<<(host->selfSlot-1)))||
           (host->tokenHi==0&&host->tokenLo==0))
            return 0;
        if(sourceEpisode_>episode)return 0; // reject old native launch
        if(sourceEpisode_<episode){
            resetLocked();sourceEpisode_=episode;
        }
        if(hasSample_)return 0; // first original entry is authoritative
        hasSample_=true;
        evidence_.originalEpisode=episode;
        evidence_.sceneEpoch=host->sceneEpoch;
        evidence_.rosterRevision=host->revision;
        evidence_.tokenHi=host->tokenHi;
        evidence_.tokenLo=host->tokenLo;
        evidence_.level=host->mission;
        evidence_.stage=host->stage;
        evidence_.selfSlot=host->selfSlot;
        evidence_.participants=host->participants;
        matched_=chosenLevel==host->mission&&chosenStage==host->stage;
        return matched_?78:79;
    }
    // The original core+stream stage sources must remain matched and the
    // real first-person character+world must remain STABLE for another
    // 30 consecutive 10Hz original game-thread source observations.
    // 81 = local original world stable on SAME authentic host stage.
    // Still NOT proof of a shared world or safe STORY_READY/COMMIT.
    int ObserveOriginalRunningWorld(uint32_t episode,uint32_t world,
                                    int coreLevel,int coreStage,
                                    bool originalSourcesMatched)noexcept{
        std::lock_guard<std::mutex> guard(mutex_);
        if(!matched_||!sourceEpisode_||episode!=sourceEpisode_||
           !world||!originalSourcesMatched||
           coreLevel!=evidence_.level||coreStage!=evidence_.stage){
            samples_=0;world_=0;return 0;
        }
        if(world_!=world){world_=world;samples_=0;}
        if(samples_<30)++samples_;
        if(samples_>=30&&!signaled_){signaled_=true;return 81;}
        return 0;
    }
    bool ReadMatchedKey(OriginalGuestLaunchMatchKey& out)const noexcept{
        std::lock_guard<std::mutex> guard(mutex_);
        if(!matched_||!hasSample_||!signaled_)return false;
        out=evidence_;return true;
    }
};
} // namespace blackcoop
