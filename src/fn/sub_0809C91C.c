#include "global.h"

// confidence=low: applies hovering height and drawable bobbing from the movement phase.
struct Drw {
    u8 pad0[0x1e];
    u16 y;
};
struct St {
    u8 pad0[0x3d4];
    u32 flags;
    u8 pad1[0x3e2 - 0x3d8];
    u8 phase;
};
struct Act {
    u8 pad0[4];
    struct Drw *drawable;
    u8 pad1[0x5e - 8];
    s16 posY;
    u8 pad2[0x9c - 0x60];
    s16 health;
    u8 pad3[0x2b5 - 0x9e];
    u8 kind;
    u8 pad4[0x2be - 0x2b6];
    u16 ground;
    u8 pad5[0x2d0 - 0x2c0];
    u32 flags2D0;
    u16 flags2D4;
    u8 pad6[0x2ea - 0x2d6];
    u16 off;
    u8 pad7[0x3d0 - 0x2ec];
    struct St *st;
};
extern s16 gUnk_086149C4[];

static inline u32 ActorFlag(struct Act *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 Bad(struct Act *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline s32 Trunc(s32 value, u32 shift)
{
    return value >= 0 ? value >> shift : -((-value) >> shift);
}

// MATCHING_CONCESSION: register-pinning
void Lun_UpdateHoverHeight(struct Act *a)
{
    struct St *st = a->st;
    s32 d, ad;
    s32 cap;

    if (Bad(a))
        return;
    {
        u32 mask = 1;
        if (Test(st->flags, mask))
            return;
    }
    if ((u8)(a->kind - 0x23) > 1) {
        u32 m = 0x100000;
        register u32 off asm("r4");
        register u32 *p asm("r0");
        asm("" ::: "r4");
        off = 0x2D0;
        asm("" : "+r"(off));
        p = (u32 *)((u8 *)a + off);
        if (*p & m)
            goto cont;
    }
    {
        u32 g = a->ground;
        a->posY = a->off + g;
    }
    return;
cont: {
    u32 r;
    u32 mode = *(u16 *)0x02000534;
    r = 0;
    if (mode != 0 && (*(u16 *)0x02000500) == 5)
        r = 1;
    if (r != 0 || (*(u8 **)0x02000580)[0x418] == 4)
        cap = 0x3c;
    else
        cap = 0x104;
}
    if ((s16)a->off < cap) {
        a->off += Trunc(gUnk_086149C4[st->phase], 9);
        if (st->phase <= 0x3f)
            st->phase++;
        a->posY = a->ground + a->off;
    }
    d = a->ground + cap - a->posY;
    ad = d >= 0 ? d : -d;
    if (ad > 0x10) {
        if (d < 0)
            a->off -= 2;
        else
            a->off += 2;
    }
    {
        u32 g = a->ground;
        a->posY = a->off + g;
    }
    {
        struct Drw *dr = a->drawable;
        dr->y += Trunc(gUnk_086149C4[st->phase] << 5, 12);
    }
    st->phase += 2;
}
