/* MIT. Private PC lab; no guest writes and no guest addresses in the report. */
#include <stdio.h>

enum BoUnitCounter {
    BO_UNIT_POLL, BO_UNIT_UNLINK, BO_UNIT_LINK,
    BO_UNIT_BEFORE_FRAME_THREAD, BO_UNIT_OTHER_THREAD,
    BO_UNIT_PUBLISHED, BO_UNIT_MAPPED, BO_UNIT_MAP_FAILED, BO_UNIT_EMPTY,
    BO_UNIT_UNLINKED, BO_UNIT_PAIRED_UNLINK, BO_UNIT_REPUBLISHED,
    BO_UNIT_LINKED, BO_UNIT_WORLD_LINK_VERIFIED, BO_UNIT_WORLD_LINK_FAILED,
    BO_UNIT_WORLD_UNLINK_VERIFIED, BO_UNIT_WORLD_UNLINK_FAILED, BO_UNIT_COUNTER_COUNT
};
static volatile LONG bo_unit_counts[BO_UNIT_COUNTER_COUNT];
static volatile LONG bo_unit_frame_thread;
static uint32_t bo_unit_world;
static struct {
    uint32_t index;
    int active, unlinked;
} bo_unit_slots[2];

static void bo_unit_bind_frame_thread(void)
{
    InterlockedCompareExchange(&bo_unit_frame_thread, (LONG)GetCurrentThreadId(), 0);
}

/* The two world-embedded unit descriptors are selected by the original
 * 001C2180 scan. Any other descriptor is outside this observation. */
static int bo_unit_enter(enum BoUnitCounter call, uint32_t unit)
{
    const LONG thread = InterlockedCompareExchange(&bo_unit_frame_thread, 0, 0);
    uint32_t world;
    InterlockedIncrement(&bo_unit_counts[call]);
    if (!thread || thread != (LONG)GetCurrentThreadId()) {
        InterlockedIncrement(&bo_unit_counts[thread ? BO_UNIT_OTHER_THREAD : BO_UNIT_BEFORE_FRAME_THREAD]);
        return -1;
    }
    if (!black_pc_ram_span(0x2D45D4u, 4u)) return -1;
    world = GUEST32(0x2D45D4u);
    if ((world & 3u) || !black_pc_ram_span(world, 0x5CB8u) ||
        !black_pc_ram_span(unit, 0x3Cu)) return -1;
    if (unit != world + 0x49D0u && unit != world + 0x5250u) return -1;
    if (world != bo_unit_world) {
        memset(bo_unit_slots, 0, sizeof(bo_unit_slots));
        bo_unit_world = world;
    }
    return unit == world + 0x49D0u ? 0 : 1;
}

/* 001D6200 creates two parallel arrays and retains the authored record
 * pointer on each live object. Check the entire bounded ordinal mapping. */
typedef struct BoUnitObjects {
    uint32_t base, count;
} BoUnitObjects;

static int bo_unit_mapping(uint32_t unit, BoUnitObjects *range)
{
    const uint32_t header = GUEST32(unit + 0xCu), objects = GUEST32(unit + 0x14u);
    uint32_t count, table, i;
    if ((header & 3u) || !black_pc_ram_span(header, 0x92u) || GUEST32(header) != 24u) return -1;
    count = GUEST16(header + 0x90u);
    if (!count) return 0;
    table = GUEST32(header + 0x1Cu);
    if (count > 1024u || (objects & 3u) || (table & 3u) ||
        !black_pc_ram_span(table, count * 0x30u) ||
        !black_pc_ram_span(objects, count * 0xF0u)) return -1;
    for (i = 0; i < count; ++i) {
        const uint32_t object = objects + i * 0xF0u;
        if (GUEST32(object) != 0x28EA60u ||
            GUEST32(object + 0xD0u) != table + i * 0x30u) return -1;
    }
    if (range) {
        range->base = objects;
        range->count = count;
    }
    return 1;
}

/* 001CE1F0 inserts at world+0x5CB4, using object+0xB0 as the next link.
 * A bounded traversal must reach null, even if all expected entries appear
 * before a cycle or invalid pointer. Only count exact object starts. */
static int bo_unit_world_members(BoUnitObjects range)
{
    uint32_t node = GUEST32(bo_unit_world + 0x5CB4u);
    unsigned visited = 0, matches = 0;
    while (node) {
        if (++visited > 4096u || (node & 3u) || !black_pc_ram_span(node, 0xB4u)) return -1;
        if (node >= range.base) {
            const uint32_t offset = node - range.base;
            if (offset < range.count * 0xF0u) {
                if (offset % 0xF0u) return -1;
                ++matches;
            }
        }
        node = GUEST32(node + 0xB0u);
    }
    return (int)matches;
}

