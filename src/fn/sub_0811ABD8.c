#include "global.h"
#include "ram.h"
struct TerrainColumn {
    u16 cells[80];
};
struct TerrainProbePos {
    s16 x, y, z, pad;
};
// confidence=low: terrain cell high nibble at base+offset (x), offset z; the packed position is a by-value scratch argument.
s32 sub_0811ABD8(s16 *base, s16 *offset, struct TerrainProbePos p)
{
    s32 clampedX = offset[0] < 0 ? 0 : offset[0];
    s32 clampedZ = offset[2] < 0 ? 0 : offset[2];
    struct TerrainColumn **columns;
    struct TerrainColumn *column;
    s32 tileX, tileZ;
    s32 x;
    s32 bx;
    bx = base[0] < 0 ? 0 : base[0];
    p.x = bx + clampedX;
    x = p.x >> 5;
    columns = &gTerrainColumns;
    column = *columns + gTerrainColumnIndices[x];
    tileX = (clampedX &= 31) >> 3;
    tileZ = clampedZ >> 4;
    tileZ *= 4;
    tileZ += 0x30;
    tileZ += tileX;
    return *(u16 *)((tileZ << 1) + (u32)column) & 0xf000;
}
