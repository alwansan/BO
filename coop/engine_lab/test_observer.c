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
static void bo_audio_write(void) {}
static LONG current_thread = 1;
static LONG GetCurrentThreadId(void) { return current_thread; }
#include "unit_observer.inc"
#include "objective_observer.inc"
#include "observer.inc"

static void test_objective_frames(void)
{
    const uint32_t world = 0x1000, level_data = 0x21000, authored = 0x21400, live = 0x22000;
    GUEST32(world + 0x5B2C) = level_data;
    GUEST32(world + 0x930) = live;
    GUEST32(world + 0x934) = 2;
    GUEST32(level_data) = 17;
    GUEST32(level_data + 4) = 2;
    GUEST32(level_data + 8) = authored;
    GUEST32(live) = authored;
    GUEST32(live + 0xC) = authored + 0x18;
    GUEST32(live + 4) = GUEST32(live + 0x10) = 1;
    bo_objective_begin();
    GUEST32(live + 0x10) = 2;
    GUEST32(live + 0x14) = 3;
    bo_objective_end();
    assert(bo_objectives.verified_frames == 1 && bo_objectives.max_records == 2);
    assert(bo_objectives.changed_records == 1 && bo_objectives.counter_advances == 1);
    assert(bo_objectives.entries_reaching_state_3 == 1);

    bo_objective_begin();
    GUEST32(live + 4) = 2;
    GUEST32(live + 0xC) = authored;
    bo_objective_end();
    assert(bo_objectives.rejected_frames == 1 && bo_objectives.changed_records == 1);
    GUEST32(live + 0xC) = authored + 0x18;

    bo_objective_begin();
    GUEST32(world + 0x5AE4) = 0x1E;
    bo_objective_end();
    assert(bo_objectives.rejected_frames == 2 && bo_objectives.verified_frames == 1);
    bo_objective_begin();
    bo_objective_end();
    assert(bo_objectives.rejected_frames == 2);
    GUEST32(world + 0x5AE4) = 0x1D;

    bo_objective_begin();
    GUEST8(world + 0x5AED) = 2;
    bo_objective_end();
    assert(bo_objectives.rejected_frames == 3 && bo_objectives.verified_frames == 1);
    GUEST8(world + 0x5AED) = 0;
    puts("BO_ENGINE_OBJECTIVE_FRAME_FIXTURE_PASS");

    BoObjectiveWriterObservation writer = bo_objective_writer_before(live + 0xC);
    GUEST32(live + 0x10)++;
    bo_objective_writer_after(&writer);
    assert(bo_objectives.verified_writer_calls == 1 && bo_objectives.changed_writer_calls == 1);

    writer = bo_objective_writer_before(live + 4); /* Interior pointer fails before original call. */
    bo_objective_writer_after(&writer);
    assert(bo_objectives.rejected_writer_calls == 1);
    writer = bo_objective_writer_before(live);
    GUEST32(live) = authored + 0x18;
    bo_objective_writer_after(&writer);
    assert(bo_objectives.rejected_writer_calls == 2 && bo_objectives.changed_writer_calls == 1);
    GUEST32(live) = authored;

    current_thread = 2;
    writer = bo_objective_writer_before(live);
    bo_objective_writer_after(&writer);
    current_thread = 1;
    assert(bo_objective_writer_other_thread_calls == 1 && bo_objectives.verified_writer_calls == 1);
    puts("BO_ENGINE_OBJECTIVE_WRITER_FIXTURE_PASS");
}

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
    assert(bo_lab.restored_immutable_ticks==1 && bo_lab.p2_distinct_physical_ticks==1);
    assert(bo_lab.p2_playing_ticks==1);
    bo_lab_before(); GUEST8(0x1030+0x50)=1; bo_lab_after();
    assert(bo_lab.restore_failures==1 && bo_lab.restore_actor_grid_failures==1);
    assert(bo_lab.restore_grid_node_failures==1 && bo_lab.restore_grid_other_failures==0);
    assert(bo_lab.restored_immutable_ticks==2 && bo_lab.restore_grid_only_ticks==1);
    assert(bo_lab.grid_region_links_ticks==1);
    assert(bo_lab.grid_region_head_ticks==0 && bo_lab.grid_region_cell_ticks==0);
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
    assert(bo_lab.scripted_control_ticks>=1 && bo_lab.scripted_owner_ticks>=1);
    assert(bo_lab.scripted_actor_ticks==0);
    assert(bo_lab.scripted_unheld_ticks>=1);
    assert(bo_lab.p2_playing_ticks==bo_lab.mission_ticks);
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
    /* The legitimate original world-grid node head can change without any
     * P1 actor context or neighbor link corruption. Count it separately. */
    const unsigned before_head = bo_lab.grid_region_head_ticks;
    const unsigned before_grid_only = bo_lab.restore_grid_only_ticks;
    bo_lab_before(); GUEST8(0x1030+0x20u)^=1u; bo_lab_after();
    assert(bo_lab.grid_region_head_ticks == before_head+1u);
    assert(bo_lab.restore_grid_only_ticks == before_grid_only+1u);
    assert(bo_lab.grid_region_cell_ticks == 0u);
    GUEST32(0x1000+0x5AE4) = 0x1D;
    bo_unit_bind_frame_thread();
    test_objective_frames();
    bo_lab_restart_scene = bo_lab_scene(0x1000);
    assert(!bo_lab_restart_headless_menu("PauseMenu"));
    InterlockedExchange(&bo_lab_restart_action, BO_RESTART_OPEN);
    assert(bo_lab_restart_headless_menu("PauseMenu") && !bo_lab_restart_headless_menu("OptionsMenuXbox"));
    bo_lab_restart_menu_tick("PauseMenu", 3, 1);
    bo_lab_restart_menu_tick("PauseMenu", 3, 0);
    assert(bo_lab_restart_armed && bo_lab_restart_action == BO_RESTART_NEUTRAL);
    bo_lab_restart_menu_tick("ConfirmWarning", 0, 1);
    bo_lab_restart_gamecode("QuitLevel");
    assert(!bo_lab_restart_requested);
    bo_lab_restart_gamecode("RestartLevel");
    assert(bo_lab_restart_requested && bo_lab_restart_action == BO_RESTART_NEUTRAL);
    assert(!bo_lab_restart_headless_menu("ConfirmWarning"));
    assert(bo_unit_enter(BO_UNIT_UNLINK, 0x1000+0x49D0) == 0);
    bo_lab_restart_after_unlink(0, 7, 1);
    assert(!bo_lab_restart_units[0].count && !bo_lab_restart_tick());
    GUEST32(0x1000+0x5AE4) = 0x1E;
    bo_lab_restart_after_unlink(0, 7, 1);
    assert(bo_lab_restart_units[0].count == 1 && !bo_lab_restart_tick());
    /* The restart can allocate a different world, unit, and object range. */
    const uint32_t reloaded = 0x10000, unit = reloaded + 0x49D0;
    const uint32_t header = 0x23000, table = 0x24000, object = 0x25000;
    GUEST32(0x2D45D4) = reloaded;
    GUEST32(reloaded + 0x5AE0) = BLACK_CORE_RUNNING;
    GUEST32(reloaded + 0x5AE4) = 0x1D;
    GUEST32(unit) = 0x1C;
    GUEST32(unit + 4) = GUEST32(unit + 8) = 8;
    GUEST32(unit + 0xC) = header;
    GUEST32(unit + 0x14) = object;
    GUEST32(header) = 24;
    GUEST32(header + 0x1C) = table;
    GUEST16(header + 0x90) = 1;
    GUEST32(object) = 0x28EA60;
    GUEST32(object + 0xD0) = table;
    GUEST32(reloaded + 0x5CB4) = object;
    GUEST8(unit + 0x38) = 1;
    const int reload_slot = bo_unit_enter(BO_UNIT_POLL, unit);
    assert(reload_slot == 0);
    bo_lab_restart_after_poll(reload_slot, unit, bo_unit_after_poll(reload_slot, unit, 3, 1));
    bo_lab_restart_after_link(reload_slot, unit, bo_unit_after_link(reload_slot, unit));
    assert(!bo_lab_restart_tick());
    GUEST32(unit + 4) = GUEST32(unit + 8) = 7;
    bo_lab_restart_after_poll(reload_slot, unit, bo_unit_after_poll(reload_slot, unit, 3, 1));
    assert(!bo_lab_restart_tick());
    bo_lab_restart_after_link(reload_slot, unit, bo_unit_after_link(reload_slot, unit));
    GUEST8(reloaded + 0x5AED) = 1;
    assert(!bo_lab_restart_tick());
    GUEST8(reloaded + 0x5AED) = 0;
    GUEST32(reloaded + 0x5CB4) = 0;
    assert(!bo_lab_restart_tick());
    GUEST32(reloaded + 0x5CB4) = object;
    assert(!bo_unit_counts[BO_UNIT_REPUBLISHED]);
    assert(bo_lab_restart_tick());
    puts("BO_ENGINE_RESTART_FIXTURE_PASS");
    return 0;
}
