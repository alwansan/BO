/* MIT. Disposable lab only. No guest writes, direct mission starts, or fake actors.
 * Included immediately before sub_001F1080, after native P2 helpers are defined.
 * USB reports are the only game inputs. All output is allowlisted integer aggregates.
 */
#include "schedule.h"
#include <stdio.h>
#include <math.h>
#include <fenv.h>

typedef struct BoLabCounts {
    unsigned mission_ticks, p2_ready_ticks, phases_completed;
    unsigned distinct_actor_ticks, distinct_proxy_ticks, distinct_weapon_ticks;
    unsigned restored_ticks, restore_failures, proxy_failures, epoch_changes, invalid_samples;
    /* Bounded region counts; never expose private original game bytes. */
    unsigned restore_scope_failures, restore_actor_grid_failures;
    unsigned restore_actor_motion_failures, restore_actor_weapon_failures;
    unsigned restore_actor_model_failures, restore_actor_tail_failures;
    /* Source 0x20..0x57: original grid-node links are world-owned. */
    unsigned restore_grid_node_failures, restore_grid_other_failures;
    unsigned p1_proxy_missing_ticks, p1_body_missing_ticks;
    unsigned p2_proxy_missing_ticks, p2_body_missing_ticks;
    unsigned proxy_pool_low_ticks, proxy_pool_unknown_ticks, scripted_control_ticks;
    /* SECONDARY diagnostic: does NOT alter the strict original game pass. */
    unsigned restored_immutable_ticks, restore_grid_only_ticks;
    unsigned grid_region_head_ticks, grid_region_cell_ticks, grid_region_links_ticks;
    unsigned p2_distinct_physical_ticks, p2_setup_wait_ticks, p2_playing_ticks;
    unsigned scripted_owner_ticks, scripted_actor_ticks, scripted_both_ticks, scripted_unheld_ticks;
    unsigned p1_move_mm, p2_move_mm, p1_idle_mm, p2_idle_mm;
    unsigned p1_body_move_mm, p2_body_move_mm, p1_shots, p2_shots;
} BoLabCounts;
static BoLabCounts bo_lab;
static volatile LONG bo_lab_mission, bo_lab_phase;
static volatile LONG bo_lab_pads[2], bo_lab_moves[2], bo_lab_fires[2];
static ULONGLONG bo_lab_boot;
static unsigned bo_lab_phase_ticks, bo_lab_seen_phase = ~0u;
static uint32_t bo_lab_epoch, bo_lab_before_core, bo_lab_before_owner;
static unsigned char bo_lab_before_actor[BLACK_ACTOR_BYTES];
static int bo_lab_before_valid;
static float bo_lab_origins[2][3], bo_lab_body_origins[2][3];
static uint32_t bo_lab_weapon_was[2], bo_lab_mag_was[2];
static unsigned bo_lab_ammo_was[2];
static int bo_lab_control_ready;
static unsigned bo_lab_ui_ticks, bo_lab_ui_now, bo_lab_ui_changes;
static int bo_lab_ui_marker_emitted;

int bo_lab_pad_report(int pad, unsigned char *out)
{
    ULONGLONG now = GetTickCount64();
    BoLabInput in;
    if (pad < 0 || pad > 1) return 0;
    /* Only the USB thread initializes this clock; observer never writes it. */
    if (!bo_lab_boot) { bo_lab_boot = now; fputs("BO_ENGINE_PAD_STARTED\n", stderr); }
    in = bo_lab_input((unsigned)pad, now - bo_lab_boot,
        InterlockedCompareExchange(&bo_lab_mission, 0, 0) != 0,
        (unsigned)InterlockedCompareExchange(&bo_lab_phase, 0, 0));
    bo_lab_encode(in, out);
    InterlockedIncrement(&bo_lab_pads[pad]);
    if (in.move) InterlockedIncrement(&bo_lab_moves[pad]);
    if (in.fire) InterlockedIncrement(&bo_lab_fires[pad]);
    return 1;
}

