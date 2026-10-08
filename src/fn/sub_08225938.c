#include "global.h"
struct N { u8 pad[0x40]; struct N *prev; struct N *next; };
struct H { u8 pad[0x18]; struct N *first; struct N *last; };
extern struct H *gUnk_030025F8;
s32 sub_0822590C(struct N *);
s32 sub_08225938(struct N *n)
{
    struct H *h = gUnk_030025F8;
    if (h == 0 || n == 0 || sub_0822590C(n) == 0)
        return 0;
    {
        if (n->prev == 0)
            h->first = n->next;
        else
            n->prev->next = n->next;
        if (n->next == 0)
            h->last = n->prev;
        else
            n->next->prev = n->prev;
    }
    return 1;
}
