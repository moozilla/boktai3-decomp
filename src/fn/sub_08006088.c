#include "global.h"
struct TerrainModifier {
    u16 id;
    u8 active, kind, tile, height;
    u16 flags, cell;
    u8 padA[2];
    u8 registration[16];
    struct TerrainModifier *previous, *next;
};
struct Manager {
    u8 pad[24];
    struct TerrainModifier *head;
};
struct Vec {
    s16 x, y, z, pad;
};
struct Map {
    u8 pad[36];
    u16 rows[1];
};
extern struct Manager *gUnk_02000028;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
static inline s32 cellIndex(struct Vec *pos)
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
