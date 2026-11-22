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
    std::cout<<"ORIGINAL_PLAYER_EYE_CONTRACT_PASS: finite bounded camera-eye quantization; no mission, actor root, scene or gameplay claims\n";
}
