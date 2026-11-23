#include "room_original_eye_contract.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
int main(){
    blackcoop::OriginalEyeCm out{};
    assert(blackcoop::QuantizeOriginalEye(1.234f,-2.0f,0.005f,out));
    assert(out.x==123&&out.y==-200&&out.z==0);
    assert(blackcoop::QuantizeOriginalEye(0.f,0.f,1.0f,out));
    assert(out.x==0&&out.y==0&&out.z==100);
    const auto nan=std::numeric_limits<float>::quiet_NaN();
    const auto inf=std::numeric_limits<float>::infinity();
    assert(!blackcoop::QuantizeOriginalEye(nan,0,0,out));
    assert(!blackcoop::QuantizeOriginalEye(0,inf,0,out));
    assert(!blackcoop::QuantizeOriginalEye(0,0,-inf,out));
    assert(!blackcoop::QuantizeOriginalEye(200001.f,0,0,out));
    assert(!blackcoop::QuantizeOriginalEye(0,0,-200001.f,out));
    assert(blackcoop::QuantizeOriginalEye(200000.f,-200000.f,0.f,out));
    assert(out.x==20000000&&out.y==-20000000);
    blackcoop::OriginalBodyFeetCm feet{};
    assert(blackcoop::QuantizeOriginalBodyFeet(-1.234f,3.f,.005f,feet));
    assert(feet.x==-123&&feet.y==300&&feet.z==0);
    assert(!blackcoop::QuantizeOriginalBodyFeet(nan,0.f,0.f,feet));
    assert(!blackcoop::QuantizeOriginalBodyFeet(0.f,inf,0.f,feet));
    assert(!blackcoop::QuantizeOriginalBodyFeet(0.f,0.f,200001.f,feet));
    int32_t yaw=900;
    assert(blackcoop::QuantizeOriginalForwardYaw(0.f,1.f,yaw)&&yaw==0);
    assert(blackcoop::QuantizeOriginalForwardYaw(1.f,0.f,yaw)&&yaw==9000);
    assert(blackcoop::QuantizeOriginalForwardYaw(-1.f,0.f,yaw)&&yaw==-9000);
    assert(blackcoop::QuantizeOriginalForwardYaw(0.f,-1.f,yaw)&&std::abs(yaw)==18000);
    assert(!blackcoop::QuantizeOriginalForwardYaw(0.f,0.f,yaw));
    assert(!blackcoop::QuantizeOriginalForwardYaw(nan,0.f,yaw));
    assert(!blackcoop::QuantizeOriginalForwardYaw(0.f,inf,yaw));
    assert(!blackcoop::QuantizeOriginalForwardYaw(3.f,0.f,yaw));
    float x=0,z=0;
    assert(blackcoop::RotateOriginalSoldierHorizontalBasis(0,1,0,9000,x,z));
    assert(std::fabs(x-1.f)<0.0001f&&std::fabs(z)<0.0001f);
    assert(blackcoop::RotateOriginalSoldierHorizontalBasis(0,1,0,-9000,x,z));
    assert(std::fabs(x+1.f)<0.0001f&&std::fabs(z)<0.0001f);
    assert(!blackcoop::RotateOriginalSoldierHorizontalBasis(nan,1,0,0,x,z));
    assert(!blackcoop::RotateOriginalSoldierHorizontalBasis(1,0,18001,0,x,z));
    std::cout<<"ORIGINAL_PLAYER_EYE_AND_BODY_FEET_CONTRACT_PASS: original +0xF0 forward heading, actor +0xA0 feet, safe planar soldier orientation; not a remote physics actor or scene READY\n";
}
