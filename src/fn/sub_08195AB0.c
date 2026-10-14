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
void sub_08193680(void *, u32);
void sub_08195A14(void *);
void sub_08195014(void *, s32, s32);
void sub_08195078(void *, s32, s32);
void sub_08195140(void *);
void sub_0819594C(void *);
void sub_08195A4C(void *, u32);
void Sound_PlaySongChecked(u32);

void sub_08195AB0(struct P08195AB0 *t)
{
    switch (t->state) {
        case 0:
            sub_08193680(t, 0);
            sub_08195A14(t);
            t->x = 0x260;
            t->pad = 0;
            t->y = 0x500;
            sub_08195014(t, 0x200, 0xf80);
            t->d = -0x300;
            t->state++;
            Sound_PlaySongChecked(0x404);
            break;
        case 1: {
            s32 z;
            t->a += t->d;
            sub_08195140(t);
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
            sub_08195078(t, 0x200, 0xf80);
            if (t->a < -0x20000) {
                sub_0819594C(t);
                sub_08195A4C(t, 2);
                t->active = 0;
            }
            break;
    }
}
