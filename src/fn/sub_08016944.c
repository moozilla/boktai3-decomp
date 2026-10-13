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
void FloatingEffect_Bob(void *m, struct Effect *p)
{
    if (p->entered) {
        p->entered = 0;
        p->sprite.flags &= ~1;
    }
    if (p->timer <= 2)
        p->position.y += 16;
    else
        p->position.y -= 16;
    FloatingEffect_Project(p);
    if (p->timer > 3) {
        int state = 1;
        p->state = state;
        p->entered = state;
        p->timer = 0;
    } else
        p->timer++;
}
