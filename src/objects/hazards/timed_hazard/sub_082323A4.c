#include "global.h"
struct N {
    u8 p[0x78];
    struct N *prev;
    struct N *next;
};
struct L {
    u8 p[0x1c];
    struct N *head;
};
u32 TimedHazard_LinkInstance(struct L *l, struct N *n)
{
    struct N *h;
    n->prev = 0;
    h = l->head;
    n->next = h;
    if (h)
        h->prev = n;
    l->head = n;
    return 0;
}
