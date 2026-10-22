#include "global.h"

struct E {
    u32 flags;
    u8 pad4[0x14];
    s16 x, y, z;
    u8 pad1E[0xA];
    u16 angle;
    u16 radius;
};
struct S {
    u8 pad0[0x1C];
    u32 flags;
    u8 pad20[0x44];
    s16 x, y, z;
    u8 pad6A[2];
    struct E e[8];
    u8 pad1CC[0x1CC - 0x1CC];
    u32 frames;
    u8 pad1D0[0xA];
    u16 timer;
};
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void SolarBomb_SetState(struct S *, void (*)(struct S *));
void sub_08233CAC(struct S *);
void SpriteDrawable_SelectTileFrame(void *, u32, u32);

static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void sub_08233B78(struct S *p)
{
    struct E *e;
    s32 i;
    if (++p->timer > 29) {
        for (i = 0; i < 8; i++) {
            p->e[i].flags |= 1;
            SolarBomb_SetState(p, sub_08233CAC);
        }
        p->flags &= ~1;
    } else {
        s32 n = 32 - p->timer;
        s32 d = n + 1;
        s16 *px = &p->x, *py = &p->y, *pz = &p->z;
        const s16 *t = gUnk_086149C4;
        e = p->e;
        for (i = 7; i >= 0; i--) {
            e->angle = (e->angle + 4) & 255;
            e->radius = Div(e->radius * n, d);
            e->x = *px + scale(e->radius * t[(e->angle + 64) & 255]);
            e->y = (30 - p->timer) * 10 + *py;
            e->z = *pz + scale(e->radius * t[e->angle & 255]);
            if (p->timer == 8)
                SpriteDrawable_SelectTileFrame(e, p->frames, 6);
            e++;
        }
    }
}
