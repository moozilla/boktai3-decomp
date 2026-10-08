#include "global.h"

static inline void orm(u32 *a, u32 m) { *a |= m; }
struct N { u32 v; u32 flags; u32 w8; struct N *next; };
struct L { u8 f00[0x18]; struct N *head; };
extern struct L *volatile gUnk_02000598;

s32 sub_0815F6F0(struct N *n, u32 v, u32 w)
{
    struct N *q;
    struct L *g = gUnk_02000598;
    if (g == 0)
        return 0;
    n->v = v;
    n->w8 = w;
    q = g->head;
    if (q != 0) {
        do {
            if (q == n)
                goto done;
            q = q->next;
        } while (q != 0);
    }
    n->next = gUnk_02000598->head;
    gUnk_02000598->head = n;
    orm(&n->flags, 0x10);
done:
    return 1;
}
