#include "terrain_modifier.h"
static inline s32 cellIndex(struct TerrainVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
void sub_0821D630(void *, s32, s32, s32, s32, s32);
s32 TerrainModifiers_Change(s32 id, s32 tile, s32 kind, s32 flags, s32 cell, s32 height)
{
    struct TerrainModifier *p;
    if (!gUnk_02000028)
        return -1;
    p = gUnk_02000028->head;
    while (p) {
        struct TerrainModifier *next = p->next;
        if (p->id == id) {
            if (kind != -1)
                p->kind = kind;
            if (tile != -1)
                p->tile = tile;
            if (cell != -1)
                p->cell = cell;
            if (height != -1)
                p->height = height;
            if (flags != -1)
                p->flags = flags;
            sub_0821D630(p->registration, p->cell, p->tile, p->height, 255, p->flags);
        }
        p = next;
    }
    return 0;
}
