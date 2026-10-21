#include "global.h"

// MATCHING_CONCESSION: register-pinning
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
struct ArmorBody {
    u8 pad0[0x5AC];
    u16 orbitX;
    u8 pad5AE[2];
    u16 orbitZ;
    u8 pad5B2[0xE];
    u8 state;
};
struct ArmorPart {
    u8 pad0[0x1C];
    u16 x;
    u8 pad1E[2];
    u16 z;
    u8 pad22[0x119 - 0x22];
    u8 flags119;
    u8 pad11A[4];
    u8 byte11E;
    u8 byte11F;
    u8 pad120[0x15C - 0x120];
    struct ArmorBody *parent;
    u8 pad160[0x1D6 - 0x160];
    u16 timer;
    u8 pad1D8;
    u8 angle;
    u8 direction;
};
void Armor_UpdatePartOrbit(struct ArmorPart *p)
{
    struct ArmorBody *parent = p->parent;
    u8 *angle = &p->angle;
    const s16 *sine;
    s32 radius;
    s32 scale;
    s32 state;
    s32 zero;
    u8 a = *angle + p->direction;
    zero = 0;
    asm("" : "+r"(zero));
    *angle = a;
    sine = gUnk_086149C4;
    p->x = parent->orbitX + Div(sine[(*angle + 64) & 255] * 200, 4096);
    p->z = parent->orbitZ + Div(sine[*angle] * 200, 4096);
    if (--p->timer == 0) {
        state = parent->state;
        if (state == 3)
            goto change;
        if (state <= 3) {
            if (state == 2)
                goto change;
            return;
        }
        if (state != 4)
            return;
    change: {
        u8 one = 1;
        p->byte11E = one;
        p->byte11F = state;
        p->flags119 |= one;
    }
    }
    asm("" ::"r"(zero));
}
