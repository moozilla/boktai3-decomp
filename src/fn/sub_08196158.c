#include "global.h"

struct P08196158 {
    u8 f0[0x18];
    u8 queue[0x5404 - 0x18];
    s16 x;
    s16 pad;
    s16 y;
    u8 f1[0x724a - 0x540a];
    u8 state;
    u8 f2[0x72a8 - 0x724b];
    s32 a, b, c, d, e, f;
};
void sub_08195014(void *, s32, s32);
void sub_08195078(void *, s32, s32);
void sub_08195A4C(void *, u32);

void sub_08196158(struct P08196158 *t)
{
    switch (t->state) {
    case 0:
        sub_08195014(t, 0x1e0, 0x500);
        if (t->a < 0)
            t->d = 0x90;
        else if (t->a > 0)
            t->d = 0x90;
        else
            t->d = 0;
        t->state = 1;
    case 1:
        t->a += t->d;
        if (t->a > 0 && t->d > 0) {
            t->a = 0;
            t->d = 0;
        } else if (t->a < 0 && t->d < 0) {
            t->a = 0;
            t->d = 0;
        }
        sub_08195078(t, 0x1e0, 0x500);
        if (t->a == 0 && t->x == 0x1e0) {
            t->x = 0x1e0;
            t->b = 0;
            t->e = 0;
            if (t->x == 0x1e0 && t->y > 0x4fb && t->y <= 0x504) {
                t->y = 0x500;
                sub_08195A4C(t, 6);
            }
        }
        break;
    }
}
