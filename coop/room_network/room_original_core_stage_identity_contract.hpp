// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// P0: correlate original BLACK core-level identity with the exact original
// streaming entry, without assuming this is an engine scene-loaded callback.
// PC MIT source b2512fafd/black_checkpoint.inc: core+0x5AEC (LEVEL),
// core+0x5AED (STAGE); c5 owner Android guest offsets are NOT independently
// confirmed. This is READ-ONLY source research, NEVER STORY_READY/COMMIT.
#pragma once
#include <cstdint>
namespace blackcoop {
class OriginalCoreStageIdentityWitness final {
    uint32_t episode_=0, world_=0;
    int level_=-2, stage_=-2, signal_=0;
    unsigned samples_=0;
    unsigned emittedMask_=0;
public:
    // Return safe diagnostic 67 MATCH / 68 CONFLICT / 69 UNKNOWN.
    // Unknown is expected if either original source field is not readable.
    // At least 30 stable consecutive 10Hz observations in the SAME episode.
    int Observe(uint32_t episode,uint32_t world,bool qualified,
                int streamingLevel,int streamingStage,
                bool coreReadable,int coreLevel,int coreStage)noexcept {
        if(!qualified||!episode||!world){Reset();return 0;}
        int next=69;
        if(coreReadable&&streamingLevel>=0&&streamingLevel<=8&&
           streamingStage>=0&&streamingStage<=15&&
           coreLevel>=0&&coreLevel<=8&&coreStage>=0&&coreStage<=15)
            next=(streamingLevel==coreLevel&&streamingStage==coreStage)?67:68;
        if(episode_!=episode||world_!=world){
            Reset();episode_=episode;world_=world;
        }
        if(level_!=coreLevel||stage_!=coreStage||signal_!=next){
            level_=coreLevel;stage_=coreStage;signal_=next;samples_=0;
        }
        if(samples_<30)++samples_;
        if(samples_<30)return 0;
        const unsigned bit=1u<<static_cast<unsigned>(next-67);
        if((emittedMask_&bit)!=0)return 0;
        emittedMask_|=bit;
        return next;
    }
    bool Matched(uint32_t episode,uint32_t world)const noexcept{
        return episode&&world&&episode_==episode&&world_==world&&
               signal_==67&&samples_>=30;
    }
    void Reset()noexcept{
        episode_=0;world_=0;level_=-2;stage_=-2;signal_=0;
        samples_=0;emittedMask_=0;
    }
};
} // namespace blackcoop
