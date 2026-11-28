#include "bosses/sabata/sabata_position.h"

void ActorVariation_SelectRandomPair(struct SabataActorState *actor, u16 *out)
{
    u32 table = (u32)gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    s32 sample = *(u16 *)(table + index * 2);
    actor->animationIndex = (sample >> 8) & 3;
    gRandomIndex = (index + 1) & 1023;
    actor->animationDetail = Mod(*(u16 *)(table + gRandomIndex * 2), 5);
    Path_GetPointPositionById(out, actor->animationIndex, actor->animationDetail);
}

static inline s32 SabataPosition_ScaleOffset(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void ActorMovement_FindQuarterTurnPoint(struct SabataActorState *actor, struct ContactPosition *out)
{
    struct ContactPosition point;
    struct ContactPosition *center = actor->selectedPoint;
    u32 start = actor->body.direction;
    s32 quarterTurn = 0;
    u32 mask = 255;
    u32 speed = 128;
    struct ContactPosition *coordinates = &point;
    for (; quarterTurn < 4; quarterTurn++) {
        u32 angle = (start + (quarterTurn << 6)) & mask;
        s32 x = SabataPosition_ScaleOffset(gUnk_086149C4[(angle + 64) & mask] * 128);
        coordinates->x = x;
        coordinates->y = 0;
        {
            s32 sample = gUnk_086149C4[angle & mask];
            coordinates->z = SabataPosition_ScaleOffset(speed * sample);
            coordinates->x += center->x;
            coordinates->y += center->y;
            coordinates->z += center->z;
        }
        if (SabataPosition_TestPath(actor, &point)) {
            *out = point;
            return;
        }
    }
    *out = *center;
}

s32 ActorProperty_GetConstant34(void)
{
    return 0x34;
}
