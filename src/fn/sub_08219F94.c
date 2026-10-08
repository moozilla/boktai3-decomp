#include "global.h"

struct Task { struct Task *prev, *next; u8 pad[0xC]; u8 f14; };
struct List { struct Task *head; u32 pad; };
extern struct List gUnk_03005280[];

void sub_08219F94(struct Task *t)
{
    struct List *l = &gUnk_03005280[t->f14];
    struct Task *prev = t->prev;
    struct Task *next = t->next;
    if (prev != 0)
        prev->next = next;
    else
        l->head = next;
    if (next != 0)
        next->prev = prev;
}
