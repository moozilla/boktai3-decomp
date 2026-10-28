#include "global.h"
#include "engine_globals.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad0[7];
    u8 done;
    u8 pad8[1];
    u8 entered;
    u8 pada[4];
    u16 flags;
    u8 pad10[8];
    u8 phase;
    u8 facing4;
    u8 facing8;
    u8 targetAngle;
    u8 pad1c[56];
    struct Vec target;
    u8 body[9];
    u8 angle;
    u8 pad66[2];
    struct Vec pos;
    struct Vec velocity;
    u8 pad78[92];
    u32 drawable;
    u8 padd8[80];
    struct Vec *sourcePos;
    u8 pad12c[52];
    struct Vec *drawPos;
    u8 pad164[0x88];
    u8 modeA, modeB;
    u8 pad1ee[2];
    u8 cap;
    u8 pad1f1[0x17];
    int speed;
    u32 speedSquared;
    u8 pad210[212];
    int (*callback)(struct Actor *);
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(int, int);
extern const s16 gUnk_086149C4[];
int Actor_PlayAnimationEntry(struct Actor *, int, int, int, int);
void Actor_PlayFootstepSound(struct Actor *, int);
void DisplayAnimation_SetFlags(void *, int);
void DisplayAnimation_ClearFlags(void *, int);
void DisplayAnimation_SetPalette(void *, int, int, int);
void Sound_PlaySong(int);
int Math_Atan2(int, int);
static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return ((struct Map *)gTerrainMap)->cells + 12 + i * 4;
}
static inline int height(struct Vec *pos)
{
    int x = pos->x >> 8, z = pos->z >> 8, index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = ((struct Map *)gTerrainMap)->rows[z] + x;
    return (*cell(index) & 15) << 8;
}
void Actor_OnTimedIdleUpdate(void *unused, struct Actor *p, int ticks)
{
    if (p->entered) {
        int zero = 0;
        p->entered = zero;
        p->done = zero;
        Actor_PlayAnimationEntry(p, 0, 1, zero, 4);
        p->cap = 1;
    }
    {
        u32 mask = 4096;
        p->flags |= mask;
    }
    if ((u8)p->callback(p) != 1 && ticks > 7)
        p->done = 1;
}
