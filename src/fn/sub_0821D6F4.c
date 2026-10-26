// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
// confidence=low: tests higher adjacent cells across the selected subcell corner.
struct Position {
    s16 x, y, z, pad;
};
struct Sample {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 CellIndex(struct Position *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *CellData(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (index * 4 + 12);
}
// confidence=low: compares base heights at positive subcell corners.
s32 Terrain_HasHigherCorner(struct Position *p, s32 threshold)
{
    s32 x = (p->x & 0xff00) + 128;
    s32 y = p->y & 0xff00;
    s32 z = (p->z & 0xff00) + 128;
    s32 index = CellIndex(p);
    u8 *data = CellData(index);
    s32 height;
    struct Sample sample;
    s32 m255;
    if (!data)
        return 0;
    height = *data & 15;
    m255 = 255;
    asm("" : "+r"(m255));
    if ((u8)p->x < threshold) {
        if ((m255 & (u16)p->z) < threshold)
            return 0;
        sample.x = x;
        sample.y = y;
        sample.z = z + 255;
        data = CellData(CellIndex((struct Position *)&sample));
    } else if ((m255 & (u16)p->z) >= threshold) {
        sample.x = x + 255;
        sample.y = y;
        sample.z = z;
        data = CellData(CellIndex((struct Position *)&sample));
        if (height < (*data & 15))
            return 1;
        sample.x = x;
        sample.y = y;
        sample.z = z + 255;
        data = CellData(CellIndex((struct Position *)&sample));
        if (height < (*data & 15))
            return 1;
        sample.x = x + 255;
        sample.y = y;
        sample.z = z + 255;
        data = CellData(CellIndex((struct Position *)&sample));
    } else {
        sample.x = x + 255;
        sample.y = y;
        sample.z = z;
        data = CellData(CellIndex((struct Position *)&sample));
    }
    if (height >= (*data & 15))
        return 0;
    return 1;
}
