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
    assert(EncodeOriginalMissionFieldEvent(0,-1,0)==100);
    assert(EncodeOriginalMissionFieldEvent(0,7,8)==180);
    assert(EncodeOriginalMissionFieldEvent(1,7,8)==261);
    assert(EncodeOriginalMissionFieldEvent(2,7,8)==342);
    assert(EncodeOriginalMissionFieldEvent(3,7,8)==423);
    assert(EncodeOriginalMissionFieldEvent(4,7,8)==-1);
    assert(EncodeOriginalMissionFieldEvent(1,2,2)==-1);
    OriginalMissionFieldObservation r;
    assert(r.ForCorrelatedEpisode(1).episode==0);
    r.CaptureAtOriginalLaunch(1,3,6,5);
    assert(r.ForCorrelatedEpisode(1).episode==0);
    assert(!r.CompleteOriginalLaunch(2,2));
    assert(r.CompleteOriginalLaunch(1,2));
    auto a=r.ForCorrelatedEpisode(1);
    assert(a.episode==1&&a.readCandidate==3&&a.launchBeforeCandidate==6&&
           a.launchAfterCandidate==2&&a.menuCandidate==5);
    assert(!r.CompleteOriginalLaunch(1,7));
    r.CaptureAtOriginalLaunch(2,0xffffffffu,7,8);
    assert(r.ForCorrelatedEpisode(1).episode==0);
    assert(r.ForCorrelatedEpisode(2).episode==0);
    assert(!r.CompleteOriginalLaunch(1,1));
    assert(r.CompleteOriginalLaunch(2,7));
    auto b=r.ForCorrelatedEpisode(2);
    assert(b.episode==2&&b.readCandidate==-1&&b.launchBeforeCandidate==7&&
           b.launchAfterCandidate==7&&b.menuCandidate==-1);
    r.CaptureAtOriginalLaunch(3,0,0xffffffffu,3);
    assert(r.CompleteOriginalLaunch(3,0xffffffffu));
    auto c=r.ForCorrelatedEpisode(3);
    assert(c.episode==3&&c.readCandidate==0&&c.launchBeforeCandidate==-1&&
           c.launchAfterCandidate==-1&&c.menuCandidate==3);
    r.CaptureAtOriginalLaunch(0,2,5,2);
    assert(r.ForCorrelatedEpisode(3).episode==3);
    std::atomic<bool> wrong{false};
    std::vector<std::thread> threads;
    for(int i=0;i<4;i++)threads.emplace_back([&r,&wrong,i]{
        for(int n=1;n<2000;n++){
            const uint32_t ep=static_cast<uint32_t>(i*2000+n+4);
            r.CaptureAtOriginalLaunch(ep,static_cast<uint32_t>(i),
                                      0xffffffffu,static_cast<uint32_t>(i+1));
            bool done=r.CompleteOriginalLaunch(ep,static_cast<uint32_t>(i+2));
            auto s=r.ForCorrelatedEpisode(ep);
            if(done&&s.episode&&(s.readCandidate!=i||s.launchBeforeCandidate!=-1||
               s.launchAfterCandidate!=i+2||s.menuCandidate!=i+1))wrong.store(true);
        }
    });
    for(auto& t:threads)t.join();
    assert(!wrong.load());
    std::cout<<"ORIGINAL_MISSION_PRE_POST_WORD_PASS: no stage loader or READY\\n";
}
