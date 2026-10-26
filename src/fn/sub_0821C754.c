#include "global.h"
#include "engine.h"

struct Pos16 {
    u16 x, y, z, w;
};

struct SurfaceTrack {
    u8 pad0[4];
    u16 prevCell;
    u16 cell;
    u8 prevHigh;
    u8 high;
    u8 prevType;
    u8 type;
    u16 prevValue;
    u16 value;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 cellIndexAt(struct Pos16 *p)
{
    s32 x = (s16)p->x >> 8;
    s32 z = (s16)p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u8 *cellProps(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}

void Terrain_SampleSurface(struct SurfaceTrack *t, struct Pos16 *out, struct Pos16 *in)
{
    if (gTerrainMap->cells) {
        u32 cell = (u16)cellIndexAt(in);
        u8 *props = cellProps(cell);
        if (t->cell != cell) {
            t->prevCell = t->cell;
            t->prevType = t->type;
            t->prevHigh = t->high;
            t->prevValue = t->value;
            t->cell = cell;
            t->type = props[0] >> 4;
            t->high = props[0] & 0xf;
            t->value = *(u16 *)(props + 2);
        }
        *(struct Pos16 *)out = *in;
        {
            u32 c2 = cellIndexAt(in);
            u8 *p2 = cellProps(c2);
            u32 type = *p2 >> 4;
            u32 h = (*p2 & 0xf) << 8;
            switch (type) {
                case 1:
                    h -= (u8)in->z;
                    break;
                case 2:
                    h -= (u8)in->x;
                    break;
            }
            out->y = h;
        }
    }
}
