#include "room_original_streaming_stage_contract.hpp"
#include <cassert>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
using namespace blackcoop;
int main(){
    assert(BucketOriginalStreamingLevel(0)==0);
    assert(BucketOriginalStreamingLevel(8)==8);
    assert(BucketOriginalStreamingLevel(9)==-1);
    assert(BucketOriginalStreamingLevel(0xffffffffu)==-1);
    assert(BucketOriginalStreamingStage(0)==0);
    assert(BucketOriginalStreamingStage(15)==15);
    assert(BucketOriginalStreamingStage(16)==-1);
    assert(EncodeOriginalStreamingStageHint(-1,-1,0)==500);
    assert(EncodeOriginalStreamingStageHint(8,15,8)==2029);
    assert(EncodeOriginalStreamingStageHint(0,0,2)==-1);
    assert(EncodeOriginalStreamingStageHint(9,0,0)==-1);
    assert(EncodeOriginalStreamingStageHint(0,16,0)==-1);
    OriginalStreamingStageObservation record;
    assert(!record.CaptureAtStreamingEntry(0,0,0));
    assert(record.ForCorrelatedEpisode(1).episode==0);
    assert(record.CaptureAtStreamingEntry(1,0,5));
    assert(!record.CaptureAtStreamingEntry(1,8,9));
    auto first=record.ForCorrelatedEpisode(1);
    assert(first.episode==1&&first.level==0&&first.stage==5);
    assert(record.CaptureAtStreamingEntry(2,8,15));
    assert(!record.CaptureAtStreamingEntry(1,0,0));
    assert(record.ForCorrelatedEpisode(1).episode==0);
    auto second=record.ForCorrelatedEpisode(2);
    assert(second.episode==2&&second.level==8&&second.stage==15);
    assert(record.CaptureAtStreamingEntry(3,255,255));
    auto third=record.ForCorrelatedEpisode(3);
    assert(third.episode==3&&third.level==-1&&third.stage==-1);
    std::atomic<bool> broken{false};
    std::vector<std::thread> workers;
    for(int i=1;i<=8;i++)workers.emplace_back([&record,&broken,i]{
        for(int n=0;n<1000;n++){
            uint32_t ep=static_cast<uint32_t>(i*1001+n+4);
            bool accepted=record.CaptureAtStreamingEntry(ep,static_cast<uint32_t>(i),0);
            auto seen=record.ForCorrelatedEpisode(ep);
            if(accepted&&seen.episode&&seen.level!=i)broken.store(true);
        }
    });
    for(auto& w:workers)w.join();
    assert(!broken.load());
    std::cout<<"ORIGINAL_STREAMING_STAGE_CONTRACT_PASS: one native entry per launch, safe fixed code, no scene READY\n";
}
