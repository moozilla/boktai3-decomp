#include "global.h"
#include "motion_0818.h"

s32 Div(s32, s32);

void SpinningApproach_UpdateOwnerRelativeSegment(struct Motion0818Object *p)
{
    struct Motion0818Displacement displacement;
    u8 *owner = p->parent;
    displacement.x = *(u16 *)(owner + 0x5ac) - p->a;
    displacement.y = *(u16 *)(owner + 0x5ae) - p->b;
    displacement.z = *(u16 *)(owner + 0x5b0) - p->c;
    p->rotation += p->byte1DD;
    p->x = displacement.x + p->startX + Div(p->dx * p->timer, 100);
    p->y = displacement.y + p->startY + Div(p->dy * p->timer, 100);
    p->z = displacement.z + p->startZ + Div(p->dz * p->timer, 100);
    p->timer++;
    if (p->timer > 100) {
        u8 one;
        one = p->done = 1;
        p->rotation = 128;
        p->byte11E = one;
        p->byte11F = one;
        p->flags119 |= one;
    }
}
