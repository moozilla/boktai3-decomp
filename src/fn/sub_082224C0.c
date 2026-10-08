#include "global.h"
struct Nd { u32 key; u32 p; struct Nd *next; };
struct T { u32 p0; struct Nd **buckets; u32 size; };
u32 sub_08249240(struct T *, u32, u32);
struct Nd *sub_082224C0(struct T *t, u32 key)
{
    struct Nd **b = t->buckets;
    struct Nd *n;
    u32 sz;
    if (b == 0 || (sz = t->size) == 0) return 0;
    n = b[sub_08249240(t, key, sz)];
    for (; n != 0; n = n->next) {
        u32 k = n->key;
        u32 m = 0;
        if (k == key) m = 1;
        if (m) return n;
    }
    return 0;
}
