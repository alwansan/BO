// BO-COOP: pinned PC and original owner XBE gameplay-core phase hint.
// Read-only, single ORIGINAL game thread, never a scene load/READY ticket.
// MIT-derived source idea only: PC recomp b2512fafd split-screen code
// examines original mission-core +0x5AE0 == 0x1C, while pinned original
// XBE static CI44980186959 finds direct field operands in two functions.
#pragma once
#include <cstdint>
namespace blackcoop {
class OriginalCoreRunningPhaseWitness final {
    uint32_t episode_=0,world_=0;
    unsigned consecutive_=0;
    bool reported_=false;
public:
    bool Observe(uint32_t episode,uint32_t world,uint32_t sourceCorePhase,
                 bool originalLocalPlayer,bool actualResourcePairFresh,
                 bool originalStreamingCandidateSameEpisode)noexcept {
        // This is a research candidate only; independent original scene
        // load completion/event and loader ABI are STILL NOT VERIFIED.
        const bool valid=episode!=0&&world!=0&&originalLocalPlayer&&
            actualResourcePairFresh&&originalStreamingCandidateSameEpisode&&
            sourceCorePhase==0x1Cu;
        if(!valid){
            episode_=0;world_=0;consecutive_=0;reported_=false;
            return false;
        }
        if(episode_!=episode||world_!=world){
            episode_=episode;world_=world;consecutive_=0;reported_=false;
        }
        if(consecutive_<30)++consecutive_;
        if(reported_||consecutive_<30)return false;
        reported_=true;
        return true;
    }
    void Reset()noexcept{episode_=0;world_=0;consecutive_=0;reported_=false;}
};
} // namespace blackcoop
