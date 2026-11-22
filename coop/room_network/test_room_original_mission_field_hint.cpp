#include "room_original_mission_field_hint.hpp"
#include <cassert>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
using namespace blackcoop;
int main(){
    assert(ClassifyOriginalMissionFieldHint(0)==0);
    assert(ClassifyOriginalMissionFieldHint(7)==7);
    assert(ClassifyOriginalMissionFieldHint(8)==-1);
    assert(ClassifyOriginalMissionFieldHint(0xffffffffu)==-1);
    OriginalMissionFieldObservation reader;
    assert(reader.ForCorrelatedEpisode(1).episode==0);
    reader.CaptureAtOriginalLaunch(1,3,6);
    auto a=reader.ForCorrelatedEpisode(1);
    assert(a.episode==1&&a.readCandidate==3&&a.writeCandidate==6);
    assert(reader.ForCorrelatedEpisode(2).episode==0);
    reader.CaptureAtOriginalLaunch(2,0xffffffffu,7);
    assert(reader.ForCorrelatedEpisode(1).episode==0);
    auto b=reader.ForCorrelatedEpisode(2);
    assert(b.episode==2&&b.readCandidate==-1&&b.writeCandidate==7);
    reader.CaptureAtOriginalLaunch(3,0,0xffffffffu);
    auto c=reader.ForCorrelatedEpisode(3);
    assert(c.episode==3&&c.readCandidate==0&&c.writeCandidate==-1);
    reader.CaptureAtOriginalLaunch(0,2,5);
    assert(reader.ForCorrelatedEpisode(3).episode==3);
    std::atomic<bool> wrong{false};
    std::vector<std::thread> threads;
    for(int i=0;i<4;i++)threads.emplace_back([&reader,&wrong,i]{
        for(int n=1;n<2000;n++){
            const uint32_t epoch=static_cast<uint32_t>(i*2000+n);
            reader.CaptureAtOriginalLaunch(epoch,static_cast<uint32_t>(i),0xffffffffu);
            const auto snap=reader.ForCorrelatedEpisode(epoch);
            if(snap.episode&&
               (snap.episode!=epoch||snap.readCandidate!=i||
                snap.writeCandidate!=-1))
                wrong.store(true);
        }
    });
    for(auto& t:threads)t.join();
    assert(!wrong.load());
    std::cout<<"ORIGINAL_MISSION_FIELD_HINTS_PASS: atomic episode-bound two-candidate read-only values 0..7; no mission identity proven, no stage launcher\n";
}
