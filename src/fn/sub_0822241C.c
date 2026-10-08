#include "global.h"
struct Nd { u32 key; u32 p; struct Nd *next; };
struct T { u32 p0; struct Nd **buckets; u32 size; };
u32 sub_08249240(struct T *, u32, u32);
struct Nd *sub_082224C0(struct T *, u32);
s32 sub_0822241C(struct T *t, struct Nd *n)
{
    struct Nd **b = t->buckets;
    u32 idx;
    u32 sz;
    if (b == 0 || (sz = t->size) == 0) return -1;
    idx = sub_08249240(t, n->key, sz);
    if (sub_082224C0(t, n->key) != 0) return -2;
    n->next = b[idx];
    b[idx] = n;
    return 0;
}
