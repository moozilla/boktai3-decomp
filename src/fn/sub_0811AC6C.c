#include "global.h"
#include "ram.h"
struct TerrainColumn {
    u16 cells[80];
};
s32 Terrain_GetCellHighByte(s16 *position)
{
    s32 clampedX = position[0] < 0 ? 0 : position[0];
    s32 clampedZ = position[2] < 0 ? 0 : position[2];
    struct TerrainColumn **columns;
    struct TerrainColumn *column;
    s32 tileX, tileZ;
    s32 x = clampedX >> 5;
    columns = &gTerrainColumns;
    column = *columns + gTerrainColumnIndices[x];
    tileX = (clampedX &= 31) >> 3;
    tileZ = clampedZ >> 3;
    if (tileZ > 7)
        tileZ = 7;
    tileZ *= 4;
    tileZ += 0x30;
    tileZ += tileX;
    return *(u16 *)((tileZ << 1) + (u32)column) & 0xff00;
}
