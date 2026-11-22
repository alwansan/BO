// BO-COOP: read-only observer of original title's already parsed D3D8
// declaration at shader creation. No guest data, no draw call, no actor input.
// This is NOT proof the observed shader is used by a fullbody human MODEL.
#pragma once
#include "coop/gpu_decl_contract.hpp"
#include "coop/room_world_tick.hpp"
#include "gpu/state.h"

namespace blackcoop {
inline void ObserveOriginalGpuDecl(const gpu::VertexShader& shader) noexcept {
    gpu_probe::Decl desc{};
    for(unsigned i=0;i<16;++i) {
        desc.input[i]={shader.inputs[i].stream,
                       shader.inputs[i].offset,shader.inputs[i].type};
        desc.stride[i]=shader.stream_stride[i];
    }
    static gpu_probe::Once observed;
    const uint32_t bits=observed.NewBits(gpu_probe::Classify(desc));
    if(bits&gpu_probe::kStride36)SendSafeWorldCodeToAndroid(4);
    if(bits&gpu_probe::kSlot12Byte4)SendSafeWorldCodeToAndroid(5);
    if(bits&gpu_probe::kSlot16Byte4)SendSafeWorldCodeToAndroid(6);
    if(bits&gpu_probe::kSlot20Byte4)SendSafeWorldCodeToAndroid(7);
    if(bits&gpu_probe::kSlot24PackedNormal)SendSafeWorldCodeToAndroid(8);
    if(bits&gpu_probe::kSlot28Float2)SendSafeWorldCodeToAndroid(9);
}
} // namespace blackcoop
