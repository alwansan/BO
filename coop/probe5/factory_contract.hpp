// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <cstdint>
#include <cmath>
#include <cstring>
namespace bo_probe5 {
struct Entry {
 uint32_t manager=0,root=0,world=0,brain=0,resource=0;
 uint32_t resource_index=0,controller_index=0,live=0,free=0,flags_used=0;
 uint32_t kind=0,selector_a=0,selector_b=0;
 bool pose_finite=false,action_binding=false;
 bool operator==(const Entry&)const=default;
};
struct Result {
 uint32_t slot=0,serial=0;bool brain_matches=false,resource_matches=false,backlink=false;
 bool operator==(const Result&)const=default;
};
inline bool Range(uint32_t p,uint32_t size)noexcept{return p>=0x10000&&uint64_t(p)+size<=UINT64_C(0x100000000);}
template<class Reader>bool SameScope(const Reader& read,const Entry& e)noexcept{
 uint32_t manager=0,root=0,world=0;
 return read.Read(0x2D1980,manager)&&read.Read(0x2D1988,root)&&read.Read(0x2D45D4,world)&&manager==e.manager&&root==e.root&&world==e.world;
}
template<class Reader>bool ReadEntry(const Reader& read,uint32_t manager,uint32_t brain,uint32_t descriptor,Entry& out,uint32_t* rejection=nullptr)noexcept{
 if(rejection)*rejection=1;
 Entry e;e.manager=manager;e.brain=brain;uint32_t observed=0,flag_root=0,live_capacity=0,free_capacity=0;
 if(!Range(manager,0x8528)||!Range(descriptor,0x5C)||!read.Read(0x2D1980,observed)||observed!=manager||
    !read.Read(0x2D1988,e.root)||!Range(e.root,0x22CE0)||!read.Read(0x2D45D4,e.world)||!Range(e.world,0x388)||
    !read.Read(0x2D45F4,flag_root)||!Range(flag_root,0x2974))return false;
 if(rejection)*rejection=2;
 const uint32_t first=e.root+0x2B00;
 if(brain<first||uint64_t(brain)>=uint64_t(first)+16*0x1FE0||(brain-first)%0x1FE0)return false;
 e.controller_index=(brain-first)/0x1FE0;
 if(rejection)*rejection=3;
 std::array<uint32_t,23> d{},again{};std::array<uint8_t,20> flags{};
 if(!read.Read(descriptor,d)||!read.Field(flag_root,0x2960,flags)||!read.Field(manager,0x79B4,live_capacity)||
    !read.Field(manager,0x79B8,e.live)||!read.Field(manager,0x79A4,free_capacity)||!read.Field(manager,0x79A8,e.free)||
    live_capacity!=16||e.live>16||free_capacity!=32||e.free>32)return false;
 if(rejection)*rejection=4;
 e.resource=d[0];const uint32_t resources=manager+0x7A20;
 if(e.resource<resources||uint64_t(e.resource)>=uint64_t(resources)+20*0x40||(e.resource-resources)%0x40)return false;
 e.resource_index=(e.resource-resources)/0x40;e.kind=d[15];e.selector_a=d[16];e.selector_b=d[17];e.action_binding=d[20]!=0||d[21]!=0;
 if(e.kind>65535)e.kind=UINT32_MAX;
 if(e.selector_a>65535)e.selector_a=UINT32_MAX;
 if(e.selector_b>65535)e.selector_b=UINT32_MAX;
 e.pose_finite=true;
 for(unsigned i=4;i<8;++i){float f=0;std::memcpy(&f,&d[i],4);if(!std::isfinite(f))e.pose_finite=false;}
 for(auto f:flags)if(f)e.flags_used++;
 if(rejection)*rejection=5;
 if(!read.Read(descriptor,again)||d!=again||!SameScope(read,e))return false;
 if(rejection)*rejection=0;
 out=e;return true;
}
template<class Reader>bool ReadResult(const Reader& read,const Entry& e,uint32_t actor,Result& out)noexcept{
 if(!SameScope(read,e)||actor<e.manager+0xA0||uint64_t(actor)>=uint64_t(e.manager)+0xA0+32*0x3C0||
    (actor-e.manager-0xA0)%0x3C0)return false;
 std::array<uint32_t,26> fields{},again{};uint32_t table=0,membership=0,back=0,back_after=0;
 if(!read.Read(actor,table)||table!=0x28C1E8||!read.Field(actor,0x250,membership)||membership!=e.manager+0x79B0||
    !read.Field(actor,0x320,fields)||fields[25]!=0||!read.Field(e.brain,0x8C,back))return false;
 Result r;r.slot=(actor-e.manager-0xA0)/0x3C0;r.serial=fields[21];r.brain_matches=fields[1]==e.brain;
 r.resource_matches=fields[0]==e.resource;r.backlink=back==actor;
 if(!r.brain_matches||!r.resource_matches||!r.backlink||!r.serial||!read.Field(actor,0x320,again)||fields!=again||
    !read.Field(e.brain,0x8C,back_after)||back_after!=actor||!read.Read(actor,table)||table!=0x28C1E8||
    !read.Field(actor,0x250,membership)||membership!=e.manager+0x79B0||!SameScope(read,e))return false;
 out=r;return true;
}
}
