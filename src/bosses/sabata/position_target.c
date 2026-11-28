#include "bosses/sabata/sabata_position.h"

static inline s32 SabataPosition_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

s32 ActorMovement_CanReachTarget(struct SabataActorState *actor)
{
    s32 targetCell, actorCell;
    struct ContactPosition *position;
    targetCell =
        SabataPosition_CellIndex(actor->selectedPoint->x >> 8, actor->selectedPoint->z >> 8);
    {
        struct ContactPosition *view = &actor->body.position;
        s32 x = view->x >> 8;
        s32 z = view->z >> 8;
        position = view;
        actorCell = SabataPosition_CellIndex(x, z);
    }
    if (targetCell == actorCell)
        return 1;
    if (((actor->requestedDirection - actor->body.direction) & 255) + 0x30U > 0x60)
        return 0;
    if (actor->threshold > 0x90000)
        return 0;
    if (!Terrain_IsSegmentBelowHeightLimit(position, actor->selectedPoint, 10, 256))
        return 0;
    return 1;
}

s32 SabataPosition_SaveSelectedPoint(struct SabataActorState *actor)
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
