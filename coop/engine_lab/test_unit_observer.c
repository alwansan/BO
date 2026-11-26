/* Synthetic memory only; exercises the exact lab observer. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long LONG;
static LONG thread = 1;
static LONG GetCurrentThreadId(void) { return thread; }
static LONG InterlockedCompareExchange(volatile LONG *p, LONG value, LONG expected)
{ LONG old = *p; if (old == expected) *p = value; return old; }
static LONG InterlockedIncrement(volatile LONG *p) { return ++*p; }
static LONG InterlockedExchangeAdd(volatile LONG *p, LONG value)
{ LONG old = *p; *p += value; return old; }
static _Alignas(uint32_t) unsigned char ram[0x300000];
#define GUEST8(at) (ram[(at)])
#define GUEST16(at) (*(uint16_t *)(ram + (at)))
#define GUEST32(at) (*(uint32_t *)(ram + (at)))
static int black_pc_ram_span(uint32_t va, uint32_t size)
{ return va && va < sizeof(ram) && size <= sizeof(ram) - va; }
#include "unit_observer.inc"

static void test_packed_props(uint32_t world, uint32_t unit, uint32_t header)
{
    const uint32_t table = 0x23000, props = 0x33000;
    const BoUnitObjects empty = {0};
    GUEST32(unit + 0x1C) = props;
    GUEST32(unit + 0x34) = 2;
    GUEST32(header + 0x2C) = table;
    GUEST16(header + 0x92) = 4;
    GUEST32(props) = GUEST32(props + 0x200) = 0x28ED98;
    GUEST32(props + 0xC4) = GUEST32(props + 0x200 + 0xC4) = 4;
    /* Packed ordinals zero and one came from authored ordinals one/three. */
    GUEST32(props + 0x114) = table + 0x50;
    GUEST32(props + 0x200 + 0x114) = table + 0xF0;
    GUEST32(props + 0xC) = GUEST32(table + 0x50 + 0x4C) = 123;
    GUEST32(props + 0x200 + 8) = GUEST32(table + 0xF0 + 0x48) = 456;
    GUEST32(world + 0x5CB4) = props;
    GUEST32(props + 0xB0) = props + 0x200;
    GUEST32(props + 0x200 + 0xB0) = 0;
    GUEST8(unit + 0x38) = 1;
    bo_unit_after_link(bo_unit_enter(BO_UNIT_LINK, unit), unit);
    assert(bo_unit_counts[BO_PROP_MAPPED] == 1 && bo_unit_counts[BO_PROP_RECORDS_MAPPED] == 2);
    assert(bo_unit_counts[BO_PROP_WORLD_LINK_VERIFIED] == 1);
    GUEST32(props + 0x200 + 0x114) = table + 0x50; /* Repeated ordinal. */
    bo_unit_after_link(bo_unit_enter(BO_UNIT_LINK, unit), unit);
    assert(bo_unit_counts[BO_PROP_MAP_FAILED] == 1);
    GUEST32(props + 0x200 + 0x114) = table + 0xF0;
    GUEST32(props + 0xC)++; /* Wrong copied selector, even with the same row. */
    assert(!bo_prop_before_unlink(unit).count && bo_unit_counts[BO_PROP_UNLINK_NOT_OBSERVED] == 1);
    GUEST32(props + 0xC)--;
    GUEST32(props + 0xB0) = props + 4;
    assert(!bo_prop_before_unlink(unit).count && bo_unit_counts[BO_PROP_UNLINK_NOT_OBSERVED] == 2);
    GUEST32(props + 0xB0) = props + 0x200;
    const BoUnitObjects before = bo_prop_before_unlink(unit);
    assert(before.count == 2 && before.stride == 0x200);
    GUEST8(unit + 0x38) = 0;
    GUEST32(world + 0x5CB4) = props + 0x200;
    bo_unit_after_unlink(bo_unit_enter(BO_UNIT_UNLINK, unit), unit, 1, empty, before);
    assert(bo_unit_counts[BO_PROP_WORLD_UNLINK_FAILED] == 1);
    GUEST32(world + 0x5CB4) = 0;
    bo_unit_after_unlink(bo_unit_enter(BO_UNIT_UNLINK, unit), unit, 1, empty, before);
    assert(bo_unit_counts[BO_PROP_WORLD_UNLINK_VERIFIED] == 1);
    puts("BO_UNIT_PROP_OBSERVER_FIXTURE_PASS");
}

