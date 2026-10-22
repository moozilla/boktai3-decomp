#include "global.h"
#define gTerrainColumns       gUnk_020001FC
#define gTerrainColumnIndices gUnk_020001F8
struct TerrainColumn {
    u16 cells[80];
};
extern struct TerrainColumn *gUnk_020001FC;
extern u8 *gUnk_020001F8;
s32 sub_0811ACCC(s16 *position)
{
    s32 clampedX = position[0] < 0 ? 0 : position[0];
    s32 clampedZ = position[2] < 0 ? 0 : position[2];
    struct TerrainColumn **columns;
    struct TerrainColumn *column;
    s32 tileX, tileZ;
    s32 x = clampedX >> 5;
    columns = &gUnk_020001FC;
    column = *columns + gUnk_020001F8[x];
    tileX = (clampedX &= 31) >> 3;
    tileZ = clampedZ >> 3;
    if (tileZ > 7)
        tileZ = 7;
    tileZ *= 4;
    tileZ += 0x30;
    tileZ += tileX;
    return *(u16 *)((tileZ << 1) + (u32)column) & 0x0f00;
}

s32 sub_0811AD2C(s16 *position)
{
    s32 clampedX = position[0] < 0 ? 0 : position[0];
    s32 clampedZ = position[2] < 0 ? 0 : position[2];
    struct TerrainColumn **columns;
    struct TerrainColumn *column;
    s32 tileX, tileZ;
    s32 x = clampedX >> 5;
    columns = &gUnk_020001FC;
    column = *columns + gUnk_020001F8[x];
    tileX = (clampedX &= 31) >> 3;
    tileZ = clampedZ >> 3;
    if (tileZ > 7)
        tileZ = 7;
    tileZ *= 4;
    tileZ += 0x30;
    tileZ += tileX;
    return *(u16 *)((tileZ << 1) + (u32)column) & 0xf000;
}
