// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
#pragma once
#include <array>
#include <cstdint>
#include <string>
namespace bo_player {
struct ModelInfo {
 uint32_t bones=0,buffers=0,draw_lists=0;
 bool has_model=false,complete=false,head=false,torso=false,legs=false,arms=false;
 bool operator==(const ModelInfo&)const=default;
};
inline bool Range(uint32_t p,uint32_t n)noexcept{return p>=0x10000&&uint64_t(p)+n<=UINT64_C(0x100000000);}
template<class Reader>bool Name(const Reader& read,uint32_t address,std::string& out)noexcept{
 if(!Range(address,128))return false;
 std::string text;
 try{
  for(uint32_t at=0;at<128;at+=16){
   std::array<char,16> block{};if(!read.Field(address,at,block))return false;
   for(unsigned char c:block){if(!c){out=text;return true;}if(c>127||c<32)return false;text.push_back(c>='A'&&c<='Z'?char(c+32):char(c));}
  }
 }catch(...){return false;}
 return false;
}
inline bool Has(const std::string& name,const char* value)noexcept{return name.find(value)!=std::string::npos;}
template<class Reader>bool ReadModel(const Reader& read,uint32_t actor,ModelInfo& out)noexcept{
 uint32_t world=0,world_after=0,table=0,kind=0,wrapper=0,model=0,after=0;
 if(!read.Read(0x2D45D4,world)||!Range(world,0x930)||actor!=world+0x30||!read.Read(actor,table)||table!=0x28CFE0||
    !read.Field(actor,0xC4,kind)||kind!=2||!read.Field(actor,0x320,wrapper))return false;
 ModelInfo info;
 if(wrapper&&(!Range(wrapper,4)||!read.Read(wrapper,model)))return false;
 std::array<uint32_t,28> header{},again{};
 if(model){
  if(!Range(model,0x70)||!read.Read(model,header)||header[4]!=0xB8000000||header[5]!=0x95820AD4||header[6]!=35)return false;
  info.has_model=true;info.bones=header[23];info.buffers=header[9];
  if(info.bones>512||info.buffers>4096)return false;
  if(info.bones<=96&&info.buffers<=128){
   if((info.bones&&!Range(header[24],info.bones*4))||(info.buffers&&!Range(header[7],info.buffers*0x1C)))return false;
   for(uint32_t i=0;i<info.bones;i++){
    uint32_t name=0;std::string text;
    if(!read.Field(header[24],i*4,name)||!Name(read,name,text))return false;
    info.head|=Has(text,"head")||Has(text,"neck");info.torso|=Has(text,"spine")||Has(text,"pelvis")||Has(text,"hip");
    info.legs|=Has(text,"thigh")||Has(text,"calf")||Has(text,"knee")||Has(text,"foot")||Has(text,"ankle");
    info.arms|=Has(text,"arm")||Has(text,"hand")||Has(text,"finger");
   }
   for(uint32_t i=0;i<info.buffers;i++){
    uint8_t list=0;if(!read.Field(header[7],i*0x1C+0x19,list)||list>15)return false;info.draw_lists|=1u<<list;
   }
   info.complete=true;
  }
  if(!read.Read(model,again)||header!=again)return false;
 }
 if(!read.Read(0x2D45D4,world_after)||world_after!=world||!read.Read(actor,table)||table!=0x28CFE0||
    !read.Field(actor,0xC4,kind)||kind!=2||!read.Field(actor,0x320,after)||after!=wrapper)return false;
 if(wrapper&&(!read.Read(wrapper,after)||after!=model))return false;
 out=info;return true;
}
}