static void bo_lab_write(void)
{
    const char *path = getenv("BO_LAB_AGGREGATE");
    FILE *file;
    if (!path || !(file = fopen(path, "wb"))) return;
    fprintf(file,
        "{\"mission_ticks\":%u,\"p2_ready_ticks\":%u,\"phases_completed\":%u,"
        "\"distinct_actor_ticks\":%u,\"distinct_proxy_ticks\":%u,\"distinct_weapon_ticks\":%u,"
        "\"restored_ticks\":%u,\"restore_failures\":%u,\"proxy_failures\":%u,"
        "\"restore_scope_failures\":%u,\"restore_actor_grid_failures\":%u,"
        "\"restore_actor_motion_failures\":%u,\"restore_actor_weapon_failures\":%u,"
        "\"restore_actor_model_failures\":%u,\"restore_actor_tail_failures\":%u,"
        "\"restore_grid_node_failures\":%u,\"restore_grid_other_failures\":%u,"
        "\"p1_proxy_missing_ticks\":%u,\"p1_body_missing_ticks\":%u,"
        "\"p2_proxy_missing_ticks\":%u,\"p2_body_missing_ticks\":%u,"
        "\"proxy_pool_low_ticks\":%u,\"proxy_pool_unknown_ticks\":%u,"
        "\"scripted_control_ticks\":%u,"
        "\"restored_immutable_ticks\":%u,\"restore_grid_only_ticks\":%u,"
        "\"grid_region_head_ticks\":%u,\"grid_region_cell_ticks\":%u,"
        "\"grid_region_links_ticks\":%u,\"p2_distinct_physical_ticks\":%u,"
        "\"p2_setup_wait_ticks\":%u,\"p2_playing_ticks\":%u,"
        "\"scripted_owner_ticks\":%u,\"scripted_actor_ticks\":%u,"
        "\"scripted_both_ticks\":%u,\"scripted_unheld_ticks\":%u,"
        "\"epoch_changes\":%u,\"invalid_samples\":%u,"
        "\"p1_move_mm\":%u,\"p2_move_mm\":%u,\"p1_idle_mm\":%u,\"p2_idle_mm\":%u,"
        "\"p1_body_move_mm\":%u,\"p2_body_move_mm\":%u,\"p1_shots\":%u,\"p2_shots\":%u,"
        "\"pad0_reports\":%lu,\"pad1_reports\":%lu,"
        "\"input0_move_reports\":%lu,\"input1_move_reports\":%lu,"
        "\"input0_fire_reports\":%lu,\"input1_fire_reports\":%lu,\"phase\":%lu,\"control_ready\":%u,\"ui_ticks\":%u,\"ui_category\":%u,\"ui_category_changes\":%u}\n",
        bo_lab.mission_ticks, bo_lab.p2_ready_ticks, bo_lab.phases_completed,
        bo_lab.distinct_actor_ticks, bo_lab.distinct_proxy_ticks, bo_lab.distinct_weapon_ticks,
        bo_lab.restored_ticks, bo_lab.restore_failures, bo_lab.proxy_failures,
        bo_lab.restore_scope_failures, bo_lab.restore_actor_grid_failures,
        bo_lab.restore_actor_motion_failures, bo_lab.restore_actor_weapon_failures,
        bo_lab.restore_actor_model_failures, bo_lab.restore_actor_tail_failures,
        bo_lab.restore_grid_node_failures, bo_lab.restore_grid_other_failures,
        bo_lab.p1_proxy_missing_ticks, bo_lab.p1_body_missing_ticks,
        bo_lab.p2_proxy_missing_ticks, bo_lab.p2_body_missing_ticks,
        bo_lab.proxy_pool_low_ticks, bo_lab.proxy_pool_unknown_ticks,
        bo_lab.scripted_control_ticks,
        bo_lab.restored_immutable_ticks, bo_lab.restore_grid_only_ticks,
        bo_lab.grid_region_head_ticks, bo_lab.grid_region_cell_ticks,
        bo_lab.grid_region_links_ticks, bo_lab.p2_distinct_physical_ticks,
        bo_lab.p2_setup_wait_ticks, bo_lab.p2_playing_ticks,
        bo_lab.scripted_owner_ticks, bo_lab.scripted_actor_ticks,
        bo_lab.scripted_both_ticks, bo_lab.scripted_unheld_ticks,
        bo_lab.epoch_changes, bo_lab.invalid_samples,
        bo_lab.p1_move_mm, bo_lab.p2_move_mm, bo_lab.p1_idle_mm, bo_lab.p2_idle_mm,
        bo_lab.p1_body_move_mm, bo_lab.p2_body_move_mm, bo_lab.p1_shots, bo_lab.p2_shots,
        (unsigned long)InterlockedCompareExchange(&bo_lab_pads[0], 0, 0),
        (unsigned long)InterlockedCompareExchange(&bo_lab_pads[1], 0, 0),
        (unsigned long)InterlockedCompareExchange(&bo_lab_moves[0], 0, 0),
        (unsigned long)InterlockedCompareExchange(&bo_lab_moves[1], 0, 0),
        (unsigned long)InterlockedCompareExchange(&bo_lab_fires[0], 0, 0),
        (unsigned long)InterlockedCompareExchange(&bo_lab_fires[1], 0, 0),
        (unsigned long)InterlockedCompareExchange(&bo_lab_phase, 0, 0), (unsigned)bo_lab_control_ready, bo_lab_ui_ticks, bo_lab_ui_now, bo_lab_ui_changes);
    fclose(file);
}

