/* Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT */
#ifndef BO_AUDIO_PUBLICATION_H
#define BO_AUDIO_PUBLICATION_H
#include <stdint.h>
typedef int (*BoAudioRead32)(void *, uint32_t, uint32_t *);
/* Actual c5 81030 traversal: outer node-10 is a publication-list head;
 * inner nodes hold the referenced audio-object address at +8. No writes. */
static int bo_audio_publication_refs(BoAudioRead32 read, void *context,
                                    uint32_t object, unsigned *references)
{
    const uint32_t root = 0x575508u;
    uint32_t group, next;
    unsigned groups = 0, refs = 0, entries = 0;
    if (!read(context, root, &group)) return 0;
    while (group != root) {
        uint32_t head, item;
        if (++groups > 256 || group < 0x10010u || group > UINT32_MAX - 8u) return 0;
        head = group - 0x10u;
        if (!read(context, head + 4u, &item)) return 0;
        while (item != head) {
            uint32_t value;
            if (++entries > 4096 || item < 0x10000u || item > UINT32_MAX - 8u ||
                !read(context, item + 8u, &value) || !read(context, item + 4u, &next)) return 0;
            if (value == object) ++refs;
            item = next;
        }
        if (!read(context, group, &next)) return 0;
        group = next;
    }
    *references = refs;
    return 1;
}
#endif
