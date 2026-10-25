#include "scene_actor_terrain.h"

u8 SceneActorTerrain_CheckNeighborHeight(s32 direction, s32 cell, struct ContactPosition *position);
s32 SceneTerrain_IsPathBlocked(struct ContactPosition *a, struct ContactPosition *b, s32 direction,
                               s32 targetCell);

static inline s32 CellHeight_08076CDC(s32 cell, const struct ContactPosition *position)
{
    u32 attributes = *SceneActorTerrain_GetCell(cell);
    u32 kind = attributes >> 4;
    s32 height = (attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= ((const u8 *)position)[4];
            break;
        case 2:
            height -= ((const u8 *)position)[0];
            break;
    }
    return height;
}

#ifdef NONMATCHING
// fuzzy: 52.0%
// scope shape 184 (differences once low registers are set aside)
// confidence=low: steps an actor toward a neighboring cell when the height step and path allow it; returns -1 when blocked.
s32 sub_08076CDC(struct SceneActorTerrainView *actor, struct ContactPosition *pos,
                 struct ContactPosition *target)
{
    struct ContactPosition centerA, centerT, delta, e;
    s32 targetCell, actorCell, h0, h1, dh, dir, dir2, flag;

    targetCell = SceneActorTerrain_GetCellIndex(target);
    actorCell = actor->terrainCell;
    h0 = CellHeight_08076CDC(actorCell, pos);
    h1 = CellHeight_08076CDC(targetCell, target);
    flag = 0;
    if (h0 == h1)
        return flag;
    if (h0 > h1) {
        SceneActorTerrain_Subtract(&delta, target, pos);
        dh = h0 - h1;
        if (dh > 255) {
            dh >>= 8;
            if (dh > 1)
                return -1;
            flag = 1;
        }
        SceneActorTerrain_GetCenter(actorCell, &centerA);
        SceneActorTerrain_GetCenter(targetCell, &centerT);
        centerA.y = h0;
        centerT.y = h1;
        SceneActorTerrain_Subtract(&e, &centerA, &centerT);
        dir = ((Math_Atan2(e.x, e.z) + 0x20) >> 6) & 3;
        if (!SceneActorTerrain_CheckNeighborHeight(dir, targetCell, &centerT))
            return -1;
        SceneActorTerrain_Subtract(&e, &centerT, &centerA);
        dir2 = ((Math_Atan2(e.x, e.z) + 0x20) >> 6) & 3;
        if (!SceneActorTerrain_CheckNeighborHeight(dir2, actorCell, &actor->position))
            return -1;
        if (SceneTerrain_IsPathBlocked(&centerA, &centerT, dir2, targetCell))
            return -1;
        return flag;
    } else {
        dh = h1 - h0;
        if (dh > 0x17F)
            return -1;
        SceneActorTerrain_GetCenter(actorCell, &centerA);
        SceneActorTerrain_GetCenter(targetCell, &centerT);
        centerA.y = h0;
        centerT.y = h1;
        SceneActorTerrain_Subtract(&e, &centerA, &centerT);
        dir = ((Math_Atan2(e.x, e.z) + 0x20) >> 6) & 3;
        if (!SceneActorTerrain_CheckNeighborHeight(dir, actorCell, &actor->position))
            return -1;
        dir2 = (dir + 2) & 3;
        if (SceneTerrain_IsPathBlocked(&centerT, &centerA, dir2, actorCell))
            return -1;
        if (dh > 255)
            return 2;
        pos->y = target->y;
        return 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08076CDC);
#endif
