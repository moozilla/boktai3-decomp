#include "global.h"
#include "contact_actor.h"
#define SceneTerrain_FindReachableCell SceneActorTerrain_FindReachableCell
s32 SceneActorTerrain_FindReachableCell(struct ContactPosition *, s32, s32);
static inline s32 SceneTerrain_RoundDistance(s32 x)
{
    if (x < 0)
        x = -x;
    x += 128;
    if (x >= 0)
        return x >> 8;
    return -((-x) >> 8);
}
s32 SceneTerrain_IsPathBlocked(struct ContactPosition *a, struct ContactPosition *b, s32 direction,
                 s32 targetCell)
{
    s32 count;
    switch (direction) {
        case 0:
        case 2:
            count = SceneTerrain_RoundDistance(a->x - b->x) + 1;
            break;
        case 1:
        case 3:
            count = SceneTerrain_RoundDistance(a->z - b->z) + 1;
            break;
        default:
            count = 0;
            break;
    }
    if (SceneActorTerrain_FindReachableCell(a, direction, count) != targetCell)
        return 1;
    return 0;
}
