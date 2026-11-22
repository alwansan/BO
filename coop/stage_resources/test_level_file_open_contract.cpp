#include "level_file_open_contract.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
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
    OriginalStageFileOpenOnce track;
    assert(track.LastResourceFolderHint()==-1);
    OriginalStageResource result{};
    assert(track.ObserveSuccessfulOpen("D:\\levels\\level_00\\LevelDat.bin",result));
    assert(result.folder==0&&result.resource==0);
    assert(!track.ObserveSuccessfulOpen("D:\\levels\\level_00\\LevelDat.bin",result));
    assert(track.ObserveSuccessfulOpen("D:\\levels\\level_00\\Unit_01.bin",result));
    assert(track.ObserveSuccessfulOpen("D:\\levels\\level_08\\LevelDat.bin",result));
    assert(track.LastResourceFolderHint()==8);
    assert(!track.BothEssentialFilesOpenedForCandidate());
    assert(track.ObserveSuccessfulOpen("D:\\levels\\level_08\\Unit_01.bin",result));
    assert(track.BothEssentialFilesOpenedForCandidate());
    assert((track.SeenMask()&(1u<<16))!=0);
    // Atomic once gate under concurrent genuine engine file threads.
    OriginalStageFileOpenOnce parallel;
    std::vector<std::thread> workers;
    std::atomic<int> announced{0};
    for(int i=0;i<16;i++)workers.emplace_back([&]{
        OriginalStageResource found;
        if(parallel.ObserveSuccessfulOpen("D:\\levels\\level_07\\LevelDat.bin",found))announced++;
    });
    for(auto& worker:workers)worker.join();
    assert(announced.load()==1);
    assert(parallel.LastResourceFolderHint()==7);
    std::cout<<"ORIGINAL_LEVEL_RESOURCE_OPEN_GATE_PASS: exact Xbox disc whitelist, 8 known stage folders x 2 essential files, first-success only; NEVER scene loaded\n";
}
