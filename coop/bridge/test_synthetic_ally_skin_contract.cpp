// Synthetic rig/math only. Never uses original BLACK MODEL vertex data.
#include "synthetic_ally_skin_contract.hpp"
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
using namespace blackcoop::synthetic;
static bool Nearly(float a,float b){return std::abs(a-b)<0.0005f;}
static void Check(Vec3 v,float x,float y,float z){
    assert(Nearly(v.x,x)&&Nearly(v.y,y)&&Nearly(v.z,z));
}
int main(){
    SyntheticRig r;
    std::array<int16_t,2> parents={-1,0};
    std::array<Mat4,2> local={Translate(2,0,0),Translate(0,3,0)};
    std::array<Mat4,2> inverse_bind={Identity(),Identity()};
    assert(r.Prepare(parents,local,inverse_bind));
    assert(r.JointCount()==2);
    WeightedVertex v;
    v.position={1,1,1};
    v.joint={0,1,0,0};
    v.weight={0.25f,0.75f,0.f,0.f};
    Vec3 out;
    assert(r.Skin(v,out));
    Check(out,3,3.25f,1);
    // Parent can appear *after* child in an RCB joint array. Do not assume
    // index order is topological, and do not treat MODEL +58 as parents.
    std::array<int16_t,3> reorder={2,-1,1};
    std::array<Mat4,3> pose={Translate(0,0,4),Translate(1,0,0),Translate(0,2,0)};
    std::array<Mat4,3> inverse={Identity(),Identity(),Identity()};
    assert(r.Prepare(reorder,pose,inverse));
    assert(r.JointCount()==3);
    v.joint={0,0,0,0};v.weight={1,0,0,0};
    assert(r.Skin(v,out));
    Check(out,2,3,5);
    // Mathematical inverse-bind cancels rest transforms (synthetic only).
    std::array<int16_t,2> root={-1,0};
    std::array<Mat4,2> rest={Translate(5,0,0),Translate(0,3,0)};
    std::array<Mat4,2> inverses={Translate(-5,0,0),Translate(-5,-3,0)};
    assert(r.Prepare(root,rest,inverses));
    v.joint={1,0,0,0};
    assert(r.Skin(v,out));Check(out,1,1,1);
    // Reject skin weights / indices rather than applying corrupted values.
    v.weight={0.4f,0.f,0.f,0.f};assert(!r.Skin(v,out));
    v.weight={1.f,0,0,0};v.joint={2,0,0,0};assert(!r.Skin(v,out));
    v.joint={1,1,0,0};v.weight={.5f,.5f,0,0};assert(!r.Skin(v,out));
    v.weight={std::numeric_limits<float>::quiet_NaN(),0,0,0};assert(!r.Skin(v,out));
    v.joint={1,0,0,0};v.weight={1,0,0,0};
    v.position.x=std::numeric_limits<float>::infinity();assert(!r.Skin(v,out));
    v.position={1,1,1};
    // Cyclic and disconnected-invalid RCB parent descriptors fail closed.
    std::array<int16_t,2> cyc={1,0};
    assert(!r.Prepare(cyc,rest,inverses));
    assert(r.JointCount()==0);
    assert(!r.Skin(v,out));
    std::array<int16_t,2> self={-1,1};
    assert(!r.Prepare(self,rest,inverses));
    std::array<int16_t,2> over={-1,3};
    assert(!r.Prepare(over,rest,inverses));
    auto bad=rest;
    bad[0].a[0]=std::numeric_limits<float>::quiet_NaN();
    assert(!r.Prepare(root,bad,inverses));
    assert(!r.Prepare(root,std::span<const Mat4>(rest.data(),1),inverses));
    std::cout<<"SYNTHETIC_ALLY_POSE_SKIN_CONTRACT_PASS: hierarchy/weights/inverse-bind/cycles validated, NO original RCB skin mapping\n";
}
