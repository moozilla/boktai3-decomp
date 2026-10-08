#include "global.h"

struct Task { struct Task *prev, *next; u8 pad[0xC]; u8 f14; };
struct List { struct Task *head; u32 pad; };
extern struct List gUnk_03005280[];

void sub_08219F74(struct Task *t)
{
    struct List *l = &gUnk_03005280[t->f14];
    struct Task *h = l->head;
    if (h != 0)
        h->prev = t;
    t->next = h;
    l->head = t;
}
