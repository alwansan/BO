/* Read-only PRIVATE lab counters. Guest addresses and object bytes stay local. */
static volatile LONG bo_audio_counts[9], bo_audio_active, bo_audio_thread, bo_audio_lock;
typedef struct BoAudioObject { uint32_t address; } BoAudioObject;
static unsigned bo_audio_next_slot;
static BoAudioObject bo_audio_objects[64];

uint32_t bo_audio_enter(unsigned kind)
{
    uint32_t object = 0;
    const LONG thread = (LONG)GetCurrentThreadId();
    LONG first = InterlockedCompareExchange(&bo_audio_thread, thread, 0);
    InterlockedIncrement(&bo_audio_counts[kind ? 1 : 0]);
    if (first && first != thread) InterlockedIncrement(&bo_audio_counts[2]);
    if (InterlockedIncrement(&bo_audio_active) > 1) InterlockedIncrement(&bo_audio_counts[3]);
    /* 7EF50's sixth cdecl argument supplies embedded storage; 7EDB0 takes object. */
    if (black_pc_ram_span(g_esp, 28u)) object = GUEST32(g_esp + (kind ? 4u : 24u));
    if (!black_pc_ram_span(object, 0x5Cu)) {
        InterlockedIncrement(&bo_audio_counts[4]);
        return 0;
    }
    return object;
}

void bo_audio_leave(unsigned kind, uint32_t object)
{
    unsigned i;
    if (object && !InterlockedCompareExchange(&bo_audio_lock, 1, 0)) {
        unsigned free_slot = 64;
        for (i = 0; i < 64; ++i) {
            if (bo_audio_objects[i].address == object) { free_slot = i; break; }
            if (!bo_audio_objects[i].address && free_slot == 64) free_slot = i;
        }
        if (kind) {
            if (i < 64) bo_audio_objects[i].address = 0;
        } else {
            if (free_slot == 64) {
                free_slot = bo_audio_next_slot++ % 64u;
                InterlockedIncrement(&bo_audio_counts[5]);
            }
            bo_audio_objects[free_slot].address = object;
        }
        InterlockedExchange(&bo_audio_lock, 0);
    } else if (object) InterlockedIncrement(&bo_audio_counts[6]);
    InterlockedDecrement(&bo_audio_active);
}

static void bo_audio_after_take(void)
{
    unsigned i;
    if (InterlockedCompareExchange(&bo_audio_lock, 1, 0)) {
        InterlockedIncrement(&bo_audio_counts[6]);
        return;
    }
    InterlockedIncrement(&bo_audio_counts[7]);
    for (i = 0; i < 64; ++i) {
        const uint32_t object = bo_audio_objects[i].address;
        /* Only inspect embedded objects whose storage is inside the live FPV. */
        const uint32_t fpv = black_fpv();
        if (fpv && object >= fpv && object - fpv <= BLACK_FPV_BYTES - 0x5Cu &&
            black_pc_ram_span(object, 0x5Cu)) {
            InterlockedIncrement(&bo_audio_counts[8]);
        }
    }
    InterlockedExchange(&bo_audio_lock, 0);
}

static void bo_audio_write(void)
{
    const char *path = getenv("BO_LAB_AUDIO_AGGREGATE");
    FILE *file;
    if (!path || !(file = fopen(path, "wb"))) return;
    fprintf(file, "{\"create_calls\":%lu,\"destroy_calls\":%lu,\"other_thread_calls\":%lu,"
        "\"overlap_calls\":%lu,\"untracked_storage\":%lu,\"ledger_evictions\":%lu,"
        "\"observation_busy\":%lu,\"restore_checks\":%lu,\"retained_fpv_storage\":%lu}\n",
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[0],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[1],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[2],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[3],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[4],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[5],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[6],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[7],0,0),
        (unsigned long)InterlockedCompareExchange(&bo_audio_counts[8],0,0));
    fclose(file);
}
