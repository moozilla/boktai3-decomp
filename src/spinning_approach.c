#include "spinning_approach.h"

void SpinningApproach_InitMotion(SpinningApproach *motion)
{
    struct SpinningApproachOwner *owner = (struct SpinningApproachOwner *)motion->parent;
    motion->angle = Math_Atan2(gGameState->positionX - motion->x, gGameState->positionZ - motion->z);
    motion->byte1DD = 0;
    motion->callback = SpinningApproach_UpdateSpinRamp;
    motion->a = 0;
    motion->b = 0;
    motion->c = 0;
    motion->fx = owner->x - motion->x;
    motion->fy = owner->y - motion->y;
    motion->fz = owner->z - motion->z;
    motion->startX = motion->x;
    motion->startY = motion->y;
    motion->startZ = motion->z;
    motion->endX = motion->x + Div(800 * gUnk_086149C4[(motion->angle + 64) & 255], 4096);
    motion->endY = (u16)gGameState->positionY + 220;
    motion->endZ = motion->z + Div(800 * gUnk_086149C4[motion->angle], 4096);
    motion->dx = motion->endX - motion->startX;
    motion->dy = motion->endY - motion->startY;
    motion->dz = motion->endZ - motion->startZ;
    motion->timer = 0;
}

static inline s32 SpinningApproach_Absolute(s32 x)
{
    if (x < 0)
        x = -x;
    return x;
}

void SpinningApproach_UpdateSpinRamp(SpinningApproach *motion)
{
    if ((((struct SpinningApproachOwner *)motion->parent)->flags & 3) == 0) {
        motion->byte1DD--;
        if (SpinningApproach_Absolute(motion->byte1DD) > 15)
            motion->byte1DD = -15;
    }
    motion->rotation += motion->byte1DD;
    motion->timer--;
    if (motion->timer < 0) {
        motion->timer = 0;
        if (SpinningApproach_Absolute(motion->byte1DD) == 15)
            motion->callback = SpinningApproach_UpdateApproach;
    }
}

void SpinningApproach_UpdateApproach(SpinningApproach *motion)
{
    struct SpinningApproachOwner *owner = (struct SpinningApproachOwner *)motion->parent;
    motion->rotation += motion->byte1DD;
    motion->x = motion->startX + Div(motion->dx * motion->timer, 100);
    motion->y = motion->startY + Div(motion->dy * motion->timer, 100);
    motion->z = motion->startZ + Div(motion->dz * motion->timer, 100);
    motion->timer++;
    if (motion->timer > 100) {
        motion->callback = SpinningApproach_UpdateOwnerRelativeSegment;
        motion->timer = 0;
        motion->a = owner->x - motion->fx - motion->startX;
        motion->b = owner->y - motion->fy - motion->startY;
        motion->c = owner->z - motion->fz - motion->startZ;
        motion->endX = motion->startX + motion->a;
        motion->endY = motion->startY + motion->b;
        motion->endZ = motion->startZ + motion->c;
        motion->startX = motion->x;
        motion->startY = motion->y;
        motion->startZ = motion->z;
        motion->dx = motion->endX - motion->startX;
        motion->dy = motion->endY - motion->startY;
        motion->dz = motion->endZ - motion->startZ;
        motion->a = owner->x;
        motion->b = owner->y;
        motion->c = owner->z;
    }
}

void SpinningApproach_UpdateOwnerRelativeSegment(SpinningApproach *motion)
{
    struct Motion0818Displacement displacement;
    struct SpinningApproachOwner *owner = (struct SpinningApproachOwner *)motion->parent;
    displacement.x = owner->x - motion->a;
    displacement.y = owner->y - motion->b;
    displacement.z = owner->z - motion->c;
    motion->rotation += motion->byte1DD;
    motion->x = displacement.x + motion->startX + Div(motion->dx * motion->timer, 100);
    motion->y = displacement.y + motion->startY + Div(motion->dy * motion->timer, 100);
    motion->z = displacement.z + motion->startZ + Div(motion->dz * motion->timer, 100);
    motion->timer++;
    if (motion->timer > 100) {
        u8 finished;
        finished = motion->done = 1;
        motion->rotation = 128;
        motion->byte11E = finished;
        motion->byte11F = finished;
        motion->flags119 |= finished;
    }
}
