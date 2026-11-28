#include "room_original_running_scene_source_gate.hpp"
#include "room_nonfrontend_original_player.hpp"
#include "room_original_core_phase_witness.hpp"
#include "room_original_core_stage_identity_contract.hpp"
#include "../stage_resources/level_file_open_contract.hpp"
#include <cassert>
#include <iostream>
using blackcoop::OriginalRunningSceneSourceGate;
int main(){
    using blackcoop::IsOriginalWorldRunning;
    static_assert(IsOriginalWorldRunning(0x1C,0x1D));
    static_assert(!IsOriginalWorldRunning(0x16,0x1D));
    for(uint32_t phase=0x1E;phase<=0x21;++phase)
        assert(!IsOriginalWorldRunning(0x1C,phase));
    OriginalRunningSceneSourceGate scene;
    assert(!scene.Observe(0,1,0,0,true));
    assert(!scene.Observe(1,0,0,0,true));
    for(int i=0;i<29;i++)assert(!scene.Observe(1,12,0,0,true));
    assert(scene.Observe(1,12,0,0,true));
    assert(scene.Observe(1,12,0,0,true));
    assert(!scene.Observe(1,13,0,0,true)); // different original world
    for(int i=1;i<29;i++)assert(!scene.Observe(1,13,0,0,true));
    assert(scene.Observe(1,13,0,0,true));
    assert(!scene.Observe(1,13,0,1,false)); // phase/file/core mismatch
    for(int i=0;i<29;i++)assert(!scene.Observe(1,13,0,1,true));
    assert(scene.Observe(1,13,0,1,true));
    assert(!scene.Observe(2,13,0,1,true)); // source episode changed
    assert(!scene.Observe(2,13,8,0,true)); // reserved/unknown mission
    assert(!scene.Observe(2,13,7,16,true));
    for(int i=0;i<29;i++)assert(!scene.Observe(3,14,7,15,true));
    assert(scene.Observe(3,14,7,15,true));
    scene.Reset();assert(!scene.Observe(3,14,7,15,true));

    // The running-scene gate needs 30 additional observations after
    // core/stream identity. The original files may age out in that interval.
    using namespace blackcoop;
    NonFrontendOriginalPlayerWitness player;
    OriginalCoreRunningPhaseWitness phase;
    OriginalCoreStageIdentityWitness identity;
    OriginalStageResourceSceneEvidence resources;
    const OriginalStageResourceScene sampledScene{1,0x10000,0,1};
    bool phaseMatched=false;
    for(unsigned sample=1;sample<=130;++sample){
        const uint64_t at=5000+sample*100;
        const OriginalStageResourcePair pair{1,at<=15000};
        player.Observe(100,200,20,sampledScene.world,true);
        const bool paired=resources.Observe(pair,sampledScene,at);
        phaseMatched=phase.Observe(1,sampledScene.world,0x1C,player.IsStable(),paired,true)||
            (phaseMatched&&paired);
        identity.Observe(1,sampledScene.world,phaseMatched,0,1,true,0,1);
        const bool matched=identity.Matched(1,sampledScene.world);
        if(matched)resources.ConfirmMatched(pair,sampledScene,at);
        assert(matched==(sample>=88));
        assert(scene.Observe(1,sampledScene.world,0,1,matched)==(sample>=117));
    }
    std::cout<<"ORIGINAL_NATIVE_STABLE_LOCAL_RUNNING_WORLD_GATE_PASS: stage + original episode + world + 30 new samples; NOT SHARED WORLD\n";
}
