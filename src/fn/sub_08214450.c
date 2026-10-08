#include "global.h"

struct Node {
    u8 pad0[4];
    u8 f4;
    u8 pad5[0x53];
    struct Node *prev;
    struct Node *next;
};

extern struct Node *gUnk_030042C8[];

void sub_08214450(struct Node *n, u32 i)
{
    struct Node *prev = n->prev;
    struct Node *next = n->next;
    n->f4 = 0;
    if (prev)
        prev->next = next;
    else
        gUnk_030042C8[i] = next;
    if (next)
        next->prev = prev;
}
