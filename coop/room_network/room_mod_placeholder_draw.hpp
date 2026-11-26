// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Mod-owned geometry and detached SDK state only. No native actor, GPU upload,
// draw call, game camera lookup, guest-memory write or production hook.
#pragma once
#include "gpu/state.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <utility>
namespace blackcoop {
struct ModPlaceholderVertex { float x,y,z;uint32_t argb; };
static_assert(sizeof(ModPlaceholderVertex)==16);
struct ModPlaceholderBox {
 float x=0,y=0,z=0,width=0,height=0,depth=0;
 uint32_t argb=0xff40c0ffu;
};
struct ModPlaceholderDraw {
 // Owned CPU data must eventually be copied into a generation-owned GPU
 // allocation and retained until its completion fence. Never submit this
 // packet directly: stream zero is intentionally unbound.
 std::array<ModPlaceholderVertex,36> vertices{};
 gpu::State state{};
};
inline bool PrepareModPlaceholderDraw(const gpu::State& target,
 const std::array<float,16>& verifiedView,
 const std::array<float,16>& verifiedProjection,
 const ModPlaceholderBox& box,ModPlaceholderDraw& output){
 // Caller supplies independently verified world-space units and matrices.
 // A finite matrix alone is NOT evidence of the game's current camera.
 auto matrix=[](const auto& m){
  bool nonzero=false;
  for(float v:m){if(!std::isfinite(v))return false;nonzero|=v!=0;}
  return nonzero;
 };
 if(!matrix(verifiedView)||!matrix(verifiedProjection)||
    !target.rt.header||!target.zs.header||!target.viewport.w||!target.viewport.h||
    !std::isfinite(target.viewport.min_z)||!std::isfinite(target.viewport.max_z)||
    target.viewport.min_z<0||target.viewport.max_z>1||
    target.viewport.min_z>=target.viewport.max_z||
    !std::isfinite(target.zscale)||target.zscale<=0)return false;
 const float scalars[]={box.x,box.y,box.z,box.width,box.height,box.depth};
 for(float v:scalars)if(!std::isfinite(v))return false;
 if(box.width<=0||box.height<=0||box.depth<=0)return false;
 ModPlaceholderDraw candidate;
 const float left=box.x-box.width/2,right=box.x+box.width/2;
 const float back=box.z-box.depth/2,front=box.z+box.depth/2,top=box.y+box.height;
 if(!(left<right)||!(back<front)||!(box.y<top))return false;
 const std::array<ModPlaceholderVertex,8> corners={{{left,box.y,back,box.argb},
  {right,box.y,back,box.argb},{right,top,back,box.argb},{left,top,back,box.argb},
  {left,box.y,front,box.argb},{right,box.y,front,box.argb},
  {right,top,front,box.argb},{left,top,front,box.argb}}};
 constexpr uint8_t indices[]={0,2,1,0,3,2,4,5,6,4,6,7,0,4,7,0,7,3,
                            1,2,6,1,6,5,3,7,6,3,6,2,0,1,5,0,5,4};
 for(unsigned i=0;i<36;++i){
  const auto v=corners[indices[i]];
  if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.z))return false;
  candidate.vertices[i]=v;
 }
 auto& s=candidate.state;
 // Copy target selection only; never inherit the game's programmable
 // shaders, textures, lights, bindings, stencil or blend/occlusion state.
 s.rt=target.rt;s.zs=target.zs;s.viewport=target.viewport;s.zscale=target.zscale;
 s.scissor_count=target.scissor_count;s.scissors=target.scissors;
 if(s.scissor_count>8)return false;
 // Pinned SDK state uses Direct3D transform slots: view2, projection3,
 // world256. FVF XYZ|DIFFUSE (0x42), unlit solid diffuse, depth-tested.
 s.transforms[2]=verifiedView;s.transforms[3]=verifiedProjection;
 s.transforms[256]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
 s.vs_handle=0x42u;
 s.rs[gpu::kRsZEnable]=1;s.rs[gpu::kRsCullMode]=0; // SDK culling disabled
 s.rs[gpu::kRsFillMode]=0x1b02u; // console D3DFILL_SOLID
 s.rs[gpu::kRsColorVertex]=1;
 s.simple[gpu::kMZFunc/4]=0x203u; // NV097/D3DCMP_LESSEQUAL
 s.simple[gpu::kMZWriteEnable/4]=0; // visual never changes world depth
 s.simple[gpu::kMColorWriteEnable/4]=0x01010101u;
 s.tss[0][gpu::kTssColorOp]=2;s.tss[0][gpu::kTssColorArg1]=0; // SELECTARG1/DIFFUSE
 s.tss[0][gpu::kTssAlphaOp]=2;s.tss[0][gpu::kTssAlphaArg1]=0;
 for(unsigned i=1;i<4;++i){s.tss[i][gpu::kTssColorOp]=1;s.tss[i][gpu::kTssAlphaOp]=1;}
 std::swap(output,candidate);return true;
}
} // namespace blackcoop
