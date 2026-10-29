#include "global.h"
#include "engine.h"
#include "terrain_cell_probe.h"

/* Play the impact cues and create a hazard at a nearby point on the same terrain level. */
union ImpactPosition {
    struct Pos16 vector;
    u32 words[2];
};

typedef struct Actor08025FFC {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xf];
    u8 signalAF;
    u8 padB0;
    u8 changed;
    u8 padB2[0x52];
    void *callbackA;
    void *callbackB;
    u8 *q;
    union ImpactPosition *copyFrom;
    u8 pad114[0x149];
    u8 field25D;
    u8 pad25E[0x10c];
    s16 field36A;
    u8 pad36C[0x40];
    u32 mode;
} Actor08025FFC;

static inline s32 s08025FFC_Trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

static inline u32 Impact_GetTerrainHeight(struct Pos16 *pos)
{
    u8 *props = cellProps(cellIndexAt(pos));
    u32 type = *props >> 4;
    u32 height = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            height -= (u8)pos->z;
            break;
        case 2:
            height -= (u8)pos->x;
            break;
    }
    return height;
}

static inline u16 Impact_Random(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

static inline void Impact_AddPosition(struct Pos16 *out, const struct Pos16 *a,
                                      const struct Pos16 *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void sub_08025FFC(Actor08025FFC *p, s32 arg)
{
    Actor08025FFC *actor = p;
    union ImpactPosition copy;
    struct Pos16 d;
    struct Pos16 *delta;
    u16 a;
    u16 b;
    s32 base;
    s32 nb;
    union ImpactPosition *src;
    s32 f;
    const u16 *table;

    if (p->changed) {
        s32 seq;
        p->changed = 0;
        seq = (p->mode & 1) ? 0x1a : 0x40;
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, seq, 2, 1, 4);
        Sound_PlaySongChecked(0x35A);
    }
    if ((u8)Callback_InvokeActor(p, p->callbackA))
        return;
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    if ((u8)Callback_InvokeActor(p, p->callbackB))
        return;
    if (arg == 0x10)
        Sound_PlaySongChecked(0x351);
    f = arg & 0xf;
    if (f == 0xf) {
        src = p->copyFrom;
        copy = *src;
        base = p->q[0x2d];
        table = gRandomTable;
        a = Impact_Random(table) & f;
        b = Impact_Random(table) & 0x7f;
        delta = &d;
        base = base + a;
        base -= 8;
        nb = b + 0x20;
        delta->x = s08025FFC_Trunc12(gUnk_086149C4[(base + 0x40) & 0xff] * nb);
        delta->y = 0;
        base &= 0xff;
        delta->z = s08025FFC_Trunc12(gUnk_086149C4[base] * nb);
        Impact_AddPosition(&d, &d, &copy.vector);
        if (Impact_GetTerrainHeight(&d) == (s16)copy.vector.y) {
            AnimatedHazard_Create(&d, actor->field36A, 0x3c);
        } else {
            AnimatedHazard_Create(&copy.vector, actor->field36A, 0x3c);
        }
    }
    if (actor->field25D & 2)
        p->signalAF = 1;
}
