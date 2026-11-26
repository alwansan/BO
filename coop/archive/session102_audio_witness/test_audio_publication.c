/* Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT */
#include "audio_publication.h"
#include <assert.h>
#include <stddef.h>
struct Word { uint32_t address, value; };
struct Memory { struct Word words[16]; unsigned count; };
static int read_word(void *context, uint32_t at, uint32_t *out)
{
    struct Memory *m = context; unsigned i;
    for (i = 0; i < m->count; i++) if (m->words[i].address == at) { *out = m->words[i].value; return 1; }
    return 0;
}
int main(void)
{
    struct Memory m = {{{0x575508,0x10010},{0x10004,0x20000},
                        {0x20008,0x30000},{0x20004,0x10000},{0x10010,0x575508}},5};
    unsigned refs = 123;
    assert(bo_audio_publication_refs(read_word,&m,0x30000,&refs) && refs == 1);
    assert(bo_audio_publication_refs(read_word,&m,0x40000,&refs) && refs == 0);
    m.words[3].value = 0x20000; refs = 123;
    assert(!bo_audio_publication_refs(read_word,&m,0x30000,&refs) && refs == 123);
    m.words[3].value = 0x10000; m.words[4].value = 0x10010;
    assert(!bo_audio_publication_refs(read_word,&m,0x30000,&refs));
    m.words[0].value = 0x575508;
    assert(bo_audio_publication_refs(read_word,&m,0x30000,&refs) && refs == 0);
    m.count = 0; assert(!bo_audio_publication_refs(read_word,&m,0x30000,&refs));
    return 0;
}
