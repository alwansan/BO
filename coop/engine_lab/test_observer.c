/* Compile and exercise the lab observer accounting, not the BLACK engine.
 * Deliberately supplies stub memory and proxy lookup; never creates gameplay evidence.
 */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if defined(__i386__) || defined(__x86_64__) || defined(_M_X64)
#include <xmmintrin.h>
#else
/* Only the fixture substitutes the x86 FP control register on ARM. */
static unsigned _mm_getcsr(void) { return 0x1F80; }
static void _mm_setcsr(unsigned value) { (void)value; }
#endif
#include <fenv.h>
typedef long LONG;
typedef unsigned long long ULONGLONG;
static LONG InterlockedCompareExchange(volatile LONG *p, LONG value, LONG expected) { LONG old=*p; if(old==expected)*p=value; return old; }
static LONG InterlockedExchange(volatile LONG *p, LONG value) { LONG old=*p; *p=value; return old; }
static LONG InterlockedIncrement(volatile LONG *p) { return ++*p; }
static ULONGLONG GetTickCount64(void) { return 1000; }
static void ExitProcess(unsigned code) { (void)code; abort(); }
static unsigned char memory[0x300000];
static uintptr_t g_xbox_mem_offset;
#define GUEST32(at) (*(uint32_t *)(memory+(at)))
#define GUEST16(at) (*(uint16_t *)(memory+(at)))
#define GUEST8(at) (*(uint8_t *)(memory+(at)))
#define BLACK_ACTOR_BYTES 0x900u
#define BLACK_ACTOR_AT 0x30u
#define BLACK_CORE_RUNNING 0x1Cu
static int black_pc_ram_span(uint32_t p, uint32_t n) { return p && p<sizeof(memory) && n<=sizeof(memory)-p; }
static int black_p2_state, black_p2_in;
static uint32_t black_p2_shadow, black_p2_proxy;
static uint32_t black_proxy_motion(uint32_t proxy) { return proxy == 0x9000 ? 0xA000 : proxy == 0x9100 ? 0xA100 : 0; }
#include "observer.inc"
int main(void) {
    unsigned char report[20];
    g_xbox_mem_offset=(uintptr_t)memory;
    assert(bo_lab_pad_report(1,report)==1 && report[4]==0);
    bo_lab_ui_page("ProfileOptions");
    assert(bo_lab_ui_now==5 && bo_lab_ui_ticks==1);
    bo_lab_ui_page("PrivateUnrecognizedPage");
    assert(bo_lab_ui_now==7 && bo_lab_ui_changes==2);
    bo_lab_after();
    assert(bo_lab.mission_ticks==0 && bo_lab.phases_completed==0);
    GUEST32(0x2D45D4)=0x1000;
    GUEST32(0x1000+0x5AE0)=BLACK_CORE_RUNNING;
    GUEST32(0x2D12B8)=0x7000;
    black_p2_state=2;
    black_p2_shadow=0x8000;
    black_p2_proxy=0x9100;
    GUEST32(0x1030+0xB4)=0x9000;
    GUEST32(0x8000+0xB4)=0x9100;
    GUEST32(0x1030+0x29C)=0xB000;
    GUEST32(0x8000+0x29C)=0xB100;
    bo_lab_before(); bo_lab_after();
    assert(bo_lab.mission_ticks==1 && bo_lab.restored_ticks==1 && bo_lab.distinct_proxy_ticks==1);
    bo_lab_before(); GUEST8(0x1030+0x50)=1; bo_lab_after();
    assert(bo_lab.restore_failures==1 && bo_lab.restore_actor_grid_failures==1);
    assert(bo_lab.restore_grid_node_failures==1 && bo_lab.restore_grid_other_failures==0);
    bo_lab_before(); GUEST8(0x1030+0x298)=1; bo_lab_after();
    assert(bo_lab.restore_failures==2 && bo_lab.restore_actor_weapon_failures==1);
    assert(bo_lab.restore_scope_failures==0 && bo_lab.restore_actor_motion_failures==0);
    bo_lab_before(); GUEST8(0x1030+0x60)=1; bo_lab_after();
    assert(bo_lab.restore_grid_other_failures==1);
    bo_lab_before(); GUEST32(0x8000+0xB4)=0; bo_lab_after();
    assert(bo_lab.p2_proxy_missing_ticks==1 && bo_lab.p2_body_missing_ticks==1);
    assert(bo_lab.proxy_pool_unknown_ticks>=1);
    bo_lab_before(); GUEST32(0x2D45F4)=0x20000;
    memset(memory+0x20000+0x2960,1,20); bo_lab_after();
    assert(bo_lab.proxy_pool_low_ticks==1);
    bo_lab_before(); GUEST8(0x7000+0x821)=1; bo_lab_after();
    assert(bo_lab.scripted_control_ticks>=1);
    /* Earlier intentional missing-proxy cases already counted failures.
     * Assert the final duplicate-proxy rejection adds exactly ONE. */
    const unsigned prior_proxy_failures = bo_lab.proxy_failures;
    bo_lab_before(); GUEST32(0x8000+0xB4)=0x9000; bo_lab_after();
    assert(bo_lab.proxy_failures==prior_proxy_failures+1);
    assert(bo_lab.phases_completed==0);
    /* Header is encoded by the actual scheduler; phase-separated delivery. */
    InterlockedExchange(&bo_lab_phase,3);
    assert(bo_lab_pad_report(1,report)==1 && report[13]==0x80);
    assert(bo_lab_moves[1]==1 && bo_lab_fires[1]==0);
    return 0;
}
