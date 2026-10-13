#include "motion_0818.h"

s32 Div(s32, s32);
void sub_0818DB10(struct Motion0818Object *);
void sub_0818D960(struct Motion0818Object *p)
{
    u8 *owner = p->parent;
    p->rotation += p->byte1DD;
    p->x = p->startX + Div(p->dx * p->timer, 100);
    p->y = p->startY + Div(p->dy * p->timer, 100);
    p->z = p->startZ + Div(p->dz * p->timer, 100);
    p->timer++;
    if (p->timer > 100) {
        p->callback = sub_0818DB10;
        p->timer = 0;
        p->a = *(u16 *)(owner + 0x5ac) - p->fx - p->startX;
        p->b = *(u16 *)(owner + 0x5ae) - p->fy - p->startY;
        p->c = *(u16 *)(owner + 0x5b0) - p->fz - p->startZ;
        p->endX = p->startX + p->a;
        p->endY = p->startY + p->b;
        p->endZ = p->startZ + p->c;
        p->startX = p->x;
        p->startY = p->y;
        p->startZ = p->z;
        p->dx = p->endX - p->startX;
        p->dy = p->endY - p->startY;
        p->dz = p->endZ - p->startZ;
        p->a = *(u16 *)(owner + 0x5ac);
        p->b = *(u16 *)(owner + 0x5ae);
        p->c = *(u16 *)(owner + 0x5b0);
    }
}
