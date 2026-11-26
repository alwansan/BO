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
static _Alignas(uint32_t) unsigned char ram[0x300000];
#define GUEST8(at) (ram[(at)])
#define GUEST16(at) (*(uint16_t *)(ram + (at)))
#define GUEST32(at) (*(uint32_t *)(ram + (at)))
static int black_pc_ram_span(uint32_t va, uint32_t size)
{ return va && va < sizeof(ram) && size <= sizeof(ram) - va; }
#include "unit_observer.inc"

int main(void)
{
    const uint32_t world = 0x10000, unit = world + 0x49D0, header = 0x20000;
    const uint32_t objects = 0x30000, table = 0x22000;
    assert(bo_unit_enter(0, unit) == -1);
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

    const int slot = bo_unit_enter(0, unit);
    assert(slot == 0);
    bo_unit_after_poll(slot, unit, 7, 1);
    bo_unit_after_poll(slot, unit, 0x1C, 1); /* Still loaded: no new publish. */
    assert(bo_unit_counts[BO_UNIT_MAPPED] == 1);
    GUEST8(unit + 0x38) = 1;
    bo_unit_after_unlink(bo_unit_enter(1, unit), unit, 1);
    assert(bo_unit_counts[BO_UNIT_UNLINKED] == 0);
    GUEST8(unit + 0x38) = 0;
    bo_unit_after_unlink(bo_unit_enter(1, unit), unit, 1);
    assert(bo_unit_counts[BO_UNIT_PAIRED_UNLINK] == 1);
    bo_unit_after_poll(bo_unit_enter(0, unit), unit, 3, 1);
    assert(bo_unit_counts[BO_UNIT_REPUBLISHED] == 1);
    GUEST32(objects + 0xF0 + 0xD0) = table + 0x60;
    bo_unit_after_poll(bo_unit_enter(0, unit), unit, 7, 1);
    assert(bo_unit_counts[BO_UNIT_MAP_FAILED] == 1);

    thread = 2;
    bo_unit_after_poll(bo_unit_enter(0, unit), unit, 7, 1);
    assert(bo_unit_counts[BO_UNIT_OTHER_THREAD] == 1);
    assert(bo_unit_counts[BO_UNIT_BEFORE_FRAME_THREAD] == 1);
    assert(bo_unit_counts[BO_UNIT_PUBLISHED] == 3);
    bo_unit_write();
    puts("BO_UNIT_OBSERVER_FIXTURE_PASS");
    return 0;
}
