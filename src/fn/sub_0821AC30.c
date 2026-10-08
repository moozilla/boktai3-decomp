#include "global.h"
struct Ent { u32 key; u32 v; };
struct Blk { struct Blk *next; s32 n; struct Ent *ents; };
extern struct Blk *gUnk_02000458;
struct Ent *sub_0821AC30(u32 key)
{
    struct Blk *b = gUnk_02000458;
    while (b) {
        struct Ent *e = b->ents;
        s32 n = b->n;
        for (; n > 0; e++, n--)
            if (e->key == key) return e;
        b = b->next;
    }
    return 0;
}
