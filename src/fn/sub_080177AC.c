#include "global.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: creates weather particles with two terrain-adjusted endpoints.
#define Weather_SpawnTerrainParticles sub_080177AC
struct Vec {
    s16 x, y, z, pad;
};
struct Part {
    u8 active, b, c, selected;
    struct Vec first;
    s16 secondX, secondY, secondZ, secondPad;
    u8 drawable[40];
};
struct Weather {
    u8 pad0[0x21], flags;
    u8 pad22[0x42];
    u16 delay;
    u8 pad66[0x102];
    void *animation;
    struct Part parts[32];
};
struct Camera {
    u8 pad[0x30];
    u16 x, y, z;
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Camera *gUnk_02000580;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC, gUnk_03005308;
#define RANDOM_TABLE ((u16 *)0x0203B400)
u32 Weather_TestFlags(u32, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void Effect_LinkToList(void *, u32);
s32 Mod(s32, s32);
static inline u32 Random(void)
{
    u32 n = (gUnk_03005308 + 1) & 0x3ff;
    gUnk_03005308 = n;
    return RANDOM_TABLE[n];
}
static inline s32 GroundHeight(struct Vec *position, s16 x, s16 z)
{
    s32 col = x >> 8, row = z >> 8;
    u32 cell;
    u8 *tile;
    s32 slope, height;
    if (col < 0 || row < 0 || (u32)col >= gUnk_030052F8 || (u32)row >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[row] + col;
    tile = TerrainModifiers_FindCellOverride(cell, 1);
    if (tile)
        tile += 4;
    else
        tile = gUnk_030052F4->cells + 12 + cell * 4;
    slope = tile[0] >> 4;
    height = (tile[0] & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
static inline u32 RandomAt(u16 *table)
{
    u32 n = (gUnk_03005308 + 1) & 0x3ff;
    gUnk_03005308 = n;
    return table[n];
}
static inline void SetPoint(struct Part *out, struct Part *sample, struct Camera *camera, u32 index)
{
    register u16 *table asm("r3") = RANDOM_TABLE;
    s32 x, z;
    x = (RandomAt(table) & 0xfff) + camera->x - 0x800;
    if (index == 0)
        out->first.x = x;
    else
        out->secondX = x;
    z = (RandomAt(table) & 0xfff) + camera->z - 0x800;
    if (index == 0)
        out->first.z = z;
    else
        out->secondZ = z;
    if (index == 0)
        out->first.y = GroundHeight(&sample->first, x, z);
    else
        out->secondY = GroundHeight((struct Vec *)&sample->secondX, x, z);
}
s32 Weather_SpawnTerrainParticles(struct Weather *p, s32 count, s32 delay, s32 delayRange)
{
    struct Camera *camera = gUnk_02000580;
    s32 i;
    u16 *timer;
    u32 remaining;
    if (Weather_TestFlags(p->flags, 4))
        return 0;
    remaining = p->delay;
    timer = &p->delay;
    if (remaining) {
        *timer = remaining - 1;
        return 0;
    }
    for (i = 0; i < count;) {
        struct Part *sample = p->parts;
        struct Part *e;
        s32 j = 0;
        s32 next = i + 1;
        e = sample;
        for (; j < 32; j++, e++, sample++) {
            if (!e->active) {
                u8 *drawable;
                u32 one;
                e->c = 0;
                e->b = 0;
                e->selected = 0;
                SetPoint(e, sample, camera, 0);
                SetPoint(e, sample, camera, 1);
                drawable = sample->drawable;
                *(struct Vec *)(e->drawable + 24) = ((struct Vec *)((u8 *)sample + 4))[e->selected];
                SpriteDrawable_SelectTileFrame(drawable, p->animation, 30);
                one = 1;
                drawable[15] = one;
                *(u32 *)e->drawable &= ~1;
                Effect_LinkToList(drawable, 0);
                e->active = one;
                break;
            }
        }
        i = next;
    }
    *timer = delay;
    if (delayRange)
        *timer += Mod(Random(), delayRange);
    return 0;
}
