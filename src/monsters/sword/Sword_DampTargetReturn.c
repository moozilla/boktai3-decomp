#include "global.h"
#include "monsters/sword/sword_return_component_layout.h"

struct SwordReturnCounter {
    u8 pad[0x126];
    u16 ticks;
};
s32 Mod(s32, s32);
void TargetMotion_AdvanceFollowSegment(struct SwordReturnComponent *);
void Sword_DampTargetReturn(struct SwordReturnComponent *p)
{
    s32 value, half;
    p->rotation += 20;
    p->x += p->velocityX;
    p->z += p->velocityZ;
    if (Mod(++((struct SwordReturnCounter *)p)->ticks, 5) == 0) {
        if (p->velocityX == 0 && p->velocityZ == 0) {
            p->attackRadius = 16;
            p->bodyRadius = 16;
            p->timer = 0;
            p->a = p->target[0] - p->fx - p->startX;
            p->b = p->target[1] - p->fy - p->startY;
            p->c = p->target[2] - p->fz - p->startZ;
            p->endX = p->startX + p->a;
            p->endY = p->startY + p->b;
            p->endZ = p->startZ + p->c;
            p->startX = p->x;
            p->startY = p->y;
            p->startZ = p->z;
            p->dx = p->endX - p->startX;
            p->dy = p->endY - p->startY;
            p->dz = p->endZ - p->startZ;
            p->a = p->target[0];
            p->b = p->target[1];
            p->c = p->target[2];
            p->direction = -p->direction;
            p->byte1DD = -15;
            p->callback17C = TargetMotion_AdvanceFollowSegment;
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
