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
s32 Effect_LinkToList(struct N *n, u32 i)
{
    struct N *h;
    n->idx = i;
    n->f4 = 1;
    n->prev = 0;
    h = gDrawableListHeads[i];
    n->next = h;
    if (h)
        h->prev = n;
    gDrawableListHeads[i] = n;
    return 0;
}
