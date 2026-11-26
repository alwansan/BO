/* Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
 * PRIVATE lab only: wraps original c5 bodies, calls each exactly once.
 * No guest writes or new audio/actor calls. Output contains counts only.
 * Included late in pinned recomp_manual.c after black_pc_ram_span exists. */
#include "bo_audio_publication.h"
#include <errno.h>
typedef struct BoAudioIdentity {
    uint32_t object, type, engine, data, flags;
    int active;
} BoAudioIdentity;
static BoAudioIdentity bo_audio_objects[128];
static volatile LONG bo_audio_gate, bo_audio_active, bo_audio_first_thread;
static volatile LONG bo_audio_creates, bo_audio_create_failures, bo_audio_releases;
static volatile LONG bo_audio_foreign_threads, bo_audio_overlaps, bo_audio_dropped;
static volatile LONG bo_audio_bad_reads, bo_audio_refs_before, bo_audio_refs_after;
static volatile LONG bo_audio_published_identity_changes, bo_audio_after_take_checks;
extern void bo_audio_original_create(void);
extern void bo_audio_original_release(void);
static int bo_audio_read(void *unused, uint32_t at, uint32_t *out)
{
    (void)unused;
    if (!black_pc_ram_span(at,4u)) return 0;
#ifdef BO_AUDIO_CALIBRATION
    *out = GUEST32(at); return 1;
#else
    /* Cross-thread publication changes are precisely what we observe. Use a
     * kernel copy, not unsynchronized C loads of another thread's storage. */
    { SIZE_T copied=0;
      return ReadProcessMemory(GetCurrentProcess(),
          (const void *)((uintptr_t)g_xbox_mem_offset+at),out,4u,&copied) && copied==4u; }
#endif
}
static int bo_audio_identity(uint32_t at, BoAudioIdentity *out)
{
    if (!black_pc_ram_span(at,0x5Cu)) return 0;
    out->object=at;
    if (!bo_audio_read(NULL,at+0xCu,&out->type) || !bo_audio_read(NULL,at+0x58u,&out->engine) ||
        !bo_audio_read(NULL,at+0x4Cu,&out->data) || !bo_audio_read(NULL,at+0x54u,&out->flags)) return 0;
    out->active=1;
    return 1;
}
static void bo_audio_thread_enter(void)
{
    const LONG thread=(LONG)GetCurrentThreadId();
    LONG first=InterlockedCompareExchange(&bo_audio_first_thread,thread,0);
    if (first && first!=thread) InterlockedIncrement(&bo_audio_foreign_threads);
    if (InterlockedIncrement(&bo_audio_active)>1) InterlockedIncrement(&bo_audio_overlaps);
}
static unsigned long bo_audio_count(volatile LONG *value)
{
    const LONG n=InterlockedCompareExchange(value,0,0);
    return n<0?100000000ul:n>100000000?100000000ul:(unsigned long)n;
}
static void bo_audio_write(void)
{
    const char *path=getenv("BO_LAB_AUDIO_AGGREGATE");FILE *out;
    if (!path || !(out=fopen(path,"wb"))) return;
    fprintf(out,"{\"creates\":%lu,\"create_failures\":%lu,\"releases\":%lu,"
        "\"foreign_thread_observations\":%lu,\"overlapping_call_observations\":%lu,"
        "\"dropped_observations\":%lu,\"invalid_publication_reads\":%lu,"
        "\"publication_refs_before_release\":%lu,\"publication_refs_after_release\":%lu,"
        "\"published_identity_changes\":%lu,\"after_take_checks\":%lu}\n",
        bo_audio_count(&bo_audio_creates),bo_audio_count(&bo_audio_create_failures),bo_audio_count(&bo_audio_releases),
        bo_audio_count(&bo_audio_foreign_threads),bo_audio_count(&bo_audio_overlaps),bo_audio_count(&bo_audio_dropped),
        bo_audio_count(&bo_audio_bad_reads),bo_audio_count(&bo_audio_refs_before),bo_audio_count(&bo_audio_refs_after),
        bo_audio_count(&bo_audio_published_identity_changes),bo_audio_count(&bo_audio_after_take_checks));
    fclose(out);
}
static void bo_audio_after_take(void)
{
    unsigned i;const int error=errno;
    if (InterlockedCompareExchange(&bo_audio_gate,1,0)) {InterlockedIncrement(&bo_audio_dropped);return;}
    InterlockedIncrement(&bo_audio_after_take_checks);
    for (i=0;i<128;i++) if (bo_audio_objects[i].active) {
        BoAudioIdentity now;unsigned refs=0;const BoAudioIdentity old=bo_audio_objects[i];
        if (!bo_audio_publication_refs(bo_audio_read,NULL,old.object,&refs) || !bo_audio_identity(old.object,&now)) {
            InterlockedIncrement(&bo_audio_bad_reads);continue;
        }
        if (refs && (now.type!=old.type || now.engine!=old.engine || now.data!=old.data ||
                     (now.flags&4u)!=(old.flags&4u))) InterlockedIncrement(&bo_audio_published_identity_changes);
    }
    bo_audio_write();InterlockedExchange(&bo_audio_gate,0);errno=error;
}
void sub_0007EF50(void)
{
    uint32_t object=0,type=0;int observe=0;unsigned i;const int error=errno;
    if (black_pc_ram_span(g_esp,0x1Cu)) {bo_audio_read(NULL,g_esp+4u,&type);bo_audio_read(NULL,g_esp+0x18u,&object);}
    if (type==0x2C2C38u && object) {observe=1;bo_audio_thread_enter();}
    errno=error;bo_audio_original_create();
    if (observe) {
        const int returned_error=errno;
        InterlockedIncrement(&bo_audio_creates);
        if (g_eax!=object) InterlockedIncrement(&bo_audio_create_failures);
        else if (!InterlockedCompareExchange(&bo_audio_gate,1,0)) {
            BoAudioIdentity identity;
            for (i=0;i<128;i++) if (!bo_audio_objects[i].active || bo_audio_objects[i].object==object) break;
            if (i==128 || !bo_audio_identity(object,&identity)) InterlockedIncrement(&bo_audio_dropped);
            else bo_audio_objects[i]=identity;
            bo_audio_write();InterlockedExchange(&bo_audio_gate,0);
        } else InterlockedIncrement(&bo_audio_dropped);
        InterlockedDecrement(&bo_audio_active);errno=returned_error;
    }
}
void sub_0007EDB0(void)
{
    uint32_t object=0;unsigned i,refs=0;int observe=0;const int error=errno;
    if (black_pc_ram_span(g_esp,8u)) bo_audio_read(NULL,g_esp+4u,&object);
    if (!InterlockedCompareExchange(&bo_audio_gate,1,0)) {
        for (i=0;i<128;i++) if (bo_audio_objects[i].active && bo_audio_objects[i].object==object) {observe=1;break;}
        if (observe) {
            if (bo_audio_publication_refs(bo_audio_read,NULL,object,&refs)) InterlockedExchangeAdd(&bo_audio_refs_before,(LONG)refs);
            else InterlockedIncrement(&bo_audio_bad_reads);
        }
        InterlockedExchange(&bo_audio_gate,0);
    } else InterlockedIncrement(&bo_audio_dropped);
    if (observe) bo_audio_thread_enter();
    errno=error;bo_audio_original_release();
    if (observe) {
        const int returned_error=errno;InterlockedIncrement(&bo_audio_releases);
        if (!InterlockedCompareExchange(&bo_audio_gate,1,0)) {
            if (bo_audio_publication_refs(bo_audio_read,NULL,object,&refs)) InterlockedExchangeAdd(&bo_audio_refs_after,(LONG)refs);
            else InterlockedIncrement(&bo_audio_bad_reads);
            for (i=0;i<128;i++) if (bo_audio_objects[i].object==object) bo_audio_objects[i].active=0;
            bo_audio_write();InterlockedExchange(&bo_audio_gate,0);
        } else InterlockedIncrement(&bo_audio_dropped);
        InterlockedDecrement(&bo_audio_active);errno=returned_error;
    }
}
