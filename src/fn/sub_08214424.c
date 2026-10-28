#include "global.h"
#include "render.h"
struct N {
    u8 pad0[0x4];
    u8 f4;
    u8 pad5[0x17];
    u8 idx;
    u8 pad1d[0x3b];
    struct N *prev;
    struct N *next;
};
s32 Anim_LinkToList(struct N *n, u32 i)
{
    struct N *h;
    n->idx = i;
    n->f4 = 1;
    n->prev = 0;
    h = gMainSpriteListHeads[i];
    n->next = h;
    if (h)
        h->prev = n;
    gMainSpriteListHeads[i] = n;
    return 0;
}
