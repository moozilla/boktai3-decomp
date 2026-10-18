#include "global.h"
#include "scene_actor_terrain.h"
s32 SceneActorTerrain_CompareNeighborHeight(s32 cell, s32 height, s32 direction)
{
    s32 slope = -1;
    s32 nextHeight, kind, delta;
    switch (direction) {
        case 0:
            ++cell;
            slope = 2;
            break;
        case 1:
            cell += SceneActorTerrain_Width();
            slope = 1;
            break;
        case 2:
            --cell;
            slope = 2;
            break;
        case 3:
            cell -= SceneActorTerrain_Width();
            slope = 1;
            break;
    }
    nextHeight = *SceneActorTerrain_GetCell(cell) & 15;
    kind = *SceneActorTerrain_GetCell(cell) >> 4;
    {
        u32 flags = *(u16 *)(SceneActorTerrain_GetCell(cell) + 2);
        u32 mask = 3;
        if (flags & mask) {
            if (nextHeight >= height)
                return 1;
            return 2;
        }
    }
    delta = nextHeight - height;
    if (delta == 1 && kind == slope)
        return 0;
    if (delta == -1 && kind == slope)
        return 0;
    if (delta > 0)
        return 1;
    if (delta < 0)
        return 2;
    return 0;
}
