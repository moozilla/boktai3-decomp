#include "engine/shared/terrain_edge_marker.h"
static inline u32 TerrainMarker_GetCellIndex(union TerrainMarkerPosition *position)
{
    s32 x = position->world.x >> 8;
    s32 z = position->world.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline struct SceneCell *TerrainMarker_GetCell(u32 index)
{
    u8 *cells = gTerrainMap->cells;
    if (!cells)
        return 0;
    return (struct SceneCell *)(cells + (index * 4 + 12));
}
static inline u32 TerrainMarker_IsOccupied(struct SceneCell *cell)
{
    u32 flags = cell->flags;
    return flags & 1;
}
s32 TerrainEdgeMarker_FindTerrainDirection(struct TerrainMarkerRegion *region)
{
    union TerrainMarkerPosition inside, outside;
    u32 insideIndex, outsideIndex;
    struct SceneCell *insideCell, *outsideCell;

    /* Probe +X, then +Z, -X, and -Z, stopping at the first connection. */
    inside.packed.x = region->maxX - 16;
    inside.packed.z = (region->maxZ + region->minZ) >> 1;
    outside = inside;
    outside.packed.x += 32;
    insideIndex = TerrainMarker_GetCellIndex(&inside);
    outsideIndex = TerrainMarker_GetCellIndex(&outside);
    if (insideIndex == outsideIndex)
        return 0;
    insideCell = TerrainMarker_GetCell(insideIndex);
    outsideCell = TerrainMarker_GetCell(outsideIndex);
    switch ((u32)(insideCell->height >> 4) & 15) {
        case 0: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if (kind == 0 && (insideCell->height & 15) == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 0;
            if ((outsideCell->height >> 4) == 2 &&
                (insideCell->height & 15) == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 0;
            break;
        }
        case 2: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if ((kind == 0 || kind == 2) &&
                (insideCell->height & 15) == (outsideCell->height & 15) + 1 &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 0;
            break;
        }
        case 1:
            break;
    }

    inside.world.x = (region->maxX + region->minX) >> 1;
    inside.world.z = region->maxZ - 16;
    outside = inside;
    outside.world.z += 32;
    insideIndex = TerrainMarker_GetCellIndex(&inside);
    outsideIndex = TerrainMarker_GetCellIndex(&outside);
    if (insideIndex == outsideIndex)
        return 1;
    insideCell = TerrainMarker_GetCell(insideIndex);
    outsideCell = TerrainMarker_GetCell(outsideIndex);
    switch ((u32)(insideCell->height >> 4) & 15) {
        case 0: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if (kind == 0 && (insideCell->height & 15) == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 1;
            if ((outsideCell->height >> 4) == 1 &&
                (insideCell->height & 15) == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 1;
            break;
        }
        case 1: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if (kind != 0) {
                if (kind != 1)
                    break;
            }
            if ((insideCell->height & 15) == (outsideCell->height & 15) + 1 &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 1;
            break;
        }
        case 2:
            break;
    }

    inside.world.x = region->minX + 16;
    inside.world.z = (region->maxZ + region->minZ) >> 1;
    outside = inside;
    outside.world.x -= 32;
    insideIndex = TerrainMarker_GetCellIndex(&inside);
    outsideIndex = TerrainMarker_GetCellIndex(&outside);
    if (insideIndex == outsideIndex)
        return 2;
    insideCell = TerrainMarker_GetCell(insideIndex);
    outsideCell = TerrainMarker_GetCell(outsideIndex);
    switch ((u32)(insideCell->height >> 4) & 15) {
        case 0: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if (kind == 0 && (insideCell->height & 15) == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 2;
            if ((outsideCell->height >> 4) == 2 &&
                (insideCell->height & 15) + 1 == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 2;
            break;
        }
        case 2: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if ((kind == 0 && (insideCell->height & 15) == (outsideCell->height & 15)) ||
                (kind == 2 && (insideCell->height & 15) + 1 == (outsideCell->height & 15))) {
                if (!TerrainMarker_IsOccupied(outsideCell))
                    return 2;
            }
            break;
        }
        case 1:
            break;
    }

    inside.world.x = (region->maxX + region->minX) >> 1;
    inside.world.z = region->minZ + 16;
    outside = inside;
    outside.world.z -= 32;
    insideIndex = TerrainMarker_GetCellIndex(&inside);
    outsideIndex = TerrainMarker_GetCellIndex(&outside);
    if (insideIndex == outsideIndex)
        return 3;
    insideCell = TerrainMarker_GetCell(insideIndex);
    outsideCell = TerrainMarker_GetCell(outsideIndex);
    switch ((u32)(insideCell->height >> 4) & 15) {
        case 0: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if (kind == 0 && (insideCell->height & 15) == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 3;
            if ((outsideCell->height >> 4) == 1 &&
                (insideCell->height & 15) + 1 == (outsideCell->height & 15) &&
                !TerrainMarker_IsOccupied(outsideCell))
                return 3;
            break;
        }
        case 1: {
            u32 kind = (outsideCell->height >> 4) & 15;
            if ((kind == 0 && (insideCell->height & 15) == (outsideCell->height & 15)) ||
                (kind == 1 && (insideCell->height & 15) + 1 == (outsideCell->height & 15))) {
                if (!TerrainMarker_IsOccupied(outsideCell))
                    return 3;
            }
            break;
        }
        case 2:
            break;
    }
    return -1;
}
