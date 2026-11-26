// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Asset-free integration test: actual pinned SDK D3D12 + render queue + memory.
#include <windows.h>
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include "app/app.h"
#include "core/log.h"
#include "gpu/backend.h"
#include "gpu/texture.h"
#include "kernel/kernel.h"
#include "kernel/memory.h"
#include "room_mod_placeholder_upload_kernel.hpp"

namespace kernel { uint8_t* g_base = nullptr; }
namespace app {
static HWND window = nullptr;
Settings& Get() { static Settings s; return s; }
float RenderScale(uint32_t) { return 1.0f; }
bool Widescreen() { return false; }
std::filesystem::path ExeDir() { return std::filesystem::current_path(); }
void* Window() { return window; }
std::recursive_mutex& UiLock() { static std::recursive_mutex m; return m; }
extern const GameInfo kGame{};
} // namespace app

namespace {
constexpr uint32_t kWidth = 64, kHeight = 64, kPitch = kWidth * 4;
constexpr uint32_t kClear = 0xff102030u, kRed = 0xffe02040u, kGreen = 0xff30d060u;
const std::array<float,16> kIdentity{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
void Require(bool ok, const char* reason) {
    if (!ok) throw std::runtime_error(reason);
}

struct OwnedSurface {
    uint32_t header = 0, data = 0;
    gpu::Surface state{};
    explicit OwnedSurface(uint32_t format) {
        data = mem::AllocContiguous(kPitch*kHeight, 0, 0xffffffffu, 4096);
        header = mem::AllocSystem(20);
        Require(data && header, "owned surface allocation failed");
        std::memset(kernel::HostPtr(data), 0xa5, kPitch*kHeight);
        const uint32_t encodedFormat = (format<<8) | (2u<<4) | (1u<<16);
        const uint32_t encodedSize = (kWidth-1) | ((kHeight-1)<<12) | ((kPitch/64-1)<<24);
        const uint32_t physical = mem::PhysicalFromVirtual(data);
        kernel::W32(header, 0x50001u);
        kernel::W32(header+4, physical);
        kernel::W32(header+8, 0);
        kernel::W32(header+12, encodedFormat);
        kernel::W32(header+16, encodedSize);
        state = {header, physical, encodedFormat, encodedSize};
        gpu::TextureDesc decoded;
        Require(gpu::Describe(header, decoded), "SDK did not decode owned surface header");
        Require(decoded.data == data && decoded.width == kWidth && decoded.height == kHeight &&
                decoded.pitch == kPitch && decoded.linear && decoded.format == format,
                "surface ABI does not match pinned SDK texture.cpp");
    }
    // Explicit release only after backend has flushed and has been destroyed.
    void Free() {
        mem::FreeSystem(header); mem::FreeContiguous(data); header=data=0;
    }
};

// This delegates allocation/upload/free to the actual production bridge.
// Instrumentation checks queued resource use has ended before freeing.
struct CheckedUpload {
    gpu::Backend& backend;
    blackcoop::ModPlaceholderUploadKernel actual;
    uint32_t allocated=0, freed=0, quarantined=0;
    bool earlyFree=false, badRelease=false;
    explicit CheckedUpload(gpu::Backend& b):backend(b),actual(b) {}
    bool BackendIs(const gpu::Backend& b) const noexcept { return actual.BackendIs(b); }
    bool Allocate(blackcoop::ModPlaceholderLease& l) {
        if (!actual.Allocate(l)) return false;
        ++allocated; return true;
    }
    bool Upload(const blackcoop::ModPlaceholderLease& l,const void* p,uint32_t n) noexcept {
        return actual.Upload(l,p,n);
    }
    void Free(const blackcoop::ModPlaceholderLease& l) noexcept {
        earlyFree |= backend.Busy(l.header);
        actual.Free(l);
        badRelease |= mem::AllocationSize(l.header)!=0 || mem::AllocationSize(l.data)!=0;
        ++freed;
    }
    void Quarantine(const blackcoop::ModPlaceholderLease& l) noexcept {
        ++quarantined; actual.Quarantine(l);
    }
};

uint32_t Pixel(const OwnedSurface& color,uint32_t x,uint32_t y) {
    return kernel::R32(color.data+y*kPitch+x*4);
}
void ExpectColor(uint32_t actual,uint32_t expected,const char* label) {
    std::printf("%s actual=%08x expected=%08x\n",label,actual,expected);
    for (unsigned shift: {0u,8u,16u,24u})
        Require(std::abs(int((actual>>shift)&255)-int((expected>>shift)&255))<=2,
                "GPU readback pixel mismatch");
}
bool SameInputState(const gpu::State& a,const gpu::State& b) {
    return a.simple==b.simple && a.rs==b.rs && a.tss==b.tss && a.textures==b.textures &&
        a.palettes==b.palettes && a.transforms==b.transforms && a.vsc==b.vsc && a.psc==b.psc &&
        a.vs==b.vs && a.ps==b.ps && a.vs_handle==b.vs_handle &&
        a.streams[0].vb==b.streams[0].vb && a.streams[0].stride==b.streams[0].stride &&
        a.rt.header==b.rt.header && a.zs.header==b.zs.header &&
        a.rt.data==b.rt.data && a.zs.data==b.zs.data && a.viewport.w==b.viewport.w &&
        a.viewport.h==b.viewport.h && a.scissors==b.scissors && a.scissor_count==b.scissor_count;
}
} // namespace

int main() {
    try {
        logx::Init(".", "sdk-render-lab");
        Require(mem::Init(), "actual SDK 4 GiB guest address reservation failed");
        kernel::g_base=mem::Base();
        WNDCLASSW wc{}; wc.lpfnWndProc=DefWindowProcW; wc.hInstance=GetModuleHandleW(nullptr);
        wc.lpszClassName=L"BOAssetFreeRenderLab";
        Require(RegisterClassW(&wc)!=0,"RegisterClass failed");
        app::window=CreateWindowExW(0,wc.lpszClassName,L"BO SDK WARP lab",WS_OVERLAPPEDWINDOW,
                                   0,0,128,128,nullptr,nullptr,wc.hInstance,nullptr);
        Require(app::window!=nullptr,"hidden window creation failed");
        IMGUI_CHECKVERSION(); ImGui::CreateContext();
        Require(ImGui_ImplWin32_Init(app::window),"ImGui Win32 initialization failed");
        // Actual SDK renderer and actual SDK deferred queue, no fake backend.
        auto backend=gpu::CreateThreadedBackend(gpu::CreateBackend());
        Require(backend && backend->Init(kWidth,kHeight),"actual SDK WARP Backend::Init failed");
        OwnedSurface color(0x12),depth(0x2e); // LIN_A8R8G8B8 / LIN_D24S8.
        gpu::State original;
        original.rt=color.state; original.zs=depth.state;
        original.viewport={0,0,kWidth,kHeight,0,1}; original.rs[gpu::kRsZEnable]=1;
        // Sentinel input state is deliberately unsuitable for rendering; the
        // placeholder helper must build its own detached FVF/transform state.
        // The real queue snapshots EVERY bound resource header even for Clear:
        // use a valid lab-owned header as the deliberately unwanted binding.
        original.vs_handle=0xdead; original.textures.fill(color.header);
        original.transforms[2].fill(17); original.transforms[3].fill(19);
        const gpu::State before=original;
        CheckedUpload upload(*backend);
        const auto memoryBefore=mem::Query();
        auto sync=[&]{ backend->Sync(color.data,kPitch*kHeight); };
        auto clear=[&](float z){
            backend->Clear(original,0,nullptr,gpu::kClearTarget|gpu::kClearZ,kClear,z,0);
            sync();
            ExpectColor(Pixel(color,32,32),kClear,"owned_clear_center");
        };
        auto draw=[&](float z,uint32_t argb){
            blackcoop::ModPlaceholderDraw packet;
            Require(blackcoop::PrepareModPlaceholderDraw(original,kIdentity,kIdentity,
                    {0,-0.4f,z,0.8f,0.8f,0.08f,argb},packet),"placeholder preparation failed");
            Require(blackcoop::SubmitModPlaceholder(*backend,upload,packet,[]{return true;})==
                    blackcoop::ModPlaceholderSubmitResult::Completed,"fenced submission did not complete");
            sync();
            Require(SameInputState(original,before),"original input SDK state changed");
            ExpectColor(Pixel(color,2,2),kClear,"outside_box_unchanged");
        };
        clear(1.0f);
        draw(0.25f,kRed);
        ExpectColor(Pixel(color,32,32),kRed,"visible_mod_box");
        // Placeholder MUST NOT write depth: this farther box remains visible.
        draw(0.70f,kGreen);
        ExpectColor(Pixel(color,32,32),kGreen,"far_box_proves_no_placeholder_depth_write");
        // Clear an owned depth plane in front: actual depth test must reject box.
        clear(0.1f);
        draw(0.45f,kRed);
        ExpectColor(Pixel(color,32,32),kClear,"owned_depth_occludes_box");
        // Same geometry becomes visible after the owned depth buffer is cleared.
        clear(1.0f);
        draw(0.45f,kGreen);
        ExpectColor(Pixel(color,32,32),kGreen,"depth_clear_restores_visibility");
        backend->Flush();
        const auto memoryAfter=mem::Query();
        Require(upload.allocated==4 && upload.freed==4 && !upload.quarantined &&
                !upload.earlyFree && !upload.badRelease,"kernel upload lease/fence/release invariant failed");
        Require(memoryBefore.contiguous_used==memoryAfter.contiguous_used &&
                memoryBefore.system_used==memoryAfter.system_used,"upload allocations leaked");
        Require(SameInputState(original,before),"source state mutated");
        ImGui_ImplDX12_Shutdown();
        backend.reset(); // no surface/header release while backend can use them.
        color.Free(); depth.Free();
        ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext(); DestroyWindow(app::window);
        std::ofstream result("rasterization-result.json");
        result << R"({"passed":true,"backend":"actual pinned SDK D3D12 through actual threaded backend","adapter":"required WARP software","owned_color_readback":true,"depth_occlusion_verified":true,"depth_readback_available":false,"placeholder_depth_write_absent":true,"kernel_upload_allocations":4,"fenced_upload_frees":4,"source_state_unchanged":true,"assets_used":false,"game_camera_verified":false,"gameplay_verified":false})";
        Require(bool(result),"result metadata write failed");
        std::puts("ACTUAL_SDK_WARP_RASTERIZATION_PASS color_depth_occlusion_no_depth_write_real_queue_fence_no_assets_no_gameplay");
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr,"ACTUAL_SDK_WARP_RASTERIZATION_FAILED: %s\n",e.what());
        return 1;
    }
}
