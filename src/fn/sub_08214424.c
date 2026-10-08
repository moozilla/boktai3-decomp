#include "global.h"
struct N { u8 pad0[0x4]; u8 f4; u8 pad5[0x17]; u8 idx; u8 pad1d[0x3b]; struct N *prev; struct N *next; };
extern struct N *gUnk_030042C8[];
s32 sub_08214424(struct N *n, u32 i)
{
    struct N *h;
    n->idx = i;
    n->f4 = 1;
    n->prev = 0;
    h = gUnk_030042C8[i];
    n->next = h;
    if (h)
        h->prev = n;
    gUnk_030042C8[i] = n;
    return 0;
}
