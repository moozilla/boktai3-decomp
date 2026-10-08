#include "global.h"
struct N { u8 pad0[0x4]; u8 f4; u8 pad5[0xc]; u8 idx; u8 pad12[0xe]; struct N *prev; struct N *next; };
extern struct N *gUnk_030042D0[];
s32 sub_082143CC(struct N *n, u32 i)
{
    struct N *h;
    n->idx = i;
    n->f4 = 1;
    n->prev = 0;
    h = gUnk_030042D0[i];
    n->next = h;
    if (h)
        h->prev = n;
    gUnk_030042D0[i] = n;
    return 0;
}
