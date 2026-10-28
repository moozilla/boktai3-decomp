#include "global.h"
#include "engine.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
struct TrailEntry {
    u32 flags;
    u8 pad4[0x18 - 4];
    u16 x;
    u16 pad1A;
    u16 z;
    u16 pad1E;
    u8 pad20[0x28 - 0x20];
    u8 active;
    s8 offset;
    u16 speed;
    u16 timer;
    u8 pad2E[2];
};
struct TrailOwner {
    u8 pad0[0x40];
    s32 next;
    u8 pad44[4];
    struct TrailEntry e[4];
};
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
struct Pos8 {
    u32 a, b;
};
static inline s32 Rand2(s32 i)
{
    gRandomIndex = (i + 1) & 1023;
    return gRandomTable[(i + 1) & 1023];
}
// confidence=low: starts the next trail sprite slot at a random offset around the owner direction.
void Task4E01_SpawnTrailSprite(struct TrailOwner *p, u8 *src)
{
    u16 mul;
    s32 dir;
    struct Pos8 *new_var2;
    s32 dir_2;
    unsigned char new_var;
    s32 *np;
    s32 ri;
    unsigned short new_var3;
    s32 off;
    const u16 *tab;
    int k2 = 0x100;
    p->e[p->next].active = 1;
    p->e[p->next].flags &= ~1;
    p->e[p->next].timer = 0;
    np = &p->next;
    new_var3 = 0x60;
    tab = gRandomTable;
    ri = (gRandomIndex + 1) & 1023;
    dir_2 = (s32)tab[ri];
    p->e[p->next].speed = ((dir_2 >> 3) & 0x7f) + 0x40;
    p->e[*np].offset = Mod(Rand2(ri), new_var3) - 0x30;
    dir = (new_var = (((u16)(s8)src[0x1e]) + 0x70) >> 5) & 7;
    dir = ((dir + 5) & 7) << 5;
    off = p->next * 0x30;
    dir = (dir + (*(s8 *)((u8 *)p + off + 0x71) + k2)) & 0xff;
    new_var2 = (struct Pos8 *)((u8 *)p + new_var3 + off);
    *new_var2 = *(struct Pos8 *)(src + 0xa8);
    mul = p->e[p->next].speed;
    p->e[p->next].x += Truncate(gUnk_086149C4[(dir + 0x40) & 0xff] * mul);
    mul = p->e[p->next].speed;
    p->e[p->next].z += Truncate(mul * gUnk_086149C4[dir]);
    p->next++;
    if (p->next > 3)
        p->next = 0;
}
