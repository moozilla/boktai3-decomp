#include "actor_position.h"

void ActorVariation_SelectRandomPair(struct ActorState *actor, u16 *out)
{
    u32 table = (u32)gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    s32 sample = *(u16 *)(table + index * 2);
    actor->animationIndex = (sample >> 8) & 3;
    gRandomIndex = (index + 1) & 1023;
    actor->animationDetail = Mod(*(u16 *)(table + gRandomIndex * 2), 5);
    Path_GetPointPositionById(out, actor->animationIndex, actor->animationDetail);
}

static inline s32 ActorPosition_ScaleOffset(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void ActorMovement_FindQuarterTurnPoint(struct ActorState *actor, struct ContactPosition *out)
{
    union ActorPositionVector point;
    struct ContactPosition *center = actor->selectedPoint;
    u32 start = actor->body.direction;
    s32 quarterTurn = 0;
    u32 mask = 255;
    u32 speed = 128;
    s16 *coordinates = point.components;
    const s16 *sine = gUnk_086149C4;
    do {
        u32 angle = (start + (quarterTurn << 6)) & mask;
        {
            s32 x = ActorPosition_ScaleOffset(gUnk_086149C4[(angle + 64) & mask] * 128);
            s32 zero = 0;
            coordinates[0] = x;
            coordinates[1] = zero;
        }
        {
            s32 sample = sine[angle & mask];
            s32 z = ActorPosition_ScaleOffset((s32)(speed * sample));
            coordinates[2] = z;
            {
                u32 x = (u16)center->x;
                u32 dx = (u16)coordinates[0];
                coordinates[0] = x + dx;
            }
            coordinates[1] = (u16)coordinates[1] + (u16)center->y;
            coordinates[2] += center->z;
        }
        if (ActorPosition_TestPath(actor, &point.position)) {
            *out = point.position;
            return;
        }
        quarterTurn++;
    } while (quarterTurn <= 3);
    *out = *center;
}

s32 ActorProperty_GetConstant34(void)
{
    return 0x34;
}