static unsigned bo_unit_after_poll(int slot, uint32_t unit, uint32_t before, uint32_t result)
{
    BoUnitObjects range;
    int mapped;
    if (slot < 0 || bo_unit_world != GUEST32(0x2D45D4u) ||
        before == 0x1Cu || (result & 0xFFu) != 1u ||
        !black_pc_ram_span(unit, 0x3Cu) || GUEST32(unit) != 0x1Cu ||
        GUEST32(unit + 8u) == UINT32_MAX || GUEST32(unit + 8u) != GUEST32(unit + 4u)) return 0;
    InterlockedIncrement(&bo_unit_counts[BO_UNIT_PUBLISHED]);
    mapped = bo_unit_mapping(unit, &range);
    InterlockedIncrement(&bo_unit_counts[mapped > 0 ? BO_UNIT_MAPPED :
                                            mapped == 0 ? BO_UNIT_EMPTY : BO_UNIT_MAP_FAILED]);
    if (mapped > 0 && bo_unit_slots[slot].unlinked &&
        bo_unit_slots[slot].index == GUEST32(unit + 8u)) {
        InterlockedIncrement(&bo_unit_counts[BO_UNIT_REPUBLISHED]);
    }
    bo_unit_slots[slot].index = GUEST32(unit + 8u);
    bo_unit_slots[slot].active = mapped > 0;
    bo_unit_slots[slot].unlinked = 0;
    return mapped > 0 ? range.count : 0;
}

static unsigned bo_unit_after_link(int slot, uint32_t unit)
{
    BoUnitObjects range;
    int mapped;
    if (slot < 0 || bo_unit_world != GUEST32(0x2D45D4u) ||
        !black_pc_ram_span(unit, 0x3Cu) || GUEST32(unit) != 0x1Cu ||
        GUEST8(unit + 0x38u) != 1u) return 0;
    InterlockedIncrement(&bo_unit_counts[BO_UNIT_LINKED]);
    mapped = bo_unit_mapping(unit, &range);
    if (mapped == 0) return 0; /* An empty unit has no membership witness. */
    if (mapped > 0 && bo_unit_world_members(range) == (int)range.count) {
        InterlockedIncrement(&bo_unit_counts[BO_UNIT_WORLD_LINK_VERIFIED]);
        return range.count;
    }
    InterlockedIncrement(&bo_unit_counts[BO_UNIT_WORLD_LINK_FAILED]);
    return 0;
}

static int bo_unit_after_unlink(int slot, uint32_t unit, uint32_t index, BoUnitObjects before)
{
    if (slot < 0 || bo_unit_world != GUEST32(0x2D45D4u) ||
        !black_pc_ram_span(unit, 0x3Cu) || GUEST8(unit + 0x38u)) return 0;
    InterlockedIncrement(&bo_unit_counts[BO_UNIT_UNLINKED]);
    bo_unit_slots[slot].unlinked = bo_unit_slots[slot].active && bo_unit_slots[slot].index == index;
    if (bo_unit_slots[slot].unlinked)
        InterlockedIncrement(&bo_unit_counts[BO_UNIT_PAIRED_UNLINK]);
    bo_unit_slots[slot].active = 0;
    if (!before.count) return 0;
    const int removed = bo_unit_world_members(before) == 0;
    InterlockedIncrement(&bo_unit_counts[removed ? BO_UNIT_WORLD_UNLINK_VERIFIED : BO_UNIT_WORLD_UNLINK_FAILED]);
    return removed;
}

static void bo_unit_write(void)
{
    static const char *const fields[BO_UNIT_COUNTER_COUNT] = {
        "poll_calls", "unlink_calls", "link_calls", "before_frame_thread_calls", "other_thread_calls",
        "unit_publications", "ordinal_maps_verified", "ordinal_map_failures", "empty_publications",
        "unit_unlinks", "paired_unit_unlinks", "same_slot_unit_republications",
        "unit_link_transitions", "world_membership_verified", "world_membership_failures",
        "world_removal_verified", "world_removal_failures"
    };
    const char *path = getenv("BO_LAB_UNIT_AGGREGATE");
    FILE *file;
    unsigned i;
    if (InterlockedCompareExchange(&bo_unit_frame_thread, 0, 0) != (LONG)GetCurrentThreadId() ||
        !path || !(file = fopen(path, "wb"))) return;
    fputc('{', file);
    for (i = 0; i < BO_UNIT_COUNTER_COUNT; ++i)
        fprintf(file, "%s\"%s\":%lu", i ? "," : "", fields[i],
                (unsigned long)InterlockedCompareExchange(&bo_unit_counts[i], 0, 0));
    fputs("}\n", file);
    fclose(file);
}
