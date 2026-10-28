#include "global.h"
#include "engine.h"

struct PathPos {
    u16 x, y, z;
};
u8 *Path_GetDefinition(u32);
s32 Mod(s32, s32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Path_PickRandomPoint(struct PathPos *out, u32 pathId)
{
    u8 *def = Path_GetDefinition(pathId);
    s32 point;
    s32 x, z, cell, height;
    u8 *props;
    u32 surface;
    if (def == 0 || def[0] == 0) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return -1;
    }
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        s32 sample = (*(&gRandomTable))[gRandomIndex];
        point = Mod(sample, def[0]);
    }
    Path_GetPointPositionById((u16 *)out, pathId, point);
    x = (s16)out->x >> 8;
    z = (s16)out->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    props = TerrainModifiers_FindCellOverride(cell, 1);
    if (props)
        props += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        props = map->cells + offset;
    }
    surface = *props >> 4;
    height = (*props & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)out->z;
            break;
        case 2:
            height -= (u8)out->x;
            break;
    }
    out->y = height;
    return point;
}
