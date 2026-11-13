#include "actor_position.h"

static inline s32 ActorPosition_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

s32 ActorMovement_CanReachTarget(struct ActorState *actor)
{
    s32 targetCell, actorCell;
    struct ContactPosition *position;
    u32 raw;
    s32 x, z;
    raw = (u16)actor->selectedPoint->x;
    x = (s16)raw >> 8;
    raw = (u16)actor->selectedPoint->z;
    z = (s16)raw >> 8;
    targetCell = ActorPosition_CellIndex(x, z);
    {
        struct ContactPosition *view = &actor->body.position;
        u32 raw;
        s32 x, z;
        raw = (u16)view->x;
        x = (s16)raw >> 8;
        raw = (u16)view->z;
        z = (s16)raw >> 8;
        position = view;
        actorCell = ActorPosition_CellIndex(x, z);
    }
    if (targetCell == actorCell)
        return 1;
    if (((actor->requestedDirection - actor->body.direction) & 255) + 0x30U > 0x60)
        return 0;
    if (actor->threshold > 0x90000)
        return 0;
    if (!sub_0821D4F0(position, actor->selectedPoint, 10, 256))
        return 0;
    return 1;
}

s32 ActorPosition_SaveSelectedPoint(struct ActorState *actor)
{
    s32 result;
    if (!ActorMovement_CanReachTarget(actor))
        result = 0;
    else {
        struct ContactPosition *source = actor->selectedPoint;
        actor->savedPosition = *source;
        result = 1;
    }
    return result;
}
