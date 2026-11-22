#include "room_original_launch_difficulty_contract.hpp"
#include <cassert>
#include <iostream>
using namespace blackcoop;
int main(){
    assert(ClassifyOriginalLaunchDifficulty(false,0)==OriginalLaunchDifficulty::Unavailable);
    assert(ClassifyOriginalLaunchDifficulty(true,0)==OriginalLaunchDifficulty::Easy);
    assert(ClassifyOriginalLaunchDifficulty(true,1)==OriginalLaunchDifficulty::Normal);
    assert(ClassifyOriginalLaunchDifficulty(true,2)==OriginalLaunchDifficulty::Hard);
    assert(ClassifyOriginalLaunchDifficulty(true,3)==OriginalLaunchDifficulty::BlackOps);
    assert(ClassifyOriginalLaunchDifficulty(true,4)==OriginalLaunchDifficulty::Unavailable);
    assert(ClassifyOriginalLaunchDifficulty(true,0xffffffffu)==OriginalLaunchDifficulty::Unavailable);
    OriginalLaunchDifficultyOnce witness;
    assert(witness.Observe(false,0)==24);
    assert(witness.Observe(false,0xffffffffu)==0);
    for(uint32_t value=0;value<=3;value++){
        assert(witness.Observe(true,value)==static_cast<int>(20+value));
        for(int repeat=0;repeat<100;repeat++)assert(witness.Observe(true,value)==0);
    }
    assert(witness.Observe(true,4)==0);
    std::cout<<"ORIGINAL_LAUNCH_DIFFICULTY_PASS: source-confirmed 0..3 difficulty only, 5 bounded codes, never selected mission/scene READY\n";
}
