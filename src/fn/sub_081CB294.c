#include "terrain_cell_probe.h"
#include "contact_position.h"

extern const s16 gUnk_086149C4[];

static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

// confidence=low: state actor rising to terrain height plus 0x80, with a sine bob on entry.
struct RiseTag {
    u8 unknown0[6];
    u16 field6;
};
struct StateActorRise {
    u8 pad0[7];
    u8 done, pad8, entered;
    u8 padA[4];
    u16 flags;
    u32 angle;
    u8 pad14[4];
    u8 phase;
    u8 pad19[0x20 - 0x19];
    struct RiseTag tag;
    u8 pad28[0x68 - 0x28];
    struct Pos16 position;
    u8 pad70[0x160 - 0x70];
    struct ContactPosition *shadow;
};

void StateActor_UpdateRiseToTerrain(void *ctx, struct StateActorRise *a, s32 tick)
{
    struct RiseTag *tag = &a->tag;
    struct Pos16 *pos = &a->position;
    s32 height;
    s32 cell;
    {
        s32 cx = (s16)pos->x >> 8;
        s32 cz = (s16)pos->z >> 8;
        if (cx < 0 || cz < 0 || (u32)cx >= gTerrainWidth || (u32)cz >= gTerrainHeight)
            cell = 0;
        else
            cell = gTerrainMap->rows[cz] + cx;
    }
    {
        const u8 *props = cellProps(cell);
        u32 type = *props >> 4;
        s32 baseHeight = (*props & 0xf) << 8;
        switch (type) {
            case 1:
                baseHeight -= (u8)pos->z;
                break;
            case 2:
                baseHeight -= (u8)pos->x;
                break;
        }
        height = baseHeight;
    }
    if (a->entered) {
        a->entered = 0;
        a->done = 0;
        pos->y = height + (shift12((s32)gUnk_086149C4[(a->angle << 2) & 0xFF] << 4) + 0x80);
        {
            s32 dx = shift12((s32)gUnk_086149C4[(a->angle << 2) & 0xFF] << 4);
            struct ContactPosition *shadow = a->shadow;
            s32 v = height + 0x80 + dx;
            shadow->y = v;
            a->phase = 0;
            tag->field6 = 0;
        }
    }
    switch (a->phase) {
        case 0: {
            u32 m = 0x400;
            a->flags |= m;
            if ((s16)pos->y >= height) {
                pos->y--;
            } else {
                pos->y = height;
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