static void bo_lab_ui_page(const char *page)
{
    const unsigned category = bo_lab_ui_category(page);
    if (!bo_lab_ui_marker_emitted) {
        bo_lab_ui_marker_emitted = 1;
        fputs("BO_ENGINE_MENU_TICK\n", stderr);
    }
    const int changed = category != bo_lab_ui_now;
    ++bo_lab_ui_ticks;
    if (changed) { bo_lab_ui_now = category; ++bo_lab_ui_changes; }
    if (changed || bo_lab_ui_ticks % 60u == 0) bo_lab_write();
}

static void bo_lab_before(void)
{
    const uint32_t core = GUEST32(0x002D45D4u);
    bo_lab_before_valid = black_p2_state == 2 && !black_p2_in &&
        black_pc_ram_span(core, 0x5CC0u) && GUEST32(core + 0x5AE0u) == BLACK_CORE_RUNNING;
    if (bo_lab_before_valid) {
        bo_lab_before_core = core;
        bo_lab_before_owner = GUEST32(0x002D12B8u);
        memcpy(bo_lab_before_actor, (uint8_t *)(uintptr_t)g_xbox_mem_offset + core + BLACK_ACTOR_AT, BLACK_ACTOR_BYTES);
    }
}

static int bo_lab_position(uint32_t at, float out[3])
{
    unsigned k;
    if (!black_pc_ram_span(at, 12u)) return 0;
    memcpy(out, (uint8_t *)(uintptr_t)g_xbox_mem_offset + at, 12u);
    for (k = 0; k < 3; ++k) if (!(out[k] == out[k] && out[k] > -1000000.0f && out[k] < 1000000.0f)) return 0;
    return 1;
}
static unsigned bo_lab_distance_mm(const float a[3], const float b[3])
{
    const double x = (double)a[0] - b[0], z = (double)a[2] - b[2];
    const double mm = sqrt(x * x + z * z) * 1000.0;
    return mm < 10000000.0 ? (unsigned)mm : 10000000u;
}
static void bo_lab_max(unsigned *value, unsigned sample) { if (sample > *value) *value = sample; }

