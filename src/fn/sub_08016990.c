#include "global.h"
#include "player.h"
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
void FloatingEffect_Wait(void *m, struct Effect *p)
{
    if (p->entered) {
        p->entered = 0;
        p->sprite.flags &= ~1;
    }
    FloatingEffect_Project(p);
    if (p->timer >= p->duration) {
        u32 mode = p->mode;
        if (mode == 0) {
            u32 index = p->index;
            if (gPlayers[index])
                p->state = 6;
            else
                p->state = 5;
            p->entered = 1;
            p->timer = mode;
        } else if (mode == 1) {
            int state = 2;
            p->state = state;
            p->entered = mode;
            p->timer = 0;
        } else if (mode == 3) {
            int state = 6;
            p->state = state;
            p->entered = 1;
            p->timer = 0;
        }
    } else
        p->timer++;
}
