#include "global.h"

struct P08195C20 {
    u8 f0[0x18];
    u8 queue[0x5404 - 0x18];
    s16 x;
    s16 pad;
    s16 y;
    u8 f1[0x7246 - 0x540a];
    u16 timer;
    u8 active;
    u8 f2;
    u8 state;
    u8 f3[0x7278 - 0x724b];
    s16 hp[9];
    u8 f4[0x72a8 - 0x728a];
    s32 a, b, c, d, e, f;
};
void sub_08193680(void *, u32);
void sub_08194FA4(void *, s32, s32);
void sub_08195078(void *, s32, s32);
void sub_08195810(void *);
void sub_081905F0(void *);
void sub_0818F38C(void *);
void sub_08195A4C(void *, u32);
void Sound_PlaySongChecked(u32);

void sub_08195C20(struct P08195C20 *t)
{
    s32 allDead, i;
    s16 *hp;
    switch (t->state) {
        case 0:
            sub_08193680(t, 0);
            sub_08194FA4(t, 0x200, 0xf80);
            if (t->a < -0x20000)
                t->state = 1;
            else {
                t->state = 2;
                t->timer = 0;
            }
            break;
        case 1:
            t->a += t->d;
            if (t->a > -0x20000) {
                t->state = 2;
                t->timer = 0;
            }
            t->d += 6;
            if (t->d > 0x800)
                t->d = 0x800;
            sub_08195078(t, 0x200, 0xf80);
            break;
        case 2:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 3:
            sub_08195810(t);
            t->a += t->d;
            if (t->a < -0x20000) {
                t->a = -0x20000;
                t->d = 0;
            } else if (t->a > 0) {
                t->a = 0;
                t->d = 0;
            }
            t->d += 6;
            if (t->d > 0x30)
                t->d = 0x30;
            sub_08195078(t, 0x200, 0xf80);
            if (t->x == 0x200) {
                t->state = 4;
                t->d = -0x600;
                sub_081905F0(t);
                sub_0818F38C(&t->x);
            }
            break;
        case 4:
            t->state++;
            Sound_PlaySongChecked(0x403);
        case 5:
            t->a += t->d;
            if (t->a < -0x20000) {
                t->a = -0x20000;
                t->d = 0;
            }
            t->d += 6;
            if (t->d > -0x30)
                t->d = -0x30;
            sub_08195078(t, 0x200, 0xf80);
            if (t->x < 0) {
                t->state = 2;
                t->d = 0x30;
            }
            break;
    }
    t->timer++;
    allDead = 1;
    hp = t->hp;
    for (i = 8; i >= 0; hp++, i--) {
        if (*hp > 0)
            allDead = 0;
    }
    if (allDead)
        sub_08195A4C(t, 3);
}
