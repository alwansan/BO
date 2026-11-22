// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original Xbox kernel NtCreateFile/NtOpenFile **successful resource-open**
// classifier. No original resource data/paths exported; NO scene-load proof.
#pragma once
#include <chrono>
#include <mutex>
#include <cstdint>
#include <string>
#include <string_view>

namespace blackcoop {
struct OriginalStageResource {
    int folder=-1;
    int resource=-1; // 0 = LevelDat.bin, 1 = Unit_01.bin
};
inline OriginalStageResource ClassifyOriginalStageResource(std::string_view raw) {
    if(raw.empty()||raw.size()>180)return {};
    std::string path;
    path.reserve(raw.size());
    for(unsigned char c:raw){
        if(c==0||c>127||c<32)return {};
        if(c=='/')c='\\';
        if(c>='a'&&c<='z')c=static_cast<unsigned char>(c-'a'+'A');
        path.push_back(static_cast<char>(c));
    }
    static constexpr std::string_view prefixes[]={
       "D:\\","\\??\\D:\\","\\DEVICE\\CDROM0\\"};
    std::string_view tail;
    for(auto prefix:prefixes)
        if(path.compare(0,prefix.size(),prefix)==0){
            tail=std::string_view(path).substr(prefix.size());break;
        }
    // Require EXACT relative path; don't accept another drive, user-save
    // path, traversal, extra suffix, or unrelated substring containing level.
    if(tail.size()<std::string_view("LEVELS\\LEVEL_00\\UNIT_01.BIN").size()||
       tail.compare(0,13,"LEVELS\\LEVEL_")!=0)return {};
    auto digit=[&](size_t i)->int{
        if(i>=tail.size()||tail[i]<'0'||tail[i]>'9')return -1;
        return tail[i]-'0';
    };
    const int a=digit(13),b=digit(14);
    if(a<0||b<0||tail.size()<17||tail[15]!='\\')return {};
    const int folder=10*a+b;
    // Owner-pinned extracted BLACK level directory list:
    // 00, 01, 03, 04, 05, 06, 07, 08. The missing 02 is NOT guessed.
    if(folder!=0&&folder!=1&&(folder<3||folder>8))return {};
    const auto suffix=tail.substr(16);
    int resource=-1;
    if(suffix=="LEVELDAT.BIN")resource=0;
    else if(suffix=="UNIT_01.BIN")resource=1;
    if(resource<0)return {};
    return {folder,resource};
}
/**
 * Correlation of exact successful resource opens with a SINGLE original
 * mission-launch attempt. No successful game loader or scene proof.
 *
 * Previously a process-lifetime bitmask retained LevelDat and Unit_01 across
 * launches and could falsely attribute a stale file to a new game. This
 * episode-scoped detector fails CLOSED across launch attempts, changing
 * folders and delayed loading, using monotonic wall-clock timestamps.
 */
class OriginalStageFileOpenOnce final {
    mutable std::mutex lock_;
    uint32_t episode_=0,reportedEpisode_=0;
    int folder_=-1;
    uint8_t resources_=0;
    uint64_t startedAtMs_=0,lastFileAtMs_=0;
    static constexpr uint64_t kEpisodeLimitMs=45000;
    static constexpr uint64_t kOpenGapLimitMs=15000;
    bool fresh(uint64_t atMs)const noexcept{
        return episode_!=0&&atMs>=startedAtMs_&&
               atMs-startedAtMs_<=kEpisodeLimitMs&&
               atMs>=lastFileAtMs_&&atMs-lastFileAtMs_<=kOpenGapLimitMs;
    }
public:
    static uint64_t NowMs()noexcept{
        using namespace std::chrono;
        return static_cast<uint64_t>(duration_cast<milliseconds>(
            steady_clock::now().time_since_epoch()).count());
    }
    uint32_t BeginOriginalLaunchAttempt(uint64_t atMs=NowMs())noexcept{
        std::lock_guard<std::mutex> guard(lock_);
        if(++episode_==0)episode_=1;
        startedAtMs_=atMs;lastFileAtMs_=atMs;
        folder_=-1;resources_=0; // Do not carry a previous level's opens
        return episode_;
    }
    // Read-only current original launch episode for another source-proven
    // original stage-stream entry. NOT a scene-loaded notification.
    uint32_t ActiveEpisode()const noexcept{
        std::lock_guard<std::mutex> guard(lock_);
        return episode_;
    }
    bool ObserveSuccessfulOpen(std::string_view path,OriginalStageResource& report,
                               uint64_t atMs=NowMs()){
        OriginalStageResource sample=ClassifyOriginalStageResource(path);
        if(sample.folder<0)return false;
        std::lock_guard<std::mutex> guard(lock_);
        if(!fresh(atMs))return false;
        if(folder_!=sample.folder){
            folder_=sample.folder;resources_=0; // no mixed-folder pair
        }
        const uint8_t bit=static_cast<uint8_t>(1u<<sample.resource);
        if(resources_&bit)return false;
        resources_=static_cast<uint8_t>(resources_|bit);
        lastFileAtMs_=atMs;
        report=sample;
        return true;
    }
    int LastResourceFolderHint()const noexcept{
        std::lock_guard<std::mutex> guard(lock_);
        return folder_;
    }
    uint32_t SeenMask()const noexcept{
        std::lock_guard<std::mutex> guard(lock_);
        return folder_<0?0:static_cast<uint32_t>(resources_)<<(folder_*2);
    }
    bool BothEssentialFilesOpenedForCandidate(uint64_t atMs=NowMs())const noexcept{
        std::lock_guard<std::mutex> guard(lock_);
        return fresh(atMs)&&resources_==3&&folder_>=0;
    }
    bool TakeCandidateFromOriginalPlayer(int& outFolder,uint32_t& outEpisode,
                                         uint64_t atMs=NowMs())noexcept{
        std::lock_guard<std::mutex> guard(lock_);
        if(!fresh(atMs)||resources_!=3||folder_<0||
           reportedEpisode_==episode_)return false;
        reportedEpisode_=episode_;outFolder=folder_;outEpisode=episode_;
        return true;
    }
};
// One source of truth shared between original Xbox kernel file-open hooks
// and original first-person-player sample; READ-ONLY observability only.
inline OriginalStageFileOpenOnce g_originalStageFileOpen;
} // namespace blackcoop
