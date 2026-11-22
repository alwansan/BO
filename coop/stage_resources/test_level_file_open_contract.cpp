#include "level_file_open_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
using namespace blackcoop;
int main(){
    auto good=ClassifyOriginalStageResource("D:\\levels\\level_00\\LevelDat.bin");
    assert(good.folder==0&&good.resource==0);
    good=ClassifyOriginalStageResource("\\??\\D:\\LEVELS\\LEVEL_01\\Unit_01.bin");
    assert(good.folder==1&&good.resource==1);
    good=ClassifyOriginalStageResource("\\Device\\CdRom0\\levels\\level_08\\leveldat.bin");
    assert(good.folder==8&&good.resource==0);
    good=ClassifyOriginalStageResource("D:/levels/level_03/Unit_01.bin");
    assert(good.folder==3&&good.resource==1);
    for(const char* bad:{
        "C:\\levels\\level_00\\LevelDat.bin",
        "D:\\levels\\level_02\\LevelDat.bin",
        "D:\\levels\\level_09\\LevelDat.bin",
        "D:\\levels\\level_000\\LevelDat.bin",
        "D:\\levels\\level_00\\LevelDat.bin\\extra",
        "D:\\levels\\level_00\\..\\LevelDat.bin",
        "D:\\levels\\level_00\\Unit_02.bin",
        "D:\\levels\\level_00\\something.bin",
        "D:\\foo\\levels\\level_00\\LevelDat.bin",
        "\\\\server\\levels\\level_01\\LevelDat.bin"
    })assert(ClassifyOriginalStageResource(bad).folder==-1);
    assert(ClassifyOriginalStageResource(std::string_view("D:\\levels\\level_00\\LevelDat.bin\0TRAIL",33)).folder==-1);

    OriginalStageFileOpenOnce detector;
    OriginalStageResource row{};
    int folder=-1;uint32_t episode=0;
    // Successful resource OPEN without an original mission attempt cannot
    // silently create a claimed loaded game.
    assert(!detector.ObserveSuccessfulOpen("D:\\levels\\level_00\\LevelDat.bin",row,100));
    assert(detector.LastResourceFolderHint()==-1);
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,110));
    assert(detector.ActiveEpisode()==0);
    assert(detector.BeginOriginalLaunchAttempt(100)==1);
    assert(detector.ActiveEpisode()==1);
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_00\\LevelDat.bin",row,120));
    assert(!detector.ObserveSuccessfulOpen("D:\\levels\\level_00\\LevelDat.bin",row,125));
    assert(!detector.BothEssentialFilesOpenedForCandidate(125));
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_00\\Unit_01.bin",row,130));
    assert(detector.BothEssentialFilesOpenedForCandidate(135));
    assert(detector.TakeCandidateFromOriginalPlayer(folder,episode,136));
    assert(folder==0&&episode==1);
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,136));

    // Same mission restarted: OLD files from epoch 1 must not count.
    assert(detector.BeginOriginalLaunchAttempt(200)==2);
    assert(detector.ActiveEpisode()==2);
    assert(!detector.BothEssentialFilesOpenedForCandidate(201));
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_00\\Unit_01.bin",row,205));
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,206));
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_00\\LevelDat.bin",row,210));
    assert(detector.TakeCandidateFromOriginalPlayer(folder,episode,215));
    assert(folder==0&&episode==2);

    // No combination of different folders can fake a single loaded level.
    assert(detector.BeginOriginalLaunchAttempt(300)==3);
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_01\\LevelDat.bin",row,301));
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_03\\Unit_01.bin",row,302));
    assert(!detector.BothEssentialFilesOpenedForCandidate(303));
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_03\\LevelDat.bin",row,304));
    assert(detector.TakeCandidateFromOriginalPlayer(folder,episode,305));
    assert(folder==3&&episode==3);
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,306));

    // Delayed file I/O and old completed pairs expire.
    assert(detector.BeginOriginalLaunchAttempt(1000)==4);
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_04\\LevelDat.bin",row,1010));
    assert(!detector.ObserveSuccessfulOpen("D:\\levels\\level_04\\Unit_01.bin",row,17020));
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,17021));
    assert(detector.BeginOriginalLaunchAttempt(20000)==5);
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_05\\LevelDat.bin",row,20001));
    assert(detector.ObserveSuccessfulOpen("D:\\levels\\level_05\\Unit_01.bin",row,20002));
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,36003));
    assert(detector.BeginOriginalLaunchAttempt(40000)==6);
    assert(!detector.ObserveSuccessfulOpen("D:\\levels\\level_05\\LevelDat.bin",row,85001));
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,85001));
    assert(!detector.TakeCandidateFromOriginalPlayer(folder,episode,39999));

    // Concurrent genuine Xbox file-opening workers => single claim per
    // original mission and exact resource, never per thread.
    OriginalStageFileOpenOnce concurrent;
    concurrent.BeginOriginalLaunchAttempt(10000);
    std::vector<std::thread> workers;
    std::atomic<int> opened{0},reported{0};
    for(int i=0;i<16;i++)workers.emplace_back([&]{
        OriginalStageResource found{};
        if(concurrent.ObserveSuccessfulOpen("D:\\levels\\level_07\\LevelDat.bin",found,10020))opened++;
        if(concurrent.ObserveSuccessfulOpen("D:\\levels\\level_07\\Unit_01.bin",found,10021))opened++;
    });
    for(auto& th:workers)th.join();
    assert(opened==2);
    workers.clear();
    for(int i=0;i<16;i++)workers.emplace_back([&]{
        int found=-1;uint32_t epoch=0;
        if(concurrent.TakeCandidateFromOriginalPlayer(found,epoch,10030)){
            assert(found==7&&epoch==1);reported++;
        }
    });
    for(auto& th:workers)th.join();
    assert(reported==1);
    assert(concurrent.SeenMask()==(3u<<(7*2)));
    std::cout<<"ORIGINAL_LEVEL_RESOURCE_EPISODE_PASS: exact pinned Xbox folders, isolated launch epochs, no cross-level mix, 15s stale expiry, one report per 2-file candidate; NEVER scene READY\n";
}
