#include "global.h"
#include "render.h"
struct N {
    u8 pad0[0x4];
    u8 f4;
    u8 pad5[0x5];
    u8 idx;
    u8 padb[0x19];
    struct N *prev;
    struct N *next;
};
s32 Sprite_LinkToList(struct N *n, u32 i)
{
    struct N *h;
    n->idx = i;
    n->f4 = 1;
    n->prev = 0;
    h = gSpriteListHeads[i];
    n->next = h;
    if (h)
        h->prev = n;
    gSpriteListHeads[i] = n;
    return 0;
}
