/* MIT. Read-only prop damage readback on the original frame thread. */
typedef struct BoPropDamage {
    uint64_t link_serial;
    uint32_t world, scene, unit, unit_index, header, authored_row;
    uint32_t part, part_index, part_count, part_definition, health;
    int impact;
} BoPropDamage;

static struct {
    unsigned verified, rejected, impact_decreases, area_decreases, depleted;
} bo_prop_damage;
static volatile LONG bo_prop_damage_other_thread;

/* 001D3610 / 001D1770 retain the authored row on each packed unit prop.
 * 001E5070 uses EAX=prop, [ESP+4]=part index, object+1A0=part array,
 * stride 0xC; part+0 is the float that the original writer reduces/clamps. */
static int bo_prop_damage_read(uint32_t object, uint32_t part_index, BoPropDamage *out)
{
    const uint32_t world = GUEST32(0x2D45D4u);
    if (world != bo_unit_world || !bo_lab_world_running(world)) return 0;

    uint32_t unit = 0;
    for (unsigned role = 0; role < 2; ++role) {
        const uint32_t candidate = world + (role ? 0x5250u : 0x49D0u);
        const uint32_t base = GUEST32(candidate + 0x1Cu);
        const uint32_t count = GUEST32(candidate + 0x34u);
        if (GUEST32(candidate) != 0x1Cu || GUEST8(candidate + 0x38u) != 1u ||
            object < base || count == 0 || count > 4096u ||
            object - base >= count * 0x200u || (object - base) % 0x200u) continue;
        BoUnitObjects range;
        if (unit || bo_prop_mapping(candidate, &range) != 1) return 0;
        unit = candidate;
    }
    const BoUnitObjects target = {object, 1, 0x200u};
    if (!unit || bo_unit_world_members(target) != 1) return 0;

    const uint32_t part_count = GUEST8(object + 0x1F5u);
    const uint32_t parts = GUEST32(object + 0x1A0u);
    if (part_index >= part_count || (parts & 3u) ||
        !black_pc_ram_span(parts, part_count * 0xCu)) return 0;
    const uint32_t part = parts + part_index * 0xCu;
    const uint32_t definition = GUEST32(part + 4u);
    const uint32_t health = GUEST32(part);
    /* Unsigned ordering agrees with the order of finite nonnegative floats. */
    if (definition == UINT32_MAX || health >= 0x7F800000u) return 0;
    const uint32_t index = GUEST32(unit + 8u);
    if (index == UINT32_MAX || index != GUEST32(unit + 4u)) return 0;

    out->link_serial = bo_unit_link_serial;
    out->world = world;
    out->scene = GUEST16(world + 0x5AECu);
    out->unit = unit;
    out->unit_index = index;
    out->header = GUEST32(unit + 0xCu);
    out->authored_row = GUEST32(object + 0x114u);
    out->part = part;
    out->part_index = part_index;
    out->part_count = part_count;
    out->part_definition = definition;
    out->health = health;
    return 1;
}

static BoPropDamage bo_prop_damage_before(uint32_t object, uint32_t stack)
{
    BoPropDamage before = {0};
    if (InterlockedCompareExchange(&bo_unit_frame_thread, 0, 0) != (LONG)GetCurrentThreadId()) {
        InterlockedIncrement(&bo_prop_damage_other_thread);
        return before;
    }
    if ((stack & 3u) || !black_pc_ram_span(stack, 8u)) return before;
    const uint32_t caller = GUEST32(stack);
    if (caller != 0x1E58E8u && caller != 0x1E55C5u) return before;
    if (!bo_prop_damage_read(object, GUEST32(stack + 4u), &before))
        ++bo_prop_damage.rejected;
    before.impact = caller == 0x1E58E8u;
    return before;
}

static void bo_prop_damage_after(uint32_t object, const BoPropDamage *before)
{
    BoPropDamage after = {0};
    if (!before->unit) return;
    if (before->link_serial != bo_unit_link_serial ||
        !bo_prop_damage_read(object, before->part_index, &after) ||
        after.world != before->world || after.scene != before->scene ||
        after.unit != before->unit || after.unit_index != before->unit_index ||
        after.header != before->header || after.authored_row != before->authored_row ||
        after.part != before->part || after.part_count != before->part_count ||
        after.part_definition != before->part_definition || after.health > before->health) {
        ++bo_prop_damage.rejected;
        return;
    }
    ++bo_prop_damage.verified;
    if (after.health == before->health) return;
    if (before->impact) ++bo_prop_damage.impact_decreases;
    else ++bo_prop_damage.area_decreases;
    if (after.health == 0) ++bo_prop_damage.depleted;
}
