#include "global.h"
struct N { u8 pad0[0x4]; u8 f4; u8 pad5[0x5]; u8 idx; u8 padb[0x19]; struct N *prev; struct N *next; };
extern struct N *gUnk_030042C0[];
s32 sub_08214374(struct N *n, u32 i)
{
    struct N *h;
    n->idx = i;
    n->f4 = 1;
    n->prev = 0;
    h = gUnk_030042C0[i];
    n->next = h;
    if (h)
        h->prev = n;
    gUnk_030042C0[i] = n;
    return 0;
}
