#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad[8];
    u32 flags;
    u8 padc[20];
    struct Vec position;
    u8 pad28[12];
    u8 scaleX, scaleY;
    u16 angle;
};
struct Effect {
    u32 pad0;
    u8 state, entered, pad6, mode;
    u32 pad8, timer;
    u16 duration;
    u8 index, pad13[5];
    struct Vec position;
    u8 pad20[4];
    struct Sprite sprite;
};
void sub_080168B8(struct Effect *);
void sub_0801689C(struct Effect *);
#define FloatingEffect_RiseAndShrink sub_08016B78
void FloatingEffect_RiseAndShrink(u8 *m, struct Effect *p, int index)
{
    struct Sprite *s = &p->sprite;
    u8 *scale;
    if (p->entered) {
        p->entered = 0;
        p->sprite.flags &= ~1;
        s->flags |= 2;
        s->angle = 0;
        p->sprite.scaleX = 64;
        p->sprite.scaleY = 64;
    }
    scale = &p->sprite.scaleX;
    if (*scale > 16)
        *scale -= 16;
    p->position.y += 64;
    sub_080168B8(p);
    s->position.x += (64 - *scale) >> 3;
    if (p->timer > 3) {
        sub_0801689C(p);
        *(u32 *)(m + 24) &= ~(1 << index);
    } else
        p->timer++;
}
