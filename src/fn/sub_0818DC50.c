#include "motion_0818.h"
struct ReturnDampingCounter {
    u8 pad[0x126];
    u16 ticks;
};
s32 Mod(s32, s32);
void SpinningApproach_UpdateOwnerRelativeSegment(struct Motion0818Object *);
void SpinningApproach_DampReturnMovement(struct Motion0818Object *p)
{
    u8 *owner = p->parent;
    s32 value, half;
    p->rotation += 20;
    p->x += p->velocityX;
    p->z += p->velocityZ;
    if (Mod(++((struct ReturnDampingCounter *)p)->ticks, 5) == 0) {
        if (p->velocityX == 0 && p->velocityZ == 0) {
            p->fieldA6 = 16;
            p->field52 = 16;
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
            p->turnDirection = -p->turnDirection;
            p->byte1DD = -15;
            p->callback = SpinningApproach_UpdateOwnerRelativeSegment;
        }
        value = p->velocityX;
        if (value >= 0)
            half = value >> 1;
        else
            half = -(-value >> 1);
        p->velocityX = half;
        value = p->velocityZ;
        if (value >= 0)
            half = value >> 1;
        else
            half = -(-value >> 1);
        p->velocityZ = half;
    }
}
