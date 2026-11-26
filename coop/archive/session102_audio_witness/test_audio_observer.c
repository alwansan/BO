/* Calibration of the actual observer include, NOT original engine evidence. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
typedef int32_t LONG;
static LONG InterlockedCompareExchange(volatile LONG *p,LONG value,LONG expected){__atomic_compare_exchange_n(p,&expected,value,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);return expected;}
static LONG InterlockedIncrement(volatile LONG *p){return __atomic_add_fetch(p,1,__ATOMIC_SEQ_CST);}
static LONG InterlockedDecrement(volatile LONG *p){return __atomic_sub_fetch(p,1,__ATOMIC_SEQ_CST);}
static LONG InterlockedExchange(volatile LONG *p,LONG value){return __atomic_exchange_n(p,value,__ATOMIC_SEQ_CST);}
static LONG InterlockedExchangeAdd(volatile LONG *p,LONG value){return __atomic_fetch_add(p,value,__ATOMIC_SEQ_CST);}
static uint32_t GetCurrentThreadId(void){return 1;}
static unsigned char memory[0x600000];
#define GUEST32(p) (*(uint32_t *)(void *)(memory+(p)))
static int black_pc_ram_span(uint32_t p,uint32_t size){return p>=0x10000&&(uint64_t)p+size<=sizeof(memory);}
static uint32_t g_esp=0x20000,g_eax;
static unsigned creates,releases;
#define BO_AUDIO_CALIBRATION 1
#include "audio_observer.inc"
void bo_audio_original_create(void){
    const uint32_t object=GUEST32(g_esp+0x18);++creates;g_eax=object;
    GUEST32(object+0xC)=0x2C2C38;GUEST32(object+0x58)=0x40000;
    GUEST32(object+0x4C)=0x50000;GUEST32(object+0x54)=4;
    GUEST32(0x575508)=0x30010;GUEST32(0x30010)=0x575508;
    GUEST32(0x30004)=0x31000;GUEST32(0x31004)=0x30000;GUEST32(0x31008)=object;
}
void bo_audio_original_release(void){++releases;GUEST32(0x30004)=0x30000;g_eax=17;}
int main(void){
    const uint32_t object=0x10000;
    GUEST32(g_esp+4)=0x2C2C38;GUEST32(g_esp+0x18)=object;
    sub_0007EF50();assert(creates==1&&g_eax==object&&bo_audio_creates==1);
    bo_audio_after_take();assert(bo_audio_published_identity_changes==0);
    GUEST32(object+0x4C)=0x51000;bo_audio_after_take();assert(bo_audio_published_identity_changes==1);
    GUEST32(g_esp+4)=object;sub_0007EDB0();
    assert(releases==1&&g_eax==17&&bo_audio_releases==1);
    assert(bo_audio_refs_before==1&&bo_audio_refs_after==0&&bo_audio_active==0);
    assert(bo_audio_bad_reads==0&&bo_audio_dropped==0);
    return 0;
}
