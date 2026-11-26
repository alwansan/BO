/* MIT. Read-only original-frame diagnostic; no descriptor IDs or addresses
 * leave the runner. A matched table is not a door resolver or apply authority. */
#define BO_OBJECTIVE_LIMIT 4096u

typedef struct BoObjectiveSource {
    uint32_t world, level_data, descriptors, rows, count, scene;
} BoObjectiveSource;

typedef struct BoObjectiveValue {
    uint32_t counter, state;
} BoObjectiveValue;

static struct {
    unsigned verified_frames, rejected_frames, max_records;
    unsigned changed_records, counter_advances, entries_reaching_state_3;
    unsigned verified_writer_calls, rejected_writer_calls, changed_writer_calls;
} bo_objectives;
static volatile LONG bo_objective_writer_other_thread_calls;
static BoObjectiveSource bo_objective_before;
static BoObjectiveValue bo_objective_old[BO_OBJECTIVE_LIMIT], bo_objective_new[BO_OBJECTIVE_LIMIT];

static int bo_lab_world_running(uint32_t world)
{
    return !(world & 3u) && black_pc_ram_span(world, 0x5CC0u) &&
        GUEST32(world + 0x5AE0u) == 0x1Cu && GUEST32(world + 0x5AE4u) == 0x1Du;
}

/* 001EDC90 stores the resolved LevelDat base at +5B2C, cleared by
 * 001EBD60. 000B70E0 relocates table pointer +8. 001C2420 constructs the
 * live array (+930/+934), preserving the pointer to each authored row. */
static int bo_objective_source(uint32_t world, BoObjectiveSource *source)
{
    const uint32_t level_data = GUEST32(world + 0x5B2Cu);
    const uint32_t rows = GUEST32(world + 0x930u);
    const uint32_t count = GUEST32(world + 0x934u);
    uint32_t descriptors;
    if ((level_data & 3u) || !black_pc_ram_span(level_data, 0x3A0u) || GUEST32(level_data) != 17u ||
        !count || count > BO_OBJECTIVE_LIMIT || count != GUEST32(level_data + 4u) ||
        (rows & 3u) || !black_pc_ram_span(rows, count * 0xCu)) return 0;
    descriptors = GUEST32(level_data + 8u);
    if ((descriptors & 3u) || descriptors < level_data + 0x3A0u ||
        !black_pc_ram_span(descriptors, count * 0x18u)) return 0;
    if ((uint64_t)rows + count * 0xCu > level_data &&
        rows < (uint64_t)descriptors + count * 0x18u) return 0;
    source->world = world;
    source->level_data = level_data;
    source->descriptors = descriptors;
    source->rows = rows;
    source->count = count;
    source->scene = GUEST8(world + 0x5AECu) * 256u + GUEST8(world + 0x5AEDu);
    return 1;
}

static int bo_objective_read(const BoObjectiveSource *source, uint32_t ordinal, BoObjectiveValue *value)
{
    const uint32_t row = source->rows + ordinal * 0xCu;
    if (GUEST32(row) != source->descriptors + ordinal * 0x18u) return 0;
    value->counter = GUEST32(row + 4u);
    value->state = GUEST32(row + 8u);
    return value->counter <= 65535u && value->state <= 3u;
}

static int bo_objective_same_source(const BoObjectiveSource *a, const BoObjectiveSource *b)
{
    return a->world == b->world && a->level_data == b->level_data &&
        a->descriptors == b->descriptors && a->rows == b->rows &&
        a->count == b->count && a->scene == b->scene;
}

static int bo_objective_capture(uint32_t world, BoObjectiveSource *source, BoObjectiveValue *values)
{
    if (!bo_objective_source(world, source)) return 0;
    for (uint32_t i = 0; i < source->count; ++i)
        if (!bo_objective_read(source, i, &values[i])) return 0;
    return 1;
}

static void bo_objective_begin(void)
{
    const uint32_t world = GUEST32(0x2D45D4u);
    bo_objective_before.count = 0;
    if (!bo_lab_world_running(world)) return;
    if (!bo_objective_capture(world, &bo_objective_before, bo_objective_old)) {
        bo_objective_before.count = 0;
        ++bo_objectives.rejected_frames;
    }
}

static void bo_objective_end(void)
{
    const uint32_t world = GUEST32(0x2D45D4u);
    const BoObjectiveSource before = bo_objective_before;
    BoObjectiveSource after;
    uint32_t i;
    if (!before.count) return;
    bo_objective_before.count = 0;
    if (world != before.world || !bo_lab_world_running(world) ||
        !bo_objective_capture(world, &after, bo_objective_new) ||
        !bo_objective_same_source(&before, &after)) {
        ++bo_objectives.rejected_frames;
        return;
    }
    ++bo_objectives.verified_frames;
    if (after.count > bo_objectives.max_records) bo_objectives.max_records = after.count;
    for (i = 0; i < after.count; ++i) {
        const BoObjectiveValue old = bo_objective_old[i], now = bo_objective_new[i];
        if (now.counter != old.counter || now.state != old.state) ++bo_objectives.changed_records;
        if (now.counter > old.counter) ++bo_objectives.counter_advances;
        if (now.state == 3u && old.state != 3u) ++bo_objectives.entries_reaching_state_3;
    }
}

/* 001BA710 calls 001BA430 with the matching live row in ESI. Observe the
 * authentic call even when it occurs outside the 001F1080 sample window. */
typedef struct BoObjectiveWriterObservation {
    BoObjectiveSource source;
    BoObjectiveValue value;
    uint32_t ordinal;
} BoObjectiveWriterObservation;

static BoObjectiveWriterObservation bo_objective_writer_before(uint32_t row)
{
    BoObjectiveWriterObservation before = {0};
    if (InterlockedCompareExchange(&bo_unit_frame_thread, 0, 0) != (LONG)GetCurrentThreadId()) {
        InterlockedIncrement(&bo_objective_writer_other_thread_calls);
        return before;
    }
    const uint32_t world = GUEST32(0x2D45D4u);
    if (!bo_lab_world_running(world)) return before;
    uint32_t offset = 0;
    if (bo_objective_source(world, &before.source) && row >= before.source.rows) {
        offset = row - before.source.rows;
        before.ordinal = offset / 0xCu;
        if (offset % 0xCu == 0 && before.ordinal < before.source.count &&
            bo_objective_read(&before.source, before.ordinal, &before.value)) return before;
    }
    before.source.count = 0;
    ++bo_objectives.rejected_writer_calls;
    return before;
}

static void bo_objective_writer_after(const BoObjectiveWriterObservation *before)
{
    if (!before->source.count) return;
    const uint32_t world = GUEST32(0x2D45D4u);
    BoObjectiveSource after;
    BoObjectiveValue value;
    if (world != before->source.world || !bo_lab_world_running(world) ||
        !bo_objective_source(world, &after) || !bo_objective_same_source(&before->source, &after) ||
        !bo_objective_read(&after, before->ordinal, &value)) {
        ++bo_objectives.rejected_writer_calls;
        return;
    }
    ++bo_objectives.verified_writer_calls;
    if (value.counter != before->value.counter || value.state != before->value.state)
        ++bo_objectives.changed_writer_calls;
}
