#define Armor_AcceleratePartReturnTurn sub_0818D590
#include "global.h"
#include "motion_0818.h"

static inline void finish(struct Motion0818Object *p, u32 one)
{
    p->byte11E = one;
    p->byte11F = one;
    p->flags119 |= one;
}

void sub_0818D590(struct Motion0818Object *p)
{
    u8 *parent = p->parent;
    s32 i;
    for (i = 0; i < (s8)p->done; i++) {
        p->rotation += (u8)p->turnDirection;
        if (p->rotation == 0x80) {
            finish(p, 1);
            return;
        }
    }
    if (!(parent[0x5C2] & 7)) {
        if ((s8)++p->done > 6)
            p->done = 7;
    }
}
