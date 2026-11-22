// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Original Xbox kernel NtCreateFile/NtOpenFile **successful resource-open**
// classifier. No original resource data/paths exported; NO scene-load proof.
#pragma once
#include <atomic>
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
class OriginalStageFileOpenOnce final {
    std::atomic<uint32_t> mask_{0};
    std::atomic<int> latest_{-1};
public:
    bool ObserveSuccessfulOpen(std::string_view path,
                               OriginalStageResource& report){
        const OriginalStageResource sample=ClassifyOriginalStageResource(path);
        if(sample.folder<0)return false;
        // RESOURCE OPEN is not stage loaded. Do not call story READY.
        latest_.store(sample.folder,std::memory_order_release);
        uint32_t bit=uint32_t(1u)<<(sample.folder*2+sample.resource);
        if(mask_.fetch_or(bit,std::memory_order_acq_rel)&bit)return false;
        report=sample;return true;
    }
    int LastResourceFolderHint()const noexcept{
        return latest_.load(std::memory_order_acquire);
    }
    uint32_t SeenMask()const noexcept{
        return mask_.load(std::memory_order_acquire);
    }
};
} // namespace blackcoop
