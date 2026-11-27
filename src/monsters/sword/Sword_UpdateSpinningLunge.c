#include "global.h"
#include "monsters/sword/sword_return_component_layout.h"
s32 Div(s32, s32);
void TargetMotion_AdvanceFollowSegment(struct SwordReturnComponent *);
void Sword_UpdateSpinningLunge(struct SwordReturnComponent *p)
{
    p->rotation += p->byte1DD;
    p->x = p->startX + Div(p->dx * p->timer, 100);
    p->y = p->startY + Div(p->dy * p->timer, 100);
    p->z = p->startZ + Div(p->dz * p->timer, 100);
    p->timer++;
    if (p->timer > 100) {
        p->callback17C = TargetMotion_AdvanceFollowSegment;
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
    }
}
