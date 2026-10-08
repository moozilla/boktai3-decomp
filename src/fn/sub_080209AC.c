#include "global.h"

struct N { u8 f[0x2c]; struct N *prev; struct N *next; };
struct S { u8 f[0x74]; struct N *head; };
u32 sub_080209AC(struct S *p, struct N *n)
{
    struct N *q = n->prev;
    if (q)
        q->next = n->next;
    else
        p->head = n->next;
    q = n->next;
    if (q)
        q->prev = n->prev;
    return 0;
}
