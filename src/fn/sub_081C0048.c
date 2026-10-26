#include "tether_actor.h"

u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline u8 *cellProperties(s32 cell)
{
    u8 *properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}

static inline u16 cellFlags(s32 cell)
{
    return *(u16 *)(cellProperties(cell) + 2);
}

static inline s32 cellIndex(s32 x, s32 z)
{
    s32 index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    return index;
}

// confidence=low: true when the tether position, or a point 1 or 2 cells out in any of the four
// directions, lies on a cell with flag 0x80 set.
s32 sub_081C0048(void *p)
{
    struct ContactPosition *pos = TetherActor_GetPosition();
    struct Vec6 {
        s16 x, y, z;
    } v;
    s32 i, j;
    if (cellFlags(cellIndex(pos->x >> 8, pos->z >> 8)) & 0x80)
        return 1;
    for (i = 0; i < 4; i++) {
        s32 angle;
        j = 1;
        angle = i << 6;
        for (; j <= 2; j++) {
            s32 r = j << 8;
            struct Vec6 *pv = &v;
            pv->x = scale(gUnk_086149C4[(angle + 64) & 255] * r);
            pv->y = 0;
            pv->z = scale(gUnk_086149C4[angle & 255] * r);
            pv->x += pos->x;
            pv->y += pos->y;
            pv->z += pos->z;
            if (cellFlags(cellIndex(pv->x >> 8, pv->z >> 8)) & 0x80)
                return 1;
        }
    }
    return 0;
}
