#include "global.h"
#include "engine.h"

struct CellObject {
    u8 pad0[4];
    u16 cell4;
    u16 cell6;
    u8 type8;
    u8 type9;
    u8 typeA;
    u8 typeB;
    u16 valueC;
    u16 valueE;
};

void CellObject_Init(struct CellObject *obj, u16 *point)
{
    u32 cell;
    u8 *def;
    s32 x, z;
    u8 lo;
    u8 hi;
    u16 value;
    struct EngineMap *map = gTerrainMap;

    if (map->cells) {
        u8 *cells = map->cells;
        u32 off;
        x = (s16)point[0] >> 8;
        z = (s16)point[2] >> 8;
        if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
            cell = 0;
        else
            cell = map->rows[z] + x;
        obj->cell6 = cell;
        obj->cell4 = cell;
        off = obj->cell6 * 4 + 12;
        def = cells + off;
        lo = *def & 15;
        obj->type9 = lo;
        hi = *def >> 4;
        obj->typeB = hi;
        value = *(u16 *)(def + 2);
        obj->valueE = value;
        obj->type8 = lo;
        obj->typeA = hi;
        obj->valueC = value;
    }
}
