#include "global.h"
#include "render.h"
struct N {
    u8 pad0[0x4];
    u8 f4;
    u8 pad5[0xc];
    u8 idx;
    u8 pad12[0xe];
    struct N *prev;
    struct N *next;
};
void Effect_UnlinkFromList(struct N *n, u32 i)
{
    struct N *p = n->prev;
    struct N *q = n->next;
    n->f4 = 0;
    if (p)
        p->next = q;
    else
        gDrawableListHeads[i] = q;
    if (q)
        q->prev = p;
}
