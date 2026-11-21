// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
#include <array>
namespace bo_probe4 {
struct InstanceKey {uint32_t actor,serial,container,vtable;};
struct InstanceGraph {uint32_t controller=0,runtime=0,binding=0,scratchA=0,scratchB=0;bool operator==(const InstanceGraph&) const=default;};
namespace detail {
struct ActorView {
    uint32_t table=0,container=0,serial=0,state=0,controller=0,scratchA=0,scratchB=0,resource320=0,resource35c=0;
    bool operator==(const ActorView&) const=default;
};
template<class Reader>bool ReadView(const Reader& read,uint32_t actor,ActorView& view) noexcept {
    std::array<uint32_t,26> meta{};
    if(!read.Read(actor,view.table) || !read.Field(actor,0x250,view.container) || !read.Field(actor,0x320,meta))return false;
    view.resource320=meta[0];view.controller=meta[2];view.scratchA=meta[11];view.scratchB=meta[12];
    view.resource35c=meta[15];view.serial=meta[21];view.state=meta[25];return true;
}
} // namespace detail
// Verified c5 image layout only. Two backlink observations and actor identity
// recheck detect ordinary replacement; no kernel-copy atomicity/ABA claim.
// Caller supplies pool-proven serial/container or independently verified local
// player identity. Never call guest functions or dereference guest pointers.
template<class Reader>bool ReadInstance(const Reader& read,const InstanceKey& key,InstanceGraph& out) noexcept {
    const bool local=key.vtable==0x28CFE0;
    if((!local && key.vtable!=0x28C1E8) || (local && key.container) || (!local && !key.container) ||
       key.actor<0x10000 || uint64_t(key.actor)+0x388>(UINT64_C(1)<<32))return false;
    detail::ActorView before{},after{};
    if(!detail::ReadView(read,key.actor,before) || before.table!=key.vtable || before.serial!=key.serial ||
       (!local && (before.container!=key.container || before.state!=0)) ||
       before.controller<0x10000 || before.controller%4 || uint64_t(before.controller)+88>(UINT64_C(1)<<32))return false;
    std::array<uint32_t,22> first{},second{};
    if(!read.Read(before.controller,first) || !read.Read(before.controller,second) ||
       first[0]!=key.actor || second[0]!=key.actor || first[20]!=second[20] || first[21]!=second[21] ||
       !detail::ReadView(read,key.actor,after) || before!=after)return false;
    out={before.controller,first[21],first[20],before.scratchA,before.scratchB};
    return true;
}
}
