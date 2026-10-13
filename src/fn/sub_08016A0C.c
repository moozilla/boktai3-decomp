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
void FloatingEffect_Project(struct Effect *);
void sub_0801689C(struct Effect *);
void FloatingEffect_ShrinkAway(u8 *m, struct Effect *p, int index)
{
    struct Sprite *s = &p->sprite;
    u8 *scale;
    if (p->entered) {
        p->entered = 0;
        s->flags |= 2;
        s->angle = 0;
        p->sprite.scaleX = 64;
        p->sprite.scaleY = 64;
    }
    scale = &p->sprite.scaleX;
    if (*scale > 8)
        *scale -= 8;
    p->sprite.scaleY = *scale;
    s->angle += 16;
    p->position.x += 8;
    p->position.z += 8;
    FloatingEffect_Project(p);
    s->position.x += (64 - *scale) >> 3;
    if (p->timer > 7) {
        sub_0801689C(p);
        *(u32 *)(m + 24) &= ~(1 << index);
    } else
        p->timer++;
}
