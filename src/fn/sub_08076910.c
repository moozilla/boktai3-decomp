// MATCHING_CONCESSION: register-pinning
/* One minimized r1 hint is in the shared terrain-height helper. */
#include "scene_actor_terrain.h"
s32 SceneActorTerrain_GetNeighborCell(struct SceneActorTerrainView *actor, s32 direction)
{
    struct ContactPosition center;
    s32 nextCell;
    s32 nextKind;
    s32 currentKind;
    u32 nextFlags;
    s32 nextHeight, currentHeight, difference, layerDelta;
    switch (direction) {
        case 0:
            nextCell = actor->terrainCell + 1;
            break;
        case 1: {
            u16 *current = &actor->terrainCell;
            nextCell = SceneActorTerrain_Width() + *current;
            break;
        }
        case 2:
            nextCell = actor->terrainCell - 1;
            break;
        case 3: {
            u16 *current = &actor->terrainCell;
            u32 cellValue = *current;
            nextCell = cellValue - SceneActorTerrain_Width();
            break;
        }
        default:
            nextCell = actor->terrainCell;
            break;
    }
    nextKind = *SceneActorTerrain_GetCell(nextCell) >> 4;
    currentKind = *SceneActorTerrain_GetCell(actor->terrainCell) >> 4;
    nextFlags = *(u16 *)(SceneActorTerrain_GetCell(nextCell) + 2);
    SceneActorTerrain_GetCenter(nextCell, &center);
    nextHeight = SceneActorTerrain_GetHeight(nextCell, &center);
    SceneActorTerrain_GetCenter(actor->terrainCell, &center);
    currentHeight = SceneActorTerrain_GetHeight(actor->terrainCell, &center);
    difference = nextHeight - currentHeight;
    layerDelta = (((currentHeight + 255) >> 8) - ((nextHeight + 255) >> 8)) * 256;
    if (nextFlags & 0x43)
        return -1;
    if (!difference)
        return nextCell;
    if (nextKind == 1) {
        if (direction == 1) {
            if (currentKind == 1) {
                if (layerDelta != 256)
                    return -1;
            } else if (layerDelta != 0)
                return -1;
        } else if (direction == 3) {
            if (currentKind == 1) {
                if (layerDelta != -256)
                    return -1;
            } else {
                if (layerDelta != -256)
                    return -1;
            }
        } else
            return -1;
    } else if (nextKind == 2) {
        if (direction == 0) {
            if (currentKind == 2) {
                if (layerDelta != 256)
                    return -1;
            } else if (layerDelta != 0)
                return -1;
        } else if (direction == 2) {
            if (currentKind == 2) {
                if (layerDelta != -256)
                    return -1;
            } else {
                if (layerDelta != -256)
                    return -1;
            }
        } else
            return -1;
    }
    if (currentKind == 1) {
        if (direction == 1) {
            if (nextKind == 1) {
                if (layerDelta != 256)
                    return -1;
            } else {
                if (layerDelta != 256)
                    return -1;
            }
        } else if (direction == 3) {
            if (nextKind == 1) {
                if (layerDelta != -256)
                    return -1;
            } else if (layerDelta != 0)
                return -1;
        } else
            return -1;
    } else if (currentKind == 2) {
        if (direction == 0) {
            if (nextKind == 2) {
                if (layerDelta != 256)
                    return -1;
            } else {
                if (layerDelta != 256)
                    return -1;
            }
        } else if (direction == 2) {
            if (nextKind == 2) {
                if (layerDelta != -256)
                    return -1;
            } else if (layerDelta != 0)
                return -1;
        } else
            return -1;
    }
    if ((currentKind == 2 && nextKind == 2) || (currentKind == 1 && nextKind == 1)) {
        if ((difference < 0 ? -difference : difference) > 256)
            return -1;
    } else if ((difference < 0 ? -difference : difference) > 255)
        return -1;
    return nextCell;
}
