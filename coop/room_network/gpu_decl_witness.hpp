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
// Called only immediately before the original g_backend->Draw path,
// while its existing D3D8 lock is held, and only for positive draw counts.
// No shader handle, GPU buffer address, XBE bytes or raw MODEL data are read
// out of the app. A bound draw is NOT proof of which character it renders.
inline void ObserveOriginalGpuDraw(const gpu::State& state, bool indexed) noexcept {
    if(!state.vs)return;
    gpu_probe::Decl desc{};
    gpu_probe::BoundStreams bound{};
    for(unsigned i=0;i<16;++i) {
        desc.input[i]={state.vs->inputs[i].stream,
                       state.vs->inputs[i].offset,state.vs->inputs[i].type};
        desc.stride[i]=state.vs->stream_stride[i];
        bound[i]={state.streams[i].vb!=0,state.streams[i].stride};
    }
    static gpu_probe::Once observedDraws;
    const uint32_t bits=observedDraws.NewBits(
        gpu_probe::ClassifyBoundDraw(desc,bound,indexed));
    if(bits&gpu_probe::kDrawIndexedActive36)SendSafeWorldCodeToAndroid(10);
    if(bits&gpu_probe::kDrawArrayActive36)SendSafeWorldCodeToAndroid(11);
    if(bits&gpu_probe::kDrawPacked12And20SameStream)SendSafeWorldCodeToAndroid(12);
}
} // namespace blackcoop
