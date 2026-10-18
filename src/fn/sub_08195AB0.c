#include "global.h"

struct P08195AB0 {
    u8 f0[0x18];
    u8 queue[0x5404 - 0x18];
    s16 x;
    s16 pad;
    s16 y;
    u8 f1[0x7248 - 0x540a];
    u8 active;
    u8 f2;
    u8 state;
    u8 f3[0x72a8 - 0x724b];
    s32 a, b, c, d, e, f;
};
void Nidhoggr_SetBackgroundMode(void *, u32);
void Nidhoggr_SetConfiguredTargetValues(void *);
void Nidhoggr_InitRelativeMotion(void *, s32, s32);
void Nidhoggr_UpdateAnchoredSway(void *, s32, s32);
void Nidhoggr_UpdateHorizontalSway(void *);
void Nidhoggr_InitFirstPhaseParts(void *);
void Nidhoggr_SetBehavior(void *, u32);
void Sound_PlaySongChecked(u32);

void Nidhoggr_UpdateEntrance(struct P08195AB0 *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_SetBackgroundMode(t, 0);
            Nidhoggr_SetConfiguredTargetValues(t);
            t->x = 0x260;
            t->pad = 0;
            t->y = 0x500;
            Nidhoggr_InitRelativeMotion(t, 0x200, 0xf80);
            t->d = -0x300;
            t->state++;
            Sound_PlaySongChecked(0x404);
            break;
        case 1: {
            s32 z;
            t->a += t->d;
            Nidhoggr_UpdateHorizontalSway(t);
            z = t->c += 0x2000;
            t->x = ((t->a + t->b) >> 8) + 0x200;
            t->y = (z >> 8) + 0xf80;
            if (t->c >= 0) {
                t->state++;
                t->d = -0xc00;
                t->e = 0x88;
                t->f = 0x400;
            }
            break;
        }
        case 2:
            t->a += t->d;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0xf80);
            if (t->a < -0x20000) {
                Nidhoggr_InitFirstPhaseParts(t);
                Nidhoggr_SetBehavior(t, 2);
                t->active = 0;
            }
            break;
    }
}
