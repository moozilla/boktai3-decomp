#include "global.h"

static inline void orm(u32 *a, u32 m)
{
    *a |= m;
}
struct N {
    u32 v;
    u32 flags;
    u32 w8;
    struct N *next;
};
struct L {
    u8 f00[0x18];
    struct N *head;
};
extern struct L *volatile gSceneLinkList;

s32 TargetRegistry_RegisterSceneActor(struct N *n, u32 v, u32 w)
{
    struct N *q;
    struct L *g = gSceneLinkList;
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
    n->next = gSceneLinkList->head;
    gSceneLinkList->head = n;
    orm(&n->flags, 0x10);
done:
    return 1;
}
