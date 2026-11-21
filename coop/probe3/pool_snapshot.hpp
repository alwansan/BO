// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
namespace bo_probe3 {
struct Container {uint32_t flags=0,capacity=0,occupied=0,data=0;bool operator==(const Container&) const=default;};
static_assert(sizeof(Container)==16);
struct Actor {uint32_t serial=0,state=0,container=0;bool operator==(const Actor&) const=default;};
struct PoolSnapshot {uint32_t counter=0;std::array<Container,4> containers{};std::array<Actor,32> actors{};std::array<std::array<uint32_t,32>,4> entries{};bool operator==(const PoolSnapshot&) const=default;};
namespace detail {
// No guest-derived pointer is dereferenced here. Reader owns safe copying.
// Layout acceptance deliberately excludes transient or nonmatching pools.
template<class Reader> bool ReadOnce(const Reader& read,uint32_t manager,PoolSnapshot& out) noexcept {
    constexpr std::array<uint32_t,4> offsets{0x79A0,0x79B0,0x79C0,0x79D0};
    constexpr std::array<uint32_t,4> capacities{32,16,8,8};
    if(manager<0x10000 || uint64_t(manager)+0x8540>(UINT64_C(1)<<32)) return false;
    uint32_t vtable=0;
    if(!read.Read(manager,vtable) || vtable!=0x28D254 || !read.Field(manager,0x8524,out.counter)) return false;
    std::array<bool,32> seen{};
    for(std::size_t c=0;c<4;++c) {
        auto& header=out.containers[c];
        if(!read.Field(manager,offsets[c],header) || header.capacity!=capacities[c] ||
           header.occupied>header.capacity || header.flags!=(c?7u:0u) ||
           header.data<0x10000 || header.data%4 ||
           uint64_t(header.data)+uint64_t(header.capacity)*4>(UINT64_C(1)<<32)) return false;
        // Read the WHOLE sparse array, including holes. Fixed upper bound32.
        auto& pointers=out.entries[c];
        for(uint32_t i=0;i<header.capacity;++i)
            if(!read.Field(header.data,i*4,pointers[i])) return false;
        uint32_t occupied=0;
        for(uint32_t i=0;i<header.capacity;++i) {
            const uint32_t address=pointers[i];
            if(!address) continue;
            ++occupied;
            const uint64_t first=uint64_t(manager)+0xA0;
            if(address<first || uint64_t(address)>=first+32*0x3C0 || (address-first)%0x3C0) return false;
            const std::size_t slot=static_cast<std::size_t>((address-first)/0x3C0);
            if(seen[slot]) return false;
            seen[slot]=true;
            uint32_t owner=0,table=0;
            auto& actor=out.actors[slot];
            if(!read.Read(address,table) || table!=0x28C1E8 ||
               !read.Field(address,0x250,owner) || owner!=manager+offsets[c] ||
               !read.Field(address,0x374,actor.serial) || !read.Field(address,0x384,actor.state)) return false;
            actor.container=static_cast<uint32_t>(c);
        }
        if(occupied!=header.occupied) return false;
    }
    for(bool present:seen) if(!present) return false;
    return true;
}
} // namespace detail
// Two complete equal observations detect changed count, list membership,
// serial and state. Equality is not proof of atomicity: ABA remains possible.
// On ANY failure caller output remains unchanged.
template<class Reader> bool ReadPool(const Reader& read,uint32_t manager,PoolSnapshot& out) noexcept {
    PoolSnapshot first{},second{};
    if(!detail::ReadOnce(read,manager,first) || !detail::ReadOnce(read,manager,second) || first!=second) return false;
    out=second;
    return true;
}
}
