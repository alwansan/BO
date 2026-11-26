/* Calibration only. Fake memory and thread IDs cannot establish engine evidence. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int32_t LONG;
static LONG InterlockedCompareExchange(volatile LONG *p,LONG value,LONG expected) { LONG old=*p; if(old==expected)*p=value; return old; }
static LONG InterlockedExchange(volatile LONG *p,LONG value) { LONG old=*p;*p=value;return old; }
static LONG InterlockedIncrement(volatile LONG *p) { return ++*p; }
static LONG InterlockedDecrement(volatile LONG *p) { return --*p; }
static LONG current_thread=1;
static LONG GetCurrentThreadId(void) { return current_thread; }
static unsigned char memory[0x10000];
static uint32_t g_esp=0x100;
#define GUEST32(at) (*(uint32_t *)(memory+(at)))
#define BLACK_FPV_BYTES 0x1330u
static int black_pc_ram_span(uint32_t p,uint32_t n) { return p && p<sizeof(memory) && n<=sizeof(memory)-p; }
static uint32_t black_fpv(void) { return 0x2000; }
#include "audio_observer.inc"
int main(void)
{
    uint32_t object;
    GUEST32(g_esp+24)=0x2208;
    object=bo_audio_enter(0);
    assert(object==0x2208 && bo_audio_active==1);
    memory[object]=1;
    bo_audio_leave(0,object);
    bo_audio_after_take();
    assert(bo_audio_counts[7]==1 && bo_audio_counts[8]==1);
    memory[object]=2;
    bo_audio_after_take();
    assert(bo_audio_counts[8]==2);
    current_thread=2;
    GUEST32(g_esp+4)=object;
    assert(bo_audio_enter(1)==object);
    assert(bo_audio_counts[2]==1);
    (void)bo_audio_enter(0);
    assert(bo_audio_counts[3]==1);
    bo_audio_leave(0,object);
    bo_audio_leave(1,object);
    assert(bo_audio_active==0);
    bo_audio_after_take();
    assert(bo_audio_counts[7]==3 && bo_audio_counts[8]==2); /* Destroyed object no longer sampled. */
    g_esp=sizeof(memory)-4;
    assert(bo_audio_enter(0)==0);
    bo_audio_leave(0,0);
    assert(bo_audio_counts[4]==1);
    bo_audio_lock=1;
    bo_audio_after_take();
    assert(bo_audio_counts[6]==1);
    bo_audio_lock=0;
    {
        unsigned i;
        g_esp=0x100;
        for(i=0;i<65;++i) {
            GUEST32(g_esp+24)=0x4000+i*0x60;
            object=bo_audio_enter(0);
            bo_audio_leave(0,object);
        }
        assert(bo_audio_counts[5]==1);
    }
    bo_audio_write();
    puts("AUDIO_OBSERVER_CALIBRATION_PASS_NO_ENGINE_EVIDENCE");
    return 0;
}
