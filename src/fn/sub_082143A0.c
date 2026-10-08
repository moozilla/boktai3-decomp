#include "global.h"
struct N { u8 pad0[0x4]; u8 f4; u8 pad5[0x5]; u8 idx; u8 padb[0x19]; struct N *prev; struct N *next; };
extern struct N *gUnk_030042C0[];
void sub_082143A0(struct N *n, u32 i)
{
    struct N *p = n->prev;
    struct N *q = n->next;
    n->f4 = 0;
    if (p)
        p->next = q;
    else
        gUnk_030042C0[i] = q;
    if (q)
        q->prev = p;
}
