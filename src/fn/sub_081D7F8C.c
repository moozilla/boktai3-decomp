#include "global.h"
#include "scene_actor_terrain.h"

/* Adjacent map dimensions at the terrain-width global. */
struct TerrainDimensions {
    u32 width;
    u32 height;
};

static inline s32 CellIndex(u16 *a)
{
    s32 x = ((s16)a[0]) >> 8;
    s32 z = ((s16)a[2]) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 AboveCell(u16 *a, s32 cell)
{
    u32 height = *SceneActorTerrain_GetCell(cell) & 15;
    u32 above = 0;
    if ((s16)a[1] > (height << 8))
        above = 1;
    return above;
}
// confidence=low: selects a cardinal direction from adjacent terrain heights for diagonal travel.
s32 Task4E01_ResolveTerrainDirection(u16 *input, s32 direction)
{
    u32 first;
    s32 cell;
    struct TerrainDimensions dimensions;
    s32 width;
    s32 x, z;
    u32 *widthPtr;
    TerrainModifiers_FindCellOverride(CellIndex(input), 1);
    x = ((s16)input[0]) >> 8;
    z = ((s16)input[2]) >> 8;
    widthPtr = &gTerrainWidth;
    if (x < 0 || z < 0 || (u32)x >= *widthPtr || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    dimensions = *(struct TerrainDimensions *)widthPtr;
    width = dimensions.width;
    if (direction == 0) {
        u32 second;
        direction = cell + 1;
        {
            u32 height = *SceneActorTerrain_GetCell(direction) & 15;
            first = 0;
            if ((s16)input[1] > (height << 8))
                first = 1;
        }
        direction = cell + width;
        second = AboveCell(input, direction);
        if (first != second) {
            direction = 2;
            if (second) {
                direction = 6;
            }
        } else {
            direction = 4;
        }
    } else if (direction == 2) {
        u32 second;
        direction = cell - 1;
        {
            u32 height = *SceneActorTerrain_GetCell(direction) & 15;
            first = 0;
            if ((s16)input[1] > (height << 8))
                first = 1;
        }
        direction = cell + width;
        second = AboveCell(input, direction);
        if (first != second) {
            direction = second ? 4 : 0;
        } else {
            direction = 6;
        }
    } else if (direction == 4) {
        u32 second;
        direction = cell - 1;
        {
            u32 height = *SceneActorTerrain_GetCell(direction) & 15;
            first = 0;
            if ((s16)input[1] > (height << 8))
                first = 1;
        }
        direction = cell - width;
        second = AboveCell(input, direction);
        if (first != second) {
            direction = 6;
            if (second) {
                direction = 2;
            }
        } else {
            direction = 0;
        }
    } else if (direction == 6) {
        u32 second;
        direction = cell + 1;
        {
            u32 height = *SceneActorTerrain_GetCell(direction) & 15;
            first = 0;
            if ((s16)input[1] > (height << 8))
                first = 1;
        }
        direction = cell - width;
        second = AboveCell(input, direction);
        if (first != second) {
            direction = 4;
            if (second) {
                direction = 0;
            }
        } else {
            direction = 2;
        }
    }
    return direction;
}
