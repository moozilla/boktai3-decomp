#include "global.h"
#include "render.h"

struct Node {
    u8 pad0[4];
    u8 f4;
    u8 pad5[0x53];
    struct Node *prev;
    struct Node *next;
};

void Anim_UnlinkFromList(struct Node *n, u32 i)
{
    struct Node *prev = n->prev;
    struct Node *next = n->next;
    n->f4 = 0;
    if (prev)
        prev->next = next;
    else
        gMainSpriteListHeads[i] = next;
    if (next)
        next->prev = prev;
}
