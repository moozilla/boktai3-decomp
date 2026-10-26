#include "global.h"
#include "engine.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Shadow {
    u8 active, flags, pad[6];
    struct Vec *source;
    u32 spriteFlags;
    u8 pad10[20];
    struct Vec position;
};
struct Cell {
    u8 height, tile;
    u16 flags;
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(int, int);
static inline int cellIndex(struct Vec *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline struct Cell *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return (void *)(p + 4);
    return (void *)(gTerrainMap->cells + 12 + i * 4);
}
static inline int noShadow(struct Cell *c)
{
    u32 value = c->flags;
    value &= 32;
    return value;
}
void Shadow_UpdateTerrain(struct Shadow *p)
{
    u32 *flags = &p->spriteFlags;
    struct Vec *ground = &p->position;
    struct Vec *source = p->source;
    int index = cellIndex(source);
    if (p->flags & 1)
        ground->y = source->y;
    else {
        struct Cell *c = cell(index);
        int slope = (c->height >> 2) >> 2; /* two shifts: matches agbcc allocation */
        int height = (c->height & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)source->z;
                break;
            case 2:
                height -= (u8)source->x;
                break;
        }
        ground->y = height;
    }
    if (source->y < ground->y || ground->y == 0 || noShadow(cell(index)))
        *flags |= 1;
    else {
        *flags &= ~1;
        ground->x = source->x;
        ground->z = source->z;
    }
}