static void bo_lab_after_impl(void)
{
    const uint32_t core = GUEST32(0x002D45D4u);
    uint32_t actors[2], proxies[2], bodies[2], weapons[2];
    float positions[2][3], body_positions[2][3];
    unsigned phase, i;
    int physical, ready;
    if (!black_pc_ram_span(core, 0x5CC0u) || GUEST32(core + 0x5AE0u) != BLACK_CORE_RUNNING) {
        if (bo_lab_epoch) bo_lab.epoch_changes = 1;
        bo_lab_control_ready = 0;
        bo_lab_write();
        return;
    }
    InterlockedExchange(&bo_lab_mission, 1); /* stop all menu presses immediately */
    ++bo_lab.mission_ticks;
    if (!bo_lab_epoch) bo_lab_epoch = core;
    if (bo_lab_epoch != core) ++bo_lab.epoch_changes;
    if (black_p2_state == 1) ++bo_lab.p2_setup_wait_ticks;
    if (black_p2_state == 2) ++bo_lab.p2_playing_ticks;
    if (bo_lab_before_valid) {
        /* A failed restore MUST remain a failure, even after categorization. */
        if (core != bo_lab_before_core || GUEST32(0x002D12B8u) != bo_lab_before_owner || black_p2_in) {
            ++bo_lab.restore_failures;
            ++bo_lab.restore_scope_failures;
        } else {
            const uint8_t *const after = (const uint8_t *)(uintptr_t)g_xbox_mem_offset + core + BLACK_ACTOR_AT;
            const uint8_t *const before = bo_lab_before_actor;
            /* Pinned PC black_p2_find_links excludes +0x20..+0x57 from
             * actor swapping. Observe this independently; never pretend a
             * full-actor restore passed just because those bytes changed. */
            const int immutable_diff = memcmp(before, after, 0x20u) ||
                memcmp(before+0x58u, after+0x58u, BLACK_ACTOR_BYTES-0x58u);
            const int node_diff = memcmp(before+0x20u, after+0x20u, 0x38u);
            if (!immutable_diff) {
                ++bo_lab.restored_immutable_ticks;
                if (node_diff) ++bo_lab.restore_grid_only_ticks;
            }
            if (memcmp(before, after, BLACK_ACTOR_BYTES) == 0) ++bo_lab.restored_ticks;
            else {
                ++bo_lab.restore_failures;
                if (memcmp(before + 0x000u, after + 0x000u, 0x080u)) {
                    ++bo_lab.restore_actor_grid_failures;
                    /* Original PC black_p2_find_links excludes 0x20..0x57:
                     * distinguish this known world-owned region without
                     * hiding it from the strict P1 restoration acceptance. */
                    if (memcmp(before + 0x020u, after + 0x020u, 0x038u)) {
                        ++bo_lab.restore_grid_node_failures;
                        if (memcmp(before + 0x020u, after + 0x020u, 0x010u)) ++bo_lab.grid_region_head_ticks;
                        if (memcmp(before + 0x030u, after + 0x030u, 0x00Cu)) ++bo_lab.grid_region_cell_ticks;
                        if (memcmp(before + 0x03Cu, after + 0x03Cu, 0x01Cu)) ++bo_lab.grid_region_links_ticks;
                    }
                    if (memcmp(before, after, 0x020u) ||
                        memcmp(before + 0x058u, after + 0x058u, 0x028u))
                        ++bo_lab.restore_grid_other_failures;
                }
                if (memcmp(before + 0x080u, after + 0x080u, 0x100u)) ++bo_lab.restore_actor_motion_failures;
                if (memcmp(before + 0x180u, after + 0x180u, 0x180u)) ++bo_lab.restore_actor_weapon_failures;
                if (memcmp(before + 0x300u, after + 0x300u, 0x100u)) ++bo_lab.restore_actor_model_failures;
                if (memcmp(before + 0x400u, after + 0x400u, 0x500u)) ++bo_lab.restore_actor_tail_failures;
            }
        }
    }
    if (black_p2_state != 2 || black_p2_in) { bo_lab_write(); return; }
    actors[0] = core + BLACK_ACTOR_AT; actors[1] = black_p2_shadow;
    if (!actors[1] || actors[0] == actors[1] || !black_pc_ram_span(actors[1], BLACK_ACTOR_BYTES)) { bo_lab_write(); return; }
    ++bo_lab.distinct_actor_ticks;
    proxies[0] = GUEST32(actors[0] + 0xB4u); proxies[1] = GUEST32(actors[1] + 0xB4u);
    bodies[0] = black_proxy_motion(proxies[0]); bodies[1] = black_proxy_motion(proxies[1]);
    /* Never export an engine pointer or pool slot; emit fixed integer
     * failure categories for the real original DF400/DBBE0 prerequisites. */
    if (!proxies[0]) ++bo_lab.p1_proxy_missing_ticks;
    if (!bodies[0]) ++bo_lab.p1_body_missing_ticks;
    if (!proxies[1]) ++bo_lab.p2_proxy_missing_ticks;
    if (!bodies[1]) ++bo_lab.p2_body_missing_ticks;
    {
        const uint32_t world = GUEST32(0x002D45F4u);
        if (!black_pc_ram_span(world, 0x2974u)) ++bo_lab.proxy_pool_unknown_ticks;
        else {
            unsigned free_slots = 0, slot;
            for (slot = 0; slot < 20u; ++slot)
                if (!GUEST8(world + 0x2960u + slot)) ++free_slots;
            if (free_slots < 2u) ++bo_lab.proxy_pool_low_ticks;
        }
    }
    {
        const uint32_t owner = GUEST32(0x002D12B8u);
        const int heldOwner = black_pc_ram_span(owner, 0x822u) && GUEST8(owner + 0x821u) != 0;
        const int heldActor = GUEST32(actors[0] + 0x324u) == actors[0] + 0x800u;
        if (heldOwner) ++bo_lab.scripted_owner_ticks;
        if (heldActor) ++bo_lab.scripted_actor_ticks;
        if (heldOwner && heldActor) ++bo_lab.scripted_both_ticks;
        if (!heldOwner && !heldActor) ++bo_lab.scripted_unheld_ticks;
        if (heldOwner || heldActor) ++bo_lab.scripted_control_ticks;
    }
    physical = proxies[0] && proxies[1] && proxies[0] != proxies[1] && proxies[1] == black_p2_proxy &&
        bodies[0] && bodies[1] && bodies[0] != bodies[1];
    if (physical) ++bo_lab.p2_distinct_physical_ticks;
    /* Camera control must have returned from the mission's original scripted intro. */
    ready = physical && black_pc_ram_span(GUEST32(0x002D12B8u), 0x822u) &&
        GUEST8(GUEST32(0x002D12B8u) + 0x821u) == 0 &&
        GUEST32(actors[0] + 0x324u) != actors[0] + 0x800u;
    bo_lab_control_ready = ready;
    if (!ready) {
        if (bo_lab.p2_ready_ticks) ++bo_lab.proxy_failures;
        bo_lab_write(); return;
    }
    ++bo_lab.p2_ready_ticks;
    ++bo_lab.distinct_proxy_ticks;
    weapons[0] = GUEST32(actors[0] + 0x29Cu); weapons[1] = GUEST32(actors[1] + 0x29Cu);
    if (weapons[0] && weapons[1] && weapons[0] != weapons[1]) ++bo_lab.distinct_weapon_ticks;
    phase = (unsigned)InterlockedCompareExchange(&bo_lab_phase, 0, 0);
    for (i = 0; i < 2; ++i) {
        if (!bo_lab_position(actors[i] + 0xA0u, positions[i]) || !bo_lab_position(bodies[i] + 0x10u, body_positions[i])) {
            ++bo_lab.invalid_samples; bo_lab_write(); return;
        }
    }
    if (bo_lab_seen_phase != phase) {
        memcpy(bo_lab_origins, positions, sizeof(positions));
        memcpy(bo_lab_body_origins, body_positions, sizeof(body_positions));
        bo_lab_seen_phase = phase;
    }
    for (i = 0; i < 2; ++i) {
        uint32_t magazine = 0;
        unsigned ammo = 0;
        const unsigned delta = bo_lab_distance_mm(positions[i], bo_lab_origins[i]);
        if (phase == 0) bo_lab_max(i ? &bo_lab.p2_idle_mm : &bo_lab.p1_idle_mm, delta);
        if (phase == (i ? 3u : 1u)) {
            bo_lab_max(i ? &bo_lab.p2_move_mm : &bo_lab.p1_move_mm, delta);
            bo_lab_max(i ? &bo_lab.p2_body_move_mm : &bo_lab.p1_body_move_mm,
                bo_lab_distance_mm(body_positions[i], bo_lab_body_origins[i]));
        }
        if (black_pc_ram_span(weapons[i], 0x100u)) magazine = GUEST32(weapons[i] + 0xF4u);
        if (black_pc_ram_span(magazine, 0x20u)) {
            ammo = GUEST16(magazine + 0x18u);
            if (phase == (i ? 7u : 5u) && weapons[i] == bo_lab_weapon_was[i] && magazine == bo_lab_mag_was[i] &&
                ammo < bo_lab_ammo_was[i] && bo_lab_ammo_was[i] - ammo < 100u) {
                if (i) bo_lab.p2_shots += bo_lab_ammo_was[i] - ammo;
                else bo_lab.p1_shots += bo_lab_ammo_was[i] - ammo;
            }
        }
        bo_lab_weapon_was[i] = weapons[i]; bo_lab_mag_was[i] = magazine; bo_lab_ammo_was[i] = ammo;
    }
    if (++bo_lab_phase_ticks >= 96u) {
        bo_lab_phase_ticks = 0;
        bo_lab.phases_completed = phase + 1u;
        InterlockedExchange(&bo_lab_phase, (LONG)(phase + 1u));
    }
    if (bo_lab.mission_ticks % 30u == 0 || bo_lab.phases_completed >= 8u) bo_lab_write();
}
static void bo_lab_after(void)
{
    unsigned csr = _mm_getcsr();
    fenv_t environment;
    fegetenv(&environment); fesetenv(FE_DFL_ENV); _mm_setcsr(0x1F80u);
    bo_lab_after_impl();
    fesetenv(&environment); _mm_setcsr(csr);
    if (bo_lab.phases_completed >= 8u) ExitProcess(0);
}
