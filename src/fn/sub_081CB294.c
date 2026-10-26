// MATCHING_CONCESSION: register-pinning
#include "terrain_cell_probe.h"

extern const s16 gUnk_086149C4[];

static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

// confidence=low: state actor rising to terrain height plus 0x80, with a sine bob on entry.
struct StateActorRise {
    u8 pad0[7];
    u8 done, pad8, entered;
    u8 padA[4];
    u16 flags;
    u32 angle;
    u8 pad14[4];
    u8 phase;
    u8 pad19[0x20 - 0x19];
    u8 pad20[6];
    u16 clear26;
    u8 pad28[0x68 - 0x28];
    s16 px, py, pz, pw;
    u8 pad70[0x160 - 0x70];
    s16 *shadow;
};

void StateActor_UpdateRiseToTerrain(void *ctx, struct StateActorRise *a, s32 tick)
{
    u16 *p20 = (u16 *)((u8 *)a + 0x20);
    register struct Pos16 *pos asm("r6") = (struct Pos16 *)&a->px;
    s32 h;
    {
        s32 cx = (s16)pos->x >> 8;
        s32 cz = (s16)pos->z >> 8;
        if (cx < 0 || cz < 0 || (u32)cx >= gTerrainWidth || (u32)cz >= gTerrainHeight)
            h = 0;
        else
            h = gTerrainMap->rows[cz] + cx;
    }
    {
        u8 *props = cellProps(h);
        u32 type = *props >> 4;
        s32 t = (*props & 0xf) << 8;
        switch (type) {
            case 1:
                t -= (u8)pos->z;
                break;
            case 2:
                t -= (u8)pos->x;
                break;
        }
        h = t;
    }
    if (a->entered) {
        a->entered = 0;
        a->done = 0;
        pos->y = h + (shift12((s32)gUnk_086149C4[(a->angle << 2) & 0xFF] << 4) + 0x80);
        {
            s32 dx = shift12((s32)gUnk_086149C4[(a->angle << 2) & 0xFF] << 4);
            s16 *sh = a->shadow;
            s32 v = h + 0x80 + dx;
            u32 z1 = 0;
            u32 z2 = 0;
            asm("" : "+r"(z1), "+r"(z2));
            sh[1] = v;
            a->phase = z1;
            p20[3] = z2;
        }
    }
    switch (a->phase) {
        case 0: {
            u32 m = 0x400;
            a->flags |= m;
            if ((s16)pos->y >= h) {
                pos->y--;
            } else {
                pos->y = h;
                a->phase++;
                a->angle = 0;
            }
            break;
        }
        case 1:
            if (tick > 0x7f)
                a->phase = 2;
            break;
        case 2:
            a->done = 1;
            break;
    }
}
