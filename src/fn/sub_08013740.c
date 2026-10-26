#include "global.h"
#include "engine.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Shadow {
    u8 active, flags, pad[6];
    struct Vec *source;
    u32 spriteFlags;
    u8 pad10[4], scaleX, scaleY, pad16[18];
    struct Vec position;
    u8 pad30[36];
    s8 scale;
    u8 minScale, maxScale, pad57;
    s16 maxHeight, minHeight;
    u16 shift, nearShift;
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
void ScaledShadow_Update(struct Shadow *p)
{
    u16 index = cellIndex(p->source);
    int height;
    p->position.x = p->source->x;
    if (p->flags & 1)
        p->position.y = p->source->y;
    else {
        struct Vec *source = p->source;
        struct Cell *c = cell(index);
        int slope = c->height >> 4, y = (c->height & 15) << 8;
        switch (slope) {
            case 1:
                y -= (u8)source->z;
                break;
            case 2:
                y -= (u8)source->x;
                break;
        }
        p->position.y = y;
    }
    p->position.z = p->source->z;
    height = p->source->y - p->position.y;
    if (height <= 0)
        p->scale = p->maxScale;
    else if (height < p->minHeight)
        p->scale = p->maxScale - (height >> p->nearShift);
    else if (height > p->maxHeight)
        p->scale = p->minScale;
    else
        p->scale = p->maxScale - (height >> p->shift);
    if (p->scale <= 0)
        p->scale = 1;
    p->scaleX = p->scale;
    p->scaleY = p->scale;
    if (p->source->y < p->position.y || p->position.y == 0 || noShadow(cell(index)))
        p->spriteFlags |= 1;
    else
        p->spriteFlags &= ~1;
}
