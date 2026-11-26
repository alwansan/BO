#include "room_mod_placeholder_draw.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
using namespace blackcoop;
int main(){
 gpu::State original;
 original.rt.header=100;original.zs.header=200;
 original.viewport={4,8,800,600,0,1};original.scissor_count=1;
 original.scissors={0,0,100,100};original.vs_handle=123;
 original.streams[0]={999,48};original.textures.fill(55);
 original.simple.fill(42);original.rs.fill(19);
 original.vs=std::make_shared<gpu::VertexShader>();
 original.ps=std::make_shared<gpu::PixelShader>();
 const auto before=original;
 std::array<float,16> identity={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
 ModPlaceholderDraw output;
 assert(PrepareModPlaceholderDraw(original,identity,identity,{10,20,30,2,4,6},output));
 float xmin=1000,xmax=-1000,ymin=1000,ymax=-1000,zmin=1000,zmax=-1000;
 for(const auto& v:output.vertices){
  xmin=std::min(xmin,v.x);xmax=std::max(xmax,v.x);
  ymin=std::min(ymin,v.y);ymax=std::max(ymax,v.y);
  zmin=std::min(zmin,v.z);zmax=std::max(zmax,v.z);
  assert(v.argb==0xff40c0ffu);
 }
 assert(xmin==9&&xmax==11&&ymin==20&&ymax==24&&zmin==27&&zmax==33);
 for(unsigned i=0;i<36;i+=3){
  auto a=output.vertices[i],b=output.vertices[i+1],c=output.vertices[i+2];
  float ux=b.x-a.x,uy=b.y-a.y,uz=b.z-a.z,vx=c.x-a.x,vy=c.y-a.y,vz=c.z-a.z;
  float nx=uy*vz-uz*vy,ny=uz*vx-ux*vz,nz=ux*vy-uy*vx;
  assert(nx*nx+ny*ny+nz*nz>0); // all twelve triangles have area
 }
 assert(output.state.vs_handle==0x42&&!output.state.vs&&!output.state.ps);
 for(auto stream:output.state.streams)assert(!stream.vb&&!stream.stride);
 for(auto t:output.state.textures)assert(!t);
 assert(output.state.rt.header==100&&output.state.zs.header==200);
 assert(output.state.simple[gpu::kMZWriteEnable/4]==0);
 assert(output.state.rs[gpu::kRsZEnable]==1);
 assert(output.state.rs[gpu::kRsCullMode]==0);
 assert(output.state.simple[gpu::kMCountEnable/4]==0);
 assert(output.state.transforms[2]==identity&&output.state.transforms[3]==identity);
 assert(original.simple==before.simple&&original.rs==before.rs&&original.textures==before.textures);
 assert(original.vs==before.vs&&original.ps==before.ps&&original.vs_handle==123);
 assert(original.streams[0].vb==999&&original.streams[0].stride==48);
 auto reject=[&](gpu::State target,std::array<float,16> view,ModPlaceholderBox box){
  const auto previous=output.vertices[0];output.state.vs_handle=0xdead;
  assert(!PrepareModPlaceholderDraw(target,view,identity,box,output));
  assert(output.vertices[0].x==previous.x&&output.state.vs_handle==0xdead);
 };
 ModPlaceholderBox good{10,20,30,2,4,6};
 auto invalid=identity;invalid[0]=std::numeric_limits<float>::quiet_NaN();reject(original,invalid,good);
 reject(original,{},good);
 auto box=good;box.width=-1;reject(original,identity,box);
 box=good;box.height=std::numeric_limits<float>::infinity();reject(original,identity,box);
 box=good;box.x=std::numeric_limits<float>::max();box.width=box.x;reject(original,identity,box);
 box=good;box.x=1e20f;reject(original,identity,box); // precision-collapsed extent
 auto bad=original;bad.rt.header=0;reject(bad,identity,good);
 bad=original;bad.zs.header=0;reject(bad,identity,good);
 bad=original;bad.viewport.w=0;reject(bad,identity,good);
 bad=original;bad.scissor_count=9;reject(bad,identity,good);
 std::cout<<"MOD_PLACEHOLDER_SDK_STATE_PASS owned_box_12_triangles_no_source_state_mutation_no_gpu_submission_no_gameplay\n";
}