int main(void)
{
    const uint32_t world = 0x10000, unit = world + 0x49D0, header = 0x20000;
    const uint32_t objects = 0x30000, table = 0x22000;
    const uint32_t other_object = 0x32000;
    const BoUnitObjects range = {objects, 2, 0xF0}, empty = {0};
    assert(bo_unit_enter(BO_UNIT_POLL, unit) == -1);
    bo_unit_bind_frame_thread();
    GUEST32(0x2D45D4) = world;
    GUEST32(unit) = 0x1C;
    GUEST32(unit + 4) = GUEST32(unit + 8) = 1;
    GUEST32(unit + 0xC) = header;
    GUEST32(unit + 0x14) = objects;
    GUEST32(header) = 24;
    GUEST32(header + 0x1C) = table;
    GUEST16(header + 0x90) = 2;
    GUEST32(objects) = GUEST32(objects + 0xF0) = 0x28EA60;
    GUEST32(objects + 0xD0) = table;
    GUEST32(objects + 0xF0 + 0xD0) = table + 0x30;

    const int slot = bo_unit_enter(BO_UNIT_POLL, unit);
    assert(slot == 0);
    bo_unit_after_poll(slot, unit, 7, 1);
    bo_unit_after_poll(slot, unit, 0x1C, 1); /* Still loaded: no new publish. */
    assert(bo_unit_counts[BO_UNIT_MAPPED] == 1);
    assert(bo_unit_counts[BO_UNIT_WORLD_LINK_VERIFIED] == 0);
    GUEST32(world + 0x5CB4) = other_object;
    GUEST32(other_object + 0xB0) = objects + 0xF0;
    GUEST32(objects + 0xF0 + 0xB0) = objects;
    GUEST8(unit + 0x38) = 1;
    bo_unit_after_link(bo_unit_enter(BO_UNIT_LINK, unit), unit);
    assert(bo_unit_counts[BO_UNIT_WORLD_LINK_VERIFIED] == 1);
    GUEST32(objects + 0xB0) = other_object;
    assert(bo_unit_world_members(range) == -1); /* Cyclic tail despite all matches. */
    GUEST32(objects + 0xB0) = objects + 4;
    assert(bo_unit_world_members(range) == -1); /* Interior pointer is not an object. */
    GUEST32(objects + 0xB0) = 0;
    bo_unit_after_unlink(bo_unit_enter(BO_UNIT_UNLINK, unit), unit, 1, range, empty);
    assert(bo_unit_counts[BO_UNIT_UNLINKED] == 0);
    GUEST32(other_object + 0xB0) = objects;
    bo_unit_after_link(bo_unit_enter(BO_UNIT_LINK, unit), unit);
    assert(bo_unit_counts[BO_UNIT_WORLD_LINK_FAILED] == 1);
    GUEST8(unit + 0x38) = 0;
    bo_unit_after_unlink(bo_unit_enter(BO_UNIT_UNLINK, unit), unit, 1, range, empty);
    assert(bo_unit_counts[BO_UNIT_WORLD_UNLINK_FAILED] == 1);
    assert(bo_unit_counts[BO_UNIT_PAIRED_UNLINK] == 1);
    bo_unit_after_poll(bo_unit_enter(BO_UNIT_POLL, unit), unit, 3, 1);
    assert(bo_unit_counts[BO_UNIT_REPUBLISHED] == 1);
    GUEST32(other_object + 0xB0) = 0;
    bo_unit_after_unlink(bo_unit_enter(BO_UNIT_UNLINK, unit), unit, 1, range, empty);
    assert(bo_unit_counts[BO_UNIT_WORLD_UNLINK_VERIFIED] == 1);
    GUEST32(objects + 0xF0 + 0xD0) = table + 0x60;
    bo_unit_after_poll(bo_unit_enter(BO_UNIT_POLL, unit), unit, 7, 1);
    assert(bo_unit_counts[BO_UNIT_MAP_FAILED] == 1);

    thread = 2;
    bo_unit_after_poll(bo_unit_enter(BO_UNIT_POLL, unit), unit, 7, 1);
    assert(bo_unit_counts[BO_UNIT_OTHER_THREAD] == 1);
    assert(bo_unit_counts[BO_UNIT_BEFORE_FRAME_THREAD] == 1);
    assert(bo_unit_counts[BO_UNIT_PUBLISHED] == 3);
    thread = 1;
    test_packed_props(world, unit, header);
    bo_unit_write();
    puts("BO_UNIT_OBSERVER_FIXTURE_PASS");
    return 0;
}
