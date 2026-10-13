#include "terrain_modifier.h"
static inline s32 heightLevel(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -((-value) >> 8);
}
static inline s32 cellIndex(struct TerrainVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_0821D698(void *, s32, s32, s32, s32, s32);
s32 TerrainModifiers_Create(s32 id, struct TerrainVec *position, s32 kind, s32 tile, s32 flags)
{
    struct TerrainModifier *p;
    if (!gUnk_02000028 && !sub_08006144(0x2153, 0))
        return 0;
    p = sub_08219C40(36);
    if (!p)
        return -1;
    sub_08219DD8(p, 36);
    p->id = id;
    p->active = 1;
    p->kind = kind;
    p->tile = tile;
    p->cell = cellIndex(position);
    p->height = heightLevel(position->y);
    p->flags = flags;
    sub_0821D698(p->registration, p->cell, p->tile, p->height, 255, p->flags);
    sub_08005E1C(p);
    return 0;
}
