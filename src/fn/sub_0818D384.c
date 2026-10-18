#include "global.h"
#include "motion_0818.h"

void Armor_UpdatePartLungeDelay(struct Motion0818Object *);

void Armor_AcceleratePartTurn(struct Motion0818Object *p)
{
    u8 *parent = p->parent;
    s32 i;
    for (i = 0; i < (s8)p->done; i++) {
        p->rotation += (u8)p->turnDirection;
        if (p->rotation == p->targetRotation) {
            p->timer = 30;
            p->callback = Armor_UpdatePartLungeDelay;
            return;
        }
    }
    if (!(parent[0x5C2] & 7)) {
        if ((s8)++p->done > 6)
            p->done = 7;
    }
}
