#include "global.h"
struct N { u8 pad0[0x4]; u8 f4; u8 pad5[0xc]; u8 idx; u8 pad12[0xe]; struct N *prev; struct N *next; };
extern struct N *gUnk_030042D0[];
void sub_082143F8(struct N *n, u32 i)
{
    struct N *p = n->prev;
    struct N *q = n->next;
    n->f4 = 0;
    if (p)
        p->next = q;
    else
        gUnk_030042D0[i] = q;
    if (q)
        q->prev = p;
}
