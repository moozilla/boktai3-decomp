#include "global.h"

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
    s8 direction;
};

static inline void SetPartState(struct ArmorPart *p, u32 state)
{
    u8 active = 1;
    p->byte11E = active;
    p->byte11F = state;
    p->flags119 |= active;
}

void Armor_UpdatePartOrbit(struct ArmorPart *p)
{
    struct ArmorBody *parent = p->parent;
    const s16 *sine;

    p->angle += (u8)p->direction;
    sine = gUnk_086149C4;
    p->x = parent->orbitX + Div(sine[(p->angle + 64) & 255] * 200, 4096);
    p->z = parent->orbitZ + Div(sine[p->angle] * 200, 4096);
    if (--p->timer == 0) {
        switch (parent->state) {
            case 2:
                SetPartState(p, 2);
                break;
            case 3:
                SetPartState(p, 3);
                break;
            case 4:
                SetPartState(p, 4);
                break;
        }
    }
}
