#include "room_original_running_scene_source_gate.hpp"
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
    std::cout<<"ORIGINAL_NATIVE_STABLE_LOCAL_RUNNING_WORLD_GATE_PASS: stage + original episode + world + 30 new samples; NOT SHARED WORLD\n";
}
